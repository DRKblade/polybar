#pragma once

#include <vector>

#include "utils/color.hpp"

POLYBAR_NS

class gradient {
 public:
  string get_by_percentage(float percentage);
  void add(hsla&& color);
  static hsla interpolate(const hsla& min, const hsla& max, float percentage);
 
 private:
  vector<hsla> colors;
};

using gradient_t = shared_ptr<gradient>;

POLYBAR_NS_END
