#pragma once

#include "utils/colorspaces.hpp"

POLYBAR_NS

struct bigcolor {
  double a, b, c, d;
  colorspaces::type colorspace;

	bigcolor() : bigcolor(0, 0, 0, 0) {}
  explicit bigcolor(double a, double b, double c, double d, colorspaces::type colorspace = colorspaces::type::RGB)
  		: a(a), b(b), c(c), d(d), colorspace(colorspace) {}
  explicit bigcolor(const string&);

	static bigcolor white();
	static bigcolor red();
	static bigcolor green();
	static bigcolor blue();
	static bigcolor black();
	static bigcolor transparent();
	static bigcolor empty_value();

	static bigcolor parse(const string& value);
	static bigcolor parse(const string& value, const bigcolor& fallback);
  static bool try_parse(const string& value, bigcolor& result);

  void set_colorspace(colorspaces::type t);
  string to_string() const;
  unsigned int to_uint() const;
  unsigned long to_ulong() const;
  unsigned int assert_to_uint() const;
  bool empty() const;
};

struct small_components {
  unsigned char b, g, r, a;
};

union smallcolor {
  small_components comp;
  unsigned int code;

	smallcolor() : smallcolor(0) {}
  smallcolor(const bigcolor& color) : code(color.to_uint()) {}
  smallcolor(unsigned int code) : code(code) {}

	inline bool operator ==(smallcolor other) { return code == other.code; }
	inline bool operator !=(smallcolor other) { return code != other.code; }
	static smallcolor empty_value();
	static smallcolor parse(const string& value);
	static smallcolor parse(const string& value, const smallcolor& fallback);
  static bool try_parse(const string& value, smallcolor& result);

  string to_string() const;
  bool empty() const;
};

POLYBAR_NS_END
