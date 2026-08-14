#include "StateAlchemist/controller.h"

#include "StateAlchemist/constants.h"
#include "StateAlchemist/fs_manager.h"
#include "StateAlchemist/meta_manager.h"

#include <borealis/core/logger.hpp>

#include <set>


Controller controller;


void Controller::setTitleId(const u64& titleId) {
  this->titleId = titleId;

  // Create the Atmosphere title ID folder for the current game
  FsManager::createFolderIfNeeded(this->getAtmospherePath());
}

void Controller::setGamePath(const std::string& name) {
  this->gamePath = name;
}

/**
 * Checks if the currenty-running game has a folder set up for Mod Alchemist
 */
bool Controller::doesGameHaveFolder() {
  return FsManager::doesFolderExist(this->getGamePath());
}

// NOTE: vectors returned from functions in controller
// are implied to be sorted alphabetically unless stated otherwise

/**
 * Load all groups from the game folder
 * 
 * @param sort Whether to sort the list of names alphabetically or not
 *             Can take considerable performance when in nested loops, so sometimes it's good to skip if not needed
 */
std::vector<std::string> Controller::loadGroups(bool sort) {
  return FsManager::listNames(this->getGamePath(), sort);
}

/**
 * Load all source options within the specified group
 * 
 * @param sort Whether to sort the list of names alphabetically or not
 *             Can take considerable performance when in nested loops, so sometimes it's good to skip if not needed
 * 
 * @requirement: group must be set
 */
std::vector<std::string> Controller::loadSources(bool sort) {
  return FsManager::listNames(this->getGroupPath(), sort);
}

/**
 * Load all mod options that could be activated for the moddable source in the group
 * 
 * @param sort Whether to sort the list of names alphabetically or not
 *             Can take considerable performance when in nested loops, so sometimes it's good to skip if not needed
 * 
 * @requirement: group and source must be set
 */
std::vector<std::string> Controller::loadMods(bool sort) {
  return FsManager::listNames(this->getSourcePath(), sort);
}


/*
 * Loads map of mods names to each rating
 * 
 * @requirement: group and source must be set
 */
std::map<std::string, u8> Controller::loadRatings() {
  std::map<std::string, u8> ratings;

  FsDir dir = FsManager::openFolder(this->getSourcePath(), FsDirOpenMode_ReadDirs);

  FsDirectoryEntry entry;
  s64 readCount = 0;
  while (R_SUCCEEDED(fsDirRead(&dir, &readCount, 1, &entry)) && readCount) {
    if (entry.type == FsDirEntryType_Dir) {
      std::string mod = MetaManager::parseName(entry.name);
      ratings[mod] = MetaManager::parseRating(entry.name);
    }
  }

  fsDirClose(&dir);

  return ratings;
}

/*
 * Loads the rating for the source (for using no mod)
 * 
 * @requirement: group and source must be set
 */
u8 Controller::loadDefaultRating() {
  u8 rating;

  FsDir dir = FsManager::openFolder(this->getGroupPath(), FsDirOpenMode_ReadDirs);

  FsDirectoryEntry entry;
  s64 readCount = 0;
  while (R_SUCCEEDED(fsDirRead(&dir, &readCount, 1, &entry))) {
    if (entry.type == FsDirEntryType_Dir && this->source == MetaManager::parseName(entry.name)) {
      rating = MetaManager::parseRating(entry.name);
      break;
    }
  }

  fsDirClose(&dir);

  return rating;
}

/*
 * Saves the ratings for each mod
 * 
 * @requirement: group and source must be set
 */
void Controller::saveRatings(const std::map<std::string, u8>& ratings) {
  for (const auto& [mod, rating]: ratings) {
    std::string currentPath = this->getModPath(mod);
    std::string newPath = this->getSourcePath() + "/" + MetaManager::buildFolderName(mod, rating);

    MetaManager::tryResult(
      fsFsRenameDirectory(
        &FsManager::sdSystem,
        FsManager::toPathBuffer(currentPath).get(),
        FsManager::toPathBuffer(newPath).get()
      )
    );
  }
}

/*
 * Saves the rating for using no mod for the current source
 */
void Controller::saveDefaultRating(const u8& rating) {
  std::string newPath = this->getGroupPath() + "/" + MetaManager::buildFolderName(this->source, rating);

  MetaManager::tryResult(
    fsFsRenameDirectory(
      &FsManager::sdSystem,
      FsManager::toPathBuffer(this->getSourcePath()).get(),
      FsManager::toPathBuffer(newPath).get()
    )
  );
}

/**
 * Gets the mod currently activated for the source
 *
 * Returns an empty string if no mod is active and vanilla files are being used
 * 
 * @requirement: group and must be set
 */
