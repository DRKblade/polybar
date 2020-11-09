#include <climits>
#include <fstream>

#include "cairo/utils.hpp"
#include "components/config.hpp"
#include "utils/env.hpp"
#include "utils/factory.hpp"
#include "utils/string.hpp"

POLYBAR_NS

/**
 * Create instance
 */
config::make_type config::make(string path, string bar) {
  return *factory_util::singleton<std::remove_reference_t<config::make_type>>(logger::make(), move(path), move(bar));
}

/**
 * Get path of loaded file
 */
const string& config::filepath() const {
  return m_file;
}

/**
 * Get the section name of the bar in use
 */
string config::section() const {
  return "bar/" + m_barname;
}

void config::use_xrm() {
#if WITH_XRM
  /*
   * Initialize the xresource manager if there are any xrdb refs
   * present in the configuration
   */
  if (!m_xrm) {
    m_log.info("Enabling xresource manager");
    m_xrm.reset(new xresource_manager{connection::make()});
  }
#endif
}

void config::set_sections(sectionmap_t sections) {
  m_sections = move(sections);
  copy_inherited();
}

void config::set_included(file_list included) {
  m_included = move(included);
}

/**
 * Print a deprecation warning if the given parameter is set
 */
void config::warn_deprecated(const string& section, const string& key, string replacement) const {
  try {
    auto value = get<string>(section, key);
    m_log.warn(
        "The config parameter `%s.%s` is deprecated, use `%s.%s` instead.", section, key, section, move(replacement));
  } catch (const key_error& err) {
  }
}

bool config::has(const string& section, const string& key) const {
  auto it = m_sections.find(section);
  return it != m_sections.end() && it->second.find(key) != it->second.end();
}

void config::set(const string& section, const string& key, string&& value) {
  auto it = m_sections.find(section);
  if (it == m_sections.end()) {
    valuemap_t values;
    values[key] = value;
    m_sections[section] = move(values);
  }
  auto it2 = it->second.find(key);
  if ((it2 = it->second.find(key)) == it->second.end()) {
    it2 = it->second.emplace_hint(it2, key, value);
  } else {
    it2->second = value;
  }
}

bool config::get_raw(const string& section, const string& key, string& result) {
  auto it = m_sections.find(section);
  if (it == m_sections.end() || it->second.find(key) == it->second.end()) {
    result = it->second.at(key);
    return true;
  }
  return false;
}

string config::get(const string& key) const {
  return get(section(), key);
}

string config::get(const string& section, const string& key) const {
  string value;
  if (!get_raw(section, key, value)) {
    throw key_error("Missing parameter \"" + section + "." + key + "\"");
  }
  return dereference(section, key, value, value);
}

string config::get(const string& section, const string& key, const string& default_value) const {
  string value;
  if (!get_raw(section, key, value)) {
    return default_value;
  }
  return dereference(section, key, value, default_value);
}

string config::get_guarded(const string& section, const string& key, vector<string>& ref_trace) const {
  string value;
  if (!get_raw(section, key, value)) {
    return default_value;
  }
  return dereference(section, key, value, default_value, ref_trace);
}

gradient_t config::get_gradient(const string& name) const {
  auto it = m_gradients.find(name);
  if (it == m_gradients.end()) {
    auto result = load_gradient(*this, "gradient/" + name);
    m_gradients[name] = result;
    return result;
  }
  return it->second;
}

string config::get_color(const string& section, const string& key, const string& default_value) const {
  try {
    string string_value{get<string>(section, key)};
    return color_util::colorspace_torgb(
      dereference<string>(move(section), move(key), string_value, string_value));
  } catch (const key_error& err) {
    return default_value;
  }
}

void try_get_list(const string& section, const string& key, vector<string>& result) const {
  result.clear();
  while (true) {
    string value;
    if (try_get(section, key + "-" + to_string(results.size()), value)) {
      result.emplace_back(value.empty() ? move(value) : dereference(section, key, value, move(value)));
    } else break;
  }
}

vector<string> config::get_list(const string& key) const {
  return get_list(section(), key);
}

