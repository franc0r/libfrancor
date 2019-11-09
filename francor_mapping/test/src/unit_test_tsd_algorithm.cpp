/**
 * Unit test for tsd algorithm and estimation functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#include <gtest/gtest.h>

#include "francor_mapping/algorihm/tsd.h"
#include "francor_mapping/tsd_grid.h"

#include <francor_base/laser_scan.h>

using francor::mapping::algorithm::tsd::pushLaserScanToGrid;
using francor::mapping::TsdGrid;
using francor::base::LaserScan;
using francor::base::Point2d;
using francor::base::Pose2d;
using francor::base::Angle;

TEST(PushLaserScanToGrid, BasicFunctionality)
{
  TsdGrid grid;
  const std::vector<double> distances(360, 12.0);// = { 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4 }; 
  constexpr Pose2d pose_ego( { 10, 10 }, M_PI);
  constexpr Pose2d pose_laser( { 0.0, 0.0 }, 0.0);
  constexpr Angle phi_min  = Angle::createFromDegree(-180.0);
  constexpr Angle phi_max  = Angle::createFromDegree( 180.0);
  constexpr Angle phi_step = Angle::createFromDegree(1.0);
  LaserScan scan(distances, pose_laser, phi_min, phi_max, phi_step, 12.0);

  ASSERT_TRUE(grid.init(10000, 100000, 0.02));
  auto start = std::chrono::system_clock::now();
  pushLaserScanToGrid(grid, scan, pose_ego);
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << "elapsed = " << elapsed.count() << " ms" << std::endl;
  // std::cout << grid << std::endl;
}

using francor::mapping::algorithm::tsd::reconstructPointsFromGrid;
using francor::base::Point2dVector;

TEST(ReconstructPointsFromGrid, BasicFunctionality)
{
  TsdGrid grid;

  ASSERT_TRUE(grid.init(100, 100, 0.2));

  for (std::size_t y = 0; y < grid.getNumCellsY(); ++y) {
    for (std::size_t x = 0; x < grid.getNumCellsX(); ++x) {
      grid(x, y).tsd = -1.0;
    }
  }

  for (std::size_t x = 0; x < grid.getNumCellsX(); ++x) {
    grid(x, 90).tsd = 1.0;
  }

  constexpr Pose2d pose( { 10.0, 1.0 }, M_PI_2);
  constexpr Angle phi_min = Angle::createFromDegree(-60.0);
  constexpr Angle phi_step = Angle::createFromDegree(1.0);
  constexpr std::size_t num_beams = 120;
  Point2dVector result;

  auto start = std::chrono::system_clock::now();
  ASSERT_TRUE(reconstructPointsFromGrid(grid, pose, phi_min, phi_step, num_beams, 30.0, result));
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "elapsed = " << elapsed.count() << " us" << std::endl;

  for (const auto& point : result)
    EXPECT_EQ(point.y(), 18.0);
    
  // std::cout << "result " << result << std::endl;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}