#pragma once

#include <switch.h>
#include <borealis.hpp>

#include "ModManager.h"

#include <string>
#include <vector>
#include <map>

/**
 * Used to provide the virtual-scrolling-esque recycler list
 * the data it needs as scrolling occurs
 *
 * Rating input & output are from 0-100 (like how the controller processes them),
 * internal to this class, however,
 * ratings are stored from 0.0-1.0 because that's what the slider control expects.
 */
class RandomDataSource : public brls::RecyclerDataSource
{
  public:

    /**
     * From 0 - 100
     */
    RandomDataSource(
      const std::map<std::string, u8>& ratings,
      u8 modlessRating
    );

    int numberOfRows(brls::RecyclerFrame* recycler, int section) override;
    int numberOfSections(brls::RecyclerFrame* recycler) override;
    std::string titleForHeader(brls::RecyclerFrame* recycler, int section) override;
    float heightForHeader(brls::RecyclerFrame* recycler, int section) override;
    brls::RecyclerCell* cellForRow(brls::RecyclerFrame* recycler, brls::IndexPath index) override;

    /**
     * From 0 - 100
     */
    std::map<std::string, u8> getChangedRatings();

    /**
     * Is something like 255 if the rating never changed (otherwise, from 0 - 100 like normal).
     *
     * Use `hasModlessRatingChanged` to determine if it changed or not.
     */
    u8 getChangedModlessRating();

    /**
     * Whether the user has yet to change the "no mod" option rating (vs leaving it the same)
     */
    bool hasModlessRatingChanged();

  private:

    std::vector<std::string> modNames;

    /**
     * From 0.0 - 1.0
     */
    std::vector<float> fractionalRatings;

    /**
     * Unsaved changes made to ratings are stored here (0.0 - 1.0).
     * Mapped with the mod name as the key.
     */
    std::map<std::string, float> changedFractionals;

    /**
     * Option for using no mod.
     * From 0.0 - 1.0.
     */
    float modlessFractionalRating;

    /**
     * This will remain -1 if not changed. Otherwise, 0.0 - 1.0.
     */
    float changedModlessFractional{-1.0f};
};

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
    BRLS_BIND(brls::RecyclerFrame, list, "list");

    RandomDataSource* dataSource;

    std::map<std::string, u8> ratings;
    u8 modlessRating;
};