#pragma once

#include <vector>

#include "utils/color.hpp"

POLYBAR_NS

class config;

struct color_point {
  color value;
  float position;

  explicit color_point(color value, float position) : value(value), position(position) {}
};

class gradient {
 public:
  gradient(vector<color_point>&& colors) : m_colors(move(colors)) {}
  string get_by_percentage(float percentage);
  color get_by_percentage_raw(float percentage);
  void add(string color, float position);
  static color interpolate(const color& min, const color& max, float percentage);
  void generate_points(size_t size, colorspaces::type colorspace);
 
 private:
  vector<color_point> m_colors;

};

using gradient_t = shared_ptr<gradient>;

gradient_t load_gradient(const config& conf, const string& section);

POLYBAR_NS_END
