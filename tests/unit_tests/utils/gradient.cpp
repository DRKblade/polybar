#include "common/test.hpp"
#include "utils/gradient.hpp"

using namespace polybar;

TEST(String, gradient) {
  gradient grad;
  grad.add(hsla(0, 1, 0.5), 10);
  grad.add(hsla(180, 1, 0.5), 30);
  EXPECT_EQ("#ff7fff00", grad.get_by_percentage(20));
  EXPECT_EQ("#ffffbf00", grad.get_by_percentage(15));
  EXPECT_EQ("#ffff0000", grad.get_by_percentage(0));
  grad.add(hsla(180, 1, 0), 40);
  EXPECT_EQ("#ff7fff00", grad.get_by_percentage(20));
  EXPECT_EQ("#ffffbf00", grad.get_by_percentage(15));
  EXPECT_EQ("#ff007f7f", grad.get_by_percentage(35));
  EXPECT_EQ("#ff000000", grad.get_by_percentage(41));
}
