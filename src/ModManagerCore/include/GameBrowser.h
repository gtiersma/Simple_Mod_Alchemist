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

  /**
   * Gets the sources for the current group (the group set in controller).
   *
   * Sources are loaded async, so a callback is used instead of a return value.
   *
   * While loading, checks for mod folders that belong to the same thing,
   * merging them into one folder.
   * Duplicates like these are rare, often related to problems of the user copying around folders,
   * but they can break things, so this function will fix them.
   */
  void loadSources(std::function<void (const std::vector<std::string>& sources)> fn);
};

extern GameBrowser gameBrowser;

#endif //SWITCHTEMPLATE_BROWSER_H
