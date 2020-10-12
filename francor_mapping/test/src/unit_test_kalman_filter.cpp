/**
 * Unit test for the kalman filter class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 08. October 2020
 */
#include <gtest/gtest.h>

#include <random>

#include "francor_mapping/kalman_filter.h"
#include "francor_mapping/ego_kalman_filter_model.h"

using francor::mapping::KinematicAttribute;
using francor::mapping::KinematicAttributePack;
using francor::mapping::KinematicStateVector;
using francor::mapping::KalmanFilter;
using francor::mapping::EgoKalmanFilterModel;

using francor::base::Angle;

template <typename StateVector>
class CsvWriter
{
public:
  CsvWriter(const StateVector& state) : _state(state) { this->printHeaderLine(); }

  void printState(const double time) const
  {
    std::cout << time;
    const auto states = static_cast<typename StateVector::Vector>(_state);
    for (std::size_t i = 0; i < StateVector::size; ++i) {
      std::cout << "; " << states[i];       
    }
    std::cout << std::endl;
  }

private:
  void printHeaderLine() const
  {
    if constexpr (std::is_same<StateVector, KalmanFilter<EgoKalmanFilterModel>::StateVector>::value) {
      std::cout << "timestamp; x; y; vel; vel_x; vel_y; acc; yaw; yaw_rate" << std::endl;
    }
  }

  const StateVector& _state;
};

TEST(KalmanFilter, Instansitate)
{
  KalmanFilter<EgoKalmanFilterModel> kalman_filter;
}

TEST(KalmanFilter, TimePredictOnInitialValues_Circle)
{
  // defines values for a path on a circle with radius 10m and an x velocity of 10m/s
  constexpr double circle_radius = 10.0;
  constexpr double velocity = 1.0;
  constexpr double needed_time_for_complete_circle = (circle_radius * 2.0 * M_PI) / velocity;
  KalmanFilter<EgoKalmanFilterModel>::StateVector initial_state;
  initial_state.x() = 0.0;
  initial_state.y() = -circle_radius;
  initial_state.velocity() = 1.0;
  initial_state.acceleration() = 0.0;
  initial_state.yaw() = Angle::createFromDegree(0.0);
  initial_state.yawRate() = (2.0 * M_PI) / needed_time_for_complete_circle;

  KalmanFilter<EgoKalmanFilterModel>::Matrix initial_covariances = KalmanFilter<EgoKalmanFilterModel>::Matrix::Identity() * 0.1;

  KalmanFilter<EgoKalmanFilterModel> kalman_filter;
  constexpr double time_step = 0.01;
  const CsvWriter<KalmanFilter<EgoKalmanFilterModel>::StateVector> csv_writer(kalman_filter.states());
  
  kalman_filter.initialize(initial_state, initial_covariances);
  
  for (double time = 0.0; time <= needed_time_for_complete_circle; time += time_step) {
    EXPECT_TRUE(kalman_filter.predictToTime(time));
    csv_writer.printState(time);
  }
}

francor::base::Vector2d calculatePositionOnCircle(const double radius, const Angle phi)
{
  static std::default_random_engine generator;
  static std::normal_distribution<double> distribution(0.0, 0.5);

  francor::base::Vector2d point(std::cos(phi) * radius, std::sin(phi) * radius);
  point.x() += distribution(generator);
  point.y() += distribution(generator);

  return point;
}

TEST(KalmanFilter, Update_Circle)
{
  using PositionSensorAttributes = KinematicAttributePack<KinematicAttribute::POS_X,
                                                          KinematicAttribute::POS_Y,
                                                          KinematicAttribute::YAW>;
  using PositionSensorStateVector = KinematicStateVector<PositionSensorAttributes>;
  using ObservationMatrix = francor::base::Matrix<EgoKalmanFilterModel::data_type,
                                                  PositionSensorStateVector::size,
                                                  EgoKalmanFilterModel::dimension>;

  ObservationMatrix observation_matrix(ObservationMatrix::Zero());
  observation_matrix(PositionSensorStateVector::getAttributeIndex<KinematicAttribute::POS_X>(),
                     EgoKalmanFilterModel::StateVector::getAttributeIndex<KinematicAttribute::POS_X>()) = 1.0;
  observation_matrix(PositionSensorStateVector::getAttributeIndex<KinematicAttribute::POS_Y>(),
                     EgoKalmanFilterModel::StateVector::getAttributeIndex<KinematicAttribute::POS_Y>()) = 1.0;
  observation_matrix(PositionSensorStateVector::getAttributeIndex<KinematicAttribute::YAW>(),
                     EgoKalmanFilterModel::StateVector::getAttributeIndex<KinematicAttribute::YAW>()) = 1.0;

  // defines values for a path on a circle with radius 10m and an x velocity of 10m/s
  constexpr double circle_radius = 10.0;
  constexpr double velocity = 1.0;
  constexpr double needed_time_for_complete_circle = (circle_radius * 2.0 * M_PI) / velocity;
  KalmanFilter<EgoKalmanFilterModel>::StateVector initial_state;
  initial_state.x() = 0.0;
  initial_state.y() = 0.0;
  initial_state.velocity() = 0.0;
  initial_state.acceleration() = 0.0;
  initial_state.yaw() = Angle::createFromDegree(0.0);
  initial_state.yawRate() = 0.0;

  KalmanFilter<EgoKalmanFilterModel>::Matrix initial_covariances = KalmanFilter<EgoKalmanFilterModel>::Matrix::Identity() * 10.0 * 10.0;

  KalmanFilter<EgoKalmanFilterModel> kalman_filter;
  const CsvWriter<KalmanFilter<EgoKalmanFilterModel>::StateVector> csv_writer(kalman_filter.states());
  constexpr double time_step = 0.01;
  constexpr Angle phi_step((velocity * time_step) / circle_radius);
  std::size_t counter = 0;
  
  kalman_filter.initialize(initial_state, initial_covariances);

  for (double time = 0.0, phi = 0.0; time <= needed_time_for_complete_circle; time += time_step, phi += phi_step, counter++) {
    // update with a measurement every 10th
    if (counter % 10) {
      // get next point measurement
      const auto point = calculatePositionOnCircle(circle_radius, phi);

      // fill sensor measurement vector and covariance matrix
      PositionSensorStateVector sensor_measurements;

      sensor_measurements.x() = point.x();
      sensor_measurements.y() = point.y();
      sensor_measurements.yaw() = phi + Angle::createFromDegree(90);
      francor::base::Matrix<double, 3, 3> measurement_covariances = francor::base::Matrix<double, 3, 3>::Identity() * 0.5;
      measurement_covariances(2, 2) = Angle::createFromDegree(5.0) * Angle::createFromDegree(5.0);

      // process new sensor measurement (predict and update)
      EXPECT_TRUE(kalman_filter.process(time, sensor_measurements, measurement_covariances, observation_matrix));
    }
    // predict only as default (state is updated with predicted state)
    else {
      EXPECT_TRUE(kalman_filter.predictToTime(time));
    }

    // write current state to csv file
    csv_writer.printState(time);
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}