#include "help/Tldr.h"


Tldr::Tldr(): brls::Box(brls::Axis::COLUMN) {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/tldr.xml");
}