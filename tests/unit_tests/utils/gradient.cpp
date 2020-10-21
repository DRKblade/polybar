#include "common/test.hpp"
#include "utils/gradient.hpp"

using namespace polybar;

void test_interpolate(hsla&& min, hsla&& max, unsigned int result) {
  EXPECT_EQ(result, static_cast<unsigned int>(gradient::interpolate(min, max, 50).to_rgba()));
}

TEST(String, interpolate) {
  test_interpolate(hsla(0,1,0.5), hsla(120,1,0.5), 0xffffff00);
  test_interpolate(hsla(0,1,0.5), hsla(180,1,0.5), 0xff7fff00);
}

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
