//
// Created by Adrien BLANCHET on 20/06/2020.
//

#ifndef SIMPLEMODMANAGER_TABABOUT_H
#define SIMPLEMODMANAGER_TABABOUT_H

#include <borealis.hpp>

class TabAbout : public brls::Box {

public:
  TabAbout();
  static brls::View* create();

private:
  brls::View* buildLeftBox();
  brls::View* buildRightBox();

  /**
   * Essentially gives the view a brls::Header, spacing it properly, contained in a brls::Box
   */
  brls::Box* wrapWithHeader(brls::View* content, std::string header);
};


#endif //SIMPLEMODMANAGER_TABABOUT_H