std::string Controller::getActiveMod(const std::string& source) {

  // Open to the correct source directory
  std::string groupPath = this->getGroupPath();
  FsDir sourceDir = FsManager::openFolder(
    groupPath + "/" + FsManager::getFolderName(groupPath, source),
    FsDirOpenMode_ReadFiles
  );

  std::vector<FsDirectoryEntry> entries(MAX_FS_ENTRY_LOAD);
  s64 readCount = 0;
  std::string activeMod = "";
  std::string name;

  // Find the .txt file in the directory. The name would be the active mod:
  while (R_SUCCEEDED(fsDirRead(&sourceDir, &readCount, MAX_FS_ENTRY_LOAD, entries.data())) && readCount) {
    for (int i = 0; i < readCount; i++) {
      FsDirectoryEntry entry = entries[i];
      if (entry.type == FsDirEntryType_File) {
        name = entry.name;
        if (name.find(TXT_EXT) != std::string::npos) {
          activeMod = name.substr(0, name.size() - TXT_EXT.size());
          break;
        }
      }
    }
    if (activeMod != "") break;
  }

  fsDirClose(&sourceDir);

  return activeMod;
}

/**
 * Activates the specified mod, moving all its files into the atmosphere folder for the game
 * 
 * Make sure to deactivate any existing active mod for this source if there is one
 * 
 * Mod won't be activated if EVERY file belonging to it has a conflict with a file already in the atmosphere folder
 * 
 * @requirement:
 *  - group and source must be set
 *  - "mod" parameter must not currently be active
 *  - the title ID folder for the current game must already exist in Atmosphere's "content" folder
 */
void Controller::activateMod(const std::string& mod) {

  // Path to the "mod" folder in alchemy's directory:
  std::string modPath = this->getModPath(mod);

  // The txt file for the active mod:
  FsFile movedFilesFile = FsManager::initFile(this->getMovedFilesListFilePath(mod));

  // Position in the txt file where we should write the next file path:
  s64 txtOffset = 0;

  // Recursively move every file of the mod, recording each one in the txt file:
  this->moveModFiles(modPath, "", movedFilesFile, txtOffset);

  fsFileClose(&movedFilesFile);
}

/**
 * Recursively moves the files of a mod from its folder into the atmosphere folder for the game.
 *
 * Every folder is enumerated with its own fresh FsDir handle opened from scratch, so a folder that
 * is reported (or read) incorrectly by the filesystem doesn't cause the remaining entries of its
 * parent to be skipped (unlike the previous approach that reused a single handle and tracked
 * iteration indices to jump between folders).
 */
void Controller::moveModFiles(
  const std::string& modPath,
  const std::string& currentBasePath,
  FsFile& movedFilesFile,
  s64& txtOffset
) {
  std::vector<FsDirectoryEntry> entries;
  FsManager::readAllEntries(modPath + currentBasePath, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, entries);

  for (FsDirectoryEntry& entry : entries) {
    std::string nextPath = currentBasePath + "/" + entry.name;
    const std::string sourcePath = modPath + nextPath;
    const std::string targetPath = this->getAtmospherePath() + nextPath;

    // If the next entry is a file, we will move it and record it as moved as long as there isn't a conflict.
    //
    // File size has to be compared for rare cases where folder is incorrectly categorized as a file.
    // If the entry type is still unclear after that, fall back to checking the actual path on the SD card,
    // since some filesystems can report corrupt or unexpected entry types.
    bool isFile = entry.type == FsDirEntryType_File && entry.file_size > 0;
    bool isDirectory = entry.type == FsDirEntryType_Dir;

    if (!isFile && !isDirectory) {
      isDirectory = FsManager::doesFolderExist(sourcePath);

      if (!isDirectory) {
        isFile = FsManager::doesFileExist(sourcePath);
      }
    }

    if (isFile) {
      // If a file already exists in the location we'll move it to, there's a conflict:
      bool fileConflict = FsManager::doesFileExist(targetPath);
      if (!fileConflict) {
        // Record the file we're moving, and move it:
        FsManager::write(movedFilesFile, nextPath + "\n", txtOffset);
        FsManager::moveFile(sourcePath, targetPath);
      }
    // If the next entry is a folder, we will traverse within it:
    } else if (isDirectory) {
      FsManager::createFolderIfNeeded(targetPath);

      this->moveModFiles(modPath, nextPath, movedFilesFile, txtOffset);

      // Delete the folder only if it's now empty. The folder should be empty,
      // but if not for whatever reason, this should just silently break and skip it:
      fsFsDeleteDirectory(&FsManager::sdSystem, FsManager::toPathBuffer(sourcePath).get());
    } else {
      brls::Logger::warning("Mod Alchemist: unknown FS entry '{}' (type {}), skipping", sourcePath, static_cast<int>(entry.type));
    }
  }
}

