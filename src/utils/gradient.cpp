#include <cmath>

#include "utils/gradient.hpp"

POLYBAR_NS

void gradient::add(hsla color, float percentage) {
  if (colors.size() == 0 || colors.back().position <= percentage)
    colors.emplace_back(color_point(color, percentage));
  else {
    auto it = colors.begin();
    for (; percentage > it->position; it++);
    colors.emplace(it, color_point(color, percentage));
  }
}

string gradient::get_by_percentage(float percentage) {
  if (colors.size() == 0) return "";
  hsla result;
  size_t upper = 0;
  for (; upper < colors.size(); upper++)
    if (colors[upper].position >= percentage) break;
    
  if (upper <= 0) {
    result = colors.front().color;
  } else if (upper >= colors.size()) {
    result = colors.back().color;
  } else {
    auto interval_percentage = math_util::percentage(percentage, colors[upper-1].position, colors[upper].position);
    result = interpolate(colors[upper-1].color, colors[upper].color, interval_percentage);
  }
  return color_util::hex<unsigned short int>(result.to_rgba());
}

hsla gradient::interpolate(const hsla& min, const hsla& max, float percentage) {
  hsla result;
  result.h = min.h + (max.h - min.h) * percentage / 100.0f;
  result.s = min.s + (max.s - min.s) * percentage / 100.0f;
  result.l = min.l + (max.l - min.l) * percentage / 100.0f;
  result.a = min.a + (max.a - min.a) * percentage / 100.0f;
  return result;
}

POLYBAR_NS_END
