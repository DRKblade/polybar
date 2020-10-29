#pragma once

#include <vector>

#include "utils/color.hpp"
#include "utils/colorspaces.hpp"

POLYBAR_NS

struct color_point {
  rgba color;
  float position;

  explicit color_point(rgba color, float position) : color(color), position(position) {}
};

class gradient {
 public:
  string get_by_percentage(float percentage);
  rgba get_by_percentage_raw(float percentage);
  void add(string color, float position);
  static rgba interpolate(const rgba& min, const rgba& max, float percentage);
  void generate_points(size_t size, colorspaces::type colorspace);
 
 private:
  vector<color_point> colors;
};

using gradient_t = shared_ptr<gradient>;

POLYBAR_NS_END
