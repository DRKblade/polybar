#include "common/test.hpp"
#include "common/config_test.hpp"
#include "drawtypes/ramp.hpp"
#include "utils/factory.hpp"
#include "components/config.hpp"
#include "drawtypes/labellist.hpp"

using namespace polybar;

config::make_type conf = load_config("./resources/components_config.ini");

TEST(Config, basic) {
  string tmp{};
  conf.try_get("bar/example", "str-a", tmp);
  EXPECT_EQ("a", conf.get("bar/example", "str-a"));
  EXPECT_EQ("a", tmp);
  EXPECT_ANY_THROW(conf.get("bar/example", "not-exist"));
  EXPECT_EQ("hello   world", conf.get("bar/example", "str-spaces"));
  EXPECT_EQ("hello world \" ", conf.get("bar/example", "str-quotes"));
  EXPECT_TRUE(conf.has("bar/example", "str-a"));
  EXPECT_FALSE(conf.has("bar/example", "not-exist"));
}
TEST(Config, dereference) {
  string tmp = "${bar/example.str-a}";
  vector<string> trace;
  EXPECT_EQ("a", conf.dereference_local("bar/example", "str-a", "bar/example", trace));
  EXPECT_EQ("fallback", conf.dereference_local("bar/example", "not-exist:fallback", "bar/example", trace));
  EXPECT_THROW(conf.dereference_local("bar/example", "not-exist", "bar/example", trace), key_error);
  EXPECT_TRUE(conf.dereference("bar/example", "ref-a", tmp));
  EXPECT_EQ("a", tmp);
  EXPECT_EQ("a", conf.get("bar/example", "ref-a"));
  EXPECT_EQ("a", conf.get("bar/example", "ref-ref-a"));
  EXPECT_THROW(conf.get("bar/example", "cyclic-ref-1"), application_error);
}
TEST(Config, get_list) {
  vector<string> list = conf.get_list("list");
  EXPECT_EQ("l0", list[0]);
  EXPECT_EQ("l1", list[1]);
  EXPECT_EQ("l2", list[2]);
  EXPECT_EQ("l3", list[3]);
  list = conf.get_list("not-exist");
  EXPECT_TRUE(list.empty());
  EXPECT_THROW(conf.get_list_throw("bar/example", "not-exist"), key_error);
}
