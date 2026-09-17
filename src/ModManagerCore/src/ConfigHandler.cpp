//
// Created by Nadrino on 16/10/2019.
//

#include "ConfigHandler.h"

#include "GenericToolbox.Switch.h"
#include "GenericToolbox.Vector.h"

#include <iostream>

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
    }
    else if(elements[0] == "show-file-list-popup"){
      config.showFileListPopup = (elements[1] == "true");
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
  ssConfig << "show-file-list-popup = " << (_config_.showFileListPopup ? "true" : "false") << std::endl;
  ssConfig << std::endl;

  GenericToolbox::dumpStringInFile(_config_.configFilePath, ssConfig.str());
}



