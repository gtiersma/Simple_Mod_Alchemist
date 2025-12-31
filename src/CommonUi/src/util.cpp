#include "util.hpp"

#include "loading_dialog.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>


std::string Util::toPercentLabel(float fractional) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << std::round(fractional * 100);
    return stream.str() + "%";
}

void Util::padContent(brls::Box* content) {
  brls::Style style = brls::Application::getStyle();
  content->setPadding(
    style["brls/tab_details/padding_top"],
    style["brls/tab_details/padding_right"],
    style["brls/tab_details/padding_bottom"],
    style["brls/tab_details/padding_left"]
  );
}

brls::Dialog* Util::buildConfirmDialog(
  const std::string& warning,
  const std::string& action,
  std::function<void(std::atomic<float>& progress)> task,
  std::function<void()> finishedCallback
) {
  brls::Dialog* dialog = new brls::Dialog(warning + " Are you sure?");

  dialog->addButton("Yes", [task, finishedCallback, action]() {
    LoadingDialog* loadingDialog = LoadingDialog::build();
    loadingDialog->setAction(action);
    loadingDialog->open();

    new std::thread([task, finishedCallback, loadingDialog]() {
      task(loadingDialog->getAtomicProgress());
      loadingDialog->close(finishedCallback);
    });
  });
  dialog->addButton("No", []() {});

  return dialog;
}