#pragma once

#include <vector>

#include "utils/color.hpp"

POLYBAR_NS

class config;

struct color_point {
  bigcolor value;
  float position;

  explicit color_point(bigcolor&& value, float position) : value(value), position(position) {}
};

class gradient {
  vector<color_point> m_colors;

 public:
  gradient() {}
  gradient(bigcolor&& start, bigcolor&& end);
  gradient(vector<color_point>&& colors) : m_colors(move(colors)) {}

  static bigcolor interpolate(const bigcolor& min, const bigcolor& max, float percentage);

  void add(string color, float position);
  void generate_points(size_t size, colorspaces::type colorspace);
  bool is_valid() const;
  const vector<color_point> get_points() const { return m_colors; }

  bigcolor get_by_percentage_raw(float percentage);
};

using gradient_t = shared_ptr<gradient>;

gradient_t load_gradient(const config& conf, const string& section);

POLYBAR_NS_END
