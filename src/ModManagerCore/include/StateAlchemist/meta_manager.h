#pragma once

#include <switch.h>

#include <string>
#include <vector>

namespace MetaManager {
  
  /**
   * Formats a u64 title ID into a hexidecimal string
   */
  std::string getHexTitleId(const u64& titleId);

  /**
   * Reverse of getHexTitleId
   */
  u64 getNumericTitleId(const std::string& titleId);

  /**
   * Checks if a string is capable of being an actual title ID
   */
  bool isTitleId(const std::string& titleId);

  /**
   * Checks if a folder name for a possible game contains a parsable title ID.
   */
  bool hasTitleId(const std::string& folderName);

  /**
   * Parses a game's title ID from a game's folder name.
   *
   * @param folderName Assumes it certainly has a parsable title ID (can be checked with hasTitleId).
   */
  u64 parseTitleId(const std::string& folderName);

  /**
   * Parses the name of an entity from a folder name
   */
  std::string parseName(const std::string& folderName);

  /**
   * Parses a rating of a mod from a folder name
   */
  u8 parseRating(const std::string& folderName);

  /**
   * Builds a folder name from a mod name and rating
   */
  std::string buildFolderName(const std::string& modName, const u8& rating);

  /**
   * Checks if the entity name belongs to a folder name
   */
  bool namesMatch(char* folderName, const std::string& entityName);

  /**
   * Limit string size of the options used by a SelectItem.
   * Needed for the UI. It hangs if strings get to be too excessively long.
   */
  std::vector<std::string> limitSelectLabels(std::vector<std::string>& rawNames);

  void tryResult(Result result);
}