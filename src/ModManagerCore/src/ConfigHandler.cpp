//
// Created by Nadrino on 16/10/2019.
//

#include "ConfigHandler.h"

#include "GenericToolbox.Switch.h"
#include "GenericToolbox.Vector.h"

#include <iostream>
#include <charconv>


bool ConfigHandler::shouldShowFileMoveReport(bool hasConflicts) {
  switch (_config_.showFileListPopup) {
    case FileMoveReporting::NEVER: return false;
    case FileMoveReporting::ALWAYS: return true;
    case FileMoveReporting::ON_CONFLICT: return hasConflicts;
    default: return false; // setting error: just fallback to false
  }
}

// io
void ConfigHandler::loadConfig(const std::string &configFilePath_) {
  ConfigHolder config{}; // loads defaults
  config.configFilePath = configFilePath_;

  if( config.configFilePath.empty() ){
    // default path or reload
    config.configFilePath = _config_.configFilePath;
  }

  if( not GenericToolbox::isFile(config.configFilePath) ){
    // immediately dump the default config to the file
    this->dumpConfigToFile();
    return;
  }

  // parse the config file
  auto configLines = GenericToolbox::dumpFileAsVectorString( config.configFilePath, true );
  for( auto& line : configLines ){

    // removing heading and trailing spaces
    GenericToolbox::trimInputString(line, " ");

    // check if it is a comment
    if( GenericToolbox::startsWith(line, "#") ) continue;

    // check if it is a valid piece of data
    auto elements = GenericToolbox::splitString(line, "=");
    if( elements.size() != 2 ) continue;

    // removing heading and trailing spaces
    GenericToolbox::trimInputString( elements[0], " " );
    GenericToolbox::trimInputString( elements[1], " " );

    if(elements[0] == "last-program-version"){
      config.lastSmmVersion = elements[1];
    } else if (elements[0] == "show-file-list-popup") {
      int fileListSetting{};
      auto result = std::from_chars(elements[1].data(), elements[1].data() + elements[1].size(), fileListSetting);
      if (result.ec == std::errc()) {
        // Load setting if valid int (todo: but it's not checking int range)
        config.showFileListPopup = static_cast<FileMoveReporting>(fileListSetting);
      }
    }

  } // lines

  // copy to the member
  _config_ = config;

  // rewrite for cleanup
  this->dumpConfigToFile();
}
void ConfigHandler::dumpConfigToFile() const {

  GenericToolbox::mkdir( GenericToolbox::getFolderPath( _config_.configFilePath ) );

  std::stringstream ssConfig;
  ssConfig << "# This is a config file" << std::endl;
  ssConfig << std::endl;
  ssConfig << std::endl;
  ssConfig << "# DO NOT TOUCH THIS : used to recognise the last version of the program config" << std::endl;
  ssConfig << "last-program-version = " << APP_VERSION << std::endl;
  ssConfig << std::endl;
  ssConfig << "# Show the file list popup after enabling a mod" << std::endl;
  ssConfig << "show-file-list-popup = " << std::to_string(static_cast<int>(_config_.showFileListPopup)) << std::endl;
  ssConfig << std::endl;

  GenericToolbox::dumpStringInFile(_config_.configFilePath, ssConfig.str());
}



