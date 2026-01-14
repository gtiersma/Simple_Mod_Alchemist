//
// Created by Adrien BLANCHET on 21/06/2020.
//

#ifndef SIMPLEMODMANAGER_MODBROWSER_H
#define SIMPLEMODMANAGER_MODBROWSER_H


#include <GuiModManager.h>
#include "ModManager.h"

#include <borealis.hpp>

#include <vector>
#include <string>

/**
 * Used to provide the virtual-scrolling-esque recycler list
 * the data it needs as scrolling occurs
 */
class ModDataSource : public brls::RecyclerDataSource
{
  public:
    ModDataSource(std::function<void (brls::SelectorCell* selector, ModSource& mod, const int& index)> selector_config_fn);

    int numberOfRows(brls::RecyclerFrame* recycler, int section) override;
    brls::RecyclerCell* cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath index) override;

  private:
    // Configures a selector cell for the specified mod source that located at the specified index in the list
    std::function<void (brls::SelectorCell* selector, ModSource& mod, const int& index)> _selector_config_fn_{nullptr};

    int getModSourceCount();
    bool hasModSources();
};

class ModBrowser : public brls::Box {

public:
  explicit ModBrowser(brls::View* parentCell);

  void refreshSelections();

private:
  BRLS_BIND(brls::RecyclerFrame, modList, "mod-list");

 /**
  * The cell from the group list parent that this list belongs to
  */
  brls::View* _parent_cell_{nullptr};

  /**
   * Called when the value of one of the select items is changed
   *
   * @param mod The mod source that was changed
   * @param selectedIndex The index of the mod the user changed to
   */
  void handleModSelect(const ModSource& mod, size_t selectedIndex);

  /**
   * Configures a single selector cell in the mod list
   *
   * @param selector The newly created cell to configure
   * @param mod The mod data for the cell
   * @param index The index of the mod source in the cell list
   */
  void configureModSelector(brls::SelectorCell* selector, ModSource& mod, const int& index);
};


#endif //SIMPLEMODMANAGER_MODBROWSER_H