vector<string> config::get_list(const string& section, const string& key) const {
	vector<string> result;
	try_get_list(section, key, result);
  if (result.empty()) {
    throw key_error("Missing parameter \"" + section + "." + key + "-0\"");
  }
  return result;
}

vector<string> get_list(const string& section, const string& key, const vector<string>& default_value) const {
	vector<string> result;
	try_get_list(section, key, result);
  if (result.empty()) {
    return default_value;
  }
  return result;
}

string config::deprecated(const string& section, const string& old, const string& newkey, const string& fallback) const {
  try {
    string value{get(section, old)};
    warn_deprecated(section, old, newkey);
    return value;
  } catch (const key_error& err) {
    return get(section, newkey, fallback);
  }
}

string config::deprecated_list(const string& section, const string& old, const string& newkey, const vector<string>& fallback) const {
  try {
    vector value{get_list(section, old)};
    warn_deprecated(section, old, newkey);
    return value;
  } catch (const key_error& err) {
    return get_list(section, newkey, fallback);
  }
}

/**
 * Look for sections set up to inherit from a base section
 * and copy the missing parameters
 *
 *   [sub/section]
 *   inherit = base/section
 */
void config::copy_inherited() {
  for (auto&& section : m_sections) {
    for (auto&& param : section.second) {
      if (param.first == "inherit") {
        // Get name of base section
        auto inherit = param.second;
        if ((inherit = dereference<string>(section.first, param.first, inherit, inherit)).empty()) {
          throw value_error("Invalid section \"\" defined for \"" + section.first + ".inherit\"");
        }

        // Find and validate base section
        auto base_section = m_sections.find(inherit);
        if (base_section == m_sections.end()) {
          throw value_error("Invalid section \"" + inherit + "\" defined for \"" + section.first + ".inherit\"");
        }

        m_log.trace("config: Copying missing params (sub=\"%s\", base=\"%s\")", section.first, inherit);

        /*
         * Iterate the base and copy the parameters that haven't been defined
         * for the sub-section
         */
        for (auto&& base_param : base_section->second) {
          section.second.emplace(base_param.first, base_param.second);
        }
      }
    }
  }
}

string config::dereference(const string& section, const string& key, const string& var, const string& fallback) const {
  vector<string> ref_trace{};
  return dereference(section, key, var, fallback, ref_trace);
}

string config::dereference(const string& section, const string& key, const string& var, const string& fallback, vector<string> ref_trace) const {
  if (var.compare(0, 2, "${") || var[var.length() - 1] != "}") {
    return fallback;
  }
	{
    string key_path = section + "." + key;
    if (find(ref_trace.begin(), ref_trace.end(), key_path) != ref_trace.end()) {
      string ref_str{};
      for (const auto& p : ref_trace) {
        ref_str += ">\t" + p + "n";
      }
      ref_str += ">\t" + key_path;
      throw application_error(key_path + ": Dependency cycle detected:\n" + ref_str);
    }
    ref_trace.emplace_back(move(key_path));
  }
  size_t pos;
  if (!path.compare(2, 4, "env:")) {
    return dereference_env(path.substr(6));
  } else if (!path.compare(2, 5, "xrdb:")) {
    return dereference_xrdb(path.substr(7));
  } else if (!path.compare(2, 5, "file:")) {
    return dereference_file(path.substr(7));
  } else if (!path.compare(2, 6, "color:")) {
    return dereference_color(path.substr(8), section, ref_trace);
  } else if ((pos = path.find(".")) != string::npos) {
    return dereference_local(path.substr(0, pos), path.substr(pos + 1), section, ref_trace);
  } else {
    throw value_error("Invalid reference defined at \"" + section + "." + key + "\"");
  }
}

