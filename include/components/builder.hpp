#pragma once

#include <map>

#include "common.hpp"
#include "components/types.hpp"

POLYBAR_NS

using std::map;

// fwd decl
using namespace drawtypes;

class builder {
 public:
  using make_type = builder&;
	static make_type make(const bar_settings& bar);

  explicit builder(const bar_settings& bar);

  void clear();
  string flush();
  void append(string text);
  void node(string str);
  void node(string str, int font_index);
  void node(const label_t& label, bool keep_colors = false);
  void node_repeat(const string& str, size_t n);
  void node_repeat(const label_t& label, size_t n);
  void offset(int pixels = 0);
  void space(size_t width);
  void space();
  void remove_trailing_space(size_t len);
  void remove_trailing_space();
  void font(int index);
  void font_close();
  void control(controltag tag);
  void cmd(mousebtn index, string action);
  void cmd(mousebtn index, string action, const label_t& label);
  void cmd_close();
	void color(smallcolor color, syntaxtag tag);
	void line_color(smallcolor color, attribute tag);

 protected:
  void line_color(smallcolor color);
  void line_color_close();
  void tag_open(syntaxtag tag, const string& value);
  void tag_open(attribute attr);
  void tag_close(syntaxtag tag);
  void tag_close(attribute attr);

 private:
  const bar_settings m_bar;
  string m_output;

  map<syntaxtag, int> m_tags{};
  map<syntaxtag, smallcolor> m_colors{};
  map<attribute, bool> m_attrs{};
  map<syntaxtag, bool> m_closing{};

  int m_fontindex{0};
};

POLYBAR_NS_END
