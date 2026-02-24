//
// Created by Nadrino on 03/09/2019.
//

#ifndef SWITCHTEMPLATE_BROWSER_H
#define SWITCHTEMPLATE_BROWSER_H

#include <ModManager.h>
#include <ConfigHandler.h>
#include <ModsPresetHandler.h>
#include <Game.h>

#include <switch.h>

#include <vector>
#include <optional>


class GameBrowser{

public:
  GameBrowser();
  void loadGames();

  // getters
  const ConfigHandler &getConfigHandler() const;
  ModManager &getModManager();
  ModsPresetHandler &getModPresetHandler();
  ConfigHandler &getConfigHandler();
  std::vector<Game> &getGameList();

  std::optional<Game> getGame(const u64 &titleId_);

  /**
   * Gets the path to a game's folder by a string title ID,
   * creating the folder for the title ID if one doesn't already exist.
   */
  std::string getOrCreateGamePath(const std::string& titleId);

  // browse
  void selectGame(const Game& game);

private:
  ModManager _modManager_{this};
  ConfigHandler _configHandler_;
  ModsPresetHandler _modPresetHandler_;

  std::vector<Game> _gameList_;

};

extern GameBrowser gameBrowser;

#endif //SWITCHTEMPLATE_BROWSER_H
