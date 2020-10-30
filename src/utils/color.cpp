#include "utils/color.hpp"
#include "utils/string.hpp"

POLYBAR_NS

namespace color_util {
  unsigned int parse(string value, unsigned int fallback) {
    if (value.empty()) return fallback;
    if (value[0] == '#') {
      if ((value = parse_hex(value)).empty())
        return fallback;
      return std::strtoul(&value[1], nullptr, 16);
    }
    size_t pos = value.find('(');
    size_t pos2 = value.find(')', pos);
    if (pos == string::npos || pos2 == string::npos) {
      return fallback;
    } else {
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
			rgba result(0);
			colorspaces::color c(result, type);
			c.data.a = stod(value = value.substr(++pos));
			
			pos = value.find(',');
			if (pos == string::npos) return fallback;
			c.data.b = stod(value = value.substr(++pos));
			
			pos = value.find(',');
			if (pos == string::npos) return fallback;
			c.data.c = stod(value = value.substr(++pos));
			
			pos = value.find(',');
			result.a = pos == string::npos ? 1.0 : stod(value.substr(++pos));

			c.set_colorspace(colorspaces::type::RGB);
			c.data.copy_to(result);
			return result;
    }
  }
}

POLYBAR_NS_END
