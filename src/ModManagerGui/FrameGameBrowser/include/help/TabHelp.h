#pragma once

#include "borealis.hpp"


class TabHelp : public brls::Box {

private:
  BRLS_BIND(brls::Sidebar, topicList, "topic-list");
  BRLS_BIND(brls::ScrollingFrame, scrollFrame, "scroll-frame");

  void handleTopicChange(brls::View* tab, std::function<brls::View* (void)> createContentFn);

public:
  explicit TabHelp();

  static brls::View* create();
};
