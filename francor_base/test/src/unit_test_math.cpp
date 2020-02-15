/**
 * Unit test for the math functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 28. December 2019
 */
#include <gtest/gtest.h>

#include "francor_base/algorithm/math.h"

TEST(Fak, Functionality)
{
  EXPECT_EQ((francor::base::algorithm::math::fak<int>(4)),      24);
  EXPECT_EQ((francor::base::algorithm::math::fak<int>(10)), 3628800);
}

TEST(Pow, Functionality)
{
  EXPECT_EQ((francor::base::algorithm::math::pow<int>(2, 8)), 256);
}

TEST(BinomialDistribution, Functionality)
{
  using francor::base::algorithm::math::floating_number;
  using distribution = francor::base::algorithm::math::binomial_distribution<10, floating_number<5, -1, float>, float>;

  EXPECT_EQ(distribution::n, 10);
  EXPECT_NEAR(distribution::p, 0.5f, 1e-6);
  EXPECT_NEAR(distribution::q, 0.5f, 1e-6);
  EXPECT_NEAR(distribution::variance, 2.5, 1e-6);
  EXPECT_NEAR(distribution::mean, 5.0f, 1e-6);

  EXPECT_NEAR(distribution::pm<0>::value, 0.000976, 1e-6);
  EXPECT_NEAR(distribution::pm<1>::value, 0.009765, 1e-6);
  EXPECT_NEAR(distribution::pm<2>::value, 0.043945, 1e-6);
  EXPECT_NEAR(distribution::pm<3>::value, 0.117188, 1e-6);
  EXPECT_NEAR(distribution::pm<4>::value, 0.205078, 1e-6);

  EXPECT_NEAR(distribution::pm<5>::value, 0.246094, 1e-6);

  EXPECT_NEAR(distribution::pm<4>::value, distribution::pm< 6>::value, 1e-6);
  EXPECT_NEAR(distribution::pm<3>::value, distribution::pm< 7>::value, 1e-6);
  EXPECT_NEAR(distribution::pm<2>::value, distribution::pm< 8>::value, 1e-6);
  EXPECT_NEAR(distribution::pm<1>::value, distribution::pm< 9>::value, 1e-6);
  EXPECT_NEAR(distribution::pm<0>::value, distribution::pm<10>::value, 1e-6);
}

TEST(BinomialDistribution, Class)
{
  using francor::base::algorithm::math::BinomialDistribution;
  constexpr BinomialDistribution<10, float> distribution(0.5f);

  EXPECT_EQ(distribution.n(), 10);
  EXPECT_NEAR(distribution.p(), 0.5f, 1e-6);
  EXPECT_NEAR(distribution.q(), 0.5f, 1e-6);
  EXPECT_NEAR(distribution.variance(), 2.5, 1e-6);
  EXPECT_NEAR(distribution.mean(), 5.0f, 1e-6);

  EXPECT_NEAR(distribution.pm(0), 0.000976, 1e-6);
  EXPECT_NEAR(distribution.pm(1), 0.009765, 1e-6);
  EXPECT_NEAR(distribution.pm(2), 0.043945, 1e-6);
  EXPECT_NEAR(distribution.pm(3), 0.117188, 1e-6);
  EXPECT_NEAR(distribution.pm(4), 0.205078, 1e-6);

  EXPECT_NEAR(distribution.pm(5), 0.246094, 1e-6);

  EXPECT_NEAR(distribution.pm(4), distribution.pm( 6), 1e-6);
  EXPECT_NEAR(distribution.pm(3), distribution.pm( 7), 1e-6);
  EXPECT_NEAR(distribution.pm(2), distribution.pm( 8), 1e-6);
  EXPECT_NEAR(distribution.pm(1), distribution.pm( 9), 1e-6);
  EXPECT_NEAR(distribution.pm(0), distribution.pm(10), 1e-6);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}