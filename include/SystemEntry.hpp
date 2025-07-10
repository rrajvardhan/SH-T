#pragma once

#include "Overseer.hpp"
#include "Provider.hpp"
#include <functional>
#include <string>

struct SystemEntry
{
  std::string name;
  std::string phase;
  int         order   = 0; // lower = runs earlier
  bool        enabled = true;

  std::function<void(Overseer&, const WorldProvider&)> func;
};
