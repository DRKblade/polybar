#pragma once

#include "common.hpp"
#include "components/config.hpp"
#include "drawtypes/label.hpp"
#include "drawtypes/labellist.hpp"
#include "utils/mixins.hpp"

POLYBAR_NS

namespace drawtypes {
  class ramp : public labellist {
   public:
    explicit ramp() = default;
    explicit ramp(vector<label_t>&& icons, label_t&& tmplate,
          gradient_t&& fg,
          gradient_t&& bg,
          gradient_t&& ul,
          gradient_t&& ol) : labellist(move(icons), move(tmplate), move(fg), move(bg), move(ul), move(ol)) {}

    void add(label_t&& icon);
    label_t get(size_t index);
    label_t get_by_percentage(float percentage);
    label_t get_by_percentage_with_borders(float percentage, float min, float max);
    label_t get_by_percentage_with_borders(int percentage, int min, int max);
    operator bool();
  };

  using ramp_t = shared_ptr<ramp>;

  ramp_t load_ramp(const config& conf, const string& section, string name, bool required = true);
}

POLYBAR_NS_END
