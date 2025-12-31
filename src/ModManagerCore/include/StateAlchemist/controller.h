#pragma once

#include <switch.h>

#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <atomic>

class Controller {
  public:
    u64 titleId; // The current Game's Title ID
    std::string group;
    std::string source;

    void init();

    void setTitleId(const u64& titleId);

    /**
     * Formats u64 title ID into a hexidecimal string
     */
    std::string getHexTitleId();

    bool doesGameHaveFolder();

    // NOTE: vectors returned from functions in controller
    // are implied to be sorted alphabetically unless stated otherwise

    /*
     * Load all groups from the game folder
     * 
     * @param sort Whether to sort the list of names alphabetically or not
     *             Can take considerable performance when in nested loops, so sometimes it's good to skip if not needed
     */
    std::vector<std::string> loadGroups(bool sort);

    /*
     * Load all source options within the specified group
     * 
     * @param sort Whether to sort the list of names alphabetically or not
     *             Can take considerable performance when in nested loops, so sometimes it's good to skip if not needed
     */
    std::vector<std::string> loadSources(bool sort);

    /*
     * Load all mod options that could be activated for the moddable source in the group
     * 
     * @param sort Whether to sort the list of names alphabetically or not
     *             Can take considerable performance when in nested loops, so sometimes it's good to skip if not needed
     */
    std::vector<std::string> loadMods(bool sort);

    /*
     * Loads map of mods names to each rating
     */
    std::map<std::string, u8> loadRatings();

    /*
     * Loads the rating for the source (for using no mod)
     * 
     * @requirement: group and source must be set
     */
    u8 loadDefaultRating();

    /*
     * Saves the ratings for each mod
     */
    void saveRatings(const std::map<std::string, u8>& ratings);

    /*
     * Saves the rating for using no mod for the current source
     */
    void saveDefaultRating(const u8& rating);

    /*
     * Gets the mod currently activated for the moddable source in the group
     *
     * Returns an empty string if no mod is active and vanilla files are being used
     */
    std::string getActiveMod(const std::string& source);

    /*
     * Activates the specified mod, moving all its files into the atmosphere folder for the game
     */
    void activateMod(const std::string& mod);

    /**
     * Deactivates the currently active mod, restoring the moddable source to its vanilla state
     */
    void deactivateMod();

    /**
     * @param progress Scale of 0.0-1.0 of the method's current progress.
     *                 Updated while the method runs.
     */
    void deactivateAll(std::atomic<float>& progress);

    /**
     * Randomly activates/deactivates all mods in the current game based upon their ratings
     *
     * @param progress Scale of 0.0-1.0 of the method's current progress.
     *                 Updated while the method runs.
     */
    void randomizeGame(std::atomic<float>& progress);

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
    void randomizeGroup(std::atomic<float>& progress, const float& percentageOfGame = 1.0f);

    /**
     * Randomly activates/deactivates a mod for the current source in the current group
     * 
     * @requirement: group and source must be set
     */
    void randomizeSource();

    /**
     * Gets Mod Alchemist's game directory:
     */
    std::string getGamePath();

    /**
     * Gets the file path for the specified group
     */
    std::string getGroupPath();

    Controller() = default;

    /**
     * Unmount SD card when destroyed 
     */
    ~Controller();

  private:

    /**
     * Returns all files belonging to a mod from the atmosphere active mods folder to their original location
     * 
     * Essentially the same as deactivating the mod, except this can't be used with the default mod option.
     */
    void returnFiles(const std::string& mod);

    /**
     * Gets the file path for the specified source within the group
     */
    std::string getSourcePath();

    /**
     * Get the file path for the specified mod within the moddable source
     */
    std::string getModPath(const std::string& mod);

    /**
     * Gets the game's path that's stored within Atmosphere's directory
     */
    std::string getAtmospherePath();

    /**
     * Gets the file path for the list of moved files for the specified mod
     * 
     * The file should only exist if the mod is currently active
     */
    std::string getMovedFilesListFilePath(const std::string& mod);
};

extern Controller controller;