#pragma once

#include "borealis.hpp"


class TabHelp : public brls::Box {

private:
  BRLS_BIND(brls::Sidebar, topicList, "topic-list");

  brls::GenericEvent::Callback createRenderFn(std::function<brls::View* ()> buildViewFn);
  brls::View* toScrollable(brls::View* content);

public:
  explicit TabHelp();

  static brls::View* create();
};
