#ifndef MOD_MIGRATOR_H
#define MOD_MIGRATOR_H

#include <string>
#include <atomic>

/**
 * Migrates old mods set up for vanilla SimpleModManager to the new format to work with this manager
 */
class ModMigrator {
  public:

    /**
     * Begins looking for old files and tries to move them over if possible
     *
     * @param progress Scale of 0.0-1.0 of the method's current progress.
     *                 Updated while the method runs.
     */
    static void begin(std::atomic<float>& progress);

  private:
    static const std::string LEGACY_BASE_PATH;
    static const std::string LEGACY_MOD_ROOT_FOLDER;
    static const std::string MIGRATION_GROUP;
    static const std::string MIGRATION_MOD_NAME;


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
    static bool migrateGame(const std::string& gameFolder, std::atomic<float>& progress, const float& percentageOfTotal);


    /**
     * Migrates a single mod from an old folder
     * 
     * @param groupPath The path to the mod group created for storing migrated mods
     * @param modFolder The folder name of the mod to migrate
     * @param titleId The hexidecimal string title ID of the game the mod belongs to
     * @param legacyGamePath The path to the mod's folder in the old SMM directory
     */
    static void migrateMod(
      const std::string& groupPath,
      const std::string& modFolder,
      const std::string& titleId,
      const std::string& legacyGamePath
    );

    /**
     * Migrates the mod folders & files from the old SMM mod folder to the new one.
     * 
     * The code was mostly just copied over and modifiedfrom Controller::activateMod since we know that method is already reliable.
     * The method was built for maximum memory efficiency though, so this could be much more CPU optimized.
     * 
     * @param oldPath The path to the original SMM folder that would contain the mod's romfs folder
     * @param newPath The path to the new folder where the folder structure under "oldPath" should be moved to
     */
    static void moveFiles(const std::string& oldPath, const std::string& newPath);
};

#endif //MOD_MIGRATOR_H