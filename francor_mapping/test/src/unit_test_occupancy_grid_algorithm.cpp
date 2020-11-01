/**
 * Unit test for grid algorithm.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. December 2019
 */
#include <gtest/gtest.h>

#include <francor_base/angle.h>
#include <francor_base/pose.h>
#include <francor_base/transform.h>
#include <francor_base/laser_scan.h>

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
using francor::base::LaserScan;
using francor::base::Point2d;
using francor::base::Transform2d;
using francor::mapping::algorithm::occupancy::reconstructLaserBeam;
using francor::mapping::algorithm::occupancy::reconstructLaserScan;

class OccupancyGridAlgorithm : public ::testing::Test
{
private:
  void SetUp() final
  {
    map_image = loadImageFromFile("/home/knueppl/repos/francor/libfrancor/example/data/occupancy-grid-example-simpler.png",
                                  ColourSpace::GRAY);
    ASSERT_TRUE(createGridFromImage(map_image, 0.03, grid));                              
    francor::mapping::algorithm::occupancy::convertGridToImage(grid, map_image_result);
    map_image_result.transformTo(ColourSpace::BGR);
  }

  void TearDown() final
  {
    cv::imshow("occupancy grid", map_image_result.cvMat());
    cv::waitKey(0);
  }

protected:
  void drawLaserBeamOnImage(const Point2d& start_point, const Angle phi, const double length,
                            const cv::Scalar& colour, Image& image)
  {
    const Transform2d transform({ phi },
                                { start_point.x(), start_point.y() });
    const Point2d end(transform * Point2d(length, 0.0));

    const auto index_start_x = grid.getIndexX(start_point.x());
    const auto index_start_y = grid.getIndexY(start_point.y());
    const auto index_end_x = grid.getIndexX(end.x());
    const auto index_end_y = grid.getIndexY(end.y());

    cv::line(image.cvMat(), 
             { static_cast<int>(index_start_x), static_cast<int>(index_start_y) },
             { static_cast<int>(index_end_x), static_cast<int>(index_end_y) }, 
             colour, 
             3);
  }

  void drawLaserScanOnImage(const LaserScan& scan, const Pose2d& ego_pose, Image& image)
  {
    const Transform2d tranform({ ego_pose.orientation() },
                               { ego_pose.position().x(), ego_pose.position().y() });
    const Pose2d pose(tranform * scan.pose());
    const auto index_start_x = grid.getIndexX(pose.position().x());
    const auto index_start_y = grid.getIndexY(pose.position().y());
    Angle current_phi = scan.phiMin();
    const int radius_px = static_cast<int>(20.0 / 0.05);

    drawLaserBeamOnImage(pose.position(), pose.orientation() + scan.phiMin(), 20,
                         cv::Scalar(0, 0, 240), image);
    drawLaserBeamOnImage(pose.position(), pose.orientation() + scan.phiMax(), 20,
                         cv::Scalar(0, 0, 240), image);
    cv::circle(image.cvMat(), 
               { static_cast<int>(index_start_x), static_cast<int>(index_start_y) }, 
               radius_px, 
               cv::Scalar(0, 0, 240), 
               2);

    for (const auto& distance : scan.distances())
    {
      if (!(std::isnan(distance) || std::isinf(distance))) {
        drawLaserBeamOnImage(pose.position(), pose.orientation() + current_phi, distance,
                            cv::Scalar(0, 240, 0), image);
      }
                             
      current_phi += scan.phiStep();
    }
  }

  Image map_image;
  Image map_image_result;
  OccupancyGrid grid;
};

TEST_F(OccupancyGridAlgorithm, RegisterLaserBeamInGrid)
{
  // construct a pose for origin of beam
  Pose2d pose(grid.getCellPosition(grid.getNumCellsX() / 2, grid.getNumCellsY() / 2), Angle::createFromDegree(180.0));

  // construct necessary paramter for reconstruction function
  Angle divergence(Angle::createFromDegree(1.0));
  const double range = grid.getSizeX() * 0.4;
  const Angle divergence_2 = divergence / 2.0;
  const double beam_width = range * std::tan(divergence_2) * 2.0;
  const Vector2i origin_idx(grid.getIndexX(pose.position().x()), grid.getIndexY(pose.position().y()));

  // expect a distance of (500 - 178) * 0.03
  const double distance = reconstructLaserBeam(grid, pose.position(), origin_idx, pose.orientation(),
                                               range, divergence, beam_width);

  EXPECT_NEAR(distance, 9.67, 0.01);
  drawLaserBeamOnImage(pose.position(), pose.orientation(), distance, cv::Scalar(0, 0, 240), map_image_result);                               
}

TEST_F(OccupancyGridAlgorithm, RegisterLaserScanInGrid)
{
  // construct a pose for origin of beam
  const Pose2d pose_ego(grid.getCellPosition(grid.getNumCellsX() / 2, grid.getNumCellsY() / 2),
                                             Angle::createFromDegree(180.0));
  const Pose2d sensor_pose({0.0, 0.0}, Angle::createFromDegree(-90.0));

  // construct necessary paramter for reconstruction function
  constexpr Angle phi_min(Angle::createFromDegree(-120.0));
  constexpr Angle phi_step(Angle::createFromDegree(1.0));
  constexpr Angle divergence(Angle::createFromDegree(0.5));
  constexpr std::size_t num_beams = 240;
  constexpr double range = 20.0;
  constexpr double time_stamp = 0.0;

  const LaserScan scan(reconstructLaserScan(grid, pose_ego, sensor_pose, phi_min, phi_step, num_beams, range, time_stamp, divergence));

  drawLaserScanOnImage(scan, pose_ego, map_image_result);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}