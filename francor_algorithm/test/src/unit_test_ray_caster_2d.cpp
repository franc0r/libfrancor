/**
 * Unit test for class Ray2d and RayCaster2d
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. October 2019
 */
#include <gtest/gtest.h>

#include "francor_algorithm/ray_caster_2d.h"

using francor::algorithm::Ray2d;

TEST(Ray, CreateUsingStaticFunction)
{
  Ray2d ray(Ray2d::create(0, 0, { 0.0, 0.0 }, 0.1, { 0.0, 0.0 }, { 1.0, 0.0 }, 1.0));
}

TEST(Ray, BoolOperator)
{
  // start von index (5, 0) = [ 1.0, 1.0 ] and walk in positive x-direction 1.0 meter distance.
  Ray2d ray(Ray2d::create(5, 0, { 1.0, 1.0 }, 0.1, { 1.0, 1.0 }, { 1.0, 0.0 }, 1.0));

  EXPECT_TRUE(ray);
}

TEST(Ray, InterateInDirectionX)
{
  // start von index (5, 0) = [ 1.0, 1.0 ] and walk in positive x-direction 1.0 meter distance.
  constexpr std::size_t idxY = 0;
  constexpr std::size_t idxXOffset = 5;
  std::size_t idxX = idxXOffset;
  Ray2d ray(Ray2d::create(idxX, 0, { 1.0, 1.0 }, 0.1, { 1.0, 1.0 }, { 1.0, 0.0 }, 1.0));

  for (; ray; ++ray)
  {
    ++idxX;
    const auto& current_idx = ray.getCurrentIndex();

    EXPECT_EQ(idxX, current_idx.x());
    EXPECT_EQ(idxY, current_idx.y());
  }

  EXPECT_EQ(idxX, 10 + idxXOffset);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}