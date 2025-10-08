#pragma once

#include "borealis.hpp"

#include <string>
#include <atomic>


class LoadingDialog : public brls::Dialog
{
  public:
    static LoadingDialog* build();

    void open() override;

    void setAction(const std::string& action);

    void setProgress(float progress);
    float getProgress();
    std::atomic<float>& getAtomicProgress();

  private:
    LoadingDialog(Box* contentView);
    ~LoadingDialog();

    std::string action;
    std::atomic<float> progress{0};
    std::atomic<bool> watchProgress{false};

    brls::Label* label{nullptr};
    brls::Label* progressLabel{nullptr};
    brls::Slider* progressBar{nullptr};
};