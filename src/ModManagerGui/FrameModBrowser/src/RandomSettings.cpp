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
    return 1; // Each section has one row
}

int RandomDataSource::numberOfSections(brls::RecyclerFrame* recycler) {
    // Additional +1 for the modless option row:
    return this->fractionalRatings.size() + 1;
}

std::string RandomDataSource::titleForHeader(brls::RecyclerFrame* recycler, int section) {
    if (section == 0) {
        return "Default " + controller.source + " (No Mod)";
    }
    return this->modNames[section - 1];
}

float RandomDataSource::heightForHeader(brls::RecyclerFrame* recycler, int section) {
    return 44.0f;
}

brls::RecyclerCell* RandomDataSource::cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath indexPath) {
    brls::SliderCell* cell = (brls::SliderCell*)recycler->dequeueReusableCell("Slider");

    if (indexPath.section == 0) {
        cell->init(
            Util::toPercentLabel(this->modlessFractionalRating),
            this->modlessFractionalRating,
            [this, cell](float value) {
                this->changedModlessFractional = value;
                cell->setText(Util::toPercentLabel(value));
            }
        );
    } else {
        std::string name = this->modNames[indexPath.section - 1];
        float rating = this->fractionalRatings[indexPath.section - 1];
        cell->init(
            Util::toPercentLabel(rating),
            rating,
            [this, cell, name](float value) {
                this->changedFractionals[name] = value;
                cell->setText(Util::toPercentLabel(value));
            }
        );
    }

    // Extra spacing to push the cell below a little further away.
    // This ensures each slider is spaced closer to the header that belongs to it.
    // Otherwise, it's more difficult to quickly distinguish which slider belongs to which.
    cell->setPaddingTop(60);
    cell->setPaddingBottom(50);

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

    list->estimatedRowHeight = 160;
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