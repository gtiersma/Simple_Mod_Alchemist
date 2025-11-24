#include "ModMigrator.h"
#include "StateAlchemist/fs_manager.h"
#include "StateAlchemist/meta_manager.h"
#include "StateAlchemist/constants.h"

#include <switch.h>

#include <vector>


// Path in the SD root that vanilla SimpleModManager used to store its data
const std::string ModMigrator::LEGACY_BASE_PATH = "/mods";

// The name of the single folder that should exist in the root of every mod that vanilla SimpleModManager used
const std::string ModMigrator::LEGACY_MOD_ROOT_FOLDER = "contents";

// Group name for storing the migrated mods
const std::string ModMigrator::MIGRATION_GROUP = "_Uncategorized";

// The actual mod folder name of the migrated mods.
// We have no reliable way of knowing the "source" that each mod replaced in the vanilla SimpleModManager,
// so each mod will have its own source with a single mod under it for enabling it.
const std::string ModMigrator::MIGRATION_MOD_NAME = "Enable Mod";

/**
 * @param progress Scale of 0.0-1.0 of the method's current progress.
 *                 Updated while the method runs.
 */
void ModMigrator::begin(std::atomic<float>& progress) {

  // Case: Vanilla SMM folder isn't there, so do nothing:
  if (!FsManager::doesFolderExist(LEGACY_BASE_PATH)) return;

  std::vector<std::string> gameFolders = FsManager::listNames(LEGACY_BASE_PATH, false);

  // Case: No legacy folders for games:
  if (gameFolders.empty()) return;

  // Percentage completed per game
  float progressPerGame = 1.0f / gameFolders.size();

  // Iterators are floats solely to allow for floating-point operations without conversion
  float i = 0;
  for (const std::string& gameFolder : gameFolders) {
    bool isMigrated = migrateGame(gameFolder, progress, progressPerGame);

    // "migrateGame" only increments progress if migrated.
    // If it was skipped, progress still needs to be incremented, so we do that here:
    if (!isMigrated) {
      progress.store(progress.load() + progressPerGame);
    }
  }

  // Delete the old "mods" folder only if it's now empty (not recursively):
  fsFsDeleteDirectory(&FsManager::sdSystem, FsManager::toPathBuffer(LEGACY_BASE_PATH).get());
}

/**
 * Migrates mods belonging to a single game
 * 
 * @param gameFolder The name of the game that matches the folder under LEGACY_BASE_PATH
 *
 * @param progress Scale of 0.0-1.0 of the method's current progress.
 *                 Updated while the method runs.
 *
 * @param percentageOfTotal The percentage of the total number of games this game represents.
 *                          The method will only increase the progress by that percentage.
 *
 * @returns "true" if it moved the game's mods (or at least attempted to move).
 *          "false" if something's not right, so it skipped moving the game's mods.
 */
bool ModMigrator::migrateGame(const std::string& gameFolder, std::atomic<float>& progress, const float& percentageOfTotal) {
  std::string legacyGamePath = LEGACY_BASE_PATH + "/" + gameFolder;

  std::vector<std::string> modFolders = FsManager::listNames(legacyGamePath, false);

  // Case: No mods for this game, so do nothing
  if (modFolders.empty()) return false;

  // Get the folder that contains the folder with the game's title ID:
  std::string contentsFolder = legacyGamePath + "/" + modFolders[0] + "/" + LEGACY_MOD_ROOT_FOLDER;

  // Case: The folder isn't found, which means there's something weird about this mod.
  // Just don't migrate these mods. Skip them. Better safe than sorry.
  if (!FsManager::doesFolderExist(contentsFolder)) return false;

  // Should be a single folder with the game's title ID
  std::vector<std::string> titleId = FsManager::listNames(contentsFolder, false);

  // Case: It wasn't a single folder with the game's title ID. Something weird about these mods.
  // Just don't migrate them. Skip them. Better safe than sorry.
  if (titleId.size() != 1) return false;
  if (!MetaManager::isTitleId(titleId[0])) return false;

  // Create the new game folder
  std::string newGamePath = ALCHEMIST_PATH + "/" + titleId[0];
  FsManager::createFolderIfNeeded(newGamePath);

  // Create a group for that game to store the migrated mods
  std::string groupPath = newGamePath + "/" + MIGRATION_GROUP;
  FsManager::createFolderIfNeeded(groupPath);

  // Percentage completed per mod folder
  float progressPerMod = percentageOfTotal / modFolders.size();

  for (const std::string& modFolder : modFolders) {
    migrateMod(groupPath, modFolder, titleId[0], legacyGamePath);
    progress.store(progress.load() + progressPerMod);
  }

  // Delete the old game folder only if it's now empty (not recursively):
  fsFsDeleteDirectory(&FsManager::sdSystem, FsManager::toPathBuffer(legacyGamePath).get());

  return true;
}

