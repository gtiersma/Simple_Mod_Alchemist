//
// Created by Adrien BLANCHET on 22/06/2020.
//

#include "TabModOptions.h"
#include "FrameModBrowser.h"

#include <StateAlchemist/controller.h>
#include <note_cell.hpp>
#include <util.hpp>


using namespace brls::literals;

TabModOptions::TabModOptions(): brls::Box(brls::Axis::COLUMN) {
  Util::padTabContent(this);
  this->buildRandomPicks();
  this->buildDisableAllMods();
}

void TabModOptions::buildRandomPicks() {
  brls::DetailCell* randomPicks = new brls::DetailCell();
  randomPicks->setText("Randomly change all mods");

  randomPicks->registerClickAction([](brls::View* view) {
    Util::buildConfirmDialog(
      "Enable/disable mods for this game at random?",
      "Changing mods.",
      [](std::atomic<float>& progress) { controller.randomizeGame(); }
    )->open();
    return true;
  });

  randomPicks->updateActionHint(brls::BUTTON_A, "Random Mods");

  this->addView(randomPicks);
}

void TabModOptions::buildDisableAllMods() {
  brls::NoteCell* disableAll = new brls::NoteCell();
  disableAll->setText("Disable all mods");
  disableAll->setNote(
    "Turn all mods off for this game, returning all files to under the \"" + ALCHEMIST_FOLDER + "\" folder. "\
    "This is useful if you want to delete some of them from the SD card."
  );

  disableAll->registerClickAction([](brls::View* view) {
    Util::buildConfirmDialog(
      "Disable all mods?",
      "Disabling all mods.",
      [](std::atomic<float>& progress) { controller.deactivateAll(); }
    )->open();
    return true;
  });

  disableAll->updateActionHint(brls::BUTTON_A, "Disable Mods");

  this->addView(disableAll);
}

TabModOptions* TabModOptions::create() {
  return new TabModOptions();
}


