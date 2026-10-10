//
// Created by Nadrino on 06/09/2019.
//

#ifndef MODAPPLIER_MOD_MANAGER_H
#define MODAPPLIER_MOD_MANAGER_H

#include <ConfigHandler.h>

#include <map>
#include <vector>
#include <string>
#include <utility>


/**
 * Object containing data related to a "source" (something moddable in a game)
 *
 * TODO: Separate into its own header/.cpp file combo
 */
class ModSource {
  public:

    // Option used for the setting to turn a mod off
    const std::string _DEFAULT_OPTION_{"UNMODIFIED"}; 

    /**
     * @param source_ String label of the name of the moddable thing (source)
     * @param mods_ List of available mods of the source
     * @param activeIndex_ Index of "mods" vector of mod that is currently active (-1 if none active)
     */
    explicit ModSource(std::string source_, std::vector<std::string> mods_, size_t activeIndex_):
      source(std::move(source_)),
      mods(std::move(mods_)),
      activeIndex(activeIndex_) {
        options = this->mods;
        options.insert(options.begin(), _DEFAULT_OPTION_);
      }

    ModSource() : source(""), mods(), options(), activeIndex(-1) {}

    const std::string& getSource() const { return this->source; }
    const std::vector<std::string>& getMods() const { return this->mods; }
    const std::vector<std::string>& getOptions() const { return this->options; }
    const int getModCount() { return this->mods.size(); }

    size_t getActiveIndex() const { return this->activeIndex; }
    void setActiveIndex(size_t index) { this->activeIndex = index; }

    /**
     * Removes a mod at the specified index from the source's data.
     * 
     * @param index Must correspond to the element to remove in the "mods" vector.
     *              The same mod is also removed from the "options" vector,
     *              converting this index to the correct index for that one.
     */
    void removeMod(size_t index) {
      this->mods.erase(this->mods.begin() + index);
      this->options.erase(this->options.begin() + index + 1);
    }

  private:

    std::string source;
    std::vector<std::string> mods;

    // Essentially just the mods list, but as options for the UI.
    // The main difference is it also has the option for using no mod at the beginning as an additional element.
    std::vector<std::string> options;

    size_t activeIndex;
};


class GameBrowser;

/**
 * Class for interacting with mods
 * 
 * Originally part of vanilla SimpleModManager. Now uses StateAlchemist as a backend.
 * 
 * Set the group in controller.group; all interactions in this class are scoped to that group.
 */
class ModManager {

public:
  explicit ModManager(GameBrowser* owner_);

  // shortcuts
  const ConfigHolder& getConfig() const;
  ConfigHolder& getConfig();

  /**
   * Set the current sources for whichever group is currently loaded.
   * 
   * This will allow this mod manager to reset its state and load some initial mods for the source list.
   * 
   * @returns The initially loaded mod source objects for the specified sources.
   *          Keep in mind NOT ALL sources are initially loaded for performance reasons.
   */
  std::vector<ModSource> setSources(const std::vector<std::string>& sources);

  /**
   * Clears the source data that is currently loaded, reloading fresh data from the SD card.
   *
   * @returns The initially loaded mod source objects for the specified sources.
   *          Keep in mind NOT ALL sources are initially loaded for performance reasons.
   */
  std::vector<ModSource> reloadSources();

  ModSource& getSource(const std::string& name);

  /**
   * Gets the source object for the source name at the specified index.
   * Assumes that we already know the source object at that index has been loaded.
   * 
   * @assumes There's a loaded object for the specified index.
   */
  ModSource& getSourceByIndex(const int& index);

  /**
   * Gets the source name at the specified index.
   * Source does not need to be loaded. 
   */
  std::string getSourceName(const int& index);

  /**
   * Gets the total number of sources in the group; whether loaded or not
   */
  int getSourceCount();

  /**
   * Gets the index of the currently-active mod listed in the source_'s "mods" vector
   * 
   * Returns -1 if no mod is active
   * 
   * @param mods Ordered vector of mod names that belong to the source.
   */
  int getActiveIndex(const std::string& sourceName, const std::vector<std::string>& mods);

  /**
   * Updates all sources currently rendered in the UI with what mods are currently active
   */
  void refreshActiveIndices();

  /**
   * Check if the source object for the source name at the specified index has already been loaded or not.
   */
  bool isSourceLoaded(const int& index);

  /**
   * Will load data for more mod sources if the source "index" parameter is close enough to an index we haven't loaded data for yet.
   *
   * This ensures we always load data ahead of time while something like scrolling is happening.
   *
   * @returns A sorted vector only of the ModSources that were just loaded from this function call.
   *          Empty if no loading occurred.
   */
  std::vector<ModSource> loadSourcesIfNeeded(const int& index);

private:
  GameBrowser* _owner_{nullptr};

  /**
   * The last index of the source name vector that had its object loaded.
   * 
   * Objects are loaded sequentially, parallel to the vector of names.
   */
  int _last_loaded_index_{-1};

  /**
   * All names of all sources belonging to a group.
   * Includes both those loaded and those not.
   * Reloaded every time setGroup() is used.
   */
  std::vector<std::string> _mod_source_names_;

  /**
   * Storage for all source objects that have been loaded so far.
   * Uses map just for fast look-ups.
   */
  std::map<std::string, ModSource> _mod_source_cache_;

  /**
   * Loads more data for the mod sources.
   *
   * There could be a lot of sources and each one requires individual filesystem operations
   * to load their data, so we do it in batches.
   *
   * @param count The number of mod sources to load data for.
   *              Loading starts at the next index of the last one that was loaded.
   *              It then loads the number specified from that point.
   *
   * @returns A sorted vector only of the ModSources that were just loaded from this function call.
   *
   * If all sources in the current group have already had all their data loaded,
   * this method does nothing.
   */
  std::vector<ModSource> loadSources(const int& count);

  /**
   * The number of mod sources to load data for initially (the first time data is loaded for mods in a group)
   *
   * Mod data is loaded in chunks due to the fact that file/folder names need to be read individually per group, so large numbers of sources can cause delays.
   */
  static const int _INIT_CHUNK_SIZE_;

  /**
   * Smaller chunks of mod data is loaded at a time after the initial chunk is loaded.
   */
  static const int _SEQUENT_CHUNK_SIZE_;
};


#endif //MODAPPLIER_MOD_MANAGER_H
