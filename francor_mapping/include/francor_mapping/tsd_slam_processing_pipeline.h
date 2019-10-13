/**
 * Processing pipeline for tsd slam
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 15. October 2019
 */
#pragma once

#include "francor_mapping/tsd_grid.h"
#include "francor_mapping/ego_object.h"

#include <francor_base/laser_scan.h>
#include <francor_algorithm/ray_caster_2d.h>
#include <francor_processing/data_processing_pipeline.h>

namespace francor {

namespace mapping {

class PushLaserScanToTsdGrid final : public processing::ProcessingStage<TsdGrid, EgoObject>
{
public:
  enum Inputs {
    IN_LASER_SCAN = 0,
    COUNT_INPUTS
  };
  enum Outpus {
    COUNT_OUTPUTS = 0
  };

  PushLaserScanToTsdGrid()
    : processing::ProcessingStage<TsdGrid, EgoObject>("push laser scan to tsd grid", COUNT_INPUTS, COUNT_OUTPUTS)
  { }

private:
  bool doProcess(TsdGrid& grid, EgoObject& ego) final
  {
    using francor::base::LogDebug;
    using francor::base::Line;
    using francor::algorithm::Ray2d;

    LogDebug() << this->name() << ": start data procssing.";

    // get laser scan and create for each measurement (distance) an ray
    const auto& laser_scan = this->input(IN_LASER_SCAN).data<base::LaserScan>();
    Angle current_phi = laser_scan.phiMin();
    const std::size_t start_index_x = grid.getIndexX(laser_scan.pose().position().x());
    const std::size_t start_index_y = grid.getIndexY(laser_scan.pose().position().y());

    for (const auto& distance : laser_scan.distances())
    {
      const auto direction = Line(current_phi + laser_scan.pose().orientation()).v();
      Ray2d ray(Ray2d::create(start_index_x, start_index_y, grid.cellSize(), laser_scan.pose().position(), v, distance));

      for (const auto& idx : ray)
        grid(idx.x(), idx.y()) = { 1.0, 1.0 }; // \todo replace constant value with tsd calculation function

      current_phi += laser_scan.phiStep();
    }

    return true;
  }
  bool doInitialization() final
  {
    return true;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<base::LaserScan>(IN_LASER_SCAN, "laser scan");

    return true;
  }
  bool isReady() const final
  {
    return this->input(IN_LASER_SCAN).numOfConnections() > 0;
  }
};

} // end namespace mapping

} // end namespace francor