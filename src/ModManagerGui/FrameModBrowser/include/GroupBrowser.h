//
// Derived from a file created by Adrien BLANCHET on 21/06/2020.
//

#ifndef SIMPLEMODMANAGER_GROUPBROWSER_H
#define SIMPLEMODMANAGER_GROUPBROWSER_H


#include "borealis.hpp"

#include "ModBrowser.h"

#include <functional>
#include <string>
#include <vector>


class GroupBrowser : public brls::Box {
  public:
    explicit GroupBrowser();

    static GroupBrowser* create();

  private:
    BRLS_BIND(brls::Sidebar, groupList, "group-list");

    /**
     * Loads the list of mod source names for the current group set in the controller.
     *
     * After loading them, it also checks for sources in the group that have duplicate folders,
     * merging them into a single folder.
     * 
     * @param completeFn The function to call after the sources have been loaded.
     */
    void loadSources(std::function<void ()> completeFn);

    ModBrowser* _current_mod_browser_{nullptr};
};

#endif //SIMPLEMODMANAGER_GROUPBROWSER_H
