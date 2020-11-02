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
		if (pos != string::npos) {
  		auto framecount = value.substr(pos,pos2-pos);
  		auto offset = value.substr(++pos2);
  		return factory_util::shared<animated_color>(conf.get_gradient(gradient), stoi(framecount), stoi(offset));
		}
  }
  throw application_error("Invalid animated color string '"+value+"'");
}

unsigned int animated_color::get(size_t frame) {
  auto percentage = math_util::percentage(
    static_cast<float>((m_offset + frame) % m_framecount), 0.0f, static_cast<float>(m_framecount));
  return m_gradient->get_by_percentage_raw(percentage);
}

POLYBAR_NS_END
