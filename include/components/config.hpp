#pragma once

#include <unordered_map>
#include <algorithm>

#include "common.hpp"
#include "components/logger.hpp"
#include "drawtypes/resources/gradient.hpp"
#include "errors.hpp"
#include "settings.hpp"
#include "utils/env.hpp"
#include "utils/file.hpp"
#include "utils/string.hpp"
#include "utils/color.hpp"
#if WITH_XRM
#include "x11/xresources.hpp"
#endif

POLYBAR_NS

DEFINE_ERROR(value_error);
DEFINE_ERROR(key_error);

using valuemap_t = std::unordered_map<string, string>;
using sectionmap_t = std::map<string, valuemap_t>;
using file_list = vector<string>;

class config {
 public:
  explicit config(const logger& logger, string&& path = "", string&& bar = "")
      : m_log(logger), m_file(move(path)), m_barname(move(bar)){};

  using make_type = const config&;
  static make_type make(string path = "", string bar = "");

  const string& filepath() const;
  string section() const;

  void use_xrm();

  void set_sections(sectionmap_t sections);

  void set_included(file_list included);

  void warn_deprecated(const string& section, const string& key, string replacement) const;

  bool has(const string& section, const string& key) const;

  void set(const string& section, const string& key, string&& value);

  string get(const string& key) const;

  string get(const string& section, const string& key) const;

  string get(const string& section, const string& key, const string& default_value) const;

  gradient_t get_gradient(const string& name) const;

  string get_color(const string& section, const string& key, const string& default_value) const;

  vector<string> get_list(const string& key) const;

  vector<string> get_list(const string& section, const string& key) const;

  vector<string> get_list(const string& section, const string& key, const vector<string>& default_value) const;

  string deprecated(const string& section, const string& old, const string& newkey, const string& fallback) const;

  string deprecated_list(const string& section, const string& old, const string& newkey, const vector<string>& fallback) const;

 protected:
  bool try_get(const string& section, const string& key, string& result) const;

  void try_get_list(const string& section, const string& key, vector<string>& result) const;

  string get_guarded(const string& section, const string& key, vector<string>& ref_trace) const;

  void copy_inherited();

  string dereference(const string& section, const string& key, const string& var, const string& fallback) const;

  string dereference(const string& section, const string& key, const string& var, const string& fallback, vector<string> ref_trace) const;

  string dereference_color(string&& value, const string& current_section, vector<string>& ref_trace) const;

  string dereference_local(string section, string&& key, const string& current_section, vector<string>& ref_trace) const;

  string dereference_env(string&& var) const;

  string dereference_xrdb(string&& var) const;

  string dereference_file(string&& var) const;

 private:
  const logger& m_log;
  string m_file;
  string m_barname;
  sectionmap_t m_sections{};
  mutable map<string,gradient_t> m_gradients;

  /**
   * Absolute path of all files that were parsed in the process of parsing the
   * config (Path of the main config file also included)
   */
  file_list m_included;
#if WITH_XRM
  unique_ptr<xresource_manager> m_xrm;
#endif
};

POLYBAR_NS_END
