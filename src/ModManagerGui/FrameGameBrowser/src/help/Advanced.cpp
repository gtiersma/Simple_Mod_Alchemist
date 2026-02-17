#include "help/Advanced.h"


Advanced::Advanced(): brls::Box(brls::Axis::COLUMN) {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/advanced.xml");
}

brls::View* Advanced::create() { return new Advanced(); }