//
// Created by Adrien Blanchet on 14/04/2023.
//


#include "SimpleModManager.h"

#include <FrameRoot.h>
#include <TabGames.h>
#include <TabGeneralSettings.h>
#include <TabAbout.h>
#include <GroupBrowser.h>
#include <TabModOptions.h>

#include <icon_applet.hpp>
#include <note_cell.hpp>

#include "ConfigHandler.h"

#include <borealis.hpp>

#include "switch.h"
#include <StateAlchemist/controller.h>
#include "StateAlchemist/constants.h"
#include "StateAlchemist/fs_manager.h"


using namespace brls::literals;

int main(int argc, char* argv[])
{
    brls::Platform::APP_LOCALE_DEFAULT = brls::LOCALE_AUTO;

    brls::Application::init();
    brls::Application::createWindow("Simple Mod Alchemist");
    brls::Application::getPlatform()->setThemeVariant(brls::ThemeVariant::DARK);

    // Have the application register an action on every activity that will quit when you press BUTTON_START
    brls::Application::setGlobalQuit(false);

    // Register custom views (including tabs, which are views)
    brls::Application::registerXMLView("TabGames", TabGames::create);
    brls::Application::registerXMLView("TabGeneralSettings", TabGeneralSettings::create);
    brls::Application::registerXMLView("TabAbout", TabAbout::create);
    brls::Application::registerXMLView("GroupBrowser", GroupBrowser::create);
    brls::Application::registerXMLView("TabModOptions", TabModOptions::create);

    brls::Application::registerXMLView("brls:IconApplet", brls::IconApplet::create);
    brls::Application::registerXMLView("brls:NoteCell", brls::NoteCell::create);

    nsInitialize();

    // Create the app's folder in the SD Root if not yet created:
    FsManager::createFolderIfNeeded(ALCHEMIST_PATH);

    gameBrowser.loadGames();

    brls::Activity* mainActivity = new FrameRoot();

    brls::Application::pushActivity(mainActivity);

    brls::AppletFrame* appFrame = (brls::AppletFrame*)mainActivity->getContentView();
    appFrame->setTitle("Simple Mod Alchemist (v" + APP_VERSION + ")");

    // Run the app
    while (brls::Application::mainLoop());

    nsExit();

    // Exit
    return EXIT_SUCCESS;
}

#ifdef __WINRT__
#include <borealis/core/main.hpp>
#endif
