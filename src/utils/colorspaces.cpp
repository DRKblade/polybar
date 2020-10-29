#include <cmath>
#include "utils/colorspaces.hpp"
#include "components/config.hpp"

POLYBAR_NS

double3::double3(const string& str) {
  string tmp = str;
  string::size_type sz;
  a = stod(tmp, &sz);
  b = stod(tmp = tmp.substr(sz), &sz);
  c = stod(tmp.substr(sz));
}

bool double3::is_near(const double3& other, double tolerance) const {
  return std::abs(a - other.a) <= tolerance &&
         std::abs(b - other.b) <= tolerance &&
         std::abs(c - other.c) <= tolerance;
}

string double3::to_string() const {
  return std::to_string(a) + " " + std::to_string(b) + " " + std::to_string(c);
}

void double3::copy_to(rgba& dest) const {
  dest.r = a;
  dest.g = b;
  dest.b = c;
}

namespace colorspaces {

	color_error unk_colorspace(type t) {
    return color_error("Unknown colorspace " + to_string(static_cast<int>(t)));
	}

	void upto_jzazbz(double3& data, type t) {
  	if (t == type::Jch)
    	ch_ab(data, data);
  	else if (t != type::Jzazbz)
    	throw unk_colorspace(t);
	}
	void downfrom_jzazbz(double3& data, type t) {
  	if (t == type::Jch)
    	ab_ch(data, data);
  	else if (t != type::Jzazbz)
    	throw unk_colorspace(t);
	}
	
  void color::set_colorspace(type t) {
    type c = colorspace;
    colorspace = t;
    type common = c | t;
    if (common == type::RGB) return;
		if ((common & ~type::Jzazbz) == type::none) {
  		upto_jzazbz(data, c);
  		downfrom_jzazbz(data, t);
  		return;
		}
    if (c == type::RGB)
			rgb_xyz(data, data);
		else if ((c & ~type::Jzazbz) == type::none) {
			upto_jzazbz(data, c);
      jzazbz_xyz(data, data);
    }
    
		if (t == type::RGB)
  		xyz_rgb(data, data);
		else if ((t & ~type::Jzazbz) == type::none) {
  		xyz_jzazbz(data, data);
  		downfrom_jzazbz(data, t);
		}
  }

  // source: https://observablehq.com/@jrus/srgb#srgb_to_xyz
  // source: https://observablehq.com/@jrus/jzazbz

  inline double gamma(double x) {
    return x > 0.0031308 ? 1.055 * pow(x, 0.4166666666666667) - 0.055
                         : 12.92 * x;
  }
  inline double inverse_gamma(double x) {
    return x > 0.04045 ? pow((x + 0.055) * 0.9478672985781991, 2.4)
                       : 0.07739938080495357 * x;
  }

  void xyz_rgb(const double3& i, double3& o, double white_lum) {
    double r = i.a, g = i.b, b = i.c;
    o.a = gamma(+ 0.03241003232976359  *r - 0.015373989694887858*g - 0.004986158819963629  *b)/ white_lum;
    o.b = gamma(- 0.009692242522025166 *r + 0.01875929983695176 *g + 0.00041554226340084706*b)/ white_lum;
    o.c = gamma(+ 0.0005563941985197545*r - 0.0020401120612391  *g + 0.010571489771875336  *b)/ white_lum;
  }
  
  void rgb_xyz(const double3& i, double3& o, double white_lum) {
    double r = inverse_gamma(i.a) * white_lum, g = inverse_gamma(i.b) * white_lum, b = inverse_gamma(i.c) * white_lum;
    o.a = 41.23865632529916  *r + 35.75914909206253 *g +  18.045049120356364*b;
    o.b = 21.26368216773238  *r + 71.51829818412506 *g +   7.218019648142546*b;
    o.c =  1.9330620152483982*r + 11.919716364020843*g +  95.03725870054352 *b;
  }

  inline double perceptual_quantizer(double x) {
    double XX = pow(x*1e-4, 0.1593017578125);
    return pow((0.8359375 + 18.8515625*XX) / (1 + 18.6875*XX), 134.034375);
  }
  inline double inv_perceptual_quantizer(double x) {
    double XX = pow(x, 7.460772656268214e-03);
    return 1e4 * pow((0.8359375 - XX) / (18.6875*XX - 18.8515625), 6.277394636015326);
  }

  void xyz_jzazbz(const double3& i, double3& o) {
    double Lp = perceptual_quantizer(0.674207838*i.a + 0.382799340*i.b - 0.047570458*i.c);
    double Mp = perceptual_quantizer(0.149284160*i.a + 0.739628340*i.b + 0.083327300*i.c);
    double Sp = perceptual_quantizer(0.070941080*i.a + 0.174768000*i.b + 0.670970020*i.c);
    double Iz = 0.5 * (Lp + Mp);
    o.b = 3.524000*Lp - 4.066708*Mp + 0.542708*Sp;
    o.c = 0.199076*Lp + 1.096799*Mp - 1.295875*Sp;
    o.a = (0.44 * Iz) / (1 - 0.56*Iz) - 1.6295499532821566e-11;
  }

  void jzazbz_xyz(const double3& i, double3& o) {
    double Jz = i.a + 1.6295499532821566e-11;
    double Iz = Jz / (0.44 + 0.56*Jz);
    double L = inv_perceptual_quantizer(Iz + 1.386050432715393e-1*i.b + 5.804731615611869e-2*i.c);
    double M = inv_perceptual_quantizer(Iz - 1.386050432715393e-1*i.b - 5.804731615611891e-2*i.c);
    double S = inv_perceptual_quantizer(Iz - 9.601924202631895e-2*i.b - 8.118918960560390e-1*i.c);
    o.a = + 1.661373055774069e+00*L - 9.145230923250668e-01*M + 2.313620767186147e-01*S;
    o.b = - 3.250758740427037e-01*L + 1.571847038366936e+00*M - 2.182538318672940e-01*S;
    o.c = - 9.098281098284756e-02*L - 3.127282905230740e-01*M + 1.522766561305260e+00*S;
  }

  void ab_ch(const double3& i, double3& o) {
    auto h = atan2(i.c, i.b);
    h = h > 0 ? (h / M_PI) * 180 : 360 + h / M_PI * 180;
    o.b = sqrt(i.b * i.b + i.c * i.c);
    o.c = h;
    o.a = i.a;
  }

  void ch_ab(const double3& i, double3& o) {
    auto h = i.c / 180 * M_PI;
    auto chroma = i.b;
    o.b = cos(h) * chroma;
    o.c = sin(h) * chroma;
    o.a = i.a;
  }
}

POLYBAR_NS_END
