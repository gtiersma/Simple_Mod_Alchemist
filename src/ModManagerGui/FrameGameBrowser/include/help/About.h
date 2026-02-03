//
// Created by Adrien BLANCHET on 20/06/2020.
//

#pragma once

#include <borealis.hpp>

class About : public brls::Box {

public:
  About();
  static brls::View* create();

private:
  brls::View* buildLeftBox();
  brls::View* buildRightBox();

  /**
   * Essentially gives the view a brls::Header, spacing it properly, contained in a brls::Box
   */
  brls::Box* wrapWithHeader(brls::View* content, std::string header);
};
