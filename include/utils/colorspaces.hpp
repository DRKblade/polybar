#pragma once

#include <string>

#include "common.hpp"
#include "errors.hpp"

POLYBAR_NS

struct bigcolor;

DEFINE_ERROR(color_error);

namespace colorspaces {

  enum class type {
    none = 0,
		XYZ 		= 0b11111111,
		RGB 		= 0b00001001,
		HSL 		= 0b00000001,
		Jzazbz  = 0b00000110,
		Jch 		= 0b00000010,
  };

	color_error unk_colorspace(type t);
	color_error unk_colorspace(const string& t);
  
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
  type to_type(string&&);

	void rgb_hsl(const bigcolor& input, bigcolor& output);
	void hsl_rgb(const bigcolor& input, bigcolor& output);
  void xyz_rgb(const bigcolor& input, bigcolor& output, double white_lum = 1);
  void rgb_xyz(const bigcolor& input, bigcolor& output, double white_lum = 1);
  void xyz_jzazbz(const bigcolor& input, bigcolor& output);
  void jzazbz_xyz(const bigcolor& input, bigcolor& output);
  void ab_ch(const bigcolor& input, bigcolor& output);
  void ch_ab(const bigcolor& input, bigcolor& output);
}

POLYBAR_NS_END
