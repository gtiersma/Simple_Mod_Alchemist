//
// Created by Nadrino on 06/09/2019.
//

#include "ModManager.h"
#include <GameBrowser.h>

#include "GenericToolbox.Switch.h"
#include "GenericToolbox.Vector.h"

#include <switch.h>

#include <StateAlchemist/controller.h>


const int ModManager::_INIT_CHUNK_SIZE_ = 15;
const int ModManager::_SEQUENT_CHUNK_SIZE_ = 10;

ModManager::ModManager(GameBrowser* owner_) : _owner_(owner_) {}

const ConfigHolder& ModManager::getConfig() const{
  return _owner_->getConfigHandler().getConfig();
}
ConfigHolder& ModManager::getConfig(){
  return _owner_->getConfigHandler().getConfig();
}

/**
 * Call this instead of doing "controller.group = group" directly.
 * This will reset/reload data that's necessary for when the group changes.
 */
void ModManager::setSources(const std::vector<std::string>& sources) {
  this->_mod_source_names_ = sources;
  this->_last_loaded_index_ = -1;
  this->_mod_source_cache_.clear();
  this->loadSources(ModManager::_INIT_CHUNK_SIZE_);
}

/**
 * Gets the data related to a moddable thing in the game (aka source)
 * 
 * @warning Expects we already know there's a loaded object for the specified index.
 */
ModSource& ModManager::getSource(const int& index) {
  return this->_mod_source_cache_[this->_mod_source_names_[index]];
}

std::string ModManager::getSourceName(const int& index) {
  return this->_mod_source_names_[index];
}

int ModManager::getSourceCount() {
  return this->_mod_source_names_.size();
}

bool ModManager::isSourceLoaded(const int& index) {
  return index <= this->_last_loaded_index_;
}

std::vector<ModSource> ModManager::loadSourcesIfNeeded(const int& index) {
  if (index > this->_last_loaded_index_ + ModManager::_SEQUENT_CHUNK_SIZE_) {
    // This call should ideally never be hit, but it's here just to be safe.
    // Loads an exceptionally large number of source objects if the "index" argument is significantly further down the list.
    return this->loadSources(index - this->_last_loaded_index_ + ModManager::_SEQUENT_CHUNK_SIZE_);
  } else if (!this->isSourceLoaded(index + ModManager::_SEQUENT_CHUNK_SIZE_)) {
    return this->loadSources(ModManager::_SEQUENT_CHUNK_SIZE_);
  }
  return {}
}

/**
 * Gets the index of the currently-active mod listed in the source_'s "mods" vector
 * 
 * Returns -1 if no mod is active
 * 
 * @param mods Ordered vector of mod names that belong to the source.
 */
int ModManager::getActiveIndex(const std::string& source, const std::vector<std::string>& mods) {
  unsigned activeIndex = -1;
  std::string activeMod = controller.getActiveMod(source);

  if (activeMod.empty()) { return activeIndex; }

  for (auto& mod : mods) {
    activeIndex++;
    if (mod == activeMod) { break; }
  }

  return activeIndex;
}

/**
 * Updates all sources currently rendered in the UI with what mods are currently active
 */
void ModManager::refreshActiveIndices() {
  for (auto& source : this->_mod_source_cache_) {
    source.second.setActiveIndex(
      this->getActiveIndex(source.first, source.second.getMods())
    );
  }
}

std::vector<ModSource> ModManager::loadSources(const int& count) {
  std::vector<ModSource> loadedSources;

  // Total number of sources; including those that have yet to load their data.
  int sourceCount = this->_mod_source_names_.size();

  // The last source index this method should load data for.
  int lastIndexToLoad = this->_last_loaded_index_ + count;

  // Last index shouldn't exceed the total number of sources:
  if (lastIndexToLoad >= sourceCount) {
    lastIndexToLoad = sourceCount - 1;
  };

  // Load the data for each mod source, storing it in the cache map:
  for (int i = this->_last_loaded_index_ + 1; i <= lastIndexToLoad; i++) {
    controller.source = this->_mod_source_names_[i];
    std::vector<std::string> mods = controller.loadMods(true);

    ModSource source = ModSource(
      controller.source,
      std::move(mods),
      this->getActiveIndex(controller.source, mods)
    );
    loadedSources.push_back(source);
    this->_mod_source_cache_.insert({ controller.source, source });
  }

  // Track the last one we loaded:
  this->_last_loaded_index_ = lastIndexToLoad;

  return loadedSources;
}
