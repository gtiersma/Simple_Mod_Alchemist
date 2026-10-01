#include "StateAlchemist/fs_manager.h"
#include "StateAlchemist/meta_manager.h"
#include "StateAlchemist/constants.h"

#include <borealis/core/logger.hpp>

#include <algorithm>
#include <cstring>
#include <set>

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

  Result result = fsFsOpenDirectory(&sdSystem, toPathBuffer(path).get(), mode, &dir);
  if (R_FAILED(result)) {
    brls::Logger::warning("FsManager: changeFolder failed for '{}' (result {:#x})", path, result);
  }
}

void FsManager::createFolderIfNeeded(const std::string& path) {
  if (doesFolderExist(path)) { return; }

  Result result = fsFsCreateDirectory(&sdSystem, toPathBuffer(path).get());
  if (R_FAILED(result)) {
    brls::Logger::warning("FsManager: createFolderIfNeeded failed for '{}' (result {:#x})", path, result);
  }
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
    brls::Logger::warning("FsManager: doesFolderExist failed for '{}' (result {:#x})", path, result);
    return false;
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
    brls::Logger::warning("FsManager: doesFileExist failed for '{}' (result {:#x})", path, result);
    return false;
  }
}

bool FsManager::hasFilesDeep(const std::string& path) {
  
  // Just to be safe, always treat the path as having files until we finish navigating the entire path's tree:
  bool hasFiles = true;

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
          break; // File was found. No more work to do.
        }
      } else {

        // EDGE CASE: For some reason, sometimes fsDirRead gets a readCount of 0 when there should be an entry within it.
        //            This can be dangerous since this method is often used with recursive empty folder deletions
        //            that may delete a file if this function returns an incorrect result.
        //            To avoid this, we're checking the actual count here to see if it matches up.
        //            If it doesn't, this method returns "false" just to be safe.
        s64 totalCount = 0;
        if (R_SUCCEEDED(fsDirGetEntryCount(&dir, &totalCount)) && totalCount != i) {
          break;
        }

        // If there's nothing left in our count storage, we've navigated everything, encountering no files:
        if (iStorage.size() == 0) {
          hasFiles = false;
          break;
        }

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

  return hasFiles;
}

std::vector<std::string> FsManager::loadNames(const std::string& path, bool sort) {
  std::vector<FsDirectoryEntry> entries;
  readAllEntries(path, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, entries);

  // Map of the parsed name of the entity TO the original folder name it came from:
  std::unordered_map<std::string, std::string> parsedToFolder = {};

  for (FsDirectoryEntry& entry : entries) {
    // Exclude hidden folders that start with "."
    if (entry.type == FsDirEntryType_Dir && entry.name[0] != '.') {
      const std::string parsedName = MetaManager::parseName(entry.name);
      
      // Check for folders that are getting parsed to the same name.
      auto duplicate = parsedToFolder.find(parsedName);
      if (duplicate == parsedToFolder.end()) {
        parsedToFolder[parsedName] = entry.name;
      } else { // If a duplicate is found, combine them:

        // Favor keeping the longer folder name since that one must have the metadata in the name:
        const std::string betterName = duplicate->second.size() > std::strlen(entry.name) ? duplicate->second : entry.name;
        const std::string worseName = duplicate->second.size() > std::strlen(entry.name) ? entry.name : duplicate->second;
          
        moveContents(ALCHEMIST_PATH + "/" + worseName, ALCHEMIST_PATH + "/" + betterName, ConflictStrategy::OVERWRITE_TARGET);
        fsFsDeleteDirectory(&sdSystem, toPathBuffer(ALCHEMIST_PATH + "/" + worseName).get());

        parsedToFolder[parsedName] = betterName;
      }
    }
  }

  // Copy de-duplicated parsed names from the map into a vector:
  std::vector<std::string> names;
  names.reserve(parsedToFolder.size());
  for (const auto& [parsed, folderName] : parsedToFolder) {
    names.push_back(parsed);
  }

  if (sort) {
    std::sort(names.begin(), names.end());
  }

  return names;
}

