#include "common/test.hpp"
#include "utils/color.hpp"

using namespace polybar;
using namespace polybar::colorspaces;

TEST(SmallColor, creation) {
  smallcolor color{0xCC123456};
  EXPECT_EQ(0xCC, color.comp.a);
  EXPECT_EQ(0x12, color.comp.r);
  EXPECT_EQ(0x34, color.comp.g);
  EXPECT_EQ(0x56, color.comp.b);
}

TEST(SmallColor, parse) {
  vector<string> tests{"#12345678", "#123", "#1234", "#123456"};
  vector<unsigned int> codes{0x12345678, 0xFF112233, 0x11223344, 0xFF123456};
  for(size_t i = 0; i < tests.size(); i++) {
    EXPECT_EQ(codes[i], smallcolor::parse(tests[i]).code);
    EXPECT_EQ(tests[i], smallcolor(codes[i]).to_string());
  }
}

TEST(BigColor, parseHex) {
  EXPECT_THROW(bigcolor::parse("invalid"), application_error);
  EXPECT_EQ(0x12345678, bigcolor::parse("#12345678").to_uint());
//  EXPECT_EQ(0, bigcolor::parse("#00AA00AA").d);
//  EXPECT_EQ(0, bigcolor::parse("#00aa00aa").b);
//  EXPECT_NEAR(0.666667, bigcolor::parse("#00AA00AA").a, 0.001);
//  EXPECT_NEAR(0.666667, bigcolor::parse("#00AA00AA").c, 0.001);
//  EXPECT_EQ(0x00AA00AA, bigcolor::parse("#00aa00aa").to_uint());
  EXPECT_EQ(0xFFFFFFFF, bigcolor::parse("#fff").to_uint());
  EXPECT_EQ(0xFF112233, bigcolor::parse("#123").to_uint());
  EXPECT_EQ(0xFF888888, bigcolor::parse("#888888").to_uint());
  EXPECT_EQ(0x11223344, bigcolor::parse("#1234").to_uint());
  EXPECT_EQ(0x1234567813572468UL, bigcolor::parse("#1234567813572468").to_ulong());
}

TEST(BigColor, parse) {
  vector<string> tests{"hsl(0.1, 0.2, 0.3)", "jzAZBZ(0, 4,5.3)", "JCH (160, -1, 300)",
  										 "hSl(.01 , 230 , -999, 0.69)", "xyz ( 199, -200, 696)", "", "rgb(0.5, 0.5, 0.5, 0.5)"};
  vector<type> types{type::HSL, type::Jzazbz, type::Jch, type::HSL, type::XYZ, type::RGB, type::RGB};
  vector<string> expected{"hsl(0.1, 0.2, 0.3, 1)", "jzazbz(0, 4, 5.3, 1)", "jch(160, -1, 300, 1)",
  										 "hsl(0.01, 230, -999, 0.69)", "xyz(199, -200, 696, 1)", "#0fff", "#80808080"};
  for(size_t i = 0; i < tests.size(); i++) {
    EXPECT_EQ(types[i], bigcolor::parse(tests[i]).colorspace);
    EXPECT_EQ(expected[i], bigcolor::parse(tests[i]).to_string()) << "Expected: " << tests[i];
  }
}

TEST(BigColor, parseInvalid) {
  EXPECT_EQ(type::RGB, bigcolor::white().colorspace);

  vector<string> invalid{"invalid", "invalid(1, 2, 3)", "#f", "#ff"};
  for(auto& str : invalid) {
    bigcolor tmp;
    EXPECT_FALSE(bigcolor::try_parse(str, tmp));
    EXPECT_EQ("#fff", bigcolor::parse(str, bigcolor::white()).to_string());
  }
}

TEST(BigColor, general) {
   EXPECT_EQ(0xffff0000, bigcolor::parse("#FF0000").to_uint());
   EXPECT_EQ(0xffff8080, bigcolor::parse("hsl(0, 1, 0.75, 1)").to_uint());
   EXPECT_EQ(0xff00ffc0, bigcolor::parse("rgb(0, 1, 0.75)").to_uint());
   EXPECT_EQ(0x80ff8080, bigcolor::parse("hsl(0, 1, 0.75, 0.5)").to_uint());
   EXPECT_EQ(0x80ffffff, bigcolor::parse("jzazbz(0.22076484311386071, -0.00016356327300187656, -0.00010207018471919405, 0.5)").to_uint());
}
