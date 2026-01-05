#pragma once

#include "StateAlchemist/fs_manager.h"

#include <switch.h>

/**
 * Simple quick txt logger I tossed together for testing. Not to be used in any release.
 */
class AlchemistLogger {

private:
  s64 offset;
  std::string rawPath;
  std::unique_ptr<char[]> path;
  FsFile file;

public:
    AlchemistLogger() = default;
    ~AlchemistLogger();

    void init();

    void log(const std::string& message);
};

extern AlchemistLogger alchemyLogger;
