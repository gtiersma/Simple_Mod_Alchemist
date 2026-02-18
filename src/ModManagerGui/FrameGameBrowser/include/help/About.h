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

  /**
   * These make up a 2x2 grid in the middle of the document.
   */
  brls::View* buildTopLeftBox();
  brls::View* buildTopRightBox();
  brls::View* buildBottomLeftBox();
  brls::View* buildBottomRightBox();

  /**
   * Essentially gives the view a brls::Header, spacing it properly, contained in a brls::Box
   */
  brls::Box* wrapWithHeader(brls::View* content, std::string header);
};
