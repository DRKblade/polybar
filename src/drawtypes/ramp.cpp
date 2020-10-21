#include "drawtypes/ramp.hpp"
#include "utils/factory.hpp"
#include "utils/math.hpp"

POLYBAR_NS

namespace drawtypes {
  void ramp::add(label_t&& icon) {
    m_labels.emplace_back(move(icon));
  }

  label_t ramp::get(size_t index) {
    return m_labels[index];
  }

  label_t ramp::get_by_percentage(float percentage) {
    size_t index = percentage * m_labels.size() / 100.0f;
    auto result = get(math_util::cap<size_t>(index, 0, m_labels.size() - 1));
    apply_gradient(result, percentage);
    return result;
  }

  label_t ramp::get_by_percentage_with_borders(int value, int min, int max) {
    return get_by_percentage_with_borders(static_cast<float>(value), static_cast<float>(min), static_cast<float>(max));
  }

  label_t ramp::get_by_percentage_with_borders(float value, float min, float max) {
    size_t index;
    value = math_util::unbounded_percentage(value, min, max);
    if (value <= 0) {
      index = 0;
    } else if (value >= 100) {
      index = m_labels.size() - 1;
    } else {
      index = value * (m_labels.size() - 2) / 100.0f + 1;
      index = math_util::cap<size_t>(index, 0, m_labels.size() - 1);
    }
    apply_gradient(m_labels[index], value);
    return m_labels[index];
  }

  ramp::operator bool() {
    return !m_labels.empty();
  }

  /**
   * Create a ramp by loading values
   * from the configuration
   */
  ramp_t load_ramp(const config& conf, const string& section, string name, bool required) {
    vector<label_t> vec;
    label_t tmplate;
    gradient_t fg, bg, ul, ol;
    load_labellist(vec, tmplate, fg, bg, ul, ol, conf, section, name, required);
    return factory_util::shared<drawtypes::ramp>(move(vec), move(tmplate), move(fg), move(bg), move(ul), move(ol));
  }
}

POLYBAR_NS_END
