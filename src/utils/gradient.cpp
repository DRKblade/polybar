#include <cmath>

#include "utils/gradient.hpp"

POLYBAR_NS

void gradient::add(hsla&& color) {
  colors.emplace_back(move(color));
}

string gradient::get_by_percentage(float percentage) {
  auto intervals = colors.size() - 1;
  auto lower = math_util::cap<size_t>(percentage * intervals / 100.0f, 0, intervals - 1);
  auto interval_percentage = fmod(percentage * intervals, 100.0f);
  return color_util::hex<unsigned short int>(interpolate(colors[lower], colors[lower+1], interval_percentage).to_rgba());
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
