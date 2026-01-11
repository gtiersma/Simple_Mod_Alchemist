//
// Created by Adrien BLANCHET on 20/06/2020.
//

#include <ctime>

#include "TabAbout.h"
#include <StateAlchemist/constants.h>
#include <util.hpp>

using namespace brls::literals;

TabAbout::TabAbout(): brls::Box(brls::Axis::COLUMN) {
  Util::padContent(this);
  this->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  this->setHeight(1080.0);
  
  // Subtitle
  brls::Label* shortDescription = new brls::Label();
  shortDescription->setText(
    "Simple Mod Alchemist is a Nintendo Switch homebrew app for changing game mods.\n"\
    "It's the fusion of two other modding apps: SimpleModManager and State Alchemist.\n"
  );
  shortDescription->setMarginLeft(30.0);
  shortDescription->setMarginRight(30.0);
  shortDescription->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  shortDescription->setIsWrapping(true);
  shortDescription->setFontSize(20.0);
  this->addView(shortDescription);

  // Two columns of content
  brls::Box* columns = new brls::Box();
  columns->setHeight(405.0);
  columns->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  // todo: Set grow?
  columns->addView(this->buildLeftBox());
  columns->addView(this->buildRightBox());
  this->addView(columns);

  brls::Label* credits = new brls::Label();
  credits->setText(
    "- Maintained by gtiersma.\n"\
    "- Built upon SimpleModManager, developed by Nadrino.\n"\
    "- Powered by Borealis, provided by the RetroNX team.\n"\
    "- Special thanks to RetroNX, devkitPro, the ethical homebrew development community in general, and Nintendo.\n"\
    "- This software is unofficial. It is not protected under any license by Nintendo.\n"
  );
  credits->setFontSize(15.0f);
  credits->setIsWrapping(true);
  this->addView(this->wrapWithHeader(credits, "Credits"));

  std::time_t now;
  struct std::tm* current;
  std::time(&now);
  current = std::localtime(&now);
  brls::Label* verse = new brls::Label();
  if (current->tm_mon == 11 & current->tm_mday < 26) {
    verse->setText(
      "\"Today in the town of David a Savior has been born to you; He is the Messiah, the Lord.\" - (Luke 2:11)"
    );
  } else {
    verse->setText(
      "\"So do not fear, for I am with you; do not be dismayed, for I am your God. I will strengthen you and help you; I will uphold you with my righteous right hand.\" - (Isaiah 41:10)"
    );
  }
  verse->setFontSize(15.0f);
  verse->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  verse->setIsWrapping(true);
  this->addView(verse);
}

brls::View* TabAbout::buildLeftBox() {
  brls::Box* leftBox = new brls::Box(brls::Axis::COLUMN);
  leftBox->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  leftBox->setWidth(500);

  brls::Label* changelog = new brls::Label();
  changelog->setText(
    " - Dialog for changing how likely each mod is to be picked at random.\n"\
    " - The game being viewed can be started from the app.\n"\
    " - Empty folders are now being removed as files get moved.\n"\
    " - General stability improvements!\n"
  );
  changelog->setHorizontalAlign(brls::HorizontalAlign::LEFT);
  changelog->setFontSize(18.0f);
  leftBox->addView(
    this->wrapWithHeader(changelog, "Version " + APP_VERSION + " - What's new ?")
  );

  brls::Label* copyright = new brls::Label();
  copyright->setText(
    "Simple Mod Alchemist is licensed under GPL-v3.0\n" \
        "\u00A9 SimpleModManager 2019 - 2023 Nadrino\n"\
        "\u00A9 Simple Mod Alchemist 2025 - 2026 gtiersma"
  );
  copyright->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  copyright->setFontSize(15.0f);
  leftBox->addView(this->wrapWithHeader(copyright, "Copyright"));

  return leftBox;
}

brls::View* TabAbout::buildRightBox() {
  brls::Box* rightBox = new brls::Box(brls::Axis::COLUMN);
  rightBox->setJustifyContent(brls::JustifyContent::CENTER);
  rightBox->setWidth(200);

  brls::Image* portrait = new brls::Image();
  portrait->setImageFromRes("img/portrait.jpg");
  portrait->setScalingType(brls::ImageScalingType::FIT);
  portrait->setImageAlign(brls::ImageAlignment::BOTTOM);
  rightBox->addView(portrait);

  brls::Label* portraitLabel = new brls::Label();
  portraitLabel->setText("SimpleModManager Original Author: Nadrino");
  portraitLabel->setFontSize(15.0f);
  portraitLabel->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  portraitLabel->setMarginTop(10.0f);
  rightBox->addView(portraitLabel);

  return rightBox;
}

brls::Box* TabAbout::wrapWithHeader(brls::View* content, std::string title) {
  brls::Box* container = new brls::Box(brls::Axis::COLUMN);

  brls::Header* header = new brls::Header();
  header->setTitle(title);
  header->setMarginBottom(20.0);
  container->addView(header);

  container->addView(content);
  return container;
}

brls::View* TabAbout::create() { return new TabAbout(); }
