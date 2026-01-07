//
// Created by Adrien BLANCHET on 21/06/2020.
//

#include "ModBrowser.h"
#include "FrameModBrowser.h"
#include "RandomSettings.h"

#include <StateAlchemist/controller.h>
#include <StateAlchemist/meta_manager.h>
#include <note_cell.hpp>


using namespace brls::literals;

ModDataSource::ModDataSource(
  std::function<void (brls::SelectorCell* selector, ModSource& mod, const int& index)> selector_config_fn
) {
  this->_selector_config_fn_ = selector_config_fn;
}

int ModDataSource::getModSourceCount() {
  return gameBrowser.getModManager().getSourceCount();
}

bool ModDataSource::hasModSources() {
  return this->getModSourceCount() > 0;
}

int ModDataSource::numberOfRows(brls::RecyclerFrame* recycler, int section) {
  if (this->hasModSources()) {
    return this->getModSourceCount();
  }

  // For the no-mods message:
  return 1;
}

brls::RecyclerCell* ModDataSource::cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath indexPath)
{
  ModManager& modManager = gameBrowser.getModManager();

  if (!this->hasModSources()) {

    // CASE: No mod sources in this group
    brls::NoteCell* item = (brls::NoteCell*)recycler->dequeueReusableCell("Note");
    item->setText("No mods are in the " + controller.group + " folder.");
    item->setNote(
      "Within that folder, organize the mods in this manner: ./<thing-being-replaced>/<mod-name>/<file-structure-in-installed-directory>"
    );
    item->setFocusable(false);
    return item;

  } else if (modManager.isSourceLoaded(indexPath.row)) {
    
    // CASE: Selector for mods
    brls::SelectorCell* item = (brls::SelectorCell*)recycler->dequeueReusableCell("Selector");
    ModSource& source = modManager.getSource(indexPath.row);
    this->_selector_config_fn_(item, source, indexPath.row);
    return item;

  }

  // CASE: Selectors for mods that aren't yet loaded.
  // These should technically never be seen in the UI, but they're here for safety in case something is off.
  brls::SelectorCell* item = (brls::SelectorCell*)recycler->dequeueReusableCell("Selector");
  item->init(modManager.getSourceName(indexPath.row), { "UNKNOWN" }, 0, [](int selected) {}, [](int selected) {});
  item->updateActionHint(brls::BUTTON_A, "ERROR");
  item->updateActionHint(brls::BUTTON_B, "To Group List");
  return item;
}

ModBrowser::ModBrowser(brls::View* parentCell): _parent_cell_(parentCell) {
  this->inflateFromXMLRes("xml/FrameModBrowser/mod_browser.xml");

  // This is just a random number I tossed here that sounded right,
  // and it seems to be working.
  // TODO: Is this really the right number though?
  modList->estimatedRowHeight = 70;

  modList->registerCell("Selector", []() { return new brls::SelectorCell(); });
  modList->registerCell("Note", []() { return new brls::NoteCell(); });

  modList->setDataSource(
    new ModDataSource(
      [this](brls::SelectorCell* selector, ModSource& mod, const int& index) {
        this->configureModSelector(selector, mod, index);
      }
    )
  );
}

void ModBrowser::refreshSelections() {
  gameBrowser.getModManager().refreshActiveIndices();
  modList->reloadData();
}

void ModBrowser::handleModSelect(ModSource& mod, size_t selectedIndex) {

  // Note: selection is -1 if backed out of selecting
  if (selectedIndex == -1) return;

  controller.source = mod.getSource();

  if (selectedIndex == 0) {
    // If the default option was chosen, deactivate whatever mod is active:
    controller.deactivateMod();
  } else if (mod.getActiveIndex() != selectedIndex - 1) {
    // If the mod was changed, deactivate the old one and activate the new one:
    controller.deactivateMod();
    // mod.mods doesn't have the default option at the begining, so index must be offset by -1:
    controller.activateMod(mod.getMods()[selectedIndex - 1]);
  }
}

void ModBrowser::configureModSelector(brls::SelectorCell* selector, ModSource& mod, const int& index) {
  selector->init(
    mod.getSource(),
    mod.getOptions(),
    mod.getActiveIndex() + 1, // Add 1 for the no-mod option added to the beginning
    [](int selected) {},
    [this, &mod](int selected) {
      this->handleModSelect(mod, selected);
    }
  );

  // If focus is drawing near to the point where we don't have data loaded
  // for the mod sources that will come into view soon, we need to load the next chunk:
  selector->getFocusEvent()->subscribe([index](brls::View* view) {
    gameBrowser.getModManager().loadSourcesIfNeeded(index);
  });

  selector->registerAction("Back", brls::BUTTON_B, [this](brls::View* view) {
    brls::Application::giveFocus(this->_parent_cell_);
    return true;
  });

  selector->updateActionHint(brls::BUTTON_A, "Change");

  selector->registerAction("Random", brls::BUTTON_X, [this, &mod](brls::View* view) {
    brls::SelectorCell* cell = dynamic_cast<brls::SelectorCell*>(view);

    controller.source = mod.getSource();
    controller.randomizeSource();

    // Update new active mod in both the object and the UI:
    int activeIndex = gameBrowser.getModManager().getActiveIndex(mod.getSource(), mod.getMods());
    mod.setActiveIndex(activeIndex);
    cell->setSelection(activeIndex + 1);

    return true;
  });

  selector->registerAction("Random Settings", brls::BUTTON_Y, [this, &mod](brls::View* view) {
    controller.source = mod.getSource();
    RandomSettings::showInDialog();
    return true;
  });
}