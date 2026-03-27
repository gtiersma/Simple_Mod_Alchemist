#pragma once

#include <switch.h>
#include <borealis.hpp>

#include "ModManager.h"

#include <string>
#include <vector>
#include <map>



/**
 * UI for changing how likely each mod is to be picked when the source is changed at random.
 */
class RandomSettings : public brls::Box
{
  public:
    explicit RandomSettings();

    /**
     * Any unsaved changes made to ratings.
     * Mapped with the mod name as the key.
     */
    std::map<std::string, u8> getChangedRatings();

    /**
     * An unsaved change to the rating for using no mod (if there is an unsaved change).
     * I think it ends up being 255 if not changed (otherwise, 0 - 100 like normal).
     * Use "hasModlessRatingChanged" before calling this to see if it has an actual rating.
     */
    u8 getChangedModlessRating();

    /**
     * Whether the user has yet to change the "no mod" option rating (vs leaving it the same)
     */
    bool hasModlessRatingChanged();

    /**
     * Displays this UI in a close-able dialog that's shown.
     */
    static void showInDialog();

    static RandomSettings* create();

  private:
    BRLS_BIND(brls::Label, title, "title");
    BRLS_BIND(brls::Box, list, "list");

    std::map<std::string, u8> ratings;
    u8 modlessRating;

    // Only stores changes made, without converting the values to u8.
    // Conversion occurs in the "get" accessor methods.
    std::map<std::string, float> changedRatings;
    float changedModlessRating = -1.0f;

    void buildUi();
    void buildRating(const std::string& name, u8 rating, const std::function<void(float)>& updateFn);
};