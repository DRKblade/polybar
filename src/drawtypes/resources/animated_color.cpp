#include <cmath>

#include "drawtypes/resources/animated_color.hpp"
#include "utils/factory.hpp"
#include "utils/math.hpp"
#include "errors.hpp"

POLYBAR_NS

animated_color_t parse_animated_color(const config& conf, const string& value) {
  auto pos = value.find(':');
  if (pos != string::npos) {
    auto gradient = value.substr(0, pos);
    auto pos2 = value.find(':', ++pos);
		if (pos2 != string::npos) {
    	return factory_util::shared<animated_color>(
      	conf.get_gradient(gradient), std::stod(&value[pos]), std::stod(&value[pos2 + 1]));
		}
  	return factory_util::shared<animated_color>(
    	conf.get_gradient(gradient), std::stod(&value[pos]));
  }
	return factory_util::shared<animated_color>(
  	conf.get_gradient(value));
}

unsigned int animated_color::get(double time) {
  auto percentage = time / m_duration + m_offset;
  return m_gradient->get_by_percentage_raw(
    static_cast<float>(percentage - (long)percentage) * 100.0f);
}

POLYBAR_NS_END
