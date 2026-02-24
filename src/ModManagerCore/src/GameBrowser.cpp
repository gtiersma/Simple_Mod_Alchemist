//
// Created by Nadrino on 03/09/2019.
//

#include <GameBrowser.h>

#include <switch.h>

#include <StateAlchemist/controller.h>
#include <StateAlchemist/fs_manager.h>
#include <StateAlchemist/meta_manager.h>
#include <StateAlchemist/constants.h>

GameBrowser gameBrowser;

GameBrowser::GameBrowser(){}

// getters
const ConfigHandler &GameBrowser::getConfigHandler() const {
  return _configHandler_;
}
ModManager &GameBrowser::getModManager(){
  return _modManager_;
}
ConfigHandler &GameBrowser::getConfigHandler(){
  return _configHandler_;
}
ModsPresetHandler &GameBrowser::getModPresetHandler(){
  return _modPresetHandler_;
}
std::vector<Game> &GameBrowser::getGameList(){
  return _gameList_;
}

std::optional<Game> GameBrowser::getGame(const u64 &titleId_) {
  for (auto& game : _gameList_) {
    if (game.titleId == titleId_) {
      return game;
    }
  }
  
  return std::nullopt;
}

std::string GameBrowser::getOrCreateGamePath(const std::string& titleId) {
  std::string path = ALCHEMIST_PATH + "/";

  std::optional<Game> game = getGame(MetaManager::getNumericTitleId(titleId));

  if (game == std::nullopt) {
    path = path + titleId;
    FsManager::createFolderIfNeeded(path);
  } else {
    path = path + game.value().folderName;
  }

  return path;
}

// Browse
void GameBrowser::selectGame(const Game& game) {
  controller.setTitleId(game.titleId);
  controller.setGameFolderName(game.folderName);
}

// protected
void GameBrowser::loadGames() {
  _gameList_.clear();

  auto folderList = GenericToolbox::lsDirs(ALCHEMIST_PATH);
  
  // Filter out any folders that are definitely no Switch Title ID:
  for (auto& folder : folderList) {
    if (MetaManager::hasTitleId(folder)) {
      u64 titleId = MetaManager::parseTitleId(folder);
      auto game = std::make_unique<Game>(titleId, folder);

      // Load the icon for the game:
      u64 gameDataSize {};
      auto gameData = std::make_unique<NsApplicationControlData>();
      if (
        R_SUCCEEDED(
          nsGetApplicationControlData(
            NsApplicationControlSource_Storage,
            titleId,
            gameData.get(),
            sizeof(NsApplicationControlData),
            &gameDataSize
          )
        )
      ) {
        const auto iconSize = gameDataSize - sizeof(NacpStruct);
        game->icon.resize(iconSize);
        std::memcpy(game->icon.data(), gameData->icon, game->icon.size());
      }

      // Load the title of the game:
      NacpLanguageEntry* nameData;
      if (R_SUCCEEDED(nsGetApplicationDesiredLanguage(&gameData->nacp, &nameData))) {
        game->name = nameData->name;
      }

      // If the folder's name is just the title ID, rename it so it has the game name in it too (for user conveniency):
      if (folder.size() == 16) {
        game->folderName = ALCHEMIST_PATH + "/" + game->name + " (" + folder + ")";
        fsFsRenameDirectory(
          &FsManager::sdSystem,
          FsManager::toPathBuffer(ALCHEMIST_PATH + "/" + folder).get(),
          FsManager::toPathBuffer(game->folderName).get()
        );
      }

      _gameList_.push_back(std::move(*game));
    }
  }
}

