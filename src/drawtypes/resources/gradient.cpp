#include <cmath>

#include "errors.hpp"
#include "components/config.hpp"
#include "drawtypes/resources/gradient.hpp"

POLYBAR_NS

void gradient::add(string color, float percentage) {
  rgba c = rgba::get_rgba(color);
  if (!m_colors.empty() && m_colors.back().position > percentage)
    throw application_error("Position of color points must be in ascending order");
  m_colors.emplace_back(color_point(c, percentage));
}

string gradient::get_by_percentage(float percentage) {
  return color_util::hex<unsigned short int>(get_by_percentage_raw(percentage));
}

unsigned int gradient::get_by_percentage_raw(float percentage) {
  if (m_colors.size() == 0) {
    throw color_error("Gradient have no color point");
  }
  size_t upper = 0;
  for (; upper < m_colors.size(); upper++)
    if (m_colors[upper].position >= percentage) break;
    
  if (upper <= 0) {
    return m_colors.front().color;
  } else if (upper >= m_colors.size()) {
    return m_colors.back().color;
  } else {
    auto interval_percentage = math_util::percentage(percentage, m_colors[upper-1].position, m_colors[upper].position);
    return interpolate(m_colors[upper-1].color, m_colors[upper].color, interval_percentage);
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
  for (color_point& point : m_colors) {
    colorspaces::color tmp(point.color, colorspaces::type::RGB);
    tmp.set_colorspace(colorspace);
    tmp.data.copy_to(point.color);
  }

  vector<color_point> new_color;
  for (size_t i = 0; i < size; i++) {
    float percentage = math_util::percentage<float>(i, 0, size-1);
    color_point point(rgba(get_by_percentage_raw(percentage)), percentage);
    colorspaces::color tmp(point.color, colorspace);
    tmp.set_colorspace(colorspaces::type::RGB);
    tmp.data.copy_to(point.color);
    new_color.push_back(point);
  }
  m_colors = move(new_color);
}

gradient_t load_gradient(const config& conf, const string& section) {
  auto points = conf.get_list(section, "point");
  vector<color_point> colors;
  float index_multiplier = 100.0f / static_cast<float>(points.size() - 1);
  for(size_t i = 0; i < points.size(); i++) {
		float percentage = static_cast<float>(i) * index_multiplier;
		percentage = conf.get(section, "point-" + to_string(i) + "-position", percentage);
		if (!colors.empty() && percentage < colors.back().position)
  		throw application_error("Position of color point at "
  			+ section + "." + "point-" + to_string(i) + "-position must be in ascending order");
		colors.emplace_back(color_point(rgba::get_rgba(points[i]), percentage));
  }

	auto result = factory_util::shared<gradient>(move(colors));
	auto colorspace = conf.get(section, "colorspace", colorspaces::type::Jzazbz);
	if (colorspace != colorspaces::type::RGB) {
		auto gradient_points = conf.get(section, "approx-count", 10);
		result->generate_points(gradient_points, colorspace);
	}
	return result;
}		

POLYBAR_NS_END
