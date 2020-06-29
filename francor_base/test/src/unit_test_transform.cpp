/**
 * Unit test for the transform class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 31. October 2019
 */
#include <gtest/gtest.h>

#include "francor_base/transform.h"
#include "francor_base/point.h"
#include "francor_base/algorithm/impl/transform_eigen.h"

using francor::base::Transform2d;
using francor::base::Vector2d;
using francor::base::Point2d;
using francor::base::Angle;

// test data were created on paper
// values were estimated using a rule
constexpr Point2d p_0{-2.3413497207036906, 7.4806155028460761};
constexpr Point2d p_1{-0.5, 4.3};
constexpr Point2d p_2{2.3159270057725982, 1.6007659593160497};
const Transform2d t_01{ { Angle::createFromDegree(38.0) }, {  0.7, 4.4 } };
const Transform2d t_02{ { Angle::createFromDegree( 2.0) }, { -4.6, 5.8 } };

TEST(Transform, TransformPoint)
{
  const Point2d p1_transformed = t_01 * p_1;
  EXPECT_NEAR(p1_transformed.x(), p_0.x(), 1e-6);
  EXPECT_NEAR(p1_transformed.y(), p_0.y(), 1e-6);

  const Point2d p2_transformed = t_02 * p_2;
  EXPECT_NEAR(p2_transformed.x(), p_0.x(), 1e-6);
  EXPECT_NEAR(p2_transformed.y(), p_0.y(), 1e-6);
}

TEST(Transform, InverseTransformPoint)
{
  Point2d p0_transformed = t_01.inverse() * p_0;
  EXPECT_NEAR(p0_transformed.x(), p_1.x(), 1e-6);
  EXPECT_NEAR(p0_transformed.y(), p_1.y(), 1e-6);

  p0_transformed = t_02.inverse() * p_0;
  EXPECT_NEAR(p0_transformed.x(), p_2.x(), 1e-6);
  EXPECT_NEAR(p0_transformed.y(), p_2.y(), 1e-6);
}

TEST(Transform, TransformTransform)
{
  const Transform2d t_12 = t_02 * t_01.inverse();

  EXPECT_NEAR(t_12.rotation().phi(), Angle::createFromDegree(-36), Angle::createFromDegree(0.1));
  EXPECT_NEAR(t_12.translation().x(), -7.752567006149345, 1e-6);
  EXPECT_NEAR(t_12.translation().y(),  2.651774901354961, 1e-6);

  const Transform2d t = t_12 * t_01;
  EXPECT_NEAR(t.rotation().phi(), t_02.rotation().phi(), Angle::createFromDegree(0.1));
  EXPECT_NEAR(t.translation().x(), t_02.translation().x(), 1e-6);
  EXPECT_NEAR(t.translation().y(), t_02.translation().y(), 1e-6);

  const Point2d p2_transformed = t_12 * t_01 * p_2;
  EXPECT_NEAR(p2_transformed.x(), p_0.x(), 1e-6);
  EXPECT_NEAR(p2_transformed.y(), p_0.y(), 1e-6);
}

TEST(TransfromEigenLib, TransformPointVector)
{
  using francor::base::algorithm::transform::eigen::transformPointVector;
  using francor::base::Point2dVector;

  Point2dVector inputPoints = { { 0.0, 1.0 }, { 1.0, 1.0 }, { 2.0, 1.0 }, { 3.0, 1.0 }, { 4.0, 1.0 },
                                { 0.0, 3.0 }, { 1.0, 3.0 }, { 2.0, 3.0 }, { 3.0, 3.0 }, { 4.0, 3.0 },
                                { 9.0, 9.0 }, { 5.0, 0.0 } };

  const Transform2d transform({ Angle::createFromDegree(180.0) }, {  0.0, 0.0 });
  transformPointVector(transform, inputPoints);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
