//
// Created by Adrien BLANCHET on 21/06/2020.
//

#include <TabGeneralSettings.h>
#include <ModMigrator.h>

#include "FrameRoot.h"

#include <util.hpp>
#include <note_cell.hpp>
#include <GameBrowser.h>

using namespace brls::literals;

TabGeneralSettings::TabGeneralSettings() {
  this->rebuildLayout();
}

void TabGeneralSettings::rebuildLayout() {
  Util::padTabContent(this);

  brls::NoteCell* migrationItem = new brls::NoteCell();
  migrationItem->setText("Bring over old SimpleModManager mods");
  migrationItem->setNote(
    "This will take any mods on the SD card that were set up for the original SimpleModManager to work with this manager."
  );
  migrationItem->registerClickAction([this](brls::View* view) {
    Util::buildConfirmDialog(
      "Migrate the mods from SimpleModManager?\n\n"\
      "This action cannot easily be undone.\n"\
      "Turn all the mods off in SimpleModManager first to clear them out before running this.",
      "Moving mods from the old SimpleModManager to this app.",
      [](std::atomic<float>& progress) { ModMigrator().begin(progress); },
      [this]() {
        gameBrowser.loadGames();
        buildMigrateFinishedDialog()->open();
      }
    )->open();
    return true;
  });
  migrationItem->updateActionHint(brls::BUTTON_A, "Move Mods");
  this->addView(migrationItem);
}

/**
 * Builds a dialog to show for after the migration finishes
 */
brls::Dialog* TabGeneralSettings::buildMigrateFinishedDialog() {
  brls::Dialog* completeDialog = new brls::Dialog(
    "Finished moving mod files & folders.\n\n"\
    "The mods have been grouped in an \"uncategorized\" folder. "\
    "It's recommended to reorganize them into group folders on your pc to make them easy to use, but you don't have to. "\
    "Some empty folders with any files that couldn't be moved have been left where they were."
  );
  completeDialog->addButton("OK", []() {});
  return completeDialog;
}

brls::View* TabGeneralSettings::create() { return new TabGeneralSettings(); }
