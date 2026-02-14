#include "NoGames.h"

#include <atomic>

#include <switch.h>

#include <util.hpp>

#include <GameBrowser.h>
#include <ModMigrator.h>
#include <FrameRoot.h>

#include "StateAlchemist/constants.h"


NoGames::NoGames(): brls::Box(brls::Axis::COLUMN) {
  this->inflateFromXMLRes("xml/FrameGameBrowser/no_games.xml");

  this->migrateButton->registerClickAction([this](brls::View* view) {
    Util::buildConfirmDialog(
      "Migrate the mods from SimpleModManager?\n\n"\
      "This action cannot easily be undone.\n"\
      "Make sure to turn all mods off in the original SimpleModManager first before running this.",
      "Moving mods from the old SimpleModManager to this app.",
      [](std::atomic<float>& progress) { ModMigrator().begin(progress); },
      [this]() {
        gameBrowser.loadGames();

        if (gameBrowser.getGameList().empty()) {
          brls::Dialog* dialog = new brls::Dialog("No game mods were found to transfer.");
          dialog->addButton("OK", [](){});
          dialog->open();
        } else {
          buildMigrateFinishedDialog()->open();
        }
      }
    )->open();
    return true;
  });

  this->topNote->setFocusable(false);
  this->bottomNote->setFocusable(false);
}

brls::Dialog* NoGames::buildMigrateFinishedDialog() {
  brls::Dialog* completeDialog = new brls::Dialog(
    "Finished moving mod files & folders.\n\n"\
    "The mods are ready to use, but are uncategorized. "\
    "Create folders for groups in \"SD:/mod-alchemy/[ game title ID ]\", and move mod folders out of \"_Uncategorized\" into the new folders to organize them. "\
    "Any files that couldn't be moved have been left where they were."
  );

  completeDialog->addButton("OK", []() {

    // TODO: Just refresh the game list. That was causing the app to crash though, so right now we're just working around it by pushing another main activity.
    brls::Activity* mainActivity = new FrameRoot();
    brls::Application::pushActivity(mainActivity);
    mainActivity->registerExitAction(brls::BUTTON_B);
    brls::AppletFrame* appFrame = (brls::AppletFrame*)mainActivity->getContentView();
    appFrame->setTitle("Simple Mod Alchemist (v" + APP_VERSION + ")");
  });

  return completeDialog;
}