//
// Created by Adrien BLANCHET on 21/06/2020.
//

#include <TabGeneralSettings.h>
#include <ModMigrator.h>

#include "FrameRoot.h"

#include <util.hpp>
#include <note_cell.hpp>
#include <GameBrowser.h>

#include <borealis/views/cells/cell_bool.hpp>

using namespace brls::literals;

TabGeneralSettings::TabGeneralSettings()
    : brls::Box(brls::Axis::COLUMN) {
  this->rebuildLayout();
}

void TabGeneralSettings::rebuildLayout() {
  Util::padContent(this);

  brls::SelectorCell* fileListCell = new brls::SelectorCell();
  fileListCell->init(
    "Show file list after enabling mods",
    {"Never", "Always", "Only on File Conflict"},
    static_cast<int>(gameBrowser.getConfigHandler().getConfig().showFileListPopup),
    [](int selected) {},
    [](int selected) {
      gameBrowser.getConfigHandler().getConfig().showFileListPopup = static_cast<FileMoveReporting>(selected);
      gameBrowser.getConfigHandler().dumpConfigToFile();
    }
  );
  this->addView(fileListCell);

  // TODO: Quite a bit of duplicate code from NoGames.cpp
  brls::NoteCell* migrationItem = new brls::NoteCell();
  migrationItem->setText("Bring over old SimpleModManager mods");
  migrationItem->setNote(
    "This will automatically move all mods on the SD card that were set up for the original SimpleModManager to work with this manager."
  );
  migrationItem->registerClickAction([this](brls::View* view) {
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
  this->addView(migrationItem);
}

/**
 * Builds a dialog to show for after the migration finishes
 */
brls::Dialog* TabGeneralSettings::buildMigrateFinishedDialog() {
  brls::Dialog* completeDialog = new brls::Dialog(
    "Finished moving mod files & folders.\n\n"\
    "The mods are ready to use, but are uncategorized. "\
    "Create folders for groups in \"SD:/mod-alchemy/[ game title ID ]\", and move mod folders out of \"_Uncategorized\" into the new folders to organize them. "\
    "Any files that couldn't be moved have been left where they were."
  );
  completeDialog->addButton("OK", []() {});
  return completeDialog;
}

brls::View* TabGeneralSettings::create() { return new TabGeneralSettings(); }
