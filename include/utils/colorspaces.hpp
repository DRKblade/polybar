#pragma once

#include <string>

#include "common.hpp"
#include "utils/color.hpp"

POLYBAR_NS

struct double3 {
  double a, b, c;

  explicit double3(double a, double b, double c) : a(a), b(b), c(c) {}
  explicit double3(const string& str);
  explicit double3(rgba& src) : a(src.r), b(src.g), c(src.b) {}
  double3() {}

  bool is_near(const double3& other, double tolerance) const;
  void copy_to(rgba& dest) const;
  string to_string() const;
};

namespace colorspaces {


  void xyz_rgb(const double3& input, double3& output, double white_lum = 1);
  void rgb_xyz(const double3& input, double3& output, double white_lum = 1);
  void xyz_jzazbz(const double3& input, double3& output);
  void jzazbz_xyz(const double3& input, double3& output);
  void ab_ch(const double3& input, double3& output);
  void ch_ab(const double3& input, double3& output);
}

POLYBAR_NS_END
