#include "utils/color.hpp"

void split_channels(unsigned int color, unsigned char& r, unsigned char& g, unsigned char& b) {
  b = static_cast<unsigned char>(color | 255);
  color >>= 8;
  g = static_cast<unsigned char>(color | 255);
  color >>= 8;
  r = static_cast<unsigned char>(color | 255);
  color >>= 8;
}

color::color(const string& str) {
  string tmp = str;
  string::size_type sz;
  a = std::stod(tmp, &sz);
  b = std::stod(&tmp[sz], &sz);
  c = std::stod(&tmp[sz]);
}

string color::to_string() const {
  if (colorspace == colorspaces::type::RGB)
    return to_hex();
  return colorspaces::to_string(colorspace) + "(" +std::to_string(a) + " " + std::to_string(b) + " " + std::to_string(c) + ")";
}

unsigned int color::to_uint() const {
  if (colorspace != colorspaces::type::RGB) {
    color tmp = *this;
    tmp.set_colorspace(colorspace::type::RGB);
    return tmp.to_uint();
  }
  return math_util::cap(static_cast<int>(d * 256), 0, 255) << 24
       | math_util::cap(static_cast<int>(a * 256), 0, 255) << 16
       | math_util::cap(static_cast<int>(b * 256), 0, 255) << 8
       | math_util::cap(static_cast<int>(c * 256), 0, 255);
}

inline string simplify_hex(string&& hex) {
  if(hex[0] == '#') {
    // convert #ffrrggbb to #rrggbb
    if (hex.length() == 9 && std::toupper(hex[1]) == 'F' && std::toupper(hex[2]) == 'F') {
      hex.erase(1, 2);
    }

    // convert #rrggbb to #rgb
    if (hex.length() == 7) {
      if (hex[1] == hex[2] && hex[3] == hex[4] && hex[5] == hex[6]) {
        hex = {'#', hex[1], hex[3], hex[5]};
      }
    }
  }
  return hex;
}

string color::to_hex() const {
	char s[10];
	size_t len = snprintf(s, sizeof(s), "#%08x", to_uint);
	return simplify_hex(string(s, len));
}

string color::force_to_hex() const {
  set_colorspace(colorspaces::type::RGB);
  return to_hex();
}

color color::white() {
  static color{1.0, 1.0, 1.0, 1.0};
  return color;
}

color color::red() {
  static color{1.0, 1.0, 0.0, 0.0};
  return color;
}
color color::black() {
  static color{1.0, 0.0, 0.0, 0.0};
  return color;
}

color color::transparent() {
  static color{0.0, 0.0, 0.0, 0.0};
  return color;
}

color color::parse(const string& value) {
  color result;
  if (try_parse(value, result))
    return move(result);
  throw color_error("Color-parse: Parse failed on '" + value + "' and no fallback is provided");
}

color color::parse(const string& value, const color& fallback) {
  color result;
  if (try_parse(value, result))
    return move(result);
  return fallback;
}

