#include "help/TabHelp.h"

#include "help/Tldr.h"
#include "help/Setup.h"
#include "help/Backups.h"
#include "help/Manage.h"
#include "help/Faqs.h"
#include "help/Advanced.h"
#include "help/About.h"


TabHelp::TabHelp() {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/tab_help.xml");

  this->topicList->addItem("TL;DR", [this](brls::View* view) {
    this->handleTopicChange(view, Tldr::create);
  });

  this->topicList->addSeparator();

  this->topicList->addItem("Setup", [this](brls::View* view) {
    this->handleTopicChange(view, Setup::create);
  });

  this->topicList->addItem("Backups", [this](brls::View* view) {
    this->handleTopicChange(view, Backups::create);
  });

  this->topicList->addItem("General", [this](brls::View* view) {
    this->handleTopicChange(view, Manage::create);
  });

  this->topicList->addItem("FAQs", [this](brls::View* view) {
    this->handleTopicChange(view, Faqs::create);
  });

  this->topicList->addItem("Advanced", [this](brls::View* view) {
    this->handleTopicChange(view, Advanced::create);
  });

  this->topicList->addSeparator();

  this->topicList->addItem("About", [this](brls::View* view) {
    this->handleTopicChange(view, About::create);
  });

  this->scrollFrame->registerAction("Back", brls::BUTTON_B, [this](brls::View* view) {
    brls::Application::giveFocus(this->topicList);
    return true;
  });
}

void TabHelp::handleTopicChange(brls::View* tab, std::function<brls::View* (void)> createContentFn) {
  // Only trigger when the sidebar item gains focus
  if (tab->isFocused()) {
    this->scrollFrame->addView(createContentFn());
    this->scrollFrame->setContentOffsetY(0, false); // Reset scroll
  }
}

brls::View* TabHelp::create() { return new TabHelp(); }