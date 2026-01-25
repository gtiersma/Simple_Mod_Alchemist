#include "NoGames.h"

#include <atomic>

#include <util.hpp>

#include <ModMigrator.h>
#include <GameBrowser.h>


NoGames::NoGames(brls::VoidEvent::Callback migrateCb): brls::Box(brls::Axis::COLUMN) {
  this->inflateFromXMLRes("xml/FrameGameBrowser/no_games.xml");

  this->migrateCb = migrateCb;

  this->migrateButton->registerClickAction([this](brls::View* view) {
    Util::buildConfirmDialog(
      "Migrate the mods from SimpleModManager?\n\n"\
      "This action cannot easily be undone.\n"\
      "Make sure to turn all mods off in the original SimpleModManager first before running this.",
      "Moving mods from the old SimpleModManager to this app.",
      [](std::atomic<float>& progress) { ModMigrator().begin(progress); },
      [this]() {
        gameBrowser.loadGames();
        buildMigrateFinishedDialog()->open();
      }
    )->open();
    return true;
  });

  this->topNote->setFocusable(false);
}

brls::Dialog* NoGames::buildMigrateFinishedDialog() {
  brls::Dialog* completeDialog = new brls::Dialog(
    "Finished moving mod files & folders.\n\n"\
    "The mods are ready to use, but are currently uncategorized. "\
    "On your PC, move mod folders out of \"uncategorized\" into new folders to organize them. "\
    "Any folders with files that couldn't be moved should have been left where they were."
  );
  completeDialog->addButton("OK", [this]() {
    this->setVisibility(brls::Visibility::GONE);
    this->migrateCb();
  });
  return completeDialog;
}