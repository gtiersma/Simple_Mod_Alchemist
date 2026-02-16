#include "help/Manage.h"


Manage::Manage(): brls::Box(brls::Axis::COLUMN) {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/manage.xml");
}