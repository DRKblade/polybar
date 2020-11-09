#include "components/builder.hpp"

#include <utility>

#include "drawtypes/label.hpp"
#include "utils/color.hpp"
#include "utils/string.hpp"
#include "utils/time.hpp"
POLYBAR_NS

builder::builder(const bar_settings& bar) : m_bar(bar) {
  clear();
}

builder::make_type builder::make(const bar_settings& bar) {
  return *factory_util::singleton<std::remove_reference_t<builder::make_type>>(bar);
}

void builder::clear() {
  /* Add all values as keys so that we never have to check if a key exists in
   * the map
   */
  m_tags.clear();
  m_tags[syntaxtag::NONE] = 0;
  m_tags[syntaxtag::A] = 0;
  m_tags[syntaxtag::B] = 0;
  m_tags[syntaxtag::F] = 0;
  m_tags[syntaxtag::T] = 0;
  m_tags[syntaxtag::R] = 0;
  m_tags[syntaxtag::o] = 0;
  m_tags[syntaxtag::u] = 0;
  m_tags[syntaxtag::P] = 0;

  m_colors.clear();
  m_colors[syntaxtag::B] = string();
  m_colors[syntaxtag::F] = string();
  m_colors[syntaxtag::o] = string();
  m_colors[syntaxtag::u] = string();

  m_attrs.clear();
  m_attrs[attribute::NONE] = false;
  m_attrs[attribute::UNDERLINE] = false;
  m_attrs[attribute::OVERLINE] = false;

  m_output.clear();
  m_fontindex = 1;
}

/**
 * Flush contents of the builder and return built string
 *
 * This will also close any unclosed tags
 */
string builder::flush() {
  if (m_tags[syntaxtag::T]) {
    font_close();
  }

  while (m_tags[syntaxtag::A]) {
    cmd_close();
  }

  string output{m_output};

  m_output.clear();

  return output;
}

/**
 * Insert raw text string
 */
void builder::append(string text) {
  m_output.reserve(text.size());
  m_output += move(text);
}

/**
 * Insert text node
 *
 * This will also parse raw syntax tags
 */
void builder::node(string str) {
  if (str.empty()) {
    return;
  }

  append(move(str));
}

/**
 * Insert text node with specific font index
 *
 * \see builder::node
 */
void builder::node(string str, int font_index) {
  font(font_index);
  node(move(str));
  font_close();
}

/**
 * Insert tags for given label
 */
void builder::node(const label_t& label, bool keep_colors) {
  if (!label || !*label) {
    return;
  }

  auto text = label->get();

  if (label->m_margin.left > 0) {
    space(label->m_margin.left);
  }

  line_color(label->m_underline, attribute::UNDERLINE);
  line_color(label->m_overline, attribute::OVERLINE);

  color(label->m_background, syntaxtag::B);
  color(label->m_foreground, syntaxtag::F);

  if (label->m_padding.left > 0) {
    space(label->m_padding.left);
  }

  node(text, label->m_font);

  if (label->m_padding.right > 0) {
    space(label->m_padding.right);
  }

	if (!keep_colors) {
    m_colors_closing[syntaxtag::B] = true;
    m_colors_closing[syntaxtag::F] = true;
    m_colors_closing[syntaxtag::u] = true;
    m_colors_closing[syntaxtag::o] = true;
	}

  if (label->m_margin.right > 0) {
    space(label->m_margin.right);
  }
}

/**
 * Repeat text string n times
 */
void builder::node_repeat(const string& str, size_t n) {
  string text;
  text.reserve(str.size() * n);
  while (n--) {
    text += str;
  }
  node(text);
}

/**
 * Repeat label contents n times
 */
void builder::node_repeat(const label_t& label, size_t n) {
  string text;
  string label_text{label->get()};
  text.reserve(label_text.size() * n);
  while (n--) {
    text += label_text;
  }
  label_t tmp{new label_t::element_type{text}};
  tmp->replace_defined_values(label);
  node(tmp);
}

/**
 * Insert tag that will offset the contents by given pixels
 */
void builder::offset(int pixels) {
  if (pixels == 0) {
    return;
  }
  tag_open(syntaxtag::O, to_string(pixels));
}

/**
 * Insert spaces
 */
void builder::space(size_t width) {
  if (width) {
    for (size_t i = 0; i < width; i++)
      m_output.append(m_bar.space_unit);
  } else {
    space();
  }
}
void builder::space() {
  for (int i = 0; i < m_bar.spacing; i++)
    m_output.append(m_bar.space_unit);
}

/**
 * Remove trailing space
 */
void builder::remove_trailing_space(size_t len) {
  if (len == 0_z || len > m_output.size()) {
    return;
  } else if (m_output.substr(m_output.size() - len) == string(len, ' ')) {
    m_output.erase(m_output.size() - len);
  }
}
void builder::remove_trailing_space() {
  remove_trailing_space(m_bar.spacing);
}

/**
 * Insert tag to alter the current font index
 */
void builder::font(int index) {
  if (index == 0) {
    return;
  }
  m_fontindex = index;
  tag_open(syntaxtag::T, to_string(index));
}

/**
 * Insert tag to reset the font index
 */
void builder::font_close() {
  m_fontindex = 1;
  tag_close(syntaxtag::T);
}

/**
 * Insert tag to alter the current color
 */
