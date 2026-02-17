#pragma once

#include "borealis.hpp"


class Advanced: public brls::Box {
  public:
    explicit Advanced();
    static brls::View* create();
};