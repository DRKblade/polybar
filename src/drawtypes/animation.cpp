#include "drawtypes/animation.hpp"
#include "drawtypes/label.hpp"
#include "utils/factory.hpp"

POLYBAR_NS

namespace drawtypes {
  void animation::add(label_t&& frame) {
    m_labels.emplace_back(forward<decltype(frame)>(frame));
    m_framecount = m_labels.size();
    m_frame = m_framecount - 1;
  }

  label_t animation::get() {
    size_t t_frame = m_frame;
    apply_gradient(m_labels[t_frame], math_util::percentage(static_cast<float>(t_frame * m_subframecount + m_subframe), 0.0f, static_cast<float>(m_framecount*m_subframecount)));
    return m_labels[t_frame];
  }

  unsigned int animation::framerate() const {
    return m_framerate_ms;
  }

  animation::operator bool() const {
    return !m_labels.empty();
  }

  void animation::increment() {
    auto t_subframe = m_subframe.load();
    ++t_subframe;
    if (t_subframe >= m_subframecount) {
      auto t_frame = m_frame.load();
      ++t_frame;
      t_frame %= m_framecount;
      t_subframe %= m_subframecount;
      m_frame = t_frame;
    }
    m_subframe = t_subframe;
  }

  /**
   * Create an animation by loading values
   * from the configuration
   */
  animation_t load_animation(const config& conf, const string& section, string name,
                             const label_t& fallback, bool required) {
    name = string_util::ltrim(string_util::rtrim(move(name), '>'), '<');
    vector<label_t> vec;
    label_t tmplate;
    gradient_t fg, bg, ul, ol;
    load_labellist(vec, tmplate, fg, bg, ul, ol, conf, section, name, fallback, required);
    auto framerate = conf.get(section, name + "-framerate", 1000);
    auto subframecount = conf.get(section, name + "-gradient-subframes", 1);
    return factory_util::shared<animation>(move(vec), framerate, subframecount, move(tmplate), move(fg), move(bg), move(ul), move(ol));
  }
}  // namespace drawtypes

POLYBAR_NS_END
