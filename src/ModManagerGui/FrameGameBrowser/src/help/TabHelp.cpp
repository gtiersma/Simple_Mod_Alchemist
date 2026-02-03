#include "help/TabHelp.h"

#include "help/About.h"


TabHelp::TabHelp() {
  this->inflateFromXMLRes("xml/FrameGameBrowser/help/tab_help.xml");

  this->topicList->addItem("About", this->createRenderFn([]() { return new About(); }));
}

brls::GenericEvent::Callback TabHelp::createRenderFn(std::function<brls::View* ()> buildViewFn) {
  return [this, buildViewFn](brls::View* view) {
    // Only trigger when the sidebar item gains focus
    if (!view->isFocused())
      return;

    // Remove the old group list before showing the new one
    // (if there is currently one shown).
    //
    // TODO: Would be a little safer if we were using an ID
    if (this->getChildren().size() == 2) {
      this->removeView(this->getChildren()[1]);
    }

    this->addView(
      this->toScrollable(buildViewFn())
    );
  };
}

brls::View* TabHelp::toScrollable(brls::View* content) {
  brls::Box* container = new brls::Box();
  brls::ScrollingFrame* scrollBox = new brls::ScrollingFrame();

  scrollBox->setGrow(1.0);
  scrollBox->addView(content);
  container->addView(scrollBox);

  return container;
}

brls::View* TabHelp::create() { return new TabHelp(); }