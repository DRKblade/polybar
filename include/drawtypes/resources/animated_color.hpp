#pragma once

#include "drawtypes/resources/gradient.hpp"
#include "components/config.hpp"

POLYBAR_NS

class animated_color {
 public:
  animated_color(gradient_t&& gradient, double duration = 2, double offset = 0)
  	: m_gradient(move(gradient)),
  	  m_duration(duration),
  	  m_offset(offset) {}

  bigcolor get(double time);

 protected:
  gradient_t m_gradient;
  double m_duration;
  double m_offset;
};

using animated_color_t = shared_ptr<animated_color>;
animated_color_t parse_animated_color(const config& conf, const string& value);

POLYBAR_NS_END
