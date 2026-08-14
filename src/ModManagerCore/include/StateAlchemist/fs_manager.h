#pragma once

#include <switch.h>
#include <switch/result.h>

#include <vector>
#include <string>
#include <memory>
#include <functional>


/**
 * Heper functions related to the filesystem
 */
namespace FsManager {
  extern FsFileSystem sdSystem;

  /**
   * Creates a new open FsDir object for the specified path
   * 
   * Don't forget to close when done
   */
  FsDir openFolder(const std::string& path, const u32& mode);

  /**
   * Changes an FsDir instance to the specified path
   */
  void changeFolder(FsDir& dir, const std::string& path, const u32& mode);

  void createFolderIfNeeded(const std::string& path);

  bool doesFolderExist(const std::string& path);
  bool doesFileExist(const std::string& path);

  /**
   * Returns "true" if the folder at the given folder path (or any subfolder within it - regardless of how deep) contains a file.
   * Returns "false" if the entire folder hierarchy at that path is empty.
   */
  bool hasFilesDeep(const std::string& path);

  /**
   * Gets a vector of all entity names that are directly within the specified path
   * (parsing the name from the folder name)
   * 
   * @param sort Whether to sort the list of names alphabetically or not
   *             Can take considerable performance when in nested loops, so sometimes it's good to skip if not needed
   */
  std::vector<std::string> listNames(const std::string& path, bool sort);

  /**
   * Reads every directory entry of the specified path into the given vector.
   *
   * The filesystem can sometimes report a premature "end of directory" before all
   * entries have been returned (see hasFilesDeep). To work around that, the directory
   * is reopened with a fresh handle and re-read until no new entries are found or the
   * reported entry count is reached.
   *
   * @param path The folder to read
   * @param mode FsDirOpenMode flags for how the folder should be opened
   * @param out The vector to fill with the directory entries. It is cleared first.
   */
  void readAllEntries(const std::string& path, const u32& mode, std::vector<FsDirectoryEntry>& out);

  /**
   * Gets the folder name for an entity with the specified name
   */
  std::string getFolderName(const std::string& path, const std::string& name);

  /**
   * Opens a file at the path (creating it if it doesn't exist)
   */
  FsFile initFile(const std::string& path);

  /**
   * Records the text parameter in the filePath, appending it to the FsFile
   * 
   * offset is expected to be at the end of the file,
   * and it's updated to the new position at the end of file
   */
  void write(FsFile& file, const std::string& text, s64& offset);

  /**
   * Changes the fromPath file parameter's location to what's specified as the toPath parameter.
   * If any folders in the "toPath" don't exist, it creates them.
   */
  void moveFile(const std::string& fromPath, const std::string& toPath);

  /**
   * Performs the provided function on every folder in the path of a file.
   *
   * Iteration order is from the root folder to the deepest in the path.
   *
   * This function itself performs no file system operations (just string operations),
   * so it won't hit any exception itself regardless of the files existance or any of its folders.
   */
  void forEachFolderInFilePath(const std::string& path, std::function<bool (const std::string& path)> fn);

  /**
   * Performs the provided function on every folder in the path of a file.
   *
   * Iteration order is from the deepest folder to the root-most one.
   *
   * This function itself performs no file system operations (just string operations),
   * so it won't hit any exception itself regardless of the files existance or any of its folders.
   *
   * @param fn - return "false" to break the iteration, skipping the rest of the folders. "true" to continue.
   */
  void forEachFolderInFilePathDeepestFirst(const std::string& path, std::function<bool (const std::string& path)> fn);

  /**
   * Formats a string as a char array that will work properly as a parameter for libnx's filesystem functions
   * 
   * Use `get()` when passing it to a libnx function
   */
  std::unique_ptr<char[]> toPathBuffer(const std::string& path);
}