#include "util.hpp"

void Util::padTabContent(brls::Box* tabContent) {
  brls::Style style = brls::Application::getStyle();
  tabContent->setPadding(
    style["brls/tab_details/padding_top"],
    style["brls/tab_details/padding_right"],
    style["brls/tab_details/padding_bottom"],
    style["brls/tab_details/padding_left"]
  );
}

brls::Dialog* Util::buildConfirmDialog(
  const std::string& warning,
  const std::string& action,
  std::function<void()> task,
  std::function<void()> finishedCallback
) {
  brls::Dialog* dialog = new brls::Dialog(warning + " Are you sure?");

  dialog->addButton("Yes", [task, finishedCallback, action]() {
    brls::Dialog* loadingDialog = buildLoadingDialog(action);
    loadingDialog->open();

    new std::thread([task, finishedCallback, loadingDialog]() {
      task();
      loadingDialog->close(finishedCallback);
    });
  });
  dialog->addButton("No", []() {});

  return dialog;
}

/**
 * Builds a dialog to show for while the a longer process is running
 * 
 * @param action Short label describing the action being performed
 */
brls::Dialog* Util::buildLoadingDialog(std::string action) {
  brls::Box* container = new brls::Box(brls::Axis::COLUMN);
  container->setFocusable(true);
  padTabContent(container);

  brls::Label* label = new brls::Label();
  label->setText(action + " Please wait...");
  container->addView(label);

  brls::ProgressSpinner* progress = new brls::ProgressSpinner();
  container->addView(progress);

  brls::Dialog* dialog = new brls::Dialog(container);
  dialog->setCancelable(false);

  // Work-around to keep anything behind it from becoming focused
  brls::Application::giveFocus(container);

  return dialog;
}