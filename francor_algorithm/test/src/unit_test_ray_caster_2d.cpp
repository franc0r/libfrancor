/**
 * Unit test for class Ray2d and RayCaster2d
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 4. October 2019
 */
#include <gtest/gtest.h>

#include <algorithm>

#include "francor_algorithm/ray_caster_2d.h"

using francor::algorithm::Ray2d;
using francor::base::Vector2d;

TEST(Ray, CreateUsingStaticFunction)
{
  Ray2d ray(Ray2d::create(0, 0, 0.1, { 0.0, 0.0 }, { 1.0, 0.0 }, 1.0));
}

TEST(Ray, BoolOperator)
{
  // start von index (5, 0) = [ 1.0, 1.0 ] and walk in positive x-direction 1.0 meter distance.
  Ray2d ray(Ray2d::create(5, 0, 0.1, { 0.5, 0.0 }, { 1.0, 0.0 }, 1.0));

  EXPECT_TRUE(ray);
}

TEST(Ray, MoveInPositiveX)
{
  // start von index (5, 0) = [ 1.0, 1.0 ] and walk in positive x-direction 1.0 meter distance.
  constexpr std::size_t idxY = 0;
  constexpr std::size_t idxXOffset = 5;
  std::size_t idxX = idxXOffset;
  Ray2d ray(Ray2d::create(idxX, idxY, 0.1, { 0.55, 0.05 }, { 1.0, 0.0 }, 1.0));

  for (; ray; ++ray, ++idxX)
  {
    const auto& current_idx = ray.getCurrentIndex();

    EXPECT_EQ(idxX, current_idx.x());
    EXPECT_EQ(idxY, current_idx.y());
  }

  EXPECT_EQ(idxX, 10 + idxXOffset);
}

TEST(Ray, MoveInPositiveY)
{
  // start von index (5, 0) = [ 1.0, 1.0 ] and walk in positive x-direction 1.0 meter distance.
  constexpr std::size_t idxX = 0;
  constexpr std::size_t idxYOffset = 5;
  std::size_t idxY = idxYOffset;
  Ray2d ray(Ray2d::create(idxX, idxY, 0.1, { 0.0, 0.51 }, { 0.0, 1.0 }, 1.0));

  for (; ray; ++ray, ++idxY)
  {
    const auto& current_idx = ray.getCurrentIndex();

    EXPECT_EQ(idxX, current_idx.x());
    EXPECT_EQ(idxY, current_idx.y());
  }

  EXPECT_EQ(idxY, 10 + idxYOffset);
}

TEST(Ray, MoveInNegativeX)
{
  // start von index (5, 0) = [ 1.0, 1.0 ] and walk in positive x-direction 1.0 meter distance.
  constexpr std::size_t idxY = 0;
  constexpr std::size_t idxXOffset = 15;
  std::size_t idxX = idxXOffset;
  Ray2d ray(Ray2d::create(idxX, idxY, 0.1, { 1.51, 0.0 }, { -1.0, 0.0 }, 1.0));

  for (; ray; ++ray, --idxX)
  {
    const auto& current_idx = ray.getCurrentIndex();

    EXPECT_EQ(idxX, current_idx.x());
    EXPECT_EQ(idxY, current_idx.y());
  }

  EXPECT_EQ(idxX, idxXOffset - 10);
}

TEST(Ray, MoveInNegativeY)
{
  // start von index (5, 0) = [ 1.0, 1.0 ] and walk in positive x-direction 1.0 meter distance.
  constexpr std::size_t idxX = 0;
  constexpr std::size_t idxYOffset = 15;
  std::size_t idxY = idxYOffset;
  Ray2d ray(Ray2d::create(idxX, idxY, 0.1, { 0.0, 1.51 }, { 0.0, -1.0 }, 1.0));

  for (; ray; ++ray, --idxY)
  {
    const auto& current_idx = ray.getCurrentIndex();

    EXPECT_EQ(idxX, current_idx.x());
    EXPECT_EQ(idxY, current_idx.y());
  }

  EXPECT_EQ(idxY, idxYOffset - 10);
}

TEST(Ray, MoveDiagonalPositiveXY)
{
  constexpr std::size_t idxX = 3;
  constexpr std::size_t idxY = 4;
  constexpr std::size_t grid_size = 10;
  std::array<std::array<int, grid_size>, grid_size> grid = { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } };

  const Vector2d track(Vector2d(idxX + 1, idxY + 3) - Vector2d(idxX, idxY));
  const Vector2d direction(track.normalized());
  Ray2d ray(Ray2d::create(idxX, idxY, 0.1, { 0.31, 0.41 }, direction, track.norm() * 0.1));
  std::size_t counter = 0;

  for (; ray; ++ray, ++counter)
  {
    const auto& current_idx = ray.getCurrentIndex();
    EXPECT_EQ(grid[current_idx.y()][current_idx.x()], 1);
  }

  EXPECT_EQ(counter, 4);
}

TEST(Ray, MoveDiagonalNegativeXY)
{
  constexpr std::size_t idxX = 4;
  constexpr std::size_t idxY = 7;
  constexpr std::size_t grid_size = 10;
  std::array<std::array<int, grid_size>, grid_size> grid = { { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 1, 1, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
                                                               { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } } };

  const Vector2d track(Vector2d(idxX - 1, idxY - 3) - Vector2d(idxX, idxY));
  const Vector2d direction(track.normalized());
  Ray2d ray(Ray2d::create(idxX, idxY, 0.1, { 0.41, 0.71 }, direction, track.norm() * 0.1));
  std::size_t counter = 0;

  for (; ray; ++ray, ++counter)
  {
    const auto& current_idx = ray.getCurrentIndex();
    EXPECT_EQ(grid[current_idx.y()][current_idx.x()], 1);
  }

  EXPECT_EQ(counter, 4);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}