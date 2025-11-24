//
// Created by Adrien BLANCHET on 28/06/2020.
//

#include "GuiModManager.h"

#include "GenericToolbox.Switch.h"
#include "GenericToolbox.Vector.h"
#include "GenericToolbox.Misc.h"

#include <string>
#include <vector>
#include <sstream>
#include <StateAlchemist/controller.h>


// TODO: Is this file even still in use?

const GameBrowser &GuiModManager::getGameBrowser() const { return _gameBrowser_; }
GameBrowser &GuiModManager::getGameBrowser(){ return _gameBrowser_; }

void GuiModManager::applyMod(const std::string &modName_) {
  modApplyMonitor = ModApplyMonitor();

  // Get the group, source and mod name from the string into a vector --- TEMP CODE
  std::istringstream iss(modName_);
  std::vector<std::string> modGrouping;
  std::string individualModGrouping;
  while (std::getline(iss, individualModGrouping, '/')) {
    modGrouping.push_back(individualModGrouping);
  }

  controller.group = modGrouping[0];
  controller.source = modGrouping[1];
  controller.activateMod(modGrouping[2]);
}

void GuiModManager::applyModsList(std::vector<std::string>& modsList_){
  for (const auto& modName : modsList_) {
    this->applyMod(modName);
  }
}

void GuiModManager::startApplyModPresetThread(const std::string &modPresetName_){
  this->_triggeredOnCancel_ = false;

  // start the parallel thread
  std::async(&GuiModManager::applyModPresetFunction, this, modPresetName_);
}

void GuiModManager::applyModPresetFunction(const std::string& presetName_){
  // push the progress bar to the view
  // BROKEN
  /*_loadingPopup_.pushView();

  _loadingPopup_.getMonitorView()->setHeaderTitle("Removing all installed mods...");
  _loadingPopup_.getMonitorView()->resetMonitorAddresses();
  _loadingPopup_.getMonitorView()->setTitlePtr( &modRemoveAllMonitor.currentMod );
  _loadingPopup_.getMonitorView()->setSubTitlePtr( &modRemoveMonitor.currentFile );
  this->removeAllMods();

  _loadingPopup_.getMonitorView()->setHeaderTitle("Applying mod preset...");
  _loadingPopup_.getMonitorView()->resetMonitorAddresses();
  _loadingPopup_.getMonitorView()->setTitlePtr( &modApplyListMonitor.currentMod );
  _loadingPopup_.getMonitorView()->setSubTitlePtr( &modApplyMonitor.currentFile );

  std::vector<std::string> modsList;
  for( auto& preset : _gameBrowser_.getModPresetHandler().getPresetList() ){
    if( preset.name == presetName_ ){ modsList = preset.modList; break; }
  }
  this->applyModsList(modsList);

  this->leaveModAction();*/
}


void GuiModManager::leaveModAction(){
  _triggerUpdateModsDisplayedStatus_ = true;
  //_loadingPopup_.popView();
  brls::Application::unblockInputs();
}

