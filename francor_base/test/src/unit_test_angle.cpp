/**
 * Unit test for the angle class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. October 2019
 */
#include <gtest/gtest.h>

#include "francor_base/angle.h"

using francor::base::Angle;
using francor::base::Angle0To2Pi;
using francor::base::AnglePi2ToPi2;
using francor::base::AnglePiToPi;

TEST(Angle, ConstructDefaultAngle)
{
  constexpr Angle angle;

  EXPECT_EQ(angle.radian(), 0.0);
}

TEST(Angle, ConvertBetweenRadianAndDegree)
{
  constexpr double radian = M_PI_4;
  constexpr double degree = M_PI_4 * 180.0 / M_PI;

  // start with radian
  Angle angle(radian);

  EXPECT_NEAR(angle.radian(), radian, 1e-6);
  EXPECT_NEAR(angle.degree(), degree, 1e-6);

  // set degree and check if it is converted correctly
  // clear old value
  angle = 0.0;
  angle.setDegree(degree);

  EXPECT_NEAR(angle.radian(), radian, 1e-6);
  EXPECT_NEAR(angle.degree(), degree, 1e-6);
}

TEST(Angle, AddOperator)
{
  constexpr double radian = M_PI_4;
  constexpr Angle angle(radian);

  // normal add operator
  constexpr Angle result_a = angle + radian;
  constexpr Angle result_b = angle + angle;

  EXPECT_NEAR(result_a.radian(), 2.0 * radian, 1e-6);
  EXPECT_NEAR(result_b.radian(), 2.0 * radian, 1e-6);

  // add to this operator
  Angle result_c(angle);

  result_c += angle;
  EXPECT_NEAR(result_c.radian(), 2.0 * radian, 1e-6);

  result_c += radian;
  EXPECT_NEAR(result_c.radian(), 3.0 * radian, 1e-6);
}

TEST(Angle, MinusOperator)
{
  constexpr double minus_value = M_PI_4;
  constexpr double radian = M_PI;
  constexpr Angle angle(radian);

  // normal add operator
  constexpr Angle result_a = angle - minus_value;
  constexpr Angle result_b = angle - Angle(minus_value);

  EXPECT_NEAR(result_a.radian(), radian - minus_value, 1e-6);
  EXPECT_NEAR(result_b.radian(), radian - minus_value, 1e-6);

  // add to this operator
  Angle result_c(angle);

  result_c -= Angle(minus_value);
  EXPECT_NEAR(result_c.radian(), radian - minus_value, 1e-6);

  result_c -= minus_value;
  EXPECT_NEAR(result_c.radian(), radian - minus_value * 2.0, 1e-6);
}

TEST(Angle, CastToDouble)
{
  constexpr double radian = M_PI_4;
  constexpr Angle angle(radian);

  constexpr double double_value = static_cast<double>(angle);

  EXPECT_NEAR(double_value, radian, 1e-6);
}



TEST(NormalizedAngle, Angle0To2Pi)
{
  constexpr double radian = 2.25 * M_PI;
  constexpr Angle0To2Pi angle_positive( radian);
  constexpr Angle0To2Pi angle_negative(-radian);

  EXPECT_NEAR(angle_positive.radian(), M_PI_4, 1e-6);
  EXPECT_NEAR(angle_negative.radian(), Angle::createFromDegree(315), 1e-6);
}

TEST(NormalizedAngle, AnglePi2ToPi2)
{
  constexpr double radian = 0.75 * M_PI;
  constexpr AnglePi2ToPi2 angle_positive( radian);
  constexpr AnglePi2ToPi2 angle_negative(-radian);

  EXPECT_NEAR(angle_positive.radian(), -M_PI_4, 1e-6);
  EXPECT_NEAR(angle_negative.radian(),  M_PI_4, 1e-6);
}

TEST(NormalizedAngle, AnglePiToPi)
{
  constexpr double radian = 1.25 * M_PI;
  constexpr AnglePiToPi angle_positive( radian);
  constexpr AnglePiToPi angle_negative(-radian);

  EXPECT_NEAR(angle_positive.radian(), -0.75 * M_PI, 1e-6);
  EXPECT_NEAR(angle_negative.radian(),  0.75 * M_PI, 1e-6);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}