#include "TabHelp.h"


TabHelp::TabHelp() {
  this->inflateFromXMLRes("xml/FrameGameBrowser/tab_help.xml");
}

brls::View* TabHelp::create() { return new TabHelp(); }