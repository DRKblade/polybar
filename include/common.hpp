#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "settings.hpp"

#define POLYBAR_NS    \
  namespace polybar {
#define POLYBAR_NS_END \
  }

#ifndef PIPE_READ
#define PIPE_READ 0
#endif
#ifndef PIPE_WRITE
#define PIPE_WRITE 1
#endif

POLYBAR_NS

using std::string;
using std::size_t;
using std::move;
using std::forward;
using std::pair;
using std::function;
using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::make_shared;
using std::make_pair;
using std::array;
using std::vector;
using std::to_string;

using namespace std::string_literals;

constexpr size_t operator"" _z(unsigned long long n) {
  return n;
}

template<typename T>
class parsable_base {
  string to_string() const;

  static bool try_parse(const string& value, T& result);
  
  static T parse_fail(const string& value);
  
  static T parse(const string& value) {
    T tmp;
    if (!try_parse(value, tmp)) {
      return parse_fail(value);
    }
    return tmp;
  }

  static T parse(const string& value, const T& fallback) {
    T tmp;
    if (!try_parse(value, tmp))
      return fallback;
    return tmp;
  }
};

template<typename T>
class parsable : public parsable_base<T> {};

template<typename T>
class empty_parsable : public parsable_base<T> {
  static T empty_value();

  bool empty() const;
};

T parsable::parse_fail(const string& value) {
	throw application_error("Parse-" + typeid(T).name() + ": Parsing failed, no fallback provided");
}

T empty_parsable::parse_fail(const string& value) {
  if (value.empty())
    return empty_value();
	throw application_error("Parse-" + typeid(T).name() + ": Parsing failed, no fallback provided");
}

POLYBAR_NS_END
