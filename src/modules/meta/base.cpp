#include <utility>

#include "components/builder.hpp"
#include "drawtypes/label.hpp"
#include "modules/meta/base.hpp"

POLYBAR_NS

namespace modules {
  // module_format {{{

	void module_format::begin(builder& builder) {
    if (offset != 0) {
      builder.offset(offset);
    }
    if (style->m_margin.left > 0) {
      builder.space(style->m_margin.left);
    }
    if (style->m_padding.left > 0) {
      builder.space(style->m_padding.left);
    }

    builder.node(prefix);
	}

	void module_format::end(builder& builder) {
    builder.node(suffix);

    if (style->m_padding.right > 0) {
      builder.space(style->m_padding.right);
    }
    if (style->m_margin.right > 0) {
      builder.space(style->m_margin.right);
    }
	}

	bool module_format::has(const string& tag) {
  	return style->get_raw().find(tag) != string::npos;
	}

	string module_format::get_value() const {
  	return style->get_raw();
	}

  // }}}
  // module_formatter {{{

  shared_ptr<module_format> module_formatter::add(string name, string fallback, vector<string>&& tags, vector<string>&& whitelist) {
    const auto formatdef = [&](
        const string& param, const auto& fallback) { return m_conf.get("settings", "format-" + param, fallback); };

    auto format = make_unique<module_format>();
    format->style = load_label(m_conf, m_modname, name, nullptr, false, fallback);
    format->offset = m_conf.get(m_modname, name + "-offset", formatdef("offset", format->offset));
    format->spacing = m_conf.get(m_modname, name + "-spacing", formatdef("spacing", format->spacing));
    format->ulsize = m_conf.get(m_modname, name + "-underline-size", formatdef("underline-size", format->ulsize));
    format->olsize = m_conf.get(m_modname, name + "-overline-size", formatdef("overline-size", format->olsize));
    format->tags.swap(tags);

    try {
      format->prefix = load_label(m_conf, m_modname, name + "-prefix", format->style);
    } catch (const key_error& err) {
      // prefix not defined
    }

    try {
      format->suffix = load_label(m_conf, m_modname, name + "-suffix", format->style);
    } catch (const key_error& err) {
      // suffix not defined
    }

    vector<string> tag_collection;
    tag_collection.reserve(format->tags.size() + whitelist.size());
    tag_collection.insert(tag_collection.end(), format->tags.begin(), format->tags.end());
    tag_collection.insert(tag_collection.end(), whitelist.begin(), whitelist.end());

    size_t start, end;
    string value{format->style->get_raw()};
    while ((start = value.find('<')) != string::npos && (end = value.find('>', start)) != string::npos) {
      if (start > 0) {
        value.erase(0, start);
        end -= start;
        start = 0;
      }
      string tag{value.substr(start, end + 1)};
      if (find(tag_collection.begin(), tag_collection.end(), tag) == tag_collection.end()) {
        throw undefined_format_tag(tag + " is not a valid format tag for \"" + name + "\"");
      }
      value.erase(0, tag.size());
    }

    m_formats.insert(make_pair(move(name), move(format)));
    return format;
  }

  bool module_formatter::has(const string& tag, const string& format_name) {
    auto format = m_formats.find(format_name);
    if (format == m_formats.end()) {
      throw undefined_format(format_name);
    }
    return format->second->has(tag);
  }

  bool module_formatter::has(const string& tag) {
    for (auto&& format : m_formats) {
      if (format.second->has(tag)) {
        return true;
      }
    }
    return false;
  }

  shared_ptr<module_format> module_formatter::get(const string& format_name) {
    auto format = m_formats.find(format_name);
    if (format == m_formats.end()) {
      throw undefined_format("Format \"" + format_name + "\" has not been added");
    }
    return format->second;
  }

  // }}}
}

POLYBAR_NS_END
