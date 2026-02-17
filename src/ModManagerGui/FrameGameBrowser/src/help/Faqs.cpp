#include "help/Faqs.h"


Faqs::Faqs(): brls::Box(brls::Axis::COLUMN) {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/faqs.xml");
}

brls::View* Faqs::create() { return new Faqs(); }