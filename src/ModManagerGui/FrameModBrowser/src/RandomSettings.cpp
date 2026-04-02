#include "RandomSettings.h"

#include <dismiss_dialog.hpp>

#include <StateAlchemist/controller.h>
#include <GameBrowser.h>

#include <util.hpp>


RandomSettings::RandomSettings() {
    this->inflateFromXMLRes("xml/FrameModBrowser/random_settings.xml");

    this->ratings = controller.loadRatings();
    this->modlessRating = controller.loadDefaultRating();

    this->buildUi();

    title->setText(controller.source);
}

void RandomSettings::buildUi() {
    std::vector<brls::SliderCell*> cells;

    this->buildRating(
        "Default " + controller.source + " (No Mod)",
        this->modlessRating,
        false,
        [this](float value) {
            this->changedModlessRating = static_cast<u8>(std::round(value * 100.0f));
        }
    );

    // Have the rating alternate between different background colors:
    bool useAltBackColor = true;
    for (const auto& entry: this->ratings) {
        this->buildRating(entry.first, entry.second, useAltBackColor, [this, entry](float value) {
            this->changedRatings[entry.first] = static_cast<u8>(std::round(value * 100.0f));
        });
        useAltBackColor = !useAltBackColor;
    }
}

void RandomSettings::buildRating(const std::string& name, u8 rating, bool useAltBackColor, const std::function<void(float)>& updateFn) {
    brls::Box* ratingItem = new brls::Box(brls::Axis::COLUMN);
    ratingItem->setPadding(30, 10, 0, 10);

    if (useAltBackColor) {
        ratingItem->setBackgroundColor(RGB(57, 58, 60));
    }

    brls::Label* header = new brls::Label();
    header->setMarginLeft(5);
    header->setFontSize(14);
    header->setText(name);

    float fractionalRating = static_cast<float>(rating) / 100.0f;
    brls::SliderCell* cell = new brls::SliderCell();
    cell->init(
        Util::toPercentLabel(fractionalRating),
        fractionalRating,
        [updateFn, cell](float value) {
            updateFn(value);
            cell->setText(Util::toPercentLabel(value));
        }
    );

    ratingItem->addView(header);
    ratingItem->addView(cell);
    list->addView(ratingItem);
}

std::map<std::string, u8> RandomSettings::getChangedRatings() {
    std::map<std::string, u8> ratings;

    for (const auto& entry: this->changedRatings) {
        ratings[entry.first] = static_cast<u8>(std::round(entry.second * 100.0f));
    }

    return ratings;
}

u8 RandomSettings::getChangedModlessRating() {
    return static_cast<u8>(std::round(this->changedModlessRating * 100.0f));
}

bool RandomSettings::hasModlessRatingChanged() {
    return this->changedModlessRating != -1.0f;
}

void RandomSettings::showInDialog() {
    RandomSettings* ui = new RandomSettings();
    brls::DismissDialog* dialog = new brls::DismissDialog(ui);

    dialog->setCancelable(true, [ui]() {
        controller.saveRatings(ui->getChangedRatings());
        if (ui->hasModlessRatingChanged()) {
            controller.saveDefaultRating(ui->getChangedModlessRating());
        }
    });

    dialog->addButton("Close", [ui]() {
        controller.saveRatings(ui->getChangedRatings());
        if (ui->hasModlessRatingChanged()) {
            controller.saveDefaultRating(ui->getChangedModlessRating());
        }
    });

    dialog->open();
}

RandomSettings* RandomSettings::create() {
  return new RandomSettings();
}