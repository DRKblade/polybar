#include <cmath>
#include "utils/colorspaces.hpp"
#include "utils/color.hpp"

POLYBAR_NS

namespace colorspaces {
	
  color_error unk_colorspace(colorspaces::type t) {
    return color_error("Unknown colorspace " + to_string(static_cast<int>(t)));
  }

	type to_type(string&& name) {
  	name = string_util::lower(name);
  	if (name == "rgb")
  		return type::RGB;
  	if (name == "hsl")
  		return type::HSL;
  	if (name == "jch")
  		return type::Jch;
  	if (name == "jzazbz")
  		return type::Jzazbz;
  	throw unk_colorspace(name);
	}

	string to_string(type t) {
  	switch (t) {
  		case type::RGB:
  			return "rgb";
  		case type::HSL:
  			return "hsl";
  		case type::Jch:
  			return "jch";
  		case type::Jzazbz:
  			return "jzazbz";
  		default:
    		throw unk_colorspace("");
  	}
	}

	void rgb_hsl(const color& input, color& output) {
    double r = input.a, g = input.b, b = input.c;
    double max = math_util::max(r, math_util::max(g, b)),
    			 min = math_util::min(r, math_util::min(g, b));
    output.c = (max + min) / 2;

    if (max == min) {
      output.a = output.b = 0;
    } else {
      auto d = max - min;
      output.b = output.c > 0.5 ? d / (2 - max - min) : d / (max + min);
      if (max == r)
        output.a = (g - b) / d + (g < b ? 6 : 0);
      else if (max == g)
        output.a = (b - r) / d + 2;
      else output.a = (r - g) / d + 4;
      output.a *= 60;
    }
	}
	
	void hsl_rgb(const color& input, color& output) {
  	double h = input.a, s = input.b, l = input.c;
		if (input.b == 0.0) {
      output.a = output.b = output.c = l;
    } else {
      const auto hue2rgb = [&](double p, double q, double t) {
        if (t < 0.0) t += 360.0;
        if (t > 360.0) t -= 360.0;
        if (t < 60.0) return p + (q - p) * t/60.0;
        if (t < 180.0) return q;
        if (t < 240.0) return p + (q - p) * (240.0 - t)/60.0;
        return p;
      };

      auto q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
      auto p = 2.0 * l - q;
      output.a = hue2rgb(p, q, h + 120.0);
      output.b = hue2rgb(p, q, h);
      output.c = hue2rgb(p, q, h - 120.0);
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

  void xyz_rgb(const color& i, color& o, double white_lum) {
    double x = i.a, y = i.b, z = i.c;
    o.a = gamma(+ 0.03241003232976359  *x - 0.015373989694887858*y - 0.004986158819963629  *z) / white_lum;
    o.b = gamma(- 0.009692242522025166 *x + 0.01875929983695176 *y + 0.00041554226340084706*z) / white_lum;
    o.c = gamma(+ 0.0005563941985197545*x - 0.0020401120612391  *y + 0.010571489771875336  *z) / white_lum;
  }
  
  void rgb_xyz(const color& i, color& o, double white_lum) {
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

  void xyz_jzazbz(const color& i, color& o) {
    double Lp = perceptual_quantizer(0.674207838*i.a + 0.382799340*i.b - 0.047570458*i.c);
    double Mp = perceptual_quantizer(0.149284160*i.a + 0.739628340*i.b + 0.083327300*i.c);
    double Sp = perceptual_quantizer(0.070941080*i.a + 0.174768000*i.b + 0.670970020*i.c);
    double Iz = 0.5 * (Lp + Mp);
    o.b = 3.524000*Lp - 4.066708*Mp + 0.542708*Sp;
    o.c = 0.199076*Lp + 1.096799*Mp - 1.295875*Sp;
    o.a = (0.44 * Iz) / (1 - 0.56*Iz) - 1.6295499532821566e-11;
  }

  void jzazbz_xyz(const color& i, color& o) {
    double Jz = i.a + 1.6295499532821566e-11;
    double Iz = Jz / (0.44 + 0.56*Jz);
    double L = inv_perceptual_quantizer(Iz + 1.386050432715393e-1*i.b + 5.804731615611869e-2*i.c);
    double M = inv_perceptual_quantizer(Iz - 1.386050432715393e-1*i.b - 5.804731615611891e-2*i.c);
    double S = inv_perceptual_quantizer(Iz - 9.601924202631895e-2*i.b - 8.118918960560390e-1*i.c);
    o.a = + 1.661373055774069e+00*L - 9.145230923250668e-01*M + 2.313620767186147e-01*S;
    o.b = - 3.250758740427037e-01*L + 1.571847038366936e+00*M - 2.182538318672940e-01*S;
    o.c = - 9.098281098284756e-02*L - 3.127282905230740e-01*M + 1.522766561305260e+00*S;
  }

  void ab_ch(const color& i, color& o) {
    auto h = atan2(i.c, i.b);
    h = h > 0 ? (h / M_PI) * 180 : 360 + h / M_PI * 180;
    o.b = sqrt(i.b * i.b + i.c * i.c);
    o.c = h;
    o.a = i.a;
  }

  void ch_ab(const color& i, color& o) {
    auto h = i.c / 180 * M_PI;
    auto chroma = i.b;
    o.b = cos(h) * chroma;
    o.c = sin(h) * chroma;
    o.a = i.a;
  }
}

POLYBAR_NS_END
