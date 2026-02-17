#pragma once

#include "borealis.hpp"


class Backups: public brls::Box {
  public:
    explicit Backups();
    static brls::View* create();
};