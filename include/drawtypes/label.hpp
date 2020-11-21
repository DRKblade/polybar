#pragma once

#include <cassert>

#include "common.hpp"
#include "components/config.hpp"
#include "components/types.hpp"
#include "utils/mixins.hpp"
#include "utils/color.hpp"

POLYBAR_NS

namespace drawtypes {
  struct token {
    string token;
    size_t min{0_z};
    size_t max{0_z};
    string suffix{""s};
    bool zpad{false};
  };

  class label : public non_copyable_mixin<label> {
   public:
    smallcolor m_foreground{};
    smallcolor m_background{};
    smallcolor m_underline{};
    smallcolor m_overline{};
    int m_font{0};
    side_values m_padding{0U, 0U};
    side_values m_margin{0U, 0U};

    size_t m_minlen{0};
    /*
     * If m_ellipsis is true, m_maxlen MUST be larger or equal to the length of
     * the ellipsis (3), everything else is a programming error
     *
     * load_label should take care of this, but be aware, if you are creating
     * labels in a different way.
     */
    size_t m_maxlen{0_z};
    alignment m_alignment{alignment::LEFT};
    bool m_ellipsis{true};

    explicit label(string text,
        smallcolor foreground, smallcolor background, smallcolor underline, smallcolor overline,
        int font = 0, struct side_values padding = {0U, 0U}, struct side_values margin = {0U, 0U},
        int minlen = 0, size_t maxlen = 0_z, alignment label_alignment = alignment::LEFT,
        bool ellipsis = true, vector<token>&& tokens = {})
        : m_foreground(foreground)
        , m_background(background)
        , m_underline(underline)
        , m_overline(overline)
        , m_font(font)
        , m_padding(padding)
        , m_margin(margin)
        , m_minlen(minlen)
        , m_maxlen(maxlen)
        , m_alignment(label_alignment)
        , m_ellipsis(ellipsis)
        , m_text(text)
        , m_tokenized(m_text)
        , m_tokens(forward<vector<token>>(tokens)) {
      assert(!m_ellipsis || (m_maxlen == 0 || m_maxlen >= 3));
    }
    explicit label(string text, int font)
    		: label(text, smallcolor::empty_value(), smallcolor::empty_value(),
    		        smallcolor::empty_value(), smallcolor::empty_value(), font) {}
    explicit label(string text) : label(text, 0) {}

    string get() const;
    string get_raw() const;
    operator bool();
    label_t clone();
    void clear();
    void reset_tokens();
    void reset_tokens(const string& tokenized);
    bool has_token(const string& token) const;
    void replace_token(const string& token, string replacement);
    void useas_token(const label_t& container, const string& token);
    void replace_defined_values(const label_t& label);
    void copy_undefined(const label_t& label);

   private:
    string m_text{};
    string m_tokenized{};
    const vector<token> m_tokens{};
  };

  label_t load_label(const config& conf, const string& section, string name,
                     const label_t& fallback, bool required = true, string def = ""s);
  label_t load_label(const config& conf, const string& section, string name
                     );
  label_t load_optional_label(const config& conf, string section, string name,
                              const label_t& fallback, string def = ""s);
  label_t load_optional_label(const config& conf, string section, string name,
                             string def = ""s);
}  // namespace drawtypes

POLYBAR_NS_END