void FsManager::readAllEntries(const std::string& path, const u32& mode, std::vector<FsDirectoryEntry>& out) {
  out.clear();

  std::set<std::string> seenNames;
  s64 expectedCount = -1;

  FsDir countDir;
  Result countResult = fsFsOpenDirectory(&sdSystem, toPathBuffer(path).get(), mode, &countDir);
  if (R_SUCCEEDED(countResult)) {
    s64 total = 0;
    if (R_SUCCEEDED(fsDirGetEntryCount(&countDir, &total))) {
      expectedCount = total;
    }
    fsDirClose(&countDir);
  }

  // Read in a loop, reopening the directory each time, until we have seen all
  // reported entries (or until a full pass adds nothing new).
  bool madeProgress = true;
  while (madeProgress) {
    madeProgress = false;

    FsDir dir = FsManager::openFolder(path, mode);

    std::vector<FsDirectoryEntry> entries(MAX_FS_ENTRY_LOAD);
    s64 readCount = 0;
    while (R_SUCCEEDED(fsDirRead(&dir, &readCount, MAX_FS_ENTRY_LOAD, entries.data())) && readCount) {
      for (s64 i = 0; i < readCount; i++) {
        FsDirectoryEntry& entry = entries[i];
        if (seenNames.insert(entry.name).second) {
          out.push_back(entry);
          madeProgress = true;
        }
      }
    }

    fsDirClose(&dir);

    if (expectedCount > 0 && out.size() >= static_cast<size_t>(expectedCount)) {
      break;
    }
  }
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

void FsManager::moveFile(const std::string& fromPath, const std::string& toPath) {
  forEachFolderInFilePath(toPath, [](std::string path) {
    createFolderIfNeeded(path);
    return true;
  });

  Result result = fsFsRenameFile(&sdSystem, toPathBuffer(fromPath).get(), toPathBuffer(toPath).get());
  if (R_FAILED(result)) {
    brls::Logger::warning("FsManager: moveFile failed '{}' -> '{}' (result {:#x})", fromPath, toPath, result);
  }
}

void FsManager::moveContents(
  const std::string& fromPath,
  const std::string& toPath,
  ConflictStrategy conflictStrategy,
  std::function<void (const std::string& relativePath, bool conflicts)> fileMoveFn,
  std::function<void (const std::string& relativePath)> folderMoveFn,
  const std::string& basePath
) {
  std::vector<FsDirectoryEntry> entries;
  readAllEntries(fromPath + basePath, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, entries);

  for (FsDirectoryEntry& entry : entries) {
    std::string nextPath = std::string(basePath + "/") + entry.name;
    const std::string sourcePath = fromPath + nextPath;
    const std::string targetPath = toPath + nextPath;

    // If the next entry is a file, we will move it and record it as moved as long as there isn't a conflict.
    //
    // File size has to be compared for rare cases where folder is incorrectly categorized as a file.
    // If the entry type is still unclear after that, fall back to checking the actual path on the SD card,
    // since some filesystems can report corrupt or unexpected entry types.
    bool isFile = entry.type == FsDirEntryType_File && entry.file_size > 0;
    bool isDirectory = entry.type == FsDirEntryType_Dir;

    if (!isFile && !isDirectory) {
      isDirectory = doesFolderExist(sourcePath);

      if (!isDirectory) {
        isFile = doesFileExist(sourcePath);
      }
    }

    if (isFile) {
      // If a file already exists in the location we'll move it to, there's a conflict:
      bool fileConflict = doesFileExist(targetPath);
      fileMoveFn(nextPath, fileConflict);
      if (fileConflict) {
        if (conflictStrategy == ConflictStrategy::OVERWRITE_TARGET) {
          fsFsDeleteFile(&sdSystem, toPathBuffer(targetPath).get());
          moveFile(sourcePath, targetPath);
        } else if (conflictStrategy == ConflictStrategy::PRESERVE_TARGET) {
          fsFsDeleteFile(&sdSystem, toPathBuffer(sourcePath).get());
        }
      } else {
        moveFile(sourcePath, targetPath);
      }
    // If the next entry is a folder, we will traverse within it:
    } else if (isDirectory) {
      folderMoveFn(nextPath);
      createFolderIfNeeded(targetPath);

      moveContents(fromPath, toPath, conflictStrategy, fileMoveFn, folderMoveFn, nextPath);

      // Delete the folder only if it's now empty. The folder should be empty,
      // but if not for whatever reason, this should just silently break and skip it:
      fsFsDeleteDirectory(&sdSystem, toPathBuffer(sourcePath).get());
    } else {
      brls::Logger::warning("Mod Alchemist: unknown FS entry '{}' (type {}), skipping", sourcePath, static_cast<int>(entry.type));
    }
  }
}

void FsManager::forEachFolderInFilePath(const std::string& path, std::function<bool (const std::string& path)> fn) {
  std::string pathRemaining = path.substr(1); // Index 0 is a "/", so start at index 1
  int slashIndex = pathRemaining.find_first_of("/");
  std::string currentPath = "";

  while (slashIndex != std::string::npos) {
    currentPath = currentPath + "/" + pathRemaining.substr(0, slashIndex);

    bool shouldContinue = fn(currentPath);
    if (!shouldContinue) {
      break;
    }

    pathRemaining.erase(0, slashIndex + 1);
    slashIndex = pathRemaining.find_first_of("/");
  }
}

void FsManager::forEachFolderInFilePathDeepestFirst(const std::string& path, std::function<bool (const std::string& path)> fn) {
  std::string currentPath = path;
  int slashIndex = path.find_last_of("/");

  while (slashIndex != std::string::npos) {
    currentPath = currentPath.substr(0, slashIndex);

    bool shouldContinue = fn(currentPath);
    if (!shouldContinue) {
      break;
    }

    slashIndex = currentPath.find_last_of("/");
  }
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