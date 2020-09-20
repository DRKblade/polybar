#include "common/test.hpp"
#include "common/config_test.hpp"
#include "drawtypes/labellist.hpp"
#include "utils/factory.hpp"

using namespace polybar;
using namespace polybar::drawtypes;

config::make_type conf = load_config("./resources/components_config.ini");

TEST(LabelList, load) {
  vector<label_t> labels;
  label_t tmplate;
  gradient_t fg, bg, ul, ol;
  load_labellist(labels, tmplate, fg, bg, ul, ol, conf, "test", "label", nullptr, false);
  EXPECT_EQ(4, labels.size());
  EXPECT_EQ("l0", labels[0]->get_raw());
  EXPECT_EQ("l1", labels[1]->get_raw());
  EXPECT_EQ("l2", labels[2]->get_raw());
  EXPECT_EQ("l3", labels[3]->get_raw());
  EXPECT_NE(nullptr, tmplate);
  EXPECT_EQ("%label% %percentage%%", tmplate->get_raw());
}
