//
// Created by Adrien BLANCHET on 20/06/2020.
//

#include <ctime>

#include "help/About.h"
#include <StateAlchemist/constants.h>

using namespace brls::literals;

About::About(): brls::Box(brls::Axis::COLUMN) {
  this->setPaddingRight(20.0);
  
  // Subtitle
  brls::Label* shortDescription = new brls::Label();
  shortDescription->setText(
    "Simple Mod Alchemist is a Nintendo Switch homebrew app for changing game mods.\n"\
    "It's the fusion of two other modding apps: SimpleModManager and State Alchemist.\n"
  );
  shortDescription->setMarginTop(10.0);
  shortDescription->setMarginLeft(20.0);
  shortDescription->setMarginBottom(50.0);
  shortDescription->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  shortDescription->setIsWrapping(true);
  shortDescription->setFontSize(18.0);
  this->addView(shortDescription);

  // Two columns of content
  brls::Box* topColumns = new brls::Box();
  topColumns->setHeight(300.0);
  topColumns->setMarginRight(10.0);
  topColumns->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  topColumns->addView(this->buildTopLeftBox());
  topColumns->addView(this->buildTopRightBox());
  this->addView(topColumns);

  brls::Box* bottomColumns = new brls::Box();
  bottomColumns->setHeight(350.0);
  bottomColumns->setMarginRight(10.0);
  bottomColumns->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  bottomColumns->addView(this->buildBottomLeftBox());
  bottomColumns->addView(this->buildBottomRightBox());
  this->addView(bottomColumns);

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
  verse->setFontSize(13.0f);
  verse->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  verse->setIsWrapping(true);
  verse->setMarginBottom(80.0);
  verse->setMarginLeft(20.0);
  this->addView(verse);

  brls::Label* copyright = new brls::Label();
  copyright->setText(
    "Simple Mod Alchemist is licensed under GPL-v3.0\n" \
        "\u00A9 SimpleModManager 2019 - 2023 Nadrino\n"\
        "\u00A9 Simple Mod Alchemist 2025 - 2026 gtiersma"
  );
  copyright->setFontSize(12.0f);
  copyright->setMarginBottom(30.0);
  copyright->setMarginLeft(20.0);
  this->addView(copyright);

  brls::Label* disclaimer = new brls::Label();
  disclaimer->setText("This software is not licensed by Nintendo Co. Ltd, nor are they affiliated with the creation of this software in any way. This software is an unofficial application, provided free of charge, payment, or donation of any kind for all Nintendo Switch owners.");
  disclaimer->setFontSize(12.0f);
  disclaimer->setMarginLeft(20.0);
  this->addView(disclaimer);
}

brls::View* About::buildTopLeftBox() {
  brls::Box* leftBox = new brls::Box(brls::Axis::COLUMN);
  leftBox->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  leftBox->setWidthPercentage(60.0);

  brls::Label* changelog = new brls::Label();
  changelog->setText(
    " - Help info is now in the app.\n"\
    " - Fixed the mod random settings, making it easier to use as well.\n"\
    " - Game folders are now automatically given the game's name with the title ID number.\n"\
    " - General stability improvements!\n"
  );
  changelog->setHorizontalAlign(brls::HorizontalAlign::LEFT);
  changelog->setFontSize(15.0f);
  leftBox->addView(
    this->wrapWithHeader(changelog, "Version " + APP_VERSION + " - What's new ?")
  );

  return leftBox;
}

brls::View* About::buildTopRightBox() {
  brls::Box* rightBox = new brls::Box(brls::Axis::COLUMN);
  rightBox->setJustifyContent(brls::JustifyContent::CENTER);
  rightBox->setWidthPercentage(30.0);

  brls::Image* qr = new brls::Image();
  qr->setImageFromRes("img/linkbreakers-io_qr.png");
  qr->setScalingType(brls::ImageScalingType::FIT);
  qr->setImageAlign(brls::ImageAlignment::BOTTOM);
  rightBox->addView(qr);

  brls::Label* qrLabel = new brls::Label();
  qrLabel->setText("Visit the GitHub repo");
  qrLabel->setFontSize(12.0f);
  qrLabel->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  qrLabel->setMarginTop(10.0f);
  rightBox->addView(qrLabel);

  return rightBox;
}

brls::View* About::buildBottomLeftBox() {
  brls::Box* leftBox = new brls::Box(brls::Axis::COLUMN);
  leftBox->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  leftBox->setWidthPercentage(60.0);

  brls::Label* credits = new brls::Label();
  credits->setText(
    "- Maintained by gtiersma.\n"\
    "- Built upon SimpleModManager, developed by Nadrino.\n"\
    "- Powered by Borealis, provided by the RetroNX team.\n"\
    "- Special thanks to RetroNX, devkitPro, the ethical homebrew development community in general, and Nintendo.\n"\
  );
  credits->setHorizontalAlign(brls::HorizontalAlign::LEFT);
  credits->setFontSize(15.0f);
  leftBox->addView(
    this->wrapWithHeader(credits, "Credits")
  );

  return leftBox;
}

brls::View* About::buildBottomRightBox() {
  brls::Box* rightBox = new brls::Box(brls::Axis::COLUMN);
  rightBox->setJustifyContent(brls::JustifyContent::CENTER);
  rightBox->setWidthPercentage(30.0);

  brls::Image* portrait = new brls::Image();
  portrait->setImageFromRes("img/portrait.jpg");
  portrait->setScalingType(brls::ImageScalingType::FIT);
  portrait->setImageAlign(brls::ImageAlignment::BOTTOM);
  rightBox->addView(portrait);

  brls::Label* portraitLabel = new brls::Label();
  portraitLabel->setText("SimpleModManager Original Author: Nadrino");
  portraitLabel->setFontSize(12.0f);
  portraitLabel->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  portraitLabel->setMarginTop(10.0f);
  rightBox->addView(portraitLabel);

  return rightBox;
}

brls::Box* About::wrapWithHeader(brls::View* content, std::string title) {
  brls::Box* container = new brls::Box(brls::Axis::COLUMN);

  brls::Header* header = new brls::Header();
  header->setTitle(title);
  header->setMarginBottom(20.0);
  container->addView(header);

  container->addView(content);
  return container;
}

brls::View* About::create() { return new About(); }
