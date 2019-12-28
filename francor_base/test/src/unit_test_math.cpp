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
  EXPECT_EQ(francor::base::algorithm::math::fak(4), 24);
  EXPECT_EQ((francor::base::algorithm::math::fak<4, int>()), 24);

  EXPECT_EQ(francor::base::algorithm::math::fak(10), 3628800);
  EXPECT_EQ((francor::base::algorithm::math::fak<10, int>()), 3628800);

}

TEST(Pow, Functionality)
{
  EXPECT_EQ((francor::base::algorithm::math::pow<8, int>(2)), 256);
}

TEST(BinomialDistribution, Functionality)
{
  using distribution = francor::base::algorithm::math::binomial_distribution<10, 5, float>;

  EXPECT_EQ(distribution::n, 10);
  EXPECT_NEAR(distribution::p, 0.5f, 1e-6);
  EXPECT_NEAR(distribution::q, 0.5f, 1e-6);
  EXPECT_NEAR(distribution::variance, 2.5, 1e-6);
  EXPECT_NEAR(distribution::mean, 5.0f, 1e-6);

  EXPECT_NEAR(distribution::pmf<0>::value, 0.000976, 1e-6);
  EXPECT_NEAR(distribution::pmf<1>::value, 0.009765, 1e-6);
  EXPECT_NEAR(distribution::pmf<2>::value, 0.043945, 1e-6);
  EXPECT_NEAR(distribution::pmf<3>::value, 0.117188, 1e-6);
  EXPECT_NEAR(distribution::pmf<4>::value, 0.205078, 1e-6);

  EXPECT_NEAR(distribution::pmf<5>::value, 0.246094, 1e-6);

  EXPECT_NEAR(distribution::pmf<4>::value, distribution::pmf< 6>::value, 1e-6);
  EXPECT_NEAR(distribution::pmf<3>::value, distribution::pmf< 7>::value, 1e-6);
  EXPECT_NEAR(distribution::pmf<2>::value, distribution::pmf< 8>::value, 1e-6);
  EXPECT_NEAR(distribution::pmf<1>::value, distribution::pmf< 9>::value, 1e-6);
  EXPECT_NEAR(distribution::pmf<0>::value, distribution::pmf<10>::value, 1e-6);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}