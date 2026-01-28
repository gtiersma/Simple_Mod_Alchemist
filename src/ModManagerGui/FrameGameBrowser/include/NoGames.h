#pragma once

#include "borealis.hpp"

#include <note_cell.hpp>


class NoGames : public brls::Box {
  public:
    explicit NoGames();

  private:
    BRLS_BIND(brls::NoteCell, topNote, "top-note");
    BRLS_BIND(brls::Button, migrateButton, "migrate");

    /**
     * Builds a dialog to show for after the migration finishes
     */
    brls::Dialog* buildMigrateFinishedDialog();
};