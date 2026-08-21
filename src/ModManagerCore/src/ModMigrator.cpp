#include "ModMigrator.h"

#include <Game.h>
#include <GameBrowser.h>

#include "StateAlchemist/fs_manager.h"
#include "StateAlchemist/meta_manager.h"
#include "StateAlchemist/constants.h"

#include <borealis/core/logger.hpp>

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

  std::string newGamePath = gameBrowser.getOrCreateGamePath(titleId[0]);

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

  // Have the old mod folders deleted if they're empty:
  fsFsDeleteDirectory(&FsManager::sdSystem, FsManager::toPathBuffer(oldModTitleIdPath).get());
  fsFsDeleteDirectory(&FsManager::sdSystem, FsManager::toPathBuffer(oldModPath + "/" + LEGACY_MOD_ROOT_FOLDER).get());
  fsFsDeleteDirectory(&FsManager::sdSystem, FsManager::toPathBuffer(oldModPath).get());
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
  std::vector<FsDirectoryEntry> entries;
  FsManager::readAllEntries(oldPath, FsDirOpenMode_ReadDirs | FsDirOpenMode_ReadFiles, entries);

  for (FsDirectoryEntry& entry : entries) {
    const std::string sourcePath = oldPath + "/" + entry.name;
    const std::string targetPath = newPath + "/" + entry.name;

    // If the next entry is a file, we will move it.
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
      FsManager::moveFile(sourcePath, targetPath);
    // If the next entry is a folder, we will traverse within it:
    } else if (isDirectory) {
      FsManager::createFolderIfNeeded(targetPath);

      ModMigrator::moveFiles(sourcePath, targetPath);

      // Delete the folder only if it's now empty. The folder should be empty,
      // but if not for whatever reason, this should just silently break and skip it:
      fsFsDeleteDirectory(&FsManager::sdSystem, FsManager::toPathBuffer(sourcePath).get());
    } else {
      brls::Logger::warning("Mod Migrator: unknown FS entry '{}' (type {}), skipping", sourcePath, static_cast<int>(entry.type));
    }
  }
}