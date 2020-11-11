#pragma once

#include "components/config.hpp"
#include "components/config_parser.hpp"

POLYBAR_NS

logger _log(loglevel::NONE);

config::make_type load_config(string&& path = "./test_config.ini", string&& bar = "example") {
  config_parser parser(_log, move(path), move(bar));
  return parser.parse();
}

POLYBAR_NS_END