/**
 * Deactivates the currently active mod, restoring the moddable source to its vanilla state
 * 
 * @requirement: group and source must be set
 */
void Controller::deactivateMod() {
  std::string activeMod(this->getActiveMod(this->source));

  // If no active mod:
  if (activeMod.empty()) { return; }

  this->returnFiles(activeMod);
}

/**
 * @param progress Scale of 0.0-1.0 of the method's current progress.
 *                 Updated while the method runs.
 */
void Controller::deactivateAll(std::atomic<float>& progress) {
  std::vector<std::string> groups = this->loadGroups(false);

  // Percentage completed per group
  float progressPerGroup = 1.0f / groups.size();

  for (const std::string& group : groups) {
    this->group = group;

    std::vector<std::string> sources = this->loadSources(false);

    // Percentage completed per source
    float progressPerSource = progressPerGroup / sources.size();

    for (const std::string& source : sources) {
      this->source = source;
      std::string activeMod(this->getActiveMod(source));

      if (!activeMod.empty()) {
        this->returnFiles(activeMod);
      }
    
      progress.store(progress.load() + progressPerSource);
    }
  }

  this->group = "";
  this->source = "";
}

/**
 * Randomly activates/deactivates all mods based upon their ratings
 *
 * @param progress Scale of 0.0-1.0 of the method's current progress.
 *                 Updated while the method runs.
 */
void Controller::randomizeGame(std::atomic<float>& progress) {
  std::vector<std::string> groups = this->loadGroups(false);

  // Percentage completed per group
  float progressPerGroup = 1.0f / groups.size();

  for (const std::string& group : groups) {
    this->group = group;
    this->randomizeGroup(progress, progressPerGroup);
  }

  this->group = "";
  this->source = "";
}

/**
 * Randomly activates/deactivates all mods in the current group
 * 
 * @requirement: group must be set
 *
 * @param progress Scale of 0.0-1.0 of the method's current progress.
 *                 Updated while the method runs.
 *
 * @param percentageOfGame If the group is being randomized as part of an entire game,
 *                         include the percentage of the total number of groups this group represents.
 *                         The method will only increase the progress by that percentage.
 *                         Otherwise, it's expected that this group is the only thing being randomized,
 *                         so the progress param is at 0% and it will move forward to 100%.
 */
void Controller::randomizeGroup(std::atomic<float>& progress, const float& percentageOfGame) {
  std::vector<std::string> sources = this->loadSources(false);

  // Percentage completed per source
  float progressPerSource = percentageOfGame / sources.size();

  for (const std::string& source : sources) {
    this->source = source;
    this->randomizeSource();
    
    progress.store(progress.load() + progressPerSource);
  }
}

/**
 * Randomly activates/deactivates a mod for the current source in the current group
 * 
 * @requirement: group and source must be set
 */
void Controller::randomizeSource() {
  
  // Seed the random number generator with the current time
  std::srand(static_cast<unsigned int>(std::time(nullptr)));

  std::map<std::string, u8> ratings = this->loadRatings();
  u8 defaultRating = this->loadDefaultRating();

  // Sum all ratings to pick one at random:
  u16 ratingTotal = defaultRating;
  for (const auto& [mod, rating]: ratings) {
    ratingTotal += rating;
  }

  // Just skip if all ratings are 0 for some reason:
  if (ratingTotal == 0) { return; }

  // Get the random number 
  u16 randomChoice = (std::rand() % ratingTotal) + 1;

  // If it's within the default option's range, deactivate it:
  if (randomChoice <= defaultRating) {
    this->deactivateMod();
  } else {
    // Otherwise, keep subtracting the ratings until we reach the one to activate:
    randomChoice -= defaultRating;

    for (const auto& [mod, rating]: ratings) {
      if (randomChoice < rating) {
        std::string activeMod = this->getActiveMod(this->source);

        // No need to do anything if the picked mod is also the currently-active one:
        if (activeMod == mod) { return; }

        // If there's an active mod, deactivate it:
        if (!activeMod.empty()) {
          this->returnFiles(activeMod);
        }

        this->activateMod(mod);
        return;
      }

      randomChoice -= rating;
    }
  }
}

Controller::Controller() {
  pmdmntInitialize();
  pminfoInitialize();

  MetaManager::tryResult(fsOpenSdCardFileSystem(&FsManager::sdSystem));
}

/**
 * Unmount SD card when destroyed 
 */
Controller::~Controller() {
  fsFsClose(&FsManager::sdSystem);
  pminfoExit();
  pmdmntExit();
}

