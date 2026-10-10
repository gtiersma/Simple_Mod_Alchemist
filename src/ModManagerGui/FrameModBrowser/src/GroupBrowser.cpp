//
// Derived from a file created by Adrien BLANCHET on 21/06/2020.
//

#include "GroupBrowser.h"
#include "ModBrowser.h"
#include "util.hpp"
#include "loading_dialog.hpp"

#include <StateAlchemist/controller.h>
#include <StateAlchemist/fs_manager.h>


using namespace brls::literals;

/**
 * This is using a side bar directly just because having a second tab frame was crashing.
 * TODO: That could've been something else at the time. Try again?
 */
GroupBrowser::GroupBrowser() {
  this->inflateFromXMLRes("xml/FrameModBrowser/group_browser.xml");

  std::vector<std::string> groups = controller.loadGroups(true);

  for (std::string& group : groups) {
    this->groupList->addItem(group, [this, group](View* view) {
      // Only trigger when the sidebar item gains focus
      if (!view->isFocused())
        return;
      
      // Remove the old group list before showing the new one
      // (if there is currently one shown).
      //
      // TODO: Would be a little safer if we were using an ID
      if (this->getChildren().size() == 2) {
        this->removeView(this->getChildren()[1]);
      }

      // Handle merging folders that could exist belonging to the same source/mods:
      controller.group = group;
      this->deduplicateSources([this, view]() {
        std::vector<ModSource> sources = gameBrowser.getModManager().setSources(controller.loadSources(true));
        this->_current_mod_browser_ = new ModBrowser(view);
        this->_current_mod_browser_->deduplicateMods(
          sources,
          [this]() { this->addView(this->_current_mod_browser_); }
        );
      });
    });
  }

  this->groupList->registerAction("Randomly Change Mods", brls::BUTTON_X, [this](brls::View* view) {
    Util::buildConfirmDialog(
      "Enable/disable all mods in \"" + controller.group + "\" at random?",
      "Changing mods in \"" + controller.group + "\".",
      [this](std::atomic<float>& progress) {
        controller.randomizeGroup(progress);
        this->_current_mod_browser_->refreshSelections();
      }
    )->open();
    return true;
  });
}

void GroupBrowser::deduplicateSources(std::function<void ()> completeFn) {
  std::vector<std::string> sourceNames = controller.loadSources(true);

  // Check for duplicate entries.
  bool hasDuplicates = false;
  const int sourceCount = sourceNames.size();
  for (int i = sourceCount - 1; i > 0; i--) {
    // Since entries are conveniently sorted, duplicates would be adjacent, so we only need to compare adjacent elements.
    if (sourceNames[i - 1] == sourceNames[i]) {
      sourceNames.erase(sourceNames.begin() + i);
      hasDuplicates = true;
    }
  }

  if (hasDuplicates) {
    LoadingDialog* loadingDialog = LoadingDialog::build();
    loadingDialog->setAction("Found mod folders that belong to the same mod. Combining them");
    loadingDialog->open();

    new std::thread([completeFn, loadingDialog]() {
      FsManager::deduplicateFolderNames(
        controller.getGroupPath(),
        loadingDialog->getAtomicProgress()
      );
      loadingDialog->close();
      completeFn();
    });
  } else {
    completeFn();
  }
}

GroupBrowser* GroupBrowser::create() {
  return new GroupBrowser();
}