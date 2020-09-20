#include <cmath>

#include "drawtypes/resources/gradient.hpp"
#include "errors.hpp"
#include "components/config.hpp"

POLYBAR_NS

void gradient::add(string color, float percentage) {
  rgba c = rgba::get_rgba(color);
  if (!m_colors.empty() && m_colors.back().position > percentage)
    throw value_error("Position of color points must be in ascending order");
  m_colors.emplace_back(color_point(c, percentage));
}

string gradient::get_by_percentage(float percentage) {
  return get_by_percentage_raw(percentage).to_hex();
}

rgba gradient::get_by_percentage_raw(float percentage) {
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

color gradient::interpolate(const color& min, const color& max, float percentage) {
  color result(min);
  result.a += (max.a - min.a) * percentage / 100.0f;
  result.b += (max.b - min.b) * percentage / 100.0f;
  result.c += (max.c - min.c) * percentage / 100.0f;
  result.d += (max.d - min.d) * percentage / 100.0f;
  return result;
}
void gradient::generate_points(size_t size, colorspaces::type colorspace) {
  // convert all color points to target colorspace
  for (color_point& point : m_colors) {
    point.color.set_colorspace(colorspace);
  }

	// make a new array of color points using interpolation, then convert the points back to RGB
  vector<color_point> new_color;
  for (size_t i = 0; i < size; i++) {
    float percentage = math_util::percentage<float>(i, 0, size-1);
    color_point point(get_by_percentage_raw(percentage), percentage);
    point.set_colorspace(colorspaces::type::RGB);
    new_color.push_back(point);
  }
  m_colors = move(new_color);
}

gradient_t load_gradient(const config& conf, const string& section) {
  // load color points
  auto points = conf.get_list(section, "point");
  vector<color_point> colors;
  float index_multiplier = 100.0f / static_cast<float>(points.size() - 1);
  
  for(size_t i = 0; i < points.size(); i++) {
		float percentage = static_cast<float>(i) * index_multiplier;
		percentage = conf.get(section, "point-" + to_string(i) + "-position", percentage);
		// make sure color points are in ascending order
		if (!colors.empty() && percentage < colors.back().position)
  		throw value_error("Position of color point at "	+ section + "." + "point-" + to_string(i) + "-position must be in ascending order");
		colors.emplace_back(color_point(color::parse(points[i]), percentage));
  }

	auto result = factory_util::shared<gradient>(move(colors));
	// generate finer RGB color points when the interpolation mode isn't RGB
	// this is to approximate color interpolation in other colorspace by RGB interpolation 
	auto colorspace = conf.get(section, "colorspace", colorspaces::type::Jzazbz);
	auto gradient_points = conf.get(section, "approx-count", 10);
	result->generate_points(gradient_points, colorspace);
	return result;
}		

POLYBAR_NS_END
