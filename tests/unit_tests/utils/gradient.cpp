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
  grad.add(hsla(0, 1, 0.5));
  grad.add(hsla(180, 1, 0.5));
  EXPECT_EQ("#ff7fff00", grad.get_by_percentage(50));
  EXPECT_EQ("#ffffbf00", grad.get_by_percentage(25));
  grad.add(hsla(180, 1, 0));
  EXPECT_EQ("#ff7fff00", grad.get_by_percentage(25));
  EXPECT_EQ("#ffffbf00", grad.get_by_percentage(12.5f));
  EXPECT_EQ("#ff007f7f", grad.get_by_percentage(75));
}
