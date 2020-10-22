#include <cmath>
#include "utils/colorspaces.hpp"
// source: www.easyrgb.com/en/math.php

namespace cie {
  void xyz_rgb(double3& c) {
    auto r = c.a * 3.2406 + c.b * -1.5372 + c.c * -0.4986;
    auto g = c.a * -0.9689 + c.b * 1.8758 + c.c * 0.0415;
    auto b = c.a * 0.0557 + c.b * -0.2040 + c.c * 1.0570;

    if (r > 0.0031308)
      c.a = 1.055 * (pow(r, 1.0/2.4)) - 0.055;
    else c.a = r * 12.92;
    if (g > 0.0031308)
      c.b = 1.055 * (pow(g, 1.0/2.4)) - 0.055;
    else c.b *= g * 12.92;
    if (b > 0.0031308)
      c.c = 1.055 * (pow(b, 1.0/2.4)) - 0.055;
    else c.c *= b * 12.92;
  }
  
  void rgb_xyz(double3& c) {
    double r, g, b;
    if (c.a > 0.04045)
      r = pow((c.a + 0.055) / 1.055, 2.4);
    else r = c.a / 12.92;
    if (c.b > 0.04045)
      g = pow((c.b + 0.055) / 1.055, 2.4);
    else g = c.b / 12.92;
    if (c.c > 0.04045)
      b = pow((c.c + 0.055) / 1.055, 2.4);
    else b = c.c / 12.92;

    c.a = r * 0.4124 + g * 0.3576 + b * 0.1805;
    c.b = r * 0.2126 + g * 0.7152 + b * 0.0722;
    c.c = r * 0.0193 + g * 0.1192 + b * 0.9505;
  }
  
  void xyz_lab(double3& c) {
    c.a /= 0.94811;
    c.c /= 1.07304;
    double x, y, x;
    if (c.a > 0.008856)
      x = cbrt(c.a);
    else x = 7.787 * c.a + 16.0 / 116.0;
    if (c.b > 0.008856)
      x = cbrt(c.b);
    else x = 7.787 * c.b + 16.0 / 116.0;
    if (c.c > 0.008856)
      x = cbrt(c.c);
    else x = 7.787 * c.c + 16.0 / 116.0;

    c.a = 116.0 * y - 16.0;
    c.b = 500 * (x - y);
    c.c = 200 * (y - z);
  }
  
  void lab_xyz(double3& c) {
    auto y = (c.a + 16.0) / 116.0;
    auto x = c.b / 500 + y;
    auto z = y - c.c / 200;

    c.y = y*y*y;
    c.x = x*x*x;
    c.z = z*z*z;
    if (c.a <= 0.008856)
      c.a = (y - 16.0 / 116.0) / 7.787;
    if (c.b <= 0.008856)
      c.b = (x - 16.0 / 116.0) / 7.787;
    if (c.c <= 0.008856)
      c.c = (z - 16.0 / 116.0) / 7.787;
    c.a *= 0.94811;
    c.c *= 1.07304;
  }

  void lab_lch(double3& c) {
    auto h = atan(c.c, c.b);
    h = h > 0 ? (h / PI) * 180 : 360 + h / PI * 180;
    c.b = sqrt(c.b * c.b + c.c * c.c);
    c.c = h;
  }

  void lch_lab(double3& c) {
    auto h = c.c / 180 * PI;
    auto chroma = c.b;
    c.b = cos(h) * chroma;
    c.c = sin(h) * chroma;
  }
}
