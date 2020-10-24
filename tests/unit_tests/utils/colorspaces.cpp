#include "common/test.hpp"
#include "utils/colorspaces.hpp"

using namespace polybar::colorspaces;
using namespace std;

constexpr double tolerance=0.00000001;

struct test_set {
  string rgb_str, xyz_str, jab_str;

  explicit test_set(string rgb, string xyz, string jab) : rgb_str(rgb), xyz_str(xyz), jab_str(jab) {}
  void test() {
    const double3 rgb(rgb_str);
    const double3 xyz(xyz_str);
    const double3 jab(jab_str);
    double3 rgb2, xyz2, jab2;
    rgb_xyz(rgb, xyz2, 2);
    expect_near(xyz2, xyz, "RGB", "XYZ", rgb_str);
    xyz_rgb(xyz, rgb2, 2);
    expect_near(rgb2, rgb2, "XYZ", "RGB", xyz_str);
    xyz_jzazbz(xyz, jab2);
    expect_near(jab2, jab, "XYZ", "Jzazbz", xyz_str);
    jzazbz_xyz(jab, xyz2);
    expect_near(xyz2, xyz, "Jzazbz", "XYZ", jab_str);
  }
  void expect_near(const double3& output, const double3& expected, string from, string to, string input) {
    EXPECT_TRUE(output.is_near(expected, tolerance))
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
    test_set("0 0 0", "0 0 0", "0 0 0"),
    test_set("1 0 0", "82.47731265059832 42.52736433546476 3.8661240304967963", "0.13354271261192754 0.11750010579770753 0.11143029691011369"),
    test_set("1 1 1", "190.0857090754361 199.99999999999997 217.7800741596255", "0.22076484311386071 -0.00016356327300187656 -0.00010207018471919405"),
    test_set("0.4980392156862745 0.24705882352941178 1", "57.149249781786736 30.57151205554765 192.0800041682566", "0.11998664741117292 0.01655935189297511 -0.14395328496691184"),
    test_set("0.11764705882352941 0.7843137254901961 0.5411764705882353", "51.550749834086695 86.83622409551975 62.127220744389575", "0.1467401451645171 -0.07197596738987641 0.027363570472776155"),
  };

  for (auto test : tests) {
    test.test();
  }
}

