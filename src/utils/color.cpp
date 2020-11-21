#include <sstream>

#include "utils/color.hpp"
#include "utils/math.hpp"

POLYBAR_NS

using namespace colorspaces;

bigcolor::bigcolor(const string& str) {
  char* tmp;
  a = std::strtod(str.c_str(), &tmp);
  b = std::strtod(tmp, &tmp);
  c = std::strtod(tmp, &tmp);
  d = 1;
}

string bigcolor::to_string() const {
  // When this is called for value from empty_value, it will output "rgb(1, 1, 1, 0)", which is intentional
  if (colorspace == type::RGB)
    return smallcolor(to_uint()).to_string();
  std::stringstream ss;
  ss << colorspaces::to_string(colorspace) << '(' << a << ", " << b << ", " << c;
  if (d == 1)
    ss << ')';
  else ss << ", " << d << ')';
  return ss.str();
}

unsigned int bigcolor::to_uint() const {
  if (colorspace != type::RGB) {
    bigcolor tmp = *this;
    tmp.set_colorspace(type::RGB);
    return tmp.to_uint();
  }
  return math_util::cap(static_cast<unsigned int>(d * 256), 0U, 0xFFU) << 24
       | math_util::cap(static_cast<unsigned int>(a * 256), 0U, 0xFFU) << 16
       | math_util::cap(static_cast<unsigned int>(b * 256), 0U, 0xFFU) << 8
       | math_util::cap(static_cast<unsigned int>(c * 256), 0U, 0xFFU);
}

unsigned long bigcolor::to_ulong() const {
  if (colorspace != type::RGB) {
    bigcolor tmp = *this;
    tmp.set_colorspace(type::RGB);
    return tmp.to_ulong();
  }
  return math_util::cap(static_cast<unsigned long>(d * 65536), 0UL, 0xFFFFUL) << 48
       | math_util::cap(static_cast<unsigned long>(a * 65536), 0UL, 0xFFFFUL) << 32
       | math_util::cap(static_cast<unsigned long>(b * 65536), 0UL, 0xFFFFUL) << 16
       | math_util::cap(static_cast<unsigned long>(c * 65536), 0UL, 0xFFFFUL);
}

#define STATIC_COLOR(name, a, b, c, d) \
  bigcolor bigcolor::name() { \
    static bigcolor color(a, b, c, d, type::RGB); \
    return color; \
  }

STATIC_COLOR(white,				1, 1, 1, 1)
STATIC_COLOR(red,					1, 0, 0, 1)
STATIC_COLOR(green,				0, 1, 0, 1)
STATIC_COLOR(blue,				0, 0, 1, 1)
STATIC_COLOR(black,				0, 0, 0, 1)
STATIC_COLOR(transparent,	0, 0, 0, 0)

#undef STATIC_COLOR

bigcolor bigcolor::parse(const string& value) {
  bigcolor result;
  if (!try_parse(value, result)) {
    if (value.empty())
      return empty_value();
    else
    	throw application_error("Parse-bigcolor: Parsing failed, no fallback provided");
  }
  return result;
}

bigcolor bigcolor::parse(const string& value, const bigcolor& fallback) {
  bigcolor result;
  if (try_parse(value, result))
    return result;
  return fallback;
}

int parse_color_code(const string& value, unsigned int& a, unsigned int& b,
                     unsigned int& c, unsigned int& d) {
  bool error = false;
  int divider;
  auto str = value.c_str();
	auto hex = [&] (int index)->unsigned int {
		auto& c = str[index];
    if (c <= '9' && c >= '0')
      return c - '0';
    if (c <= 'F' && c >= 'A')
      return c - 'A' + 10;
    if (c <= 'f' && c >= 'a')
      return c - 'a' + 10;
  	error = true;
  	return 0;
	};

	switch(value.length()) {
  	case 5:
    	d = hex(1);
    	str++;
    	if (d == 0) {
      	a = b = c = 0;
      	break;
		case 4:
  			d = 15;
    	}
  		a = hex(1);
  		b = hex(2);
  		c = hex(3);
  		divider = 15;
			break;
		case 9:
			d = (hex(1) * 16 + hex(2));
			str += 2;
			if (d == 0) {
  			a = b = c = 0;
  			break;
		case 7:
    		d = 255;
			}
			a = hex(1) * 16 + hex(2);
			b = hex(3) * 16 + hex(4);
			c = hex(5) * 16 + hex(6);
			divider = 255;
			break;
		case 17:
			d = hex(1)*4096 + hex(2)*256 + hex(3)*16 + hex(4);
			str += 4;
			if (d == 0) {
  			a = b = c = 0;
  			break;
		case 13:
  			d = 65535;
			}
			a = hex(1)*4096 + hex(2)*256 + hex(3)*16 + hex(4);
			b = hex(5)*4096 + hex(6)*256 + hex(7)*16 + hex(8);
			c = hex(9)*4096 + hex(10)*256 + hex(11)*16 + hex(12);
			divider = 65535;
			break;
		default:
  		return 0;
	}
  if(error)
    return 0;
  return divider;
}

