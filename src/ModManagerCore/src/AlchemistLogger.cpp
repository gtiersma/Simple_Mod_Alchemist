#include "AlchemistLogger.h"

AlchemistLogger alchemyLogger;

void AlchemistLogger::init() {
  rawPath = "/The Log of the Alchemist.txt";
  path = FsManager::toPathBuffer(rawPath);

  if (FsManager::doesFileExist(rawPath)) {
    fsFsDeleteFile(&FsManager::sdSystem, path.get());
  }

  fsFsCreateFile(&FsManager::sdSystem, path.get(), 0, 0);

  fsFsOpenFile(&FsManager::sdSystem, path.get(), FsOpenMode_Write | FsOpenMode_Append, &file);
}
  
AlchemistLogger::~AlchemistLogger() {
  fsFileClose(&file);
}

void AlchemistLogger::log(const std::string& message) {
  std::string linedMessage = message + "\n";
  fsFileWrite(&file, offset, linedMessage.c_str(), linedMessage.size(), FsWriteOption_Flush);
  offset += linedMessage.size();
}