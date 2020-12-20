/**
 * Unit test for grid algorithm.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. December 2019
 */
#include <gtest/gtest.h>

#include <random>

#include <francor_base/angle.h>
#include <francor_base/pose.h>
#include <francor_base/transform.h>
#include <francor_base/laser_scan.h>

#include "francor_mapping/occupancy_grid.h"
#include "francor_mapping/algorithm/occupancy_grid.h"
#include "francor_mapping/algorithm/grid.h"

#include <francor_vision/image.h>
#include <francor_vision/io.h>

using francor::mapping::OccupancyGrid;
using francor::mapping::OccupancyCell;
using francor::mapping::algorithm::occupancy::createGridFromImage;
using francor::mapping::algorithm::occupancy::reconstructLaserBeam;
using francor::mapping::algorithm::occupancy::reconstructLaserScan;
using francor::mapping::algorithm::grid::registerLaserScan;
using francor::mapping::algorithm::occupancy::updateGridCell;

using francor::vision::Image;
using francor::vision::ColourSpace;
using francor::vision::loadImageFromFile;

using francor::base::Pose2d;
using francor::base::Vector2i;
using francor::base::Angle;
using francor::base::LaserScan;
using francor::base::Point2d;
using francor::base::Transform2d;

class OccupancyGridAlgorithm : public ::testing::Test
{
private:
  void SetUp() final
  {
    map_image = loadImageFromFile("/home/knueppl/repos/francor/libfrancor/example/data/robocup-map.png",
                                  ColourSpace::GRAY);
    ASSERT_TRUE(createGridFromImage(map_image, 0.01, grid));                              
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

    const auto index_start = grid.find().cell().index(start_point);
    const auto index_end   = grid.find().cell().index(end);

    cv::line(image.cvMat(), 
             { static_cast<int>(index_start.x()), static_cast<int>(index_start.y()) },
             { static_cast<int>(index_end.x()), static_cast<int>(index_end.y()) }, 
             colour, 
             3);
  }

