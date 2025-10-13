#include "loading_dialog.hpp"

#include "util.hpp"

LoadingDialog* LoadingDialog::build() {
  brls::Box* container = new brls::Box(brls::Axis::COLUMN);
  container->setFocusable(true);
  Util::padTabContent(container);

  return new LoadingDialog(container);
}

void LoadingDialog::open() {
  brls::Application::pushActivity(new brls::Activity(this));

  this->watchProgress.store(true);
  new std::thread([this]() {
    while(this->watchProgress.load()) {
      float progress = this->progress.load();
      this->progressLabel->setText(std::to_string(progress * 100) + "%");
      this->progressBar->setProgress(progress);
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });
}

void LoadingDialog::setAction(const std::string& action) {
  this->label->setText(action + " Please wait...");
}

void LoadingDialog::setProgress(float progress) {
  this->progress.store(progress);
}

float LoadingDialog::getProgress() {
  return this->progress.load();
}

std::atomic<float>& LoadingDialog::getAtomicProgress() {
  return this->progress;
}

LoadingDialog::LoadingDialog(Box* contentView): brls::Dialog(contentView) {
  this->label = new brls::Label();
  this->label->setText("Please wait...");
  contentView->addView(this->label);

  this->progressLabel = new brls::Label();
  this->progressLabel->setText("0%");
  contentView->addView(this->progressLabel);

  this->progressBar = new ProgressBar();
  this->progressBar->setProgress(0);
  contentView->addView(this->progressBar);

  // Work-around to keep anything behind it from becoming focused
  //brls::Application::giveFocus(contentView);
  this->setCancelable(false);
}

LoadingDialog::~LoadingDialog() {
  this->watchProgress.store(false);
}