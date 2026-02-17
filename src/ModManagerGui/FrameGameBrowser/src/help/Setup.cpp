#include "help/Setup.h"


Setup::Setup(): brls::Box(brls::Axis::COLUMN) {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/setup.xml");
}

brls::View* Setup::create() { return new Setup(); }