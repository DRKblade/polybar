#pragma once

#include <vector>

#include "utils/color.hpp"
#include "utils/colorspaces.hpp"

POLYBAR_NS

class config;

struct color_point {
  rgba color;
  float position;

  explicit color_point(rgba color, float position) : color(color), position(position) {}
};

class gradient {
 public:
  gradient(vector<color_point>&& colors) : m_colors(move(colors)) {}
  string get_by_percentage(float percentage);
  unsigned int get_by_percentage_raw(float percentage);
  void add(string color, float position);
  static rgba interpolate(const rgba& min, const rgba& max, float percentage);
  void generate_points(size_t size, colorspaces::type colorspace);
 
 protected:
  vector<color_point> m_colors;

};

using gradient_t = shared_ptr<gradient>;

gradient_t load_gradient(const config& conf, const string& section);

POLYBAR_NS_END