bool bigcolor::empty() const {
  return a == 1 && b == 1 && c == 1 && d == 0;
}

bigcolor bigcolor::empty_value() {
  return bigcolor(1, 1, 1, 0);
}

bool bigcolor::try_parse(const string& value, bigcolor& result) {
  if (value.empty())
    return false;
  if (value[0] == '#') {
    unsigned int a, b, c, d;
    double divider = parse_color_code(value, a, b, c, d);
    if (!divider)
      return false;
    result.colorspace = type::RGB;
    result.a = a / divider;
    result.b = b / divider;
    result.c = c / divider;
    result.d = d / divider;
    return true;
  } else {
    double a, b, c, d;
    size_t pos = value.find('(');
    size_t pos2 = value.find(')', pos);
    if (pos == string::npos || pos2 == string::npos) {
      return false;
    }
  	type type = to_type(value.substr(0, pos));

  	a = strtod(&value[++pos], nullptr);

  	pos = value.find(',', pos);
  	if (pos == string::npos) return false;
  	b = strtod(&value[++pos], nullptr);

  	pos = value.find(',', pos);
  	if (pos == string::npos) return false;
  	c = strtod(&value[++pos], nullptr);

  	pos = value.find(',', pos);
  	d = pos == string::npos ? 1.0 : std::stod(&value[++pos]);

  	result.a = a;
  	result.b = b;
  	result.c = c;
  	result.d = d;
  	result.colorspace = type;

  	return true;
  }
}

void bigcolor::set_colorspace(type t) {
  type c = colorspace;
  colorspace = t;
  type common = c | t;
  auto& data = *this;

#define CONTAINS(parent, node) (node & ~type::parent) == type::none
#define SINGLE_UPTO(parent, child, transform) \
	if (c == type::child) { \
		transform(data, data);	\
	} else if (c != type::parent) { \
		throw unk_colorspace(c); \
	}
#define SINGLE_DOWNFROM(parent, child, transform) \
	if (t == type::child) { \
		transform(data, data);	\
	} else if (t != type::parent) { \
		throw unk_colorspace(t); \
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

bool smallcolor::empty() const {
  return comp.a == 0 && comp.r == 255 && comp.g == 255 && comp.b == 255;
}

smallcolor smallcolor::empty_value() {
  return smallcolor(0x00FFFFFF);
}

bool smallcolor::try_parse(const string& value, smallcolor& result) {
  if (value.empty())
    return false;
  if (value[0] != '#') {
    bigcolor tmp;
    if (!bigcolor::try_parse(value, tmp))
      return false;
    result.code = tmp.to_uint();
    return true;
  }
  unsigned int a, b, c, d;
  auto divider = parse_color_code(value, a, b, c, d);
  if (divider == 0 || divider > 255)
    return false;
  result.comp.r = a * 255 / divider;
  result.comp.g = b * 255 / divider;
  result.comp.b = c * 255 / divider;
  result.comp.a = d * 255 / divider;
  return true;
}

smallcolor smallcolor::parse(const string& value) {
  smallcolor tmp;
  if (!try_parse(value, tmp)) {
    if (value.empty())
      return empty_value();
    else
    	throw application_error("Parse-smallcolor: Parsing failed, no fallback provided");
  }
  return tmp;
}

smallcolor smallcolor::parse(const string& value, const smallcolor& fallback) {
  smallcolor tmp;
  if (!try_parse(value, tmp))
    return fallback;
  return tmp;
}

inline string simplify_hex(string&& hex) {
  if(hex[0] == '#') {
    // convert #ffrrggbb to #rrggbb
    if (hex.length() == 9) {
      if (std::toupper(hex[1]) == 'F' && std::toupper(hex[2]) == 'F')
        hex.erase(1, 2);
      else if (hex[1] == hex[2] && hex[3] == hex[4] && hex[5] == hex[6] && hex[7] == hex[8])
        hex = {'#', hex[1], hex[3], hex[5], hex[7]};
    }

    // convert #rrggbb to #rgb
    if (hex.length() == 7) {
      if (hex[1] == hex[2] && hex[3] == hex[4] && hex[5] == hex[6]) {
        hex = {'#', hex[1], hex[3], hex[5]};
      }
    }
  }
  return forward<string>(hex);
}

string smallcolor::to_string() const {
  // When this is called for value from empty_value, it will output "#00FFFFFF", which is intentional
	char s[10];
	size_t len = snprintf(s, sizeof(s), "#%08x", code);
	return simplify_hex(string(s, len));
}

POLYBAR_NS_END