/**
 * Returns all files belonging to a mod from the atmosphere active mods folder to their original location
 * 
 * Essentially the same as deactivating the mod, except this can't be used with the default mod option.
 */
void Controller::returnFiles(const std::string& mod) {

  std::unique_ptr<char[]> movedFilesListPath = FsManager::toPathBuffer(this->getMovedFilesListFilePath(mod));
  std::string modPath = this->getModPath(mod);
  std::string atmoRootPath = this->getAtmospherePath();
  int atmoRootPathSize = atmoRootPath.size();

  // Try to open the active mod's txt file to get the list of files that were moved to atmosphere's folder:
  FsFile movedFilesList;
  MetaManager::tryResult(
    fsFsOpenFile(&FsManager::sdSystem, movedFilesListPath.get(), FsOpenMode_Read, &movedFilesList)
  );

  s64 fileSize;
  MetaManager::tryResult(fsFileGetSize(&movedFilesList, &fileSize));

  // Initialize buffer and path builder:
  s64 offset = 0;
  char* buffer = new char[FILE_LIST_BUFFER_SIZE];
  std::string pathBuilder = "";

  // As long as there is still data in the file:
  while (offset < fileSize) {

    // Read some of the text into our buffer:
    MetaManager::tryResult(
      fsFileRead(&movedFilesList, offset, buffer, FILE_LIST_BUFFER_SIZE, FsReadOption_None, nullptr)
    );

    // Append it to the string we're using to build the next path:
    pathBuilder += std::string_view(buffer, FILE_LIST_BUFFER_SIZE);

    // If the path builder got a new line character from the buffer, we have a full path:
    std::size_t newLinePos = pathBuilder.find('\n');
    while (newLinePos != std::string::npos) {
      // Trim the new line and any characters that were gathered after it to get the cleaned atmosphere file path:
      std::string basePath = pathBuilder.substr(0, newLinePos);
      std::string atmoPath = atmoRootPath + basePath;

      // Move any characters gathered after the new line to the pathBuilder string for the next path:
      pathBuilder = pathBuilder.substr(newLinePos + 1);

      // Move the file back to the mod's folder:
      FsManager::moveFile(atmoPath, modPath + basePath);

      // If there are any folders now empty after moving this file, delete them:
      FsManager::forEachFolderInFilePathDeepestFirst(atmoPath, [atmoRootPathSize](std::string path) {
        if (path.size() <= atmoRootPathSize) {
          return false; // If this point is reached, we are at the game's root folder, so stop deleting.
        }

        Result deleted = fsFsDeleteDirectory(&FsManager::sdSystem, FsManager::toPathBuffer(path).get());
        if (R_FAILED(deleted)) {
          return false; // If the deletion failed, it's probably because the folder has content, so end the iteration.
        }

        // If this point is reached, go on to the next parent folder:
        return true;
      });

      newLinePos = pathBuilder.find('\n');
    }

    offset += FILE_LIST_BUFFER_SIZE;
  }

  delete[] buffer;

  fsFileClose(&movedFilesList);

  // Once all the files have been returned, delete the txt list:
  MetaManager::tryResult(
    fsFsDeleteFile(&FsManager::sdSystem, movedFilesListPath.get())
  );
}

/*
 * Gets Mod Alchemist's game directory:
 */
std::string Controller::getGamePath() { return this->gamePath; }

/*
 * Gets the file path for the specified group
 * 
 * @requirement: group must be set
 */
std::string Controller::getGroupPath() {
  return this->getGamePath() + "/" + this->group;
}

/*
 * Gets the file path for the specified source within the group
 * 
 * @requirement: group and source must be set
 */
std::string Controller::getSourcePath() {
  std::string groupPath = this->getGroupPath();
  return groupPath + "/" + FsManager::getFolderName(groupPath, this->source);
}

/*
 * Get the file path for the specified mod within the moddable source
 * 
 * @requirement: group and source must be set
 */
std::string Controller::getModPath(const std::string& mod) {
  std::string sourcePath = this->getSourcePath();
  return sourcePath + "/" + FsManager::getFolderName(sourcePath, mod);
}

/**
 * Gets the game's path that's stored within Atmosphere's directory
 */
std::string Controller::getAtmospherePath() {
  return ATMOSPHERE_PATH + MetaManager::getHexTitleId(this->titleId);
}

/**
 * Gets the file path for the list of moved files for the specified mod
 * 
 * The file should only exist if the mod is currently active
 * 
 * @requirement: group and source must be set
 */
std::string Controller::getMovedFilesListFilePath(const std::string& mod) {
  return this->getSourcePath() + "/" + mod + TXT_EXT;
}
