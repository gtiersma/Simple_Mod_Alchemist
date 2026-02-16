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
    // Only trigger when the sidebar item gains focus
    if (view->isFocused()) {
      this->scrollFrame->addView(new Tldr());
    }
  });

  this->topicList->addSeparator();

  this->topicList->addItem("Setup", [this](brls::View* view) {
    // Only trigger when the sidebar item gains focus
    if (view->isFocused()) {
      this->scrollFrame->addView(new Setup());
    }
  });

  this->topicList->addItem("Backups", [this](brls::View* view) {
    // Only trigger when the sidebar item gains focus
    if (view->isFocused()) {
      this->scrollFrame->addView(new Backups());
    }
  });

  this->topicList->addItem("Using", [this](brls::View* view) {
    // Only trigger when the sidebar item gains focus
    if (view->isFocused()) {
      this->scrollFrame->addView(new Manage());
    }
  });

  this->topicList->addItem("FAQs", [this](brls::View* view) {
    // Only trigger when the sidebar item gains focus
    if (view->isFocused()) {
      this->scrollFrame->addView(new Faqs());
    }
  });

  this->topicList->addItem("Advanced", [this](brls::View* view) {
    // Only trigger when the sidebar item gains focus
    if (view->isFocused()) {
      this->scrollFrame->addView(new Advanced());
    }
  });

  this->topicList->addSeparator();

  this->topicList->addItem("About", [this](brls::View* view) {
    // Only trigger when the sidebar item gains focus
    if (view->isFocused()) {
      this->scrollFrame->addView(new About());
    }
  });
}

brls::View* TabHelp::create() { return new TabHelp(); }