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
   * Takes a string that's to be used for a folder name and "cleans" it to make it safer for using for folder names.
   * 
   * - Removes almost all (if not all) characters that may not be used in a folder name.
   * - Removed characters are replaced with a space.
   * - Name is trimmed of spaces. Multiple adjacent spaces combined into a single space.
   * - Name length is limited.
   *
   * @param softLimit The approximate maximum length the name should be.
   *                  If the limit is reached, this function will limit the length close to this parameter,
   *                  cutting the string off at the end of a word.
   *
   *                  This parameter is necessary because some of the folder paths to individual mod files can get really long,
   *                  and there's a maximum length to the path length.
   *                  We'll get errors if the maximum length is reached, so for foldernames that may be long and part of a long path,
   *                  it's a good idea to use this parameter to ensure the path strings computed off of this folder name won't be too excessively long.
   *
   *                  What this parameter should be set to is dependent on how long you expect the path strings this folder may be part of can get to be.
   *
   *
   * @returns In this function's current state, there are definitely ways this string may still be unsafe,
   *          but this function at least is likely to make the string safe 99% of the time. Close enough.
   */
  std::string makeFolderNameSafe(const std::string& dirtyName, const int softLimit);

  /**
   * Limit string size of the options used by a SelectItem.
   * Needed for the UI. It hangs if strings get to be too excessively long.
   */
  std::vector<std::string> limitSelectLabels(std::vector<std::string>& rawNames);

  /**
   * Throws an error if the Result fails
   */
  void tryResult(Result result);
}