  void drawLaserScanOnImage(const LaserScan& scan, const Pose2d& ego_pose, Image& image)
  {
    const Transform2d tranform({ ego_pose.orientation() },
                               { ego_pose.position().x(), ego_pose.position().y() });
    const Pose2d pose(tranform * scan.pose());
    const auto index_start = grid.find().cell().index(pose.position());
    Angle current_phi = scan.phiMin();
    const int radius_px = static_cast<int>(20.0 / 0.05);

    drawLaserBeamOnImage(pose.position(), pose.orientation() + scan.phiMin(), 20,
                         cv::Scalar(0, 0, 240), image);
    drawLaserBeamOnImage(pose.position(), pose.orientation() + scan.phiMax(), 20,
                         cv::Scalar(0, 0, 240), image);
    cv::circle(image.cvMat(), 
               { static_cast<int>(index_start.x()), static_cast<int>(index_start.y()) }, 
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

  void applyGaussianNoise(LaserScan& scan)
  {
    std::normal_distribution<double> distribution(0.0, 0.2 * 0.2);
    std::vector<double> modified_distances;

    modified_distances.reserve(scan.distances().size());

    for (const auto distance : scan.distances()) {
      modified_distances.push_back(distance + distribution(generator));
    }

    scan = LaserScan(modified_distances, scan.pose(), scan.phiMin(),
                     scan.phiMax(), scan.phiStep(), scan.range(), scan.divergence(), "laser", scan.timeStamp());
  }

  Image map_image;
  Image map_image_result;
  OccupancyGrid grid;
  std::default_random_engine generator;
};

// test reconstruction of an single beam
TEST_F(OccupancyGridAlgorithm, ReconstructLaserBeamInGrid)
{
  // construct a pose for origin of beam
  Pose2d pose(grid.find().cell().position({grid.cell().count().x() * 66 / 100, grid.cell().count().y() * 800 / 1000}),
              Angle::createFromDegree(180.0));

  // construct necessary paramter for reconstruction function
  Angle divergence(Angle::createFromDegree(1.0));
  const double range = grid.size().x() * 0.4;
  const Angle divergence_2 = divergence / 2.0;
  const double beam_width = range * std::tan(divergence_2) * 2.0;
  const auto origin_idx(grid.find().cell().index(pose.position()));

  // expect a distance of (500 - 178) * 0.03
  const double distance = reconstructLaserBeam(grid, pose.position(), {origin_idx.x(), origin_idx.y()}, pose.orientation(),
                                               range, divergence, beam_width);

  EXPECT_NEAR(distance, 9.67, 0.01);
  drawLaserBeamOnImage(pose.position(), pose.orientation(), distance, cv::Scalar(0, 0, 240), map_image_result);                               
}

// test reconstruction of an complete laser scan
TEST_F(OccupancyGridAlgorithm, ReconstructLaserScanInGrid)
{
  // construct a pose for origin of beam
  const Pose2d pose_ego(grid.find().cell().position(grid.cell().count() /= 2), Angle::createFromDegree(180.0));
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

// test registration and reconstruction together in a loop
TEST_F(OccupancyGridAlgorithm, RegisterAndReconstructLaserScanInGrid)
{
  // construct a pose for origin of beam
  const Pose2d pose_ego(grid.find().cell().position(grid.cell().count() /= 2), Angle::createFromDegree(180.0));
  const Pose2d sensor_pose({0.0, 0.0}, Angle::createFromDegree(-90.0));

  // construct necessary paramter for reconstruction function
  constexpr Angle phi_min(Angle::createFromDegree(-180.0));
  constexpr Angle phi_step(Angle::createFromDegree(1.0));
  constexpr Angle divergence(Angle::createFromDegree(0.5));
  constexpr std::size_t num_beams = 360;
  constexpr double range = 20.0;
  constexpr double time_stamp = 0.0;

  const LaserScan scan(reconstructLaserScan(grid, pose_ego, sensor_pose, phi_min, phi_step,
                                            num_beams, range, time_stamp, divergence));
  constexpr std::size_t iterations = 1000;

  for (std::size_t i = 0; i < iterations; ++i) {
    OccupancyGrid working_grid;
    working_grid.init(grid.cell().count(), grid.cell().size());
    registerLaserScan(working_grid, pose_ego, scan, OccupancyCell{0.1}, OccupancyCell{0.9});
    const auto result_scan(reconstructLaserScan(working_grid, pose_ego, sensor_pose, phi_min, phi_step,
                                                num_beams, range, time_stamp, divergence));

    ASSERT_EQ(scan.distances().size(), result_scan.distances().size());

    EXPECT_NEAR(scan.divergence(), result_scan.divergence(), Angle::createFromDegree(0.01));
    EXPECT_NEAR(scan.phiMin(), result_scan.phiMin(), Angle::createFromDegree(0.01));
    EXPECT_NEAR(scan.phiStep(), result_scan.phiStep(), Angle::createFromDegree(0.01));
    EXPECT_NEAR(scan.pose().position().x(), result_scan.pose().position().x(), 0.005);
    EXPECT_NEAR(scan.pose().position().y(), result_scan.pose().position().y(), 0.005);
    EXPECT_NEAR(scan.pose().orientation(), result_scan.pose().orientation(), Angle::createFromDegree(0.01));
    EXPECT_NEAR(scan.range(), result_scan.range(), 0.005);
    EXPECT_NEAR(scan.timeStamp(), result_scan.timeStamp(), 0.001);
    EXPECT_EQ(scan.sensorName(), result_scan.sensorName());

    for (std::size_t d = 0; d < scan.distances().size(); ++d) {
      if (std::isinf(scan.distances()[d])) {
        EXPECT_EQ(std::isinf(scan.distances()[d]), std::isinf(result_scan.distances()[d]));
      }
      else {
        EXPECT_NEAR(scan.distances()[d], result_scan.distances()[d], 0.02);
      }
    }

    // std::cout << "input scan:" << std::endl << scan << std::endl;
    // std::cout << "result scan:" << std::endl << result_scan << std::endl;                                         
    // Image debug;
    // francor::mapping::algorithm::occupancy::convertGridToImage(working_grid, debug);
    // debug.transformTo(ColourSpace::BGR);
    // cv::imshow("debug", debug.cvMat());
    // cv::waitKey(10);
  }
}

// test registration of noised scan measurement
TEST_F(OccupancyGridAlgorithm, RegisterNoisedLaserScanInGrid)
{
  // construct a pose for origin of beam
  const Pose2d pose_ego(grid.find().cell().position(grid.cell().count() /= 2), Angle::createFromDegree(180.0));
  const Pose2d sensor_pose({0.0, 0.0}, Angle::createFromDegree(-90.0));

  // construct necessary paramter for reconstruction function
  constexpr Angle phi_min(Angle::createFromDegree(-180.0));
  constexpr Angle phi_step(Angle::createFromDegree(1.0));
  constexpr Angle divergence(Angle::createFromDegree(1.0));
  constexpr std::size_t num_beams = 360;
  constexpr double range = 20.0;
  constexpr double time_stamp = 0.0;

  const LaserScan scan(reconstructLaserScan(grid, pose_ego, sensor_pose, phi_min, phi_step,
                                            num_beams, range, time_stamp, divergence));
  OccupancyGrid working_grid;
  working_grid.init(grid.cell().count(), grid.cell().size());
  constexpr std::size_t iterations = 1000;

  for (std::size_t i = 0; i < iterations; ++i) {
    LaserScan noised_scan(scan);
    applyGaussianNoise(noised_scan);

    OccupancyGrid measurement_grid;
    measurement_grid.init(grid.cell().count(), grid.cell().size());
    
    registerLaserScan(measurement_grid, pose_ego, noised_scan, OccupancyCell{0.2}, OccupancyCell{0.75});

    for (std::size_t row = 0; row < working_grid.cell().count().y(); ++row) {
      for (std::size_t col = 0; col < working_grid.cell().count().y(); ++col) {
        updateGridCell(working_grid(col, row), measurement_grid(col, row).value);
      }
    }

    Image debug;
    francor::mapping::algorithm::occupancy::convertGridToImage(working_grid, debug);
    debug.transformTo(ColourSpace::BGR);
    cv::imshow("debug", debug.cvMat());
    cv::waitKey(10);
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}