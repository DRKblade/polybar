#pragma once

#include "common.hpp"

POLYBAR_NS

// fwd decl {{{

struct rgb;
struct rgba;

// }}}

// Represent HSL color space colors, with h values in the set [0, 360] and s, l values in the set [0, 1]
struct hsla {
  double h;
  double s;
  double l;
  double a;
  
  explicit hsla() {}
  explicit hsla(double h, double s, double l, double a) : h(h), s(s), l(l), a(a) {}
  explicit hsla(double h, double s, double l) : h(h), s(s), l(l), a(1) {}

  void get_rgb(double& r, double& g, double& b);
  rgb to_rgb();
  rgba to_rgba();
  
  static hsla from_rgba(const rgba& color);
  static bool try_parse(const string& color, hsla& result);
  static hsla get_hsla(const string& color, unsigned int fallback = 0);
  static string convert_hsla(const string& color);
};

using hsla_t = shared_ptr<hsla>;

POLYBAR_NS_END
