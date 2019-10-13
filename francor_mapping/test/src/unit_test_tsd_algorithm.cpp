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

TEST(PushLaserScanToGrid, BasicFunction)
{
  TsdGrid grid;
  const std::vector<double> distances = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 }; 
  constexpr Pose2d pose( { 5.0, 5.0 }, 0.0);
  constexpr Angle phi_min = 
  LaserScan scan(distances, pose, Angle().setDegree(-5.0), Angle().setDegree(5.0), Angle().setDegree(1.0));

  ASSERT_TRUE(grid.init(100, 100, 0.1));

}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}