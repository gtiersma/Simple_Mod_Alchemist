//
// Created by Adrien BLANCHET on 21/06/2020.
//

#include "TabGames.h"
#include "FrameModBrowser.h"

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
  std::vector<Game> gameList = gameBrowser.getGameList();

  if (gameList.empty()) {
    brls::NoteCell* message = new brls::NoteCell();
    message->setText("No game folders found. Folders should be like this:");
    message->setNote(
      "SD:/mod-alchemy/<title-id-of-the-game>/<group>/<thing-being-replaced>/<mod-name>/<mod-files-and-folders>"
    );
    Util::padContent(message);
    this->addView(message);
  } else {
     brls::Box* container = new brls::Box(brls::Axis::COLUMN);
     Util::padContent(container);
     this->addView(container);

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
    for (auto& game : _gameItems_) { container->addView( game.item ); }
  }
}

brls::IconCell* TabGames::buildGameCell(const Game& game) {
  brls::IconCell* item = new brls::IconCell();

  item->setText(game.name);
  if (game.icon.size() > 0) {
    item->setIconFromMem(game.icon.data(), 0x20000);
  }

  item->registerClickAction([game](brls::View* view) {
    gameBrowser.selectGame(game.titleId);

    // Let the user know if there's no mods:
    // TODO: Groups are loaded only to check if they exist here. Not efficient.
    std::vector<std::string> groups = controller.loadGroups(false);
    if (groups.empty()) {
      brls::Dialog* dialog = new brls::Dialog(
        "No mod group folders exist in \"" + controller.getGamePath() +
        "\". Within that folder, organize the mods in this manner: ./<group>/<thing-being-replaced>/<mod-name>/<mod-file-structure>"
      );
      dialog->setCancelable(true);
      dialog->addButton("OK", []() {});
      dialog->open();
      return true;
    }

    FrameModBrowser* modsBrowser = new FrameModBrowser();
    brls::Application::pushActivity(modsBrowser);
    modsBrowser->initialize();
    return true;
  });

  item->registerAction("Randomly Change Game's Mods", brls::BUTTON_X, [game](brls::View* view) {
    gameBrowser.selectGame(game.titleId);
    Util::buildConfirmDialog(
      "Enable/disable mods for " + game.name + " at random?",
      "Changing mods.",
      [](std::atomic<float>& progress) { controller.randomizeGame(progress); }
    )->open();
    return true;
  });

  item->updateActionHint(brls::BUTTON_A, "View Game's Mods");

  return item;
}

brls::View* TabGames::create() { return new TabGames(); }
