#include "utils/color.hpp"
#include "utils/string.hpp"

POLYBAR_NS

bool rgba::parse(const string& value, rgba& result) {
  size_t pos = value.find('(');
  size_t pos2 = value.find(')', pos);
  if (pos == string::npos || pos2 == string::npos) {
    return false;
  }
	auto colorspace = string_util::lower(value.substr(0, pos));
	colorspaces::type type;
	if (colorspace == "hsl")
		type = colorspaces::type::HSL;
	else if (colorspace == "rgb")
		type = colorspaces::type::RGB;
	else if (colorspace == "jch")
		type = colorspaces::type::Jch;
	else if (colorspace == "jzazbz")
		type = colorspaces::type::Jzazbz;
	else throw color_error("Unknown colorspace type '" + colorspace + "'");
	
	colorspaces::color c(result, type);
	c.data.a = strtod(&value[++pos], nullptr);

	pos = value.find(',', pos);
	if (pos == string::npos) return false;
	c.data.b = strtod(&value[++pos], nullptr);

	pos = value.find(',', pos);
	if (pos == string::npos) return false;
	c.data.c = strtod(&value[++pos], nullptr);

	pos = value.find(',', pos);
	result.a = pos == string::npos ? 1.0 : stod(value.substr(++pos));

	c.set_colorspace(colorspaces::type::RGB);
	c.data.copy_to(result);
	return true;
}
  
namespace color_util {
  /**
   * Convert the hexadecimal color code in `hex` from #rgb and #rrggbb to the format #aarrggbb
   * Returns whether the conversion succeded
   * Throws an exception when `hex` starts with "#" but is not a valid color code.
   * `result` is untouched when the method return false
   */
  bool parse_hex(string& hex) {
    if (hex[0] == '#') {
      if (hex.length() == 4)
        hex = {'#', hex[1], hex[1], hex[2], hex[2], hex[3], hex[3]};
      if (hex.length() == 7)
        hex = "#ff" + hex.substr(1);
      if (hex.length() == 9) 
        return true;
      throw application_error("Invalid color code '" + hex + "'");
    }
    return false;
  }
	string colorspace_torgb(string&& value) {
		rgba color{0};
    if (!value.empty() && value[0] != '#' && rgba::parse(value, color))
  		return hex<unsigned short int>(color);
    return move(value);
  }
  unsigned int parse(const string& value) {
   	unsigned int result;
   	if (check(value, result))
     	return result;
   	throw application_error("Parsed value is empty and a fallback is not provided");
  }
  unsigned int parse(const string& value, unsigned int fallback) {
   	unsigned int result;
   	if (check(value, result))
     	return result;
		return fallback;
  }

  /**
   * Check `value` for color codes:
   * If the color code is valid, convert it, set `result` to the color value and return true.
   * If there is no color code, return false
   * If the color code is invalid, throw an exception.
   */
  bool check(string value, unsigned int& result) {
		if (value.empty()) {
			return false;
		} else {
      if (parse_hex(value)) {
        result = std::strtoul(&value[1], nullptr, 16);
        return true;
      } else {
    		rgba color{0};
    		if (rgba::parse(value, color)) {
      		result = color;
      		return true;
    		}
      }
		}
		result = 0xffff0000;
		return true;
		throw application_error("Invalid color code '" + value + "'");
  }
}

POLYBAR_NS_END
