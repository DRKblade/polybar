#include "common/test.hpp"
#include "common/config_test.hpp"
#include "drawtypes/ramp.hpp"
#include "utils/factory.hpp"
#include "components/config.hpp"
#include "drawtypes/labellist.hpp"

using namespace polybar::drawtypes;
using namespace polybar;

TEST(Ramp, perc) {
  logger log(loglevel::NONE);
  string config_txt = "./test_config.ini";
  config_parser parser(log, move(config_txt), "example");
  config::make_type conf = parser.parse();
  auto r = load_ramp(conf, "test", "label", false);
  r->get_template()->replace_token("%percentage%", "100");
  r->apply_template();
  EXPECT_EQ("A 100%", r->get_by_percentage(33)->get());
  EXPECT_EQ("B 100%", r->get_by_percentage(34)->get());
  EXPECT_EQ("C 100%", r->get_by_percentage(67)->get());
  EXPECT_EQ("A 100%", r->get_by_percentage_with_borders(19, 20, 40)->get());
  EXPECT_EQ("B 100%", r->get_by_percentage_with_borders(21, 20, 40)->get());
  EXPECT_EQ("B 100%", r->get_by_percentage_with_borders(39, 20, 40)->get());
  EXPECT_EQ("C 100%", r->get_by_percentage_with_borders(41, 20, 40)->get());

}

TEST(Ramp, gradient) {
  logger log(loglevel::NONE);
  string config_txt = "./test_config.ini";
  config_parser parser(log, move(config_txt), "example");
  config::make_type conf = parser.parse();
  auto ramp = load_ramp(conf, "test", "label", false);
  EXPECT_EQ(ramp->get(0)->get(), "A");
  EXPECT_EQ(ramp->get(0)->m_background, "#000");
  EXPECT_EQ(ramp->get_by_percentage(25)->m_background, "#ff808080");
  EXPECT_EQ(ramp->get_by_percentage(75)->m_background, "#ff80ffff");
  EXPECT_EQ(ramp->get_by_percentage(33)->m_background, "A");
  EXPECT_EQ(ramp->get_by_percentage(34)->m_background, "B");
  EXPECT_EQ(ramp->get_by_percentage_with_borders(10, 10, 40)->m_background, "A");
  EXPECT_EQ(ramp->get_by_percentage_with_borders(11, 10, 40)->m_background, "B");
  EXPECT_EQ(ramp->get_by_percentage_with_borders(40, 10, 40)->m_background, "C");
}

