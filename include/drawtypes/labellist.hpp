#pragma once

#include "common.hpp"
#include "components/config.hpp"
#include "drawtypes/label.hpp"
#include "drawtypes/resources/gradient.hpp"
#include "utils/mixins.hpp"

POLYBAR_NS

namespace drawtypes {
  class labellist : public non_copyable_mixin<labellist> {
   public:
    labellist() {}
    explicit labellist(vector<label_t>&& labels, label_t&& tmplate,
          gradient_t&& foreground_grad,
          gradient_t&& background_grad,
          gradient_t&& underline_grad,
          gradient_t&& overline_grad)
      : m_labels(move(labels)),
        m_template(move(tmplate)),
        m_foreground_grad(move(foreground_grad)),
        m_background_grad(move(background_grad)),
        m_underline_grad(move(underline_grad)),
        m_overline_grad(move(overline_grad)) {}
    label_t& get_template();
    void apply_template();

   protected:
    vector<label_t> m_labels;
    label_t m_template;
    gradient_t m_foreground_grad;
    gradient_t m_background_grad;
    gradient_t m_underline_grad;
    gradient_t m_overline_grad;

    void apply_gradient(label_t& label, float percentage);
  };

  using labellist_t = shared_ptr<labellist>;
  void load_labellist(vector<label_t>& labels, label_t& tmplate,
      gradient_t& foreground_grad,
      gradient_t& background_grad,
      gradient_t& underline_grad,
      gradient_t& overline_grad,
      const config& conf, const string& section, string name, bool required = true);
}

POLYBAR_NS_END
