#include "StateAlchemist/fs_manager.h"
#include "StateAlchemist/meta_manager.h"
#include "StateAlchemist/constants.h"

#include <algorithm>
#include <cstring>

FsFileSystem FsManager::sdSystem;

/**
 * Creates a new open FsDir object for the specified path
 * 
 * Don't forget to close when done
 */
FsDir FsManager::openFolder(const std::string& path, const u32& mode) {
  FsDir dir;
  changeFolder(dir, path, mode);
  return dir;
}

/**
 * Changes an FsDir instance to the specified path
 */
void FsManager::changeFolder(FsDir& dir, const std::string& path, const u32& mode) {
  fsDirClose(&dir);

  MetaManager::tryResult(
    fsFsOpenDirectory(&sdSystem, toPathBuffer(path).get(), mode, &dir)
  );
}

void FsManager::createFolderIfNeeded(const std::string& path) {
  if (doesFolderExist(path)) { return; }

  MetaManager::tryResult(
    fsFsCreateDirectory(&sdSystem, toPathBuffer(path).get())
  );
}

bool FsManager::doesFolderExist(const std::string& path) {
  FsDir dir;
  Result result = fsFsOpenDirectory(
    &sdSystem,
    toPathBuffer(path).get(),
    FsOpenMode_Read,
    &dir
  );

  if (R_SUCCEEDED(result)) {
    fsDirClose(&dir);
    return true; // File exists
  } else if (result == 0x202) {
    return false; // File does not exist
  } else {
    MetaManager::tryResult(result); // Handle other exceptions
    return false; // This line will never be reached, but added for completeness
  }
}

bool FsManager::doesFileExist(const std::string& path) {
  FsFile file;
  Result result = fsFsOpenFile(
    &sdSystem,
    toPathBuffer(path).get(),
    FsOpenMode_Read,
    &file
  );

  if (R_SUCCEEDED(result)) {
    fsFileClose(&file);
    return true; // File exists
  } else if (result == 0x202) {
    return false; // File does not exist
  } else {
    MetaManager::tryResult(result); // Handle other exceptions
    return false; // This line will never be reached, but added for completeness
  }
}



bool FsManager::hasFilesDeep(const std::string& path) {
  bool fileFound = false;

  FsDir dir = openFolder(path, FsDirOpenMode_ReadDirs);

  // Iterartor for current entry in the current directory:
  short i = 0;

  // Used for "storing" where the iteration left off at when traversing deeper into the hierarchy:
  std::vector<u64> iStorage;

  // The path we are currently at relative to the original path.
  // Empty string is original path itself:
  std::string currentBasePath = "";

  // The index of the current entry we're iterating over in the current directory:
  short entryIndex = 0;

  // The current number of files read at a time
  // It reads 1 at a time, so it will always be either 1 or 0 (0 if all have been read)
  s64 readCount = 0;

  FsDirectoryEntry entry;

  while (R_SUCCEEDED(fsDirRead(&dir, &readCount, 1, &entry))) {

    // Continue iterating the index until it catches up with the iteration we should be on (if needed):
    entryIndex++;
    if (entryIndex > i) {
      i++;

      if (readCount > 0) {
        std::string nextPath = currentBasePath + "/" + entry.name;

        // If the next entry is a folder, we will traverse within it:
        if (entry.type == FsDirEntryType_Dir) {

          // Add the current count to the storage:
          iStorage.push_back(i);

          currentBasePath = nextPath;
          changeFolder(dir, path + nextPath, FsDirOpenMode_ReadDirs);

          // Reset the index & iterator because we're starting in a new folder:
          entryIndex = 0;
          i = 0;
        } else {
          fileFound = true;
          break;
        }
      } else {
        // If there's nothing left in our count storage, we've navigated everything, so we're done:
        if (iStorage.size() == 0) { break; }

        // Otherwise, let's get back the count data of where we left off in the parent:
        i = iStorage.back();
        iStorage.pop_back();

        // Remove the string portion after the last '/' to get the parent's path:
        std::size_t lastSlashIndex = currentBasePath.rfind('/');
        currentBasePath = currentBasePath.substr(0, lastSlashIndex);
        changeFolder(dir, path + currentBasePath, FsDirOpenMode_ReadDirs);

        // Reset the entry index because it will start at the beginning again:
        entryIndex = 0;
      }
    }

  }

  fsDirClose(&dir);

  return fileFound;
}

