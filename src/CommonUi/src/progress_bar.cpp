/*
    Copyright 2021 XITRIX

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <borealis/core/application.hpp>
#include <borealis/core/touch/pan_gesture.hpp>
#include <borealis/core/util.hpp>

#include <progress_bar.hpp>



ProgressBar::ProgressBar()
{
    lineEmpty = new brls::Rectangle();
    lineEmpty->detach();
    addView(lineEmpty);

    setHeight(30);
    lineEmpty->setHeight(30);
    lineEmpty->setWidthPercentage(100.0f);

    brls::Theme theme = brls::Application::getTheme();
    lineEmpty->setColor(theme["brls/slider/line_empty"]);
}

void ProgressBar::onLayout()
{
    Box::onLayout();
    updateUI();
}

void ProgressBar::draw(NVGcontext* vg, float x, float y, float width, float height, brls::Style style, brls::FrameContext* ctx)
{
    Box::draw(vg, x, y, width, height, style, ctx);

    nvgSave(vg);
    nvgResetScissor(vg);

    float progressWidth = width * this->progress;

    float gradientX, gradientY, color;
    brls::getHighlightAnimation(&gradientX, &gradientY, &color);

    NVGcolor highlightColor1 = ctx->theme["brls/highlight/color1"];

    NVGcolor pulsationColor = RGBAf((color * highlightColor1.r) + (1 - color) * highlightColor1.r,
        (color * highlightColor1.g) + (1 - color) * highlightColor1.g,
        (color * highlightColor1.b) + (1 - color) * highlightColor1.b,
        alpha);

    NVGcolor borderColor = ctx->theme["brls/highlight/color2"];
    borderColor.a        = 0.5f * alpha * this->getAlpha();

    NVGpaint border1Paint = nvgRadialGradient(vg,
        x + gradientX * progressWidth, y + gradientY * height,
        100, 400,
        borderColor, brls::TRANSPARENT);

    NVGpaint border2Paint = nvgRadialGradient(vg,
        x + (1 - gradientX) * progressWidth, y + (1 - gradientY) * height,
        100, 400,
        borderColor, brls::TRANSPARENT);

    nvgBeginPath(vg);
    nvgFillColor(vg, pulsationColor);
    nvgRoundedRect(vg, x, y, progressWidth, height, 1.5f);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillPaint(vg, border1Paint);
    nvgRoundedRect(vg, x, y, progressWidth, height, 1.5f);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillPaint(vg, border2Paint);
    nvgRoundedRect(vg, x, y, progressWidth, height, 1.5f);
    nvgFill(vg);

    nvgRestore(vg);
}

void ProgressBar::setProgress(float progress)
{
    static int lastProgressTicker = this->progress * 10;

    this->progress = progress;

    if (this->progress < 0)
        this->progress = 0;

    if (this->progress > 1)
        this->progress = 1;

    if (lastProgressTicker != (int)(this->progress * 10))
    {
        lastProgressTicker = this->progress * 10;
    }

    progressEvent.fire(this->progress);
    updateUI();
}

void ProgressBar::updateUI()
{
    lineEmpty->setPositionLeftPercentage(this->progress * 100.0f);
}

float ProgressBar::getProgress()
{
    return progress;
}

brls::Event<float>* ProgressBar::getProgressEvent()
{
    return &progressEvent;
}

void ProgressBar::setStep(float step)
{
    this->step = step;
}

brls::View* ProgressBar::create()
{
    return new ProgressBar();
}
