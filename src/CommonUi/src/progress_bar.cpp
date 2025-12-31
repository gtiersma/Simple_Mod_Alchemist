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

#include <progress_bar.hpp>


double ProgressBar::highlightFrame = 0;
double ProgressBar::highlightGradientX = 0;
double ProgressBar::highlightGradientY = 0;
double ProgressBar::highlightPulseColor = 0;

ProgressBar::ProgressBar()
{
    setWidthPercentage(100.0f);
    setHeight(30);

    brls::Theme theme = brls::Application::getTheme();
    setBackgroundColor(theme["brls/slider/line_empty"]);

    // Animation begins at the last highlight frame that was rendered for a progress bar: 
    this->lastFrame = ProgressBar::highlightFrame;
}

void ProgressBar::draw(NVGcontext* vg, float x, float y, float width, float height, brls::Style style, brls::FrameContext* ctx)
{
    Box::draw(vg, x, y, width, height, style, ctx);

    nvgSave(vg);
    nvgResetScissor(vg);

    float progressWidth = width * this->progress;

    // Ready for a new highlight animation frame.
    // It only needs to update the highlight animation data if it's on the latest frame from the highlight data:
    if (this->lastFrame == ProgressBar::highlightFrame) {
        ProgressBar::updateHighlight();
    }

    // We will now render a frame for the latest highlight data:
    this->lastFrame = ProgressBar::highlightFrame;

    NVGcolor highlightColor1 = ctx->theme["brls/highlight/color1"];

    NVGcolor pulsationColor = RGBAf(
        (ProgressBar::highlightPulseColor * highlightColor1.r) + (1 - ProgressBar::highlightPulseColor) * highlightColor1.r,
        (ProgressBar::highlightPulseColor * highlightColor1.g) + (1 - ProgressBar::highlightPulseColor) * highlightColor1.g,
        (ProgressBar::highlightPulseColor * highlightColor1.b) + (1 - ProgressBar::highlightPulseColor) * highlightColor1.b,
        alpha
    );

    NVGcolor highlightColor2 = ctx->theme["brls/highlight/color2"];
    highlightColor2.a        = 0.7f * alpha * this->getAlpha();

    NVGpaint gradient1Paint = nvgRadialGradient(vg,
        x + ProgressBar::highlightGradientX * progressWidth, y + ProgressBar::highlightGradientY * height,
        100, 300,
        highlightColor2, brls::TRANSPARENT);

    NVGpaint gradient2Paint = nvgRadialGradient(vg,
        x + (1 - ProgressBar::highlightGradientX) * progressWidth, y + (1 - ProgressBar::highlightGradientY) * height,
        100, 300,
        highlightColor2, brls::TRANSPARENT);

    nvgBeginPath(vg);
    nvgFillColor(vg, pulsationColor);
    nvgRoundedRect(vg, x, y, progressWidth, height, 1.5f);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillPaint(vg, gradient1Paint);
    nvgRoundedRect(vg, x, y, progressWidth, height, 1.5f);
    nvgFill(vg);

    nvgBeginPath(vg);
    nvgFillPaint(vg, gradient2Paint);
    nvgRoundedRect(vg, x, y, progressWidth, height, 1.5f);
    nvgFill(vg);

    nvgRestore(vg);
}

void ProgressBar::setProgress(float progress)
{
    this->progress = progress;

    if (this->progress < 0)
        this->progress = 0;

    if (this->progress > 1)
        this->progress = 1;
}

float ProgressBar::getProgress()
{
    return progress;
}

brls::View* ProgressBar::create()
{
    return new ProgressBar();
}

void ProgressBar::updateHighlight()
{
    brls::Time currentTime = brls::getCPUTimeUsec() / 1000;

    ProgressBar::highlightGradientX  = (cos((double)currentTime / ProgressBar::HIGHLIGHT_SPEED / 3.0) + 1.0) / 2.0;
    ProgressBar::highlightGradientY  = (sin((double)currentTime / ProgressBar::HIGHLIGHT_SPEED / 3.0) + 1.0) / 2.0;
    ProgressBar::highlightPulseColor = (sin((double)currentTime / ProgressBar::HIGHLIGHT_SPEED * 2.0) + 1.0) / 2.0;

    ProgressBar::highlightFrame++;
}
