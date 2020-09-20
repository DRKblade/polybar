#pragma once

#include <string>

#include "common.hpp"
#include "errors.hpp"

POLYBAR_NS

struct color;

namespace colorspaces {
  enum class type {
    none = 0,
		XYZ 		= 0b11111111,
		RGB 		= 0b00001001,
		HSL 		= 0b00000001,
		Jzazbz  = 0b00000110,
		Jch 		= 0b00000010,
  };

  inline type operator~(type t) {
    return static_cast<type>(~static_cast<int>(t));
  }

  inline type operator&(type a, type b) {
    return static_cast<type>(static_cast<int>(a) & static_cast<int>(b));
  }
  
  inline type operator|(type a, type b) {
    return static_cast<type>(static_cast<int>(a) | static_cast<int>(b));
  }

  string to_string(type);
  type to_type(string);

	void rgb_hsl(const color& input, color& output);
	void hsl_rgb(const color& input, color& output);
  void xyz_rgb(const color& input, color& output, double white_lum = 1);
  void rgb_xyz(const color& input, color& output, double white_lum = 1);
  void xyz_jzazbz(const color& input, color& output);
  void jzazbz_xyz(const color& input, color& output);
  void ab_ch(const color& input, color& output);
  void ch_ab(const color& input, color& output);
}

POLYBAR_NS_END
