#include "help/Faqs.h"


Faqs::Faqs(): brls::Box(brls::Axis::COLUMN) {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/faqs.xml");
}