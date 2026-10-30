//
// Created by Nadrino on 16/10/2019.
//

#ifndef SIMPLEMODMANAGER_CONFIGHANDLER_H
#define SIMPLEMODMANAGER_CONFIGHANDLER_H


#include "GenericToolbox.String.h"
#include "GenericToolbox.Macro.h"

#include "StateAlchemist/constants.h"

#include <string>
#include <sstream>

/**
 * When to show the dialog reporting the status of mod files that were just moved.
 */
enum class FileMoveReporting {
  NEVER,
  ALWAYS,
  ON_CONFLICT // Only show if file conflicts occurred.
}


struct ConfigHolder{
  std::string lastSmmVersion{};
  FileMoveReporting showFileListPopup{FileMoveReporting::ON_CONFLICT};

  std::string configFilePath{"/config/SimpleModAlchemist/parameters.ini"};

  [[nodiscard]] std::string getSummary() const {
    std::stringstream ss;
    ss << GET_VAR_NAME_VALUE(lastSmmVersion) << std::endl;
    return ss.str();
  }
};

class ConfigHandler {

public:
  ConfigHandler() { this->loadConfig(); }

  /**
   * Computes whether the moved file listing should be shown to the user after moving files.
   *
   * @param hasConflicts Whether there were any conflicting files during the transfer or not.
   */
  bool shouldShowFileMoveReport(bool hasConflicts);

  // getters
  [[nodiscard]] const ConfigHolder &getConfig() const { return _config_; }
  ConfigHolder &getConfig() { return _config_; }

  // io
  void loadConfig(const std::string& configFilePath_ = "");
  void dumpConfigToFile() const;

private:
  ConfigHolder _config_{};

};


#endif //SIMPLEMODMANAGER_CONFIGHANDLER_H
