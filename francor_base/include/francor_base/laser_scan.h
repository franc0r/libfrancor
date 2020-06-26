/**
 * Laser scan class that represents a scan from a 2d Lidar.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 12. October 2019
 */
#pragma once

#include <vector>
#include <ostream>

#include "francor_base/angle.h"
#include "francor_base/pose.h"
#include "francor_base/sensor_measurment.h"

namespace francor {

namespace base {

class LaserScan : public SensorMeasurment
{
public:
  LaserScan() : SensorMeasurment(SensorMeasurment::SensorName::NONE, SensorMeasurment::SensorType::LIDAR_2D, { }) { }
  LaserScan(const std::vector<double>& distances,
            const Pose2d& sensor_pose,
            const Angle phiMin,
            const Angle phiMax,
            const Angle phiStep,
            const double range,
            const Angle divergence = 0.0,
            const std::string sensor_name = std::string(SensorMeasurment::SensorName::NONE),
            const char* sensor_type = SensorMeasurment::SensorType::LIDAR_2D)
    : SensorMeasurment(sensor_name, sensor_type, sensor_pose),
      _distances(distances),
      _phi_step(phiStep),
      _phi_min(phiMin),
      _phi_max(phiMax),
      _range(range),
      _divergence(divergence)
  {
    this->estimatePointDiameters();
  }

  inline Angle phiMax() const noexcept { return _phi_max; }
  inline Angle phiMin() const noexcept { return _phi_min; }
  inline Angle phiStep() const noexcept { return _phi_step; }
  inline Angle divergence() const noexcept { return _divergence; }
  inline const std::vector<double>& distances() const noexcept { return _distances; }
  inline double range() const noexcept { return _range; }
  inline const std::vector<float>& pointExpansions() const noexcept { return _point_diameters; }

private:
  void estimatePointDiameters()
  {
    _point_diameters.resize(_distances.size());

    for (std::size_t i = 0; i < _point_diameters.size(); ++i) {
      _point_diameters[i] = std::sin(_divergence / 2.0) * _distances[i] * 2.0;
    }
  }

  std::vector<double> _distances;
  Angle _phi_step{0.0};
  Angle _phi_min{0.0};
  Angle _phi_max{0.0};
  double _range{0.0};
  Angle _divergence{0.0};
  std::vector<float> _point_diameters; //> holds the point diameter of each distance measurement (dealing with divergence)
};

} // end namespace base

} // end namespace francor


namespace std {

inline ostream& operator<<(ostream& os, const francor::base::LaserScan& scan)
{
  os << "### laser scan ###" << std::endl;
  os << "pose       : " << scan.sensorPose() << std::endl;
  os << "phi min    : " << scan.phiMin() << std::endl;
  os << "phi max    : " << scan.phiMax() << std::endl;
  os << "phi step   : " << scan.phiStep() << std::endl;
  os << "range      : " << scan.range() << std::endl;
  os << "distances[]: ";
  
  for (const auto& distance : scan.distances())
    os << "[" << distance << "] ";

  return os;
}

} // end namespace std