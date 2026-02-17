#pragma once

#include "borealis.hpp"


class Manage: public brls::Box {
  public:
    explicit Manage();
    static brls::View* create();
};