#include <cmath>

#include "utils/gradient.hpp"

POLYBAR_NS

void gradient::add(string color, float percentage) {
  rgba c = rgba::get_rgba(color);
  if (colors.size() == 0 || colors.back().position <= percentage)
    colors.emplace_back(color_point(c, percentage));
  else {
    auto it = colors.begin();
    for (; percentage > it->position; it++);
    colors.emplace(it, color_point(c, percentage));
  }
}

string gradient::get_by_percentage(float percentage) {
  return color_util::hex<unsigned short int>(get_by_percentage_raw(percentage));
}

rgba gradient::get_by_percentage_raw(float percentage) {
  if (colors.size() == 0) {
    throw color_error("Gradient have no color point");
  }
  size_t upper = 0;
  for (; upper < colors.size(); upper++)
    if (colors[upper].position >= percentage) break;
    
  if (upper <= 0) {
    return colors.front().color;
  } else if (upper >= colors.size()) {
    return colors.back().color;
  } else {
    auto interval_percentage = math_util::percentage(percentage, colors[upper-1].position, colors[upper].position);
    return interpolate(colors[upper-1].color, colors[upper].color, interval_percentage);
  }
}

rgba gradient::interpolate(const rgba& min, const rgba& max, float percentage) {
  rgba result(min);
  result.r += (max.r - min.r) * percentage / 100.0f;
  result.g += (max.g - min.g) * percentage / 100.0f;
  result.b += (max.b - min.b) * percentage / 100.0f;
  result.a += (max.a - min.a) * percentage / 100.0f;
  return result;
}
void gradient::generate_points(size_t size, colorspaces::type colorspace) {
  for (color_point& point : colors) {
    colorspaces::color tmp(point.color, colorspaces::type::RGB);
    tmp.set_colorspace(colorspace);
    tmp.data.copy_to(point.color);
  }

  vector<color_point> new_color;
  for (size_t i = 0; i < size; i++) {
    float percentage = math_util::percentage<float>(i, 0, size-1);
    color_point point(get_by_percentage_raw(percentage), percentage);
    colorspaces::color tmp(point.color, colorspace);
    tmp.set_colorspace(colorspaces::type::RGB);
    tmp.data.copy_to(point.color);
    new_color.push_back(point);
  }
  colors = move(new_color);
}


POLYBAR_NS_END
