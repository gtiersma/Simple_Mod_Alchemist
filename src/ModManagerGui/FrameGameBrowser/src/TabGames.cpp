//
// Created by Adrien BLANCHET on 21/06/2020.
//

#include "TabGames.h"
#include "FrameModBrowser.h"
#include "NoGames.h"

#include "GenericToolbox.Switch.h"
#include "GenericToolbox.Vector.h"

#include <StateAlchemist/constants.h>
#include <StateAlchemist/meta_manager.h>
#include <StateAlchemist/controller.h>
#include <Game.h>
#include <util.hpp>
#include <note_cell.hpp>

#include <atomic>

using namespace brls::literals;

TabGames::TabGames() {
  this->container = new brls::Box(brls::Axis::COLUMN);
  Util::padContent(this->container);
  this->addView(this->container);

  if (gameBrowser.getGameList().empty()) {
    this->addView(new NoGames());
  } else {
    this->load();
  }
}

void TabGames::load() {
  std::vector<Game> gameList = gameBrowser.getGameList();
  _gameItems_.reserve(gameList.size());

  for(auto& gameEntry : gameList) {
    _gameItems_.emplace_back();
    _gameItems_.back().title = gameEntry.name;
    _gameItems_.back().item = this->buildGameCell(gameEntry);
  }

  GenericToolbox::sortVector(_gameItems_, [](const GameItem& a_, const GameItem& b_){
    return GenericToolbox::toLowerCase(a_.title) < GenericToolbox::toLowerCase(b_.title);
  });
  
  // add to the view
  for (auto& game : _gameItems_) { this->container->addView(game.item); }
}

brls::IconCell* TabGames::buildGameCell(const Game& game) {
  brls::IconCell* item = new brls::IconCell();

  item->setText(game.name);
  if (game.icon.size() > 0) {
    item->setIconFromMem(game.icon.data(), 0x20000);
  }

  item->registerClickAction([game](brls::View* view) {
    gameBrowser.selectGame(game);

    // Let the user know if there's no mods:
    // TODO: Groups are loaded only to check if they exist here. Not efficient.
    std::vector<std::string> groups = controller.loadGroups(false);
    if (groups.empty()) {
      brls::Dialog* dialog = new brls::Dialog(
        "No mod group folders exist in \"" + controller.getGamePath() +
        "\". Within that folder, organize the mods in this manner: ./<group>/<thing-being-replaced>/<mod-name>/<mod-file-structure>"
      );
      dialog->addButton("OK", []() {});
      dialog->open();
      return true;
    }

    FrameModBrowser* modsBrowser = new FrameModBrowser();
    brls::Application::pushActivity(modsBrowser);
    modsBrowser->initialize();
    return true;
  });

  item->registerAction("Randomly Change Mods", brls::BUTTON_X, [game](brls::View* view) {
    gameBrowser.selectGame(game);
    Util::buildConfirmDialog(
      "Enable/disable mods for " + game.name + " at random?",
      "Changing mods.",
      [](std::atomic<float>& progress) { controller.randomizeGame(progress); }
    )->open();
    return true;
  });

  item->updateActionHint(brls::BUTTON_A, "View Mods");

  return item;
}

brls::View* TabGames::create() { return new TabGames(); }
