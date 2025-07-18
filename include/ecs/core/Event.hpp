#pragma once

#include <string>

class Event
{
public:
  virtual ~Event()                      = default;
  virtual std::string to_string() const = 0;
};
