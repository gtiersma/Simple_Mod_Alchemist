#include "help/Backups.h"


Backups::Backups(): brls::Box(brls::Axis::COLUMN) {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/backups.xml");
}