void builder::color(string color, syntaxtag tag) {
  if (color.empty()) {
    if (m_colors_closing[tag]) {
      tag_close(tag);
      m_colors[tag].clear();
      m_colors_closing[tag] = false;
    }
  } else if (color != m_colors[tag]) {
    m_colors[tag] = color;
    m_colors_closing[tag] = false;
    color = color_util::simplify_hex(color);
    tag_open(tag, color);
  }
}

void builder::line_color(string color, attribute attr) {
  auto tag = static_cast<syntaxtag>(static_cast<int>(attr));
  if (color.empty()) {
    if (m_colors_closing[tag]) {
      tag_close(attr);
      m_colors[tag].clear();
      m_colors_closing[tag] = false;
    }
  } else if (color != m_colors[tag]) {
    m_colors[tag] = color;
    m_colors_closing[tag] = false;
    color = color_util::simplify_hex(color);
    tag_open(tag, color);
    tag_open(attr);
  }
}

/**
 * Insert tag to alter the current overline/underline color
 */
void builder::line_color(const string& color) {
  line_color(color, attribute::UNDERLINE);
  line_color(color, attribute::OVERLINE);
}

/**
 * Close overline/underline color tag
 */
void builder::line_color_close() {
  tag_close(attribute::UNDERLINE);
  tag_close(attribute::OVERLINE);
}

/**
 * Add a polybar control tag
 */
void builder::control(controltag tag) {
  string str;
  switch (tag) {
    case controltag::R:
      str = "R";
      break;
    default:
      break;
  }

  if (!str.empty()) {
    tag_open(syntaxtag::P, str);
  }
}

/**
 * Open command tag
 */
void builder::cmd(mousebtn index, string action) {
  if (!action.empty()) {
    action = string_util::replace_all(action, ":", "\\:");
    tag_open(syntaxtag::A, to_string(static_cast<int>(index)) + ":" + action + ":");
  }
}

/**
 * Wrap label in command block
 */
void builder::cmd(mousebtn index, string action, const label_t& label) {
  if (label && *label) {
    cmd(index, action);
    node(label);
    tag_close(syntaxtag::A);
  }
}

/**
 * Close command tag
 */
void builder::cmd_close() {
  tag_close(syntaxtag::A);
}

/**
 * Get default color hex string
 */
string builder::color_hex(syntaxtag tag) {
  string& value = m_colors_default[tag];
  if (value.empty()) {
    unsigned int code;
		switch(tag) {
  	 case syntaxtag::B:
    	code = m_bar.background;
    	break;
  	 case syntaxtag::F:
    	code = m_bar.foreground;
    	break;
  	 case syntaxtag::u:
    	code = m_bar.underline.color;
    	break;
  	 case syntaxtag::o:
    	code = m_bar.overline.color;
    	break;
     default:
      throw application_error("Unknown color tag '" + to_string(static_cast<int>(tag)) + "'");
		}
		value = color_util::hex<unsigned short int>(code);
  }
  return value;
}

/**
 * Insert directive to change value of given tag
 */
void builder::tag_open(syntaxtag tag, const string& value) {
  m_tags[tag]++;

  switch (tag) {
    case syntaxtag::NONE:
      break;
    case syntaxtag::A:
      append("%{A" + value + "}");
      break;
    case syntaxtag::F:
      append("%{F" + value + "}");
      break;
    case syntaxtag::B:
      append("%{B" + value + "}");
      break;
    case syntaxtag::T:
      append("%{T" + value + "}");
      break;
    case syntaxtag::u:
      append("%{u" + value + "}");
      break;
    case syntaxtag::o:
      append("%{o" + value + "}");
      break;
    case syntaxtag::R:
      append("%{R}");
      break;
    case syntaxtag::O:
      append("%{O" + value + "}");
      break;
    case syntaxtag::P:
      append("%{P" + value + "}");
      break;
  }
}

/**
 * Insert directive to use given attribute unless already set
 */
void builder::tag_open(attribute attr) {
  if (m_attrs[attr]) {
    return;
  }

  m_attrs[attr] = true;

  switch (attr) {
    case attribute::NONE:
      break;
    case attribute::UNDERLINE:
      append("%{+u}");
      break;
    case attribute::OVERLINE:
      append("%{+o}");
      break;
  }
}

/**
 * Insert directive to reset given tag if it's open and closable
 */
void builder::tag_close(syntaxtag tag) {
  if (!m_tags[tag]) {
    return;
  }

  m_tags[tag]--;

  switch (tag) {
    case syntaxtag::A:
      append("%{A}");
      break;
    case syntaxtag::F:
    case syntaxtag::B:
    case syntaxtag::u:
    case syntaxtag::o:
    case syntaxtag::T:
      append("%{"s.append(1, static_cast<char>(tag)) + "-}");
    case syntaxtag::NONE:
    case syntaxtag::R:
    case syntaxtag::P:
    case syntaxtag::O:
      break;
  }
}

/**
 * Insert directive to remove given attribute if set
 */
void builder::tag_close(attribute attr) {
  if (!m_attrs[attr]) {
    return;
  }

  m_attrs[attr] = false;

  switch (attr) {
    case attribute::NONE:
      break;
    case attribute::UNDERLINE:
    case attribute::OVERLINE:
      append("%{-"s.append(1, static_cast<char>(attr)) + "}");
      break;
  }
}

POLYBAR_NS_END