/**
 * Gets a vector of all entity names that are directly within the specified path
 * (parsing the name from the folder name)
 * 
 * @param sort Whether to sort the list of names alphabetically or not
 *             Can take considerable performance when in nested loops, so sometimes it's good to skip if not needed
 */
std::vector<std::string> FsManager::listNames(const std::string& path, bool sort) {
  std::vector<std::string> names;

  FsDir dir = FsManager::openFolder(path, FsDirOpenMode_ReadDirs);

  std::vector<FsDirectoryEntry> entries(MAX_FS_ENTRY_LOAD);
  s64 readCount = 0;
  while (R_SUCCEEDED(fsDirRead(&dir, &readCount, MAX_FS_ENTRY_LOAD, entries.data())) && readCount) {
    for (int i = 0; i < readCount; i++) {
      FsDirectoryEntry entry = entries[i];
      // Exclude hidden folders that start with "."
      if (entry.type == FsDirEntryType_Dir && entry.name[0] != '.') {
        names.push_back(MetaManager::parseName(entry.name));
      }
    }
  }

  fsDirClose(&dir);

  if (sort) {
    std::sort(names.begin(), names.end());
  }

  return names;
}

/**
 * Gets the name of the folder that currently exists with the name of the specified entity
 */
std::string FsManager::getFolderName(const std::string& path, const std::string& name) {
  std::string folderName;
  
  FsDir dir = FsManager::openFolder(path, FsDirOpenMode_ReadDirs);

  FsDirectoryEntry entry;
  s64 readCount = 0;
  while (R_SUCCEEDED(fsDirRead(&dir, &readCount, 1, &entry)) && readCount) {
    if (entry.type == FsDirEntryType_Dir && MetaManager::namesMatch(entry.name, name)) {
      folderName = entry.name;
      break;
    }
  }

  fsDirClose(&dir);

  return folderName;
}

/**
 * Opens a file at the path (creating it if it doesn't exist)
 */
FsFile FsManager::initFile(const std::string& path) {
  std::unique_ptr<char[]> charPath = toPathBuffer(path);

  // If the file hasn't been created yet, create it:
  if (!doesFileExist(path)) {
    MetaManager::tryResult(
      fsFsCreateFile(&sdSystem, charPath.get(), 0, 0)
    );
  }

  // Open the file:
  FsFile file;
  MetaManager::tryResult(
    fsFsOpenFile( &sdSystem, charPath.get(), FsOpenMode_Write | FsOpenMode_Append, &file)
  );

  return file;
}

/**
 * Records the text parameter in the filePath, appending it to the FsFile
 * 
 * offset is expected to be at the end of the file,
 * and it's updated to the new position at the end of file
 */
void FsManager::write(FsFile& file, const std::string& text, s64& offset) {

  // Write the path to the end of the list:
  MetaManager::tryResult(
    fsFileWrite(&file, offset, text.c_str(), text.size(), FsWriteOption_Flush)
  );

  // Update the offset to the end of the file:
  offset += text.size();
}

/**
 * Changes the fromPath file parameter's location to what's specified as the toPath parameter
 */
void FsManager::moveFile(const std::string& fromPath, const std::string& toPath) {
  MetaManager::tryResult(
    fsFsRenameFile(&sdSystem, toPathBuffer(fromPath).get(), toPathBuffer(toPath).get())
  );
}

/**
 * Formats a string as a char array that will work properly as a parameter for libnx's filesystem functions
 * 
 * Use `get()` when passing it to a libnx function
 */
std::unique_ptr<char[]> FsManager::toPathBuffer(const std::string& path) {
  // Allocate memory for the char array with a fixed size
  std::unique_ptr<char[]> pathBuffer(new char[FS_MAX_PATH]);

  // Copy the input string into the buffer
  std::strcpy(pathBuffer.get(), path.c_str());

  // Return the unique_ptr which will handle garbage collection automatically
  return pathBuffer;
}