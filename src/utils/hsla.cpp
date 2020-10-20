#include <regex>
#include <vector>

#include "utils/math.hpp"
#include "utils/hsla.hpp"
#include "utils/color.hpp"

POLYBAR_NS

void hsla::get_rgb(double& r, double& g, double& b) {
  if (s == 0.0) {
    r = g = b = l;
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
    r = hue2rgb(p, q, h + 120.0);
    g = hue2rgb(p, q, h);
    b = hue2rgb(p, q, h - 120.0);
  }
}

rgb hsla::to_rgb() {
  double r, g, b;
  get_rgb(r, g, b);
  return rgb(r, g, b);
}

rgba hsla::to_rgba() {
  double r, g, b;
  get_rgb(r, g, b);
  return rgba(r, g, b, a);
}

hsla hsla::from_rgba(const rgba& color) {
  const double& r = color.r, g = color.g, b = color.b;
  double max = math_util::max(r, math_util::max(g, b)), min = math_util::min(r, math_util::min(g, b));
  double h, s, l = (max + min) / 2;

  if (max == min) {
    h = s = 0;
  } else {
    auto d = max - min;
    s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
    if (max == r)
      h = (g - b) / d + (g < b ? 6 : 0);
    else if (max == g)
      h = (b - r) / d + 2;
    else h = (r - g) / d + 4;
    h *= 60;
  }

  return hsla(h, s, l, color.a);
}

bool hsla::try_parse(const string& color, hsla& result) {
  if (color.rfind("hsla") == 0) {
    std::regex rgx(R"(hsla\( *([0-9\.]+), *([0-9\.]+), *([0-9\.]+), *([0-9\.]+) *\))");
    std::smatch match;
    if (std::regex_search(color, match, rgx)) {
      result.h = stod(match[1].str());
      result.s = stod(match[2].str());
      result.l = stod(match[3].str());
      result.a = stod(match[4].str());
      return true;
    }
  } else if (color.rfind("hsl") == 0) {
    std::regex rgx(R"(hsl\( *([0-9\.]+), *([0-9\.]+), *([0-9\.]+) *\))");
    std::smatch match;
    if (std::regex_search(color, match, rgx)) {
      result.h = stod(match[1].str());
      result.s = stod(match[2].str());
      result.l = stod(match[3].str());
      result.a = 1;
      return true;
    }
  }
  return false;
}

hsla hsla::get_hsla(const string& color, unsigned int fallback) {
  hsla result;
  if(hsla::try_parse(color, result)) {
    return result;
  }
  return hsla::from_rgba(rgba(color_util::parse(color, fallback)));
}

string hsla::convert_hsla(const string& color) {
  hsla parsed;
  if (try_parse(color, parsed)) {
    return color_util::hex<unsigned short int>(parsed.to_rgba());
  }
  return color;
}


POLYBAR_NS_END
