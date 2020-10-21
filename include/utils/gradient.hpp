#pragma once

#include <vector>

#include "utils/color.hpp"

POLYBAR_NS

struct color_point {
  hsla color;
  float position;

  explicit color_point(hsla color, float position) : color(color), position(position) {}
};

class gradient {
 public:
  string get_by_percentage(float percentage);
  void add(hsla color, float position);
  static hsla interpolate(const hsla& min, const hsla& max, float percentage);
 
 private:
  vector<color_point> colors;
};

using gradient_t = shared_ptr<gradient>;

POLYBAR_NS_END
