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
        [this](float value) {
            this->changedModlessRating = static_cast<u8>(std::round(value * 100.0f));
        }
    );

    for (const auto& entry: this->ratings) {
        this->buildRating(entry.first, entry.second, [this, entry](float value) {
            this->changedRatings[entry.first] = static_cast<u8>(std::round(value * 100.0f));
        });
    }
}

void RandomSettings::buildRating(const std::string& name, u8 rating, const std::function<void(float)>& updateFn) {
    brls::Label* header = new brls::Label();
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

    // Extra spacing to push the UI for this mod a little further away from others.
    // This ensures each slider is spaced closer to the header that belongs to it.
    // Otherwise, it's more difficult to quickly distinguish which slider belongs to which.
    header->setMarginTop(60);
    cell->setMarginBottom(50);

    list->addView(header);
    list->addView(cell);
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