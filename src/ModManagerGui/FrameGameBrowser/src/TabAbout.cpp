//
// Created by Adrien BLANCHET on 20/06/2020.
//

#include "TabAbout.h"
#include <StateAlchemist/constants.h>
#include <util.hpp>
#include <AlchemistLogger.h>

using namespace brls::literals;

TabAbout::TabAbout(): brls::Box(brls::Axis::COLUMN) {
  alchemyLogger.log("TabAbout:::TabAbout");

  // Low padding; We got a lot to fit
  // TODO: Make this scrollable; spread things out a little
  this->setPadding(16.0f, 40.0f, 32.0f, 40.0f);
  this->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  
  // Subtitle
  alchemyLogger.log("TabAbout:::1");
  brls::Label* shortDescription = new brls::Label();
  shortDescription->setText(
    "Simple Mod Alchemist is a Nintendo Switch homebrew app for changing game mods.\n"\
    "It resulted from the fusion of two other apps: SimpleModManager and State Alchemist.\n"
  );
  shortDescription->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  shortDescription->setIsWrapping(true);
  shortDescription->setFontSize(15.0f);
  this->addView(shortDescription);

  // Two columns of content
  alchemyLogger.log("TabAbout:::2");
  brls::Box* columns = new brls::Box();
  columns->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  columns->setHeight(300.0f);
  this->addView(columns);

  alchemyLogger.log("TabAbout:::3");
  brls::Box* leftBox = new brls::Box(brls::Axis::COLUMN);
  leftBox->setJustifyContent(brls::JustifyContent::SPACE_BETWEEN);
  leftBox->setWidth(500);
  columns->addView(leftBox);

  alchemyLogger.log("TabAbout:::4");
  brls::Header* versionHeader = new brls::Header();
  versionHeader->setTitle("Version " + APP_VERSION + " - What's new ?");
  leftBox->addView(versionHeader);

  alchemyLogger.log("TabAbout:::5");
  brls::Label* changelog = new brls::Label();
  changelog->setText(
    " - Atmosphere v1.10.0 support\n"\
    " - Mods can now be changed at random\n"\
    " - Working progress bars\n"
    " - The best part: General stability improvements\n"
  );
  changelog->setHorizontalAlign(brls::HorizontalAlign::LEFT);
  changelog->setFontSize(15.0f);
  leftBox->addView(changelog);

  alchemyLogger.log("TabAbout:::6");
  brls::Header* copyrightHeader = new brls::Header();
  copyrightHeader->setTitle("Copyright");
  leftBox->addView(copyrightHeader);

  alchemyLogger.log("TabAbout:::7");
  brls::Label* copyright = new brls::Label();
  copyright->setText(
    "Simple Mod Alchemist is licensed under GPL-v3.0\n" \
        "\u00A9 SimpleModManager 2019 - 2023 Nadrino\n"\
        "\u00A9 Simple Mod Alchemist 2025 gtiersma"
  );
  copyright->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  copyright->setFontSize(13.0f);
  leftBox->addView(copyright);

  alchemyLogger.log("TabAbout:::8");
  brls::Box* rightBox = new brls::Box(brls::Axis::COLUMN);
  rightBox->setJustifyContent(brls::JustifyContent::CENTER);
  rightBox->setWidth(200);
  columns->addView(rightBox);

  alchemyLogger.log("TabAbout:::9");
  brls::Image* portrait = new brls::Image();
  portrait->setImageFromRes("img/portrait.jpg");
  portrait->setScalingType(brls::ImageScalingType::FIT);
  rightBox->addView(portrait);

  alchemyLogger.log("TabAbout:::10");
  brls::Label* portraitLabel = new brls::Label();
  portraitLabel->setText("SimpleModManager Original Author: Nadrino");
  portraitLabel->setFontSize(13.0f);
  portraitLabel->setHorizontalAlign(brls::HorizontalAlign::CENTER);
  portraitLabel->setMarginTop(4.0f);
  rightBox->addView(portraitLabel);

  alchemyLogger.log("TabAbout:::11");
  this->addView(columns);

  alchemyLogger.log("TabAbout:::12");
  brls::Header* creditsHeader = new brls::Header();
  creditsHeader->setTitle("Credits");
  this->addView(creditsHeader);

  alchemyLogger.log("TabAbout:::13");
  brls::Label* credits = new brls::Label();
  credits->setText(
    "- Maintained by gtiersma.\n"\
    "- Built upon SimpleModManager, developed by Nadrino.\n"\
    "- Powered by Borealis, provided by the RetroNX team.\n"\
    "- Special thanks to RetroNX, devkitPro, the ethical homebrew development community in general, and Nintendo.\n"\
    "- This software is unofficial. It is not protected under any license by Nintendo.\n"
  );
  credits->setFontSize(13.0f);
  credits->setIsWrapping(true);
  this->addView(credits);
}

brls::View* TabAbout::create() { return new TabAbout(); }
