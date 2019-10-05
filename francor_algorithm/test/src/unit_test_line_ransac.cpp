/**
 * Unit test for the line ransac class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 24. March 2019
 */
#include <gtest/gtest.h>

#include "francor_algorithm/ransac.h"

using francor::base::Vector2d;
using francor::base::VectorVector2d;
using francor::base::Line;
using francor::base::LineVector;
using francor::algorithm::LineRansac;
using francor::algorithm::RansacLineModel;

/**
 * Instantiate a line ransac object.
 */
TEST(LineRansac, Instantiate)
{
  LineRansac ransac;
}

/**
 * Tests if a line model is estimated by the ransac line model.
 */
TEST(RansacLineModel, EstimateModelFromTwoPoints)
{
  RansacLineModel model;
  std::array<RansacLineModel::Input::type, RansacLineModel::Input::count> inputData = { Vector2d(0.0, 1.0), Vector2d(1.0, 1.0) };

  ASSERT_TRUE(model.estimate(inputData));

  EXPECT_NEAR(model.model().phi(), 0.0, 1e-3);
  EXPECT_NEAR(model.model().y0() , 1.0, 1e-3);
}

/**
 * Tests if the error between a point and the line model is calculated well.
 */
TEST(RansacLineModel, CalculateErrorToModel)
{
  RansacLineModel model;
  std::array<RansacLineModel::Input::type, RansacLineModel::Input::count> inputData = { Vector2d(0.0, 1.0), Vector2d(1.0, 1.0) };

  ASSERT_TRUE(model.estimate(inputData));

  const Vector2d point(0.5, 2.0);

  EXPECT_NEAR(model.error(point), 1.0, 1e-3);
}

TEST(LineRansac, FindTwoLines)
{
  LineRansac ransac;
  const VectorVector2d inputPoints = { Vector2d(0.0, 1.0), Vector2d(1.0, 1.0), Vector2d(2.0, 1.0), Vector2d(3.0, 1.0), Vector2d(4.0, 1.0),
                                       Vector2d(0.0, 3.0), Vector2d(1.0, 3.0), Vector2d(2.0, 3.0), Vector2d(3.0, 3.0), Vector2d(4.0, 3.0),
                                       Vector2d(9.0, 9.0), Vector2d(5.0, 0.0) }; // the last two are outliers

  ransac.setEpsilon(0.1);
  ransac.setMaxIterations(10);
  ransac.setMinNumPoints(4);

  LineVector result = ransac(inputPoints);

  ASSERT_EQ(result.size(), 2);

  EXPECT_NEAR(result[0].phi(), 0.0, 1e-3);
  EXPECT_NEAR(result[1].phi(), 0.0, 1e-3);
  EXPECT_NEAR(std::max(result[0].y0(), result[1].y0()), 3.0, 1e-3);
  EXPECT_NEAR(std::min(result[0].y0(), result[1].y0()), 1.0, 1e-3);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
