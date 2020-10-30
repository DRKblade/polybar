#include "common/test.hpp"
#include "utils/color.hpp"
#include "components/config_parser.hpp"

using namespace polybar::colorspaces;
using namespace std;
using namespace polybar;

constexpr double tolerance=0.00001;

struct test_set {
  string rgb_str, hsl_str, xyz_str, jzazbz_str;

  explicit test_set(string rgb, string hsl, string xyz, string jab)
      : rgb_str(rgb), hsl_str(hsl), xyz_str(xyz), jzazbz_str(jab) {}
  void test() {
    const double3 rgb(rgb_str);
    const double3 hsl(hsl_str);
    const double3 xyz(xyz_str);
    const double3 jzazbz(jzazbz_str);
    double3 rgb2, hsl2, xyz2, jzazbz2;

#define TEST_ROUTE(point1, point2)																	\
  point1##_##point2(point1, point2##2);															\
  expect_near(point2##2, point2, "point1", "point2", point1##_str);	\
  point2##_##point1(point2, point1##2);															\
  expect_near(point1##2, point1, "point2", "point1", point2##_str);

		TEST_ROUTE(rgb, xyz)
		TEST_ROUTE(rgb, hsl)
		TEST_ROUTE(jzazbz, xyz)

#undef TEST_ROUTE
    
    rgb_xyz(rgb, rgb2);
    xyz_jzazbz(rgb2, rgb2);
    ab_ch(rgb2, rgb2);
    ch_ab(rgb2, rgb2);
    jzazbz_xyz(rgb2, rgb2);
    xyz_rgb(rgb2, rgb2);
    expect_near(rgb, rgb2, "RGB", "Jzazbz and back", rgb_str);
  }
  bool is_near(const double3& d1, const double3& d2, double tolerance) const {
       return std::abs(d1.a - d2.a) <= tolerance &&
              std::abs(d1.b - d2.b) <= tolerance &&
              std::abs(d1.c - d2.c) <= tolerance;
  }
  
  void expect_near(const double3& output, const double3& expected, string from, string to, string input) {
    EXPECT_TRUE(is_near(output, expected, tolerance))
      << "Conversion from " << from << " to " << to << " is wrong, original RGB: " << rgb_str << endl
      << "Input: " << input << "\nOutput: " << output.to_string() << "\nExpected: " << expected.to_string();
  }
};

TEST(Double3, basic) {
  double3 tmp("0.321 159 200");
  EXPECT_EQ(0.321, tmp.a);
  EXPECT_EQ(159, tmp.b);
  EXPECT_EQ(200, tmp.c);
}

TEST(Color, rgb_xyz_jab) {
  test_set tests[] {
    test_set("0 0 0", "0 0 0", "0 0 0", "0 0 0"),
    test_set("1 1 1", "0 0 1", "95.04285453771806 99.99999999999999 108.89003707981276", "0.1671724373463987 -0.00014327497373994857 -0.00008941115235272523"),
    test_set("1 0 0", "0 1 0.5", "41.23865632529916 21.26368216773238 1.9330620152483982", "0.09896658240491542 0.09964570963272626 0.09123479805000667"),
    test_set("0.4980392156862745 0.24705882352941178 1", "260.000000 1.000000 0.623529", "28.574624890893368 15.285756027773825 96.0400020841283", "0.0883974209977316 0.01490064294789667 -0.12246317046770028"),
  };

  for (auto test : tests) {
    test.test();
  }
}

TEST(Color, reference) {
  logger log(loglevel::NONE);
  string config_txt = "./test_config.ini";
  config_parser parser(log, move(config_txt), "example");
  config::make_type conf = parser.parse();
  EXPECT_EQ("#80ffffff", conf.get("test", "derived-color"));
}
