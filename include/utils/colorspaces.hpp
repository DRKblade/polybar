#pragma once

struct double3 {
  double a, b, c;

  explicit double3(double a, double b, double c) : a(a), b(b), c(c) {}
}

namespace cie {
  void xyz_rgb(double3& color);
  void rgb_xyz(double3& color);
  void xyz_lab(double3& color);
  void lab_xyz(double3& color);
  void lab_lch(double3& color);
  void lch_lab(double3& color);
}