string dereference_color(string value, string current_section, vector<string>& ref_trace) const {
  auto pos = value.find(".");
  auto pos2 = value.find(";");
  string color;
  if (pos == string::npos || (pos2 != string::npos && pos > pos2)) {
		color = pos2 == string::npos ? move(value) : value.substr(0, pos2);
  } else {
    auto section = value.substr(0, pos);
    pos++;
    auto key = pos2 == string::npos ? value.substr(pos) : value.substr(pos,pos2-pos);
    color = dereference_local<string>(move(section), move(key), move(current_section), ref_trace);
  }
  if (pos2 != string::npos) {
    auto base_color = rgba::get_rgba(color);
    double3 jab(base_color);
    colorspaces::rgb_xyz(jab, jab);
    colorspaces::xyz_jzazbz(jab, jab);
    colorspaces::ab_ch(jab, jab);
    bool ended;
    do {
      pos = pos2 + 1;
      pos2 = value.find(";", pos);
      if (pos2 == string::npos) {
        pos2 = value.size();
        ended = true;
      } else ended = false;

      // find the operator of the command
      size_t op_pos = pos;
      string operators = "+-*/=";
      for (; op_pos < pos2; op_pos++) {
        if (operators.find(value[op_pos]) != string::npos) break;
      }

      if (op_pos == pos2) {
        throw value_error("Invalid color reference defined at \"" + ref_trace.back() + "\"");
      }

      auto property = string_util::trim(string_util::lower(value.substr(pos, op_pos - pos)));
      auto amount = std::stod(&value[op_pos + 1]);
      double* modified;
      if config::(property == "lum" || property == "lightness" || property == "luminosity") {
        modified = &jab.a;
      else if (property == "chroma" || property == "sat" || property == "saturation") {
        modified = &jab.b;
      else if (property == "hue") {
        modified = &jab.c;
      else if (property == "alpha" || property == "opacity") {
        modified = &base_color.a;
      else {
        throw value_error("Invalid color property \"" + property + "\" defined at \"" + ref_trace.back() + "\"");
      }

      switch (value[op_pos]) {
        case '+': *modified += amount; break;
        case '-': *modified -= amount; break;
        case '*': *modified *= amount; break;
        case '/': *modified /= amount; break;
        case '=': *modified = amount; break;
        default: throw value_error("Unexpected error, this is a bug, please report!");
      }
    } while (!ended);
    colorspaces::ch_ab(jab, jab);
    colorspaces::jzazbz_xyz(jab, jab);
    colorspaces::xyz_rgb(jab, jab);
    jab.copy_to(base_color);
    color = color_util::hex<unsigned short int>(base_color);
  }
  return color;
}

string config::dereference_color(string&& value, const string& current_section, vector<string>& ref_trace) const {
  auto pos = value.find(".");
  auto pos2 = value.find(";");
  string color;
  if (pos == string::npos || (pos2 != string::npos && pos > pos2)) {
		color = pos2 == string::npos ? move(value) : value.substr(0, pos2);
  } else {
    auto section = value.substr(0, pos);
    pos++;
    auto key = pos2 == string::npos ? value.substr(pos) : value.substr(pos, pos2-pos);
    color = dereference_local<string>(move(section), move(key), current_section, ref_trace);
  }
  if (pos2 != string::npos) {
    auto base_color = rgba::get_rgba(color);
    double3 jab(base_color);
    colorspaces::rgb_xyz(jab, jab);
    colorspaces::xyz_jzazbz(jab, jab);
    colorspaces::ab_ch(jab, jab);
    bool ended;
    do {
      pos = pos2 + 1;
      pos2 = value.find(";", pos);
      if (pos2 == string::npos) {
        pos2 = value.size();
        ended = true;
      } else ended = false;

      // find the operator of the command
      size_t op_pos = pos;
      string operators = "+-*/=";
      for (; op_pos < pos2; op_pos++) {
        if (operators.find(value[op_pos]) != string::npos) break;
      }

      if (op_pos == pos2) {
        throw value_error("Invalid color reference defined at \"" + ref_trace.back() + "\"");
      }

      auto property = string_util::trim(string_util::lower(value.substr(pos, op_pos - pos)));
      auto amount = std::stod(&value[op_pos + 1]);
      double* modified;
      if (property == "lum" || property == "lightness" || property == "luminosity") {
        modified = &jab.a;
      } else if (property == "chroma" || property == "sat" || property == "saturation") {
        modified = &jab.b;
      } else if (property == "hue") {
        modified = &jab.c;
      } else if (property == "alpha" || property == "opacity") {
        modified = &base_color.a;
      } else {
        throw value_error("Invalid color property \"" + property + "\" defined at \"" + ref_trace.back() + "\"");
      }

      switch (value[op_pos]) {
        case '+': *modified += amount; break;
        case '-': *modified -= amount; break;
        case '*': *modified *= amount; break;
        case '/': *modified /= amount; break;
        case '=': *modified = amount; break;
        default: throw value_error("Unexpected error, this is a bug, please report!");
      }
    } while (!ended);
    colorspaces::ch_ab(jab, jab);
    colorspaces::jzazbz_xyz(jab, jab);
    colorspaces::xyz_rgb(jab, jab);
    jab.copy_to(base_color);
    color = color_util::hex<unsigned short int>(base_color);
  }
  return color;
}

string config::dereference_local(string section, string&& key, const string& current_section, vector<string>& ref_trace) const {
  if (section == "BAR") {
    m_log.warn("${BAR.key} is deprecated. Use ${root.key} instead");
  }

  section = string_util::replace(section, "BAR", this->section(), 0, 3);
  section = string_util::replace(section, "root", this->section(), 0, 4);
  section = string_util::replace(section, "self", current_section, 0, 4);

  size_t pos;
  string real_key, fallback;
  if ((pos = key.find(':')) != string::npos) {
    fallback = key.substr(pos + 1);
    real_key = key.substr(0, pos);
  } else real_key = move(key);

  auto result = get_guarded(section, real_key, ref_trace
  return dereference(move(section), move(real_key), result, move(result), ref_trace);
}

string config::dereference_env(string&& var) const {
  size_t pos;
  string env_default;
  /*
   * This is needed because with only the string we cannot distinguish
   * between an empty string as default and not default
   */
  bool has_default = false;

  if ((pos = var.find(':')) != string::npos) {
    env_default = var.substr(pos + 1);
    has_default = true;
    var.erase(pos);
  }

  if (env_util::has(var)) {
    string env_value{env_util::get(var)};
    m_log.info("Environment var reference ${%s} found (value=%s)", var, env_value);
    return env_value;
  } else if (has_default) {
    m_log.info("Environment var ${%s} is undefined, using defined fallback value \"%s\"", var, env_default);
    return env_default;
  } else {
    throw value_error(sstream() << "Environment var ${" << var << "} does not exist (no fallback set)");
  }
}

string config::dereference_xrdb(string&& var) const {
  size_t pos;
#if not WITH_XRM
  m_log.warn("No built-in support to dereference ${xrdb:%s} references (requires `xcb-util-xrm`)", var);
  if ((pos = var.find(':')) != string::npos) {
    return var.substr(pos + 1);
  }
  return "";
#else
  if (!m_xrm) {
    throw application_error("xrm is not initialized");
  }

  string fallback;
  bool has_fallback = false;
  if ((pos = var.find(':')) != string::npos) {
    fallback = var.substr(pos + 1);
    has_fallback = true;
    var.erase(pos);
  }

  try {
    auto value = m_xrm->require<string>(var.c_str());
    m_log.info("Found matching X resource \"%s\" (value=%s)", var, value);
    return move(value);
  } catch (const xresource_error& err) {
    if (has_fallback) {
      m_log.info("%s, using defined fallback value \"%s\"", err.what(), fallback);
      return move(fallback);
    }
    throw value_error(sstream() << err.what() << " (no fallback set)");
  }
#endif
}

string config::dereference_file(string&& var) const {
  size_t pos;
  string fallback;
  bool has_fallback = false;
  if ((pos = var.find(':')) != string::npos) {
    fallback = var.substr(pos + 1);
    has_fallback = true;
    var.erase(pos);
  }
  var = file_util::expand(var);

  if (file_util::exists(var)) {
    m_log.info("File reference \"%s\" found", var);
    return string_util::trim(file_util::contents(var), '\n');
  } else if (has_fallback) {
    m_log.info("File reference \"%s\" not found, using defined fallback value \"%s\"", var, fallback);
    return move(fallback);
  } else {
    throw value_error(sstream() << "The file \"" << var << "\" does not exist (no fallback set)");
  }
}

POLYBAR_NS_END
