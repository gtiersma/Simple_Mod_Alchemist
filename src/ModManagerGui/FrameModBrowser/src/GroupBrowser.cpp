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

      controller.group = group;
      this->loadSources([this, view](const std::vector<std::string>& sources) => {
        gameBrowser.getModManager().setSources(sources);
        this->_current_mod_browser_ = new ModBrowser(view);
        this->addView(this->_current_mod_browser_);
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

void GroupBrowser::loadSources(std::function<void (const std::vector<std::string>& sources)> fn) {
  std::vector<std::string> sources = controller.loadSources(true);

  // Check for duplicate entries.
  // Since entries are conveniently sorted, duplicates would be adjacent, so we only need to compare adjacent elements.
  bool hasDuplicates = false;
  const int sourceCount = sources.size();
  for (int i = sourceCount - 1; i > 0; i--) {
    if (sources[i - 1] == sources[i]) {
      sources.erase(i);
      hasDuplicates = true;
    }
  }

  if (hasDuplicates) {
    LoadingDialog* loadingDialog = LoadingDialog::build();
    loadingDialog->setAction("Found mod folders that belong to the same mod. Combining them");
    loadingDialog->open();
    
    new std::thread([sources, fn, loadingDialog]() => {
      FsManager::deduplicateFolderNames(controller.getGroupPath(), loadingDialog->getAtomicProgress());
      loadingDialog->close();
      fn(sources);
    });
  }
}

GroupBrowser* GroupBrowser::create() {
  return new GroupBrowser();
}