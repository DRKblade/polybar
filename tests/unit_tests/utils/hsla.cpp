#include "common/test.hpp"
#include "utils/color.hpp"

using namespace polybar;

template <typename ValueType>
ValueType delta(ValueType one, ValueType two) {
  return one > two ? one - two : two - one;
}

TEST(String, to_rgb) {
  // Value of each channel can deviate from the expected value by 1
  EXPECT_EQ(delta(0xff000000, static_cast<unsigned int>(hsla(360, 0, 0).to_rgb())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0xff00ff00, static_cast<unsigned int>(hsla(120, 1, 0.5).to_rgb())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0xff0000ff, static_cast<unsigned int>(hsla(240, 1, 0.5).to_rgb())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0xff00ffff, static_cast<unsigned int>(hsla(180, 1, 0.5).to_rgb())) & 0xfffefefe, 0);
  EXPECT_EQ(0.5, hsla(240, 1, 0.25).to_rgba().b);
  EXPECT_EQ(1.0, hsla(240, 1, 0.75).to_rgba().b);
  EXPECT_EQ(0.5, hsla(240, 1, 0.75).to_rgba().r);
}

TEST(String, to_rgba) {
  // Value of each channel can deviate from the expected value by 1
  EXPECT_EQ(delta(0xff000000u, static_cast<unsigned int>(hsla(360, 0, 0, 1).to_rgba())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0x0000ff00u, static_cast<unsigned int>(hsla(120, 1, 0.5, 0).to_rgba())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0x7f0000ffu, static_cast<unsigned int>(hsla(240, 1, 0.5, 0.5).to_rgba())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0x3f00ffffu, static_cast<unsigned int>(hsla(180, 1, 0.5, 0.25).to_rgba())) & 0xfffefefe, 0);
  EXPECT_EQ(0.5, hsla(240, 1, 0.25).to_rgba().b);
  EXPECT_EQ(1.0, hsla(240, 1, 0.75).to_rgba().b);
  EXPECT_EQ(0.5, hsla(240, 1, 0.75).to_rgba().r);
}

TEST(String, from_rgba) {
  EXPECT_EQ(delta(0xff008888, static_cast<unsigned int>(hsla::from_rgba(rgba{0xff008888}).to_rgba())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0xffff8888, static_cast<unsigned int>(hsla::from_rgba(rgba{0xffff8888}).to_rgba())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0xff009988, static_cast<unsigned int>(hsla::from_rgba(rgba{0xff009988}).to_rgba())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0xff888888, static_cast<unsigned int>(hsla::from_rgba(rgba{0xff888888}).to_rgba())) & 0xfffefefe, 0);
  EXPECT_EQ(delta(0xff123456, static_cast<unsigned int>(hsla::from_rgba(rgba{0xff123456}).to_rgba())) & 0xfffefefe, 0);
}

TEST(String, convert_hsla) {
  EXPECT_EQ("#ffff0000", hsla::convert_hsla("hsl(0, 1, 0.5)"));
  EXPECT_EQ("#00ff0000", hsla::convert_hsla("hsla(0, 1, 0.5, 0)"));
  EXPECT_EQ("hslinvalid", hsla::convert_hsla("hslinvalid"));
  EXPECT_EQ("hslainvalid", hsla::convert_hsla("hslainvalid"));
  EXPECT_EQ("invalid", hsla::convert_hsla("invalid"));
}

TEST(STring, get_hsla) {
  EXPECT_EQ("#ffff0000", color_util::hex<unsigned short int>(hsla::get_hsla("#ff0000").to_rgba()));
  EXPECT_EQ("#ffff0000", color_util::hex<unsigned short int>(hsla::get_hsla("hsl(360,1,0.5)").to_rgba()));
  EXPECT_EQ("#7f00ffff", color_util::hex<unsigned short int>(hsla::get_hsla("hsla(180,1,0.5,0.5)").to_rgba()));
  EXPECT_EQ("#00000000", color_util::hex<unsigned short int>(hsla::get_hsla("invalid").to_rgba()));
  EXPECT_EQ("#ff00ff00", color_util::hex<unsigned short int>(hsla::get_hsla("invalid", 0xff00ff00).to_rgba()));
} 
    
