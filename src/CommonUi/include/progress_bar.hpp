/*
    Copyright 2019-2021 natinusala
    Copyright 2019 p-sam

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

    -----

    Not an official file that's part of the Borealis library,
    but rather a direct copy with minimal modifications,
    so the license above still applies.
*/

#pragma once

#include <borealis/core/application.hpp>
#include <borealis/core/bind.hpp>
#include <borealis/core/box.hpp>
#include <borealis/views/label.hpp>
#include <borealis/views/rectangle.hpp>

class ProgressBar : public brls::Box
{
  public:
    ProgressBar();

    void onLayout() override;
    void draw(NVGcontext* vg, float x, float y, float width, float height, brls::Style style, brls::FrameContext* ctx) override;

    void setProgress(float progress);

    float getProgress();

    brls::Event<float>* getProgressEvent();

    void setStep(float step);

    static brls::View* create();

  private:
    brls::InputManager* input;
    brls::Rectangle* line;
    brls::Rectangle* lineEmpty;

    brls::Event<float> progressEvent;

    float progress = 0;
    float step = 0.5f;

    void updateUI();
};
