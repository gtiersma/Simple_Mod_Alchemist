#pragma once

#include <switch.h>
#include <switch/result.h>

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <atomic>


enum class ConflictStrategy {
  OVERWRITE_TARGET, // Delete target file, moving source file in its place
  PRESERVE_TARGET, // Leave target file; delete source file
  KEEP_BOTH // Leave both target & source file
};


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
   * (parsing the name from the folder name).
   *
   * While doing that, it also checks for the existence of folders that belong to the same entity, but have different names.
   * This is rare, but could happen when the user copies folder structures around.
   * Having multiple folders for the same entity could break things.
   * If duplicates are found, the folders get combined into one to prevent the issue.
   * 
   * @param sort Whether to sort the list of names alphabetically or not.
   *             Can take considerable performance when in nested loops, so sometimes it's good to skip if not needed.
   */
  std::vector<std::string> loadNames(const std::string& path, bool sort);

  /**
   * Combines any folders that are found with duplicate entity names under the same path.
   *
   * @param path The directory to look for duplicates directly within.
   *
   * @param progress Scale of 0.0-1.0 of the current deduplication progress.
   *                 Updated while the method runs.
   * 
   * @param percentageOfWhole If there is other work happening in other methods
                              (or this method is being called multiple times),
   *                          include the percentage of the total work that this method call makes up.
   *                          The method will only increase the progress by that percentage.
   *                          By default, it's expected that this is the only code to be progress-tracked,
   *                          so the progress param is at 0% and it will move forward to 100%.
   */
  void deduplicateFolderNames(const std::string& path, std::atomic<float>& progress, const float& percentageOfWhole = 1.0f);

  /**
   * Reads every directory entry of the specified path into the given vector.
   *
   * The filesystem can sometimes report a premature "end of directory" before all
   * entries have been returned (see hasFilesDeep). To work around that, the directory
   * is reopened with a fresh handle and re-read until no new entries are found or the
   * reported entry count is reached.
   *
   * @param path The folder to read
   *
   * @param mode FsDirOpenMode flags for how the folder should be opened
   *
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
   * Moves everything from within a folder into a different folder.
   *
   * @param fromPath     The folder path to move contents out of.
   * @param toPath       The folder path to move contents into.
   * @param overwrite    Action to take if file conflict occurs. See enum.
   *                     Default: OVERWRITE_TARGET
   * @param fileMoveFn   Optionally provide a callback to run every time a file is moved.
   *                       @subparam relativePath The base file path being moved (from the relative point of "toPath").
   *                       @subparam conflicts    True if the files conflicts with an already-existing file under "toPath".
   * @param folderMoveFn Optionally provide a callback to run every time a folder is moved.
   *                       @subparam relativePath The base folder path being moved (from the relative point of "toPath").
   * @param basePath     USED ONLY IN RECURSIVE CALLS. The base file path being moved (from the relative point of "toPath").
   */
  void moveContents(
    const std::string& fromPath,
    const std::string& toPath,
    ConflictStrategy conflictStrategy = ConflictStrategy::OVERWRITE_TARGET,
    std::function<void (const std::string& relativePath, bool conflicts)> fileMoveFn = [](const std::string&, bool conflicts) {},
    std::function<void (const std::string& relativePath)> folderMoveFn = [](const std::string&) {},
    const std::string& basePath = ""
  );

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