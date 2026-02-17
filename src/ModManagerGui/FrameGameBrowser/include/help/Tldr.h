#pragma once

#include "borealis.hpp"


class Tldr: public brls::Box {
  public:
    explicit Tldr();
    static brls::View* create();
};