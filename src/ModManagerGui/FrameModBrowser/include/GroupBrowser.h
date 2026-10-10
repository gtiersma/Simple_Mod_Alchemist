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
     * Checks for mod sources in the group currently set in the controller that have duplicate folders,
     * merging them into a single folder.
     * 
     * @param completeFn The function to call after the sources have been loaded.
     */
    void deduplicateSources(std::function<void ()> completeFn);

    ModBrowser* _current_mod_browser_{nullptr};
};

#endif //SIMPLEMODMANAGER_GROUPBROWSER_H
