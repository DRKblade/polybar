#include "common/test.hpp"
#include "drawtypes/ramp.hpp"
#include "utils/factory.hpp"
#include "components/config.hpp"
#include "drawtypes/labellist.hpp"

using namespace polybar::drawtypes;
using namespace polybar;

TEST(Ramp, perc) {
  ramp r;
  r.add(factory_util::shared<label>("test1", 0));
  r.add(factory_util::shared<label>("test2", 0));
  r.add(factory_util::shared<label>("test3", 0));
  EXPECT_EQ("test1", r.get_by_percentage(33)->get());
  EXPECT_EQ("test2", r.get_by_percentage(34)->get());
  EXPECT_EQ("test3", r.get_by_percentage(67)->get());
  EXPECT_EQ("test1", r.get_by_percentage_with_borders(19, 20, 40)->get());
  EXPECT_EQ("test2", r.get_by_percentage_with_borders(21, 20, 40)->get());
  EXPECT_EQ("test2", r.get_by_percentage_with_borders(39, 20, 40)->get());
  EXPECT_EQ("test3", r.get_by_percentage_with_borders(41, 20, 40)->get());
  r.add(factory_util::shared<label>("test4", 0));
  EXPECT_EQ("test2", r.get_by_percentage_with_borders(29, 20, 40)->get());
  EXPECT_EQ("test3", r.get_by_percentage_with_borders(31, 20, 40)->get());
}

TEST(Ramp, config) {
  logger log(loglevel::NONE);
  config conf(log);
  string section = "test", name = "label-list";
  conf.set(section, name + "-0", "A");
  conf.set(section, name + "-1", "B");
  conf.set(section, name + "-2", "C");
  conf.set(section, name + "-0-background", "#000");
  conf.set(section, name + "-1-background", "#fff");
  conf.set(section, name + "-2-background", "#0ff");
  conf.set(section, name + "-background-gradient", "true");
  auto ramp = load_ramp(conf, section, name, false);
  EXPECT_EQ(ramp->get(0)->get(), "A");
  EXPECT_EQ(ramp->get(0)->m_background, "#000");
  EXPECT_EQ(ramp->get_by_percentage(25)->m_background, "#ff7f7f7f");
  EXPECT_EQ(ramp->get_by_percentage(75)->m_background, "#ff7fffff");
  EXPECT_EQ(ramp->get_by_percentage(33)->m_background, "A");
  EXPECT_EQ(ramp->get_by_percentage(34)->m_background, "B");
  EXPECT_EQ(ramp->get_by_percentage_with_borders(10, 10, 40)->m_background, "A");
  EXPECT_EQ(ramp->get_by_percentage_with_borders(11, 10, 40)->m_background, "B");
  EXPECT_EQ(ramp->get_by_percentage_with_borders(40, 10, 40)->m_background, "C");
}