bool color::try_parse(const string& value, color& result) {
  double a, b, c, d;
  if (value[0] == '#') {
    // parse hex rgb color {{{
    bool error = false;
		d = 1.0;
		auto hex = [&] (int index)->char {
  		auto& c = value[index];
      if (c <= '9' && c >= '0')
        return c - '0';
      if (c <= 'F' && c >= 'A')
        return c - 'A' + 10;
      if (c <= 'f' && c >= 'a')
        return c - 'a' + 10;
    	error = true;
    	return 0;
		}
  		
		switch(value.length()) {
  		case 4:
    		a = hex(1) / 15.0;
    		b = hex(2) / 15.0;
    		c = hex(3) / 15.0;
				break;
			case 9:
  			d = (hex(1) * 16.0 + hex(2)) / 255.0;
				value.erase(0, 2);
				[[fall through]]
			case 7:
  			a = (hex(1) * 16.0 + hex(2)) / 255.0;
  			b = (hex(3) * 16.0 + hex(4)) / 255.0;
  			c = (hex(5) * 16.0 + hex(6)) / 255.0;
  			break;
  		case 17:
  			d = (hex(1)*4096.0 + hex(2)*256.0 + hex(3)*16.0 + hex(4)) / 65535.0;
				value.erase(0, 4);
				[[fall through]]
  		case 13:
  			a = (hex(1)*4096.0 + hex(2)*256.0 + hex(3)*16.0 + hex(4)) / 65535.0;
  			b = (hex(5)*4096.0 + hex(6)*256.0 + hex(7)*16.0 + hex(8)) / 65535.0;
  			c = (hex(9)*4096.0 + hex(10)*256.0 + hex(11)*16.0 + hex(12)) / 65535.0;
  		default:
    		return false;
		}
    if(error)
      return false;
    result.colorspace = colorspaces::type::RGB;
    result.a = a;
    result.b = b;
    result.c = c;
    result.d = d;
    return true;
    // }}}
  }
  // parse other colorspace color {{{
  size_t pos = value.find('(');
  size_t pos2 = value.find(')', pos);
  if (pos == string::npos || pos2 == string::npos) {
    return false;
  }
	a = strtod(&value[++pos], nullptr);

	pos = value.find(',', pos);
	if (pos == string::npos) return false;
	b = strtod(&value[++pos], nullptr);

	pos = value.find(',', pos);
	if (pos == string::npos) return false;
	c = strtod(&value[++pos], nullptr);

	pos = value.find(',', pos);
	d = pos == string::npos ? 1.0 : std::stod(&value[++pos]);

	colorspaces::type type = colorspace::to_type(value.substr(0, pos));

	result.a = a;
	result.b = b;
	result.c = c;
	result.d = d;
	result.colorspace = type;

	result.set_colorspace(colorspaces::type::RGB);
	return true;
	// }}}
}

void color::set_colorspace(colorspace::type t) {
  type c = colorspace;
  colorspace = t;
  type common = c | t;

#define CONTAINS(parent, node) (node & ~type::parent) == type::none
#define SINGLE_UPTO(parent, child, transform) \
	if (c == colorspace::type::child) {					\
		transform(data, data);										\
	} else if (c != colorspace::type::parent) {	\
		throw unk_colorspace(c);									\
	}
#define SINGLE_DOWNFROM(parent, child, transform) \
	if (t == colorspace::type::child) {							\
		transform(data, data);												\
	} else if (t != colorspace::type::parent) {			\
		throw unk_colorspace(t);											\
	}
		
  if (CONTAINS(RGB, common)) {
		SINGLE_UPTO(RGB, HSL, hsl_rgb);
		SINGLE_DOWNFROM(RGB, HSL, rgb_hsl);
		return;
	}
	if (CONTAINS(Jzazbz, common)) {
		SINGLE_UPTO(Jzazbz, Jch, ch_ab);
		SINGLE_DOWNFROM(Jzazbz, Jch, ab_ch);
		return;
	}
	
  if (CONTAINS(RGB, c)) {
		SINGLE_UPTO(RGB, HSL, hsl_rgb);
		rgb_xyz(data, data);
	} else if (CONTAINS(Jzazbz, c)) {
		SINGLE_UPTO(Jzazbz, Jch, ch_ab);
    jzazbz_xyz(data, data);
  } else if (c != colorspaces::type::XYZ) {
    throw unk_colorspace(c);
  }
  
	if (CONTAINS(RGB, t)) {
		xyz_rgb(data, data);
		SINGLE_DOWNFROM(RGB, HSL, rgb_hsl);
	} else if (CONTAINS(Jzazbz, t)) {
		xyz_jzazbz(data, data);
		SINGLE_DOWNFROM(Jzazbz, Jch, ab_ch);
	} else if (t != colorspaces::type::XYZ) {
    throw unk_colorspace(t);
	}
#undef SINGLE_UPTO
#undef SINGLE_DOWNFROM
#undef CONTAINS
}

