#pragma once

#include "drawtypes/resources/gradient.hpp"
#include "components/config.hpp"

POLYBAR_NS

class animated_color {
 public:
  animated_color(gradient_t&& gradient, size_t framecount = 10, size_t offset = 0)
  	: m_gradient(move(gradient)),
  	  m_framecount(framecount),
  	  m_offset(offset) {}

  unsigned int get(size_t frame);

 protected:
  gradient_t m_gradient;
  size_t m_framecount;
  size_t m_offset;
};

using animated_color_t = shared_ptr<animated_color>;
animated_color_t parse_animated_color(const config& conf, const string& value);

POLYBAR_NS_END
