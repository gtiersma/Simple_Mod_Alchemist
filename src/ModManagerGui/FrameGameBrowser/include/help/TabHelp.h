#pragma once

#include "borealis.hpp"


class TabHelp : public brls::Box {

private:
  BRLS_BIND(brls::Sidebar, topicList, "topic-list");
  BRLS_BIND(brls::ScrollingFrame, scrollFrame, "scroll-frame");

public:
  explicit TabHelp();

  static brls::View* create();
};
