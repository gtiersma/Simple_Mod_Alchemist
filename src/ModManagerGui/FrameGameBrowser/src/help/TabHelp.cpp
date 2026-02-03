#include "help/TabHelp.h"

#include "help/About.h"


TabHelp::TabHelp() {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/tab_help.xml");

  this->topicList->addItem("About", [this](brls::View* view) {
    // Only trigger when the sidebar item gains focus
    if (view->isFocused()) {
      this->scrollFrame->addView(new About());
    }
  });
}

brls::View* TabHelp::create() { return new TabHelp(); }