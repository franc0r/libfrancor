/**
 * Unit test for grid algorithm.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 29. January 2020
 */
#include <gtest/gtest.h>

#include "francor_mapping/occupancy_grid.h"
#include "francor_mapping/algorithm/occupancy_grid.h"

#include <francor_base/laser_scan.h>

#include <francor_vision/image.h>
#include <francor_vision/io.h>

using francor::mapping::OccupancyGrid;
using francor::base::LaserScan;
using francor::base::Angle;
using francor::base::Pose2d;
using francor::vision::Image;

TEST(ValidationOccupancy, RegisterAndReconstruct)
{
  using francor::mapping::algorithm::occupancy::pushLaserScanToGrid;
  using francor::mapping::algorithm::occupancy::reconstructLaserScanFromGrid;

  constexpr Pose2d ego_pose({ 12.5, 12.5 }, { 0.0 });
  constexpr Pose2d sensor_pose({ 0.0, 0.0 }, { 0.0 });
  const std::vector<double> distances = { 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0 };
  LaserScan origin(distances,
                   sensor_pose,
                   Angle::createFromDegree(-180.0),
                   Angle::createFromDegree(180.0 - 22.5),
                   Angle::createFromDegree(22.5),
                   12.0,
                   Angle::createFromDegree(5.0));

  OccupancyGrid grid;
  
  ASSERT_TRUE(grid.init(250, 250, 0.1));

  for (std::size_t i = 0; i < 5; ++i) {
    pushLaserScanToGrid(grid, origin, ego_pose);
  }
    
  std::cout << grid << std::endl;
  Image out_grid;
  francor::mapping::algorithm::occupancy::convertGridToImage(grid, out_grid);
  cv::Mat image;
  cv::resize(out_grid.cvMat(), image, cv::Size(1000, 1000));
  cv::imshow("grid", image);
  cv::waitKey(0);

  LaserScan result;
  EXPECT_TRUE(reconstructLaserScanFromGrid(grid, ego_pose, sensor_pose, origin.phiMin(), origin.phiMax(), distances.size(), origin.range(), result));
  EXPECT_EQ(origin.distances().size(), result.distances().size());

  for (std::size_t i = 0; i < origin.distances().size(); ++i) {
    EXPECT_NEAR(origin.distances()[i], result.distances()[i], 0.0);
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
