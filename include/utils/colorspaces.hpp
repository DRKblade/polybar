#pragma once

#include <string>

#include "common.hpp"

POLYBAR_NS

namespace colorspaces {

  struct double3 {
    double a, b, c;

    explicit double3(double a, double b, double c) : a(a), b(b), c(c) {}
    explicit double3(const string& str);
    double3() {}

    bool is_near(const double3& other, double tolerance) const;
    string to_string() const;
  };

  void xyz_rgb(const double3& input, double3& output, double white_lum = 1);
  void rgb_xyz(const double3& input, double3& output, double white_lum = 1);
  void xyz_jzazbz(const double3& input, double3& output);
  void jzazbz_xyz(const double3& input, double3& output);
  void ab_ch(const double3& input, double3& output);
  void ch_ab(const double3& input, double3& output);
}

POLYBAR_NS_END