/**
 * Migrates a single mod from an old folder
 * 
 * @param groupPath The path to the mod group created for storing migrated mods
 * @param modFolder The folder name of the mod to migrate
 * @param titleId The hexidecimal string title ID of the game the mod belongs to
 * @param legacyGamePath The path to the mod's folder in the old SMM directory
 */
void ModMigrator::migrateMod(
  const std::string& groupPath,
  const std::string& modFolder,
  const std::string& titleId,
  const std::string& legacyGamePath
) {
  std::string newModPath = groupPath + "/" + modFolder;
  std::string oldModPath = legacyGamePath + "/" + modFolder;

  // The path to the title ID folder that should exist under the old mod's folder:
  std::string oldModTitleIdPath = oldModPath + "/" + LEGACY_MOD_ROOT_FOLDER + "/" + titleId;

  // Case: Something weird about this mod. Just don't migrate it. Skip it. Better safe than sorry.
  if (!FsManager::doesFolderExist(oldModTitleIdPath)) return;

  // Create the new source/mod folders for the mod to migrate:
  FsManager::createFolderIfNeeded(newModPath);
  newModPath = newModPath + "/" + MIGRATION_MOD_NAME;
  FsManager::createFolderIfNeeded(newModPath);

  moveFiles(oldModTitleIdPath, newModPath);

  // Delete the old mod folder.
  // But we're checking if there are any files left in that folder.
  //
  // If there are, something went wrong and we'll pernamently delete some mod files if we do so,
  // so then just leave everything there to be safe.
  if (!FsManager::hasFilesDeep(oldModTitleIdPath)) {
    fsFsDeleteDirectoryRecursively(&FsManager::sdSystem, FsManager::toPathBuffer(oldModPath).get());
  }
}

/**
 * Migrates the mod folders & files from the old SMM mod folder to the new one.
 * 
 * The code was mostly just copied over and modifiedfrom Controller::activateMod since we know that method is already reliable.
 * The method was built for maximum memory efficiency though, so this could be much more CPU optimized.
 * 
 * @param oldPath The path to the original SMM folder that would contain the mod's romfs folder
 * @param newPath The path to the new folder where the folder structure under "oldPath" should be moved to
 */
void ModMigrator::moveFiles(const std::string& oldPath, const std::string& newPath) {
  FsDir dir = FsManager::openFolder(oldPath, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles);

  // Iterartor for current entry in the current directory:
  short i = 0;

  // Used for "storing" where the iteration left off at when traversing deeper into the hierarchy:
  std::vector<u64> iStorage;

  // The path we are currently at relative to the mod path.
  // Empty string is mod path itself:
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

        // If the next entry is a file, we will move it.
        //
        // File size has to be compared for rare cases where folder is incorrectly categorized as a file.
        // In these cases, the entry loaded is corrupt, so we have to skip it and not load the mod files within it.
        if (entry.type == FsDirEntryType_File && entry.file_size > 0) {
          FsManager::moveFile(oldPath + nextPath, newPath + nextPath);
        // If the next entry is a folder, we will traverse within it:
        } else if (entry.type == FsDirEntryType_Dir) {
          FsManager::createFolderIfNeeded(newPath + nextPath);

          // Add the current count to the storage:
          iStorage.push_back(i);

          currentBasePath = nextPath;
          FsManager::changeFolder(dir, oldPath + nextPath, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles);

          // Reset the index & iterator because we're starting in a new folder:
          entryIndex = 0;
          i = 0;
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
        FsManager::changeFolder(dir, oldPath + currentBasePath, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles);

        // Reset the entry index because it will start at the beginning again:
        entryIndex = 0;
      }
    }

  }

  fsDirClose(&dir);
}