/**
 * Unit test for grid algorithm.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. December 2019
 */
#include <gtest/gtest.h>

#include <francor_base/angle.h>
#include <francor_base/pose.h>

#include "francor_mapping/occupancy_grid.h"
#include "francor_mapping/algorithm/occupancy_grid.h"

#include <francor_vision/image.h>
#include <francor_vision/io.h>

using francor::mapping::OccupancyGrid;
using francor::mapping::algorithm::occupancy::createGridFromImage;
using francor::vision::Image;
using francor::vision::ColourSpace;
using francor::vision::loadImageFromFile;
using francor::base::Pose2d;
using francor::base::Vector2i;
using francor::base::Angle;
using francor::mapping::algorithm::occupancy::reconstructLaserBeam;

TEST(OccupancyGridAlgorithm, RegisterLaserBeamInGrid)
{
  Image map_image = loadImageFromFile("/home/knueppl/repos/francor/libfrancor/example/data/occupancy-grid-example-simpler.png",
                                      ColourSpace::GRAY);
  OccupancyGrid grid;
  ASSERT_TRUE(createGridFromImage(map_image, 0.03, grid));                                      

  // construct a pose for origin of beam
  Pose2d sensor_pose(grid.getCellPosition(grid.getNumCellsX() / 2, grid.getNumCellsY() / 2), Angle::createFromDegree(180.0));

  // construct necessary paramter for reconstruction function
  Angle divergence(Angle::createFromDegree(1.0));
  const double range = grid.getSizeX() * 0.4;
  const Angle divergence_2 = divergence / 2.0;
  const double beam_width = range * std::tan(divergence_2) * 2.0;
  const Vector2i origin_idx(grid.getIndexX(sensor_pose.position().x()), grid.getIndexY(sensor_pose.position().y()));

  // expect a distance of (500 - 178) * 0.03
  const double distance = reconstructLaserBeam(grid, sensor_pose.position(), origin_idx, sensor_pose.orientation(),
                                               range, divergence, beam_width);

  EXPECT_NEAR(distance, 9.67, 0.01);                                         
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}