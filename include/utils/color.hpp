#pragma once

#include "utils/colorspaces.hpp"

POLYBAR_NS

DEFINE_ERROR(color_error);

void split_channels(unsigned int color, unsigned char& r, unsigned char& g, unsigned char& b);

struct color {
  double a, b, c, d;
  colorspaces::type colorspace;

	explicit color() : color(0, 0, 0, 0) {}
  explicit color(double a, double b, double c, double d, colorspaces::type colorspace = colorspaces::type::RGB)
  		: a(a), b(b), c(c), d(d), colorspace(colorspace) {}
  explicit color(const string&);

	static color white();
	static color red();
	static color black();
	static color transparent();
	static color parse(const string& value);
	static color parse(const string& value, const color& fallback);
  static bool try_parse(const string& value, color& result);

  void set_colorspace(colorspaces::type t);
  string to_string() const;
  unsigned int to_uint() const;
  string to_hex() const;
  string force_to_hex() const;
};

POLYBAR_NS_END
