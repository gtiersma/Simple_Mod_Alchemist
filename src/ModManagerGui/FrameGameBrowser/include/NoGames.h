#pragma once

#include "borealis.hpp"


class NoGames : public brls::Box {
  public:

    /**
     * @param migrateCb Functionality to call after a mod migration completes
     */
    explicit NoGames(brls::VoidEvent::Callback migrateCb);

  private:
    BRLS_BIND(brls::Button, migrateButton, "migrate");

    brls::VoidEvent::Callback migrateCb;

    /**
     * Builds a dialog to show for after the migration finishes
     */
    brls::Dialog* buildMigrateFinishedDialog();
};