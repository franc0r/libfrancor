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
  std::array<RansacLineModel::InputType, RansacLineModel::NumRequiredData> inputData = { Vector2d(0.0, 1.0), Vector2d(1.0, 1.0) };

  ASSERT_TRUE(model.estimate(inputData));

  EXPECT_NEAR(model.model().m(), 0.0, 1e-3);
  EXPECT_NEAR(model.model().t(), 1.0, 1e-3);
}

/**
 * Tests if the error between a point and the line model is calculated well.
 */
TEST(RansacLineModel, CalculateErrorToModel)
{
  RansacLineModel model;
  std::array<RansacLineModel::InputType, RansacLineModel::NumRequiredData> inputData = { Vector2d(0.0, 1.0), Vector2d(1.0, 1.0) };

  ASSERT_TRUE(model.estimate(inputData));

  const Vector2d point(0.5, 2.0);

  EXPECT_NEAR(model.error(point), 1.0, 1e-3);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
