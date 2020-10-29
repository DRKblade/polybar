#pragma once

#include <string>

#include "common.hpp"
#include "utils/color.hpp"
#include "errors.hpp"

POLYBAR_NS

DEFINE_ERROR(color_error);

struct double3 {
  double a, b, c;

  explicit double3(double a, double b, double c) : a(a), b(b), c(c) {}
  explicit double3(const string& str);
  explicit double3(const rgba& src) : a(src.r), b(src.g), c(src.b) {}
  double3() {}

  bool is_near(const double3& other, double tolerance) const;
  void copy_to(rgba& dest) const;
  string to_string() const;
};

namespace colorspaces {
  enum class type {
    none = 0,
		XYZ 		= 0b11111111,
		RGB 		= 0b00000001,
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
  
//  inline type operator==(type a, type b) {
//    return static_cast<type>(static_cast<int>(a) == static_cast<int>(b));
//  }
//  
//  inline type operator!(type a, type b) {
//    return static_cast<type>(static_cast<int>(a) != static_cast<int>(b));
//  }

  class color {
   private:
    type colorspace;
    
   public:
    double3 data;

    color(const rgba& src, type t) : colorspace(t), data(src) {}
    void set_colorspace(type t);
  };


  void xyz_rgb(const double3& input, double3& output, double white_lum = 1);
  void rgb_xyz(const double3& input, double3& output, double white_lum = 1);
  void xyz_jzazbz(const double3& input, double3& output);
  void jzazbz_xyz(const double3& input, double3& output);
  void ab_ch(const double3& input, double3& output);
  void ch_ab(const double3& input, double3& output);
}

POLYBAR_NS_END
