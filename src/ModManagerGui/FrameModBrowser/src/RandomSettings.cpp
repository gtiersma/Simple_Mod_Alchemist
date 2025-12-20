#include "RandomSettings.h"

#include <StateAlchemist/controller.h>
#include <GameBrowser.h>

#include <util.hpp>


RandomDataSource::RandomDataSource(
    const std::map<std::string, u8>& ratings,
    u8 modlessRating
) {
    this->modlessFractionalRating = static_cast<float>(modlessRating) / 100.0f;

    for (auto& entry: ratings) {
        this->modNames.push_back(entry.first);
        this->fractionalRatings.push_back(static_cast<float>(entry.second) / 100.0f);
    }
}

int RandomDataSource::numberOfRows(brls::RecyclerFrame* recycler, int section) {
    // Additional +1 for the modless option row:
    return this->fractionalRatings.size() + 1;
}

brls::RecyclerCell* RandomDataSource::cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath indexPath) {
    brls::SliderCell* cell = (brls::SliderCell*)recycler->dequeueReusableCell("Slider");

    if (indexPath.row == 0) {
        cell->init(
            "Default " + controller.source + " (No Mod)",
            this->modlessFractionalRating,
            [this](float value) { this->changedModlessFractional = value; }
        );
    } else {
        std::string& name = this->modNames[indexPath.row - 1];
        float& rating = this->fractionalRatings[indexPath.row - 1];
        cell->init(name, rating, [this, name, rating](float value) {
            this->changedFractionals[name] = rating;
        });
    }

    return cell;
}

std::map<std::string, u8> RandomDataSource::getChangedRatings() {
    std::map<std::string, u8> ratings;

    for (int i = 0; i < this->modNames.size(); i++) {
        ratings[this->modNames[i]] = static_cast<int>(this->fractionalRatings[i] * 100.0f);
    }

    return ratings;
}

u8 RandomDataSource::getChangedModlessRating() {
    if (this->changedModlessFractional == -1.0f) {
        return -1;
    }
    return static_cast<int>(this->changedModlessFractional * 100.0f);
}

RandomSettings::RandomSettings() {
    this->inflateFromXMLRes("xml/FrameModBrowser/random_settings.xml");

    this->ratings = controller.loadRatings();
    this->modlessRating = controller.loadDefaultRating();
    this->dataSource = new RandomDataSource(this->ratings, this->modlessRating);

    title->setText(controller.source);

    list->estimatedRowHeight = 70;
    list->registerCell("Slider", []() { return new brls::SliderCell(); });
    list->setDataSource(this->dataSource);
}

std::map<std::string, u8> RandomSettings::getChangedRatings() {
    return this->dataSource->getChangedRatings();
}

u8 RandomSettings::getChangedModlessRating() {
    return this->dataSource->getChangedModlessRating();
}

void RandomSettings::showInDialog() {
    RandomSettings* ui = new RandomSettings();
    brls::Dialog* dialog = new brls::Dialog(ui);

    dialog->addButton("Save & Close", [ui]() {
        controller.saveRatings(ui->getChangedRatings());

        u8 modlessRating = ui->getChangedModlessRating();
        if (modlessRating != -1) {
            controller.saveDefaultRating(modlessRating);
        }
    });

    dialog->open();
}

RandomSettings* RandomSettings::create() {
  return new RandomSettings();
}