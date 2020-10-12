/**
 * Unit test for the kalman filter class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 08. October 2020
 */
#include <gtest/gtest.h>

#include "francor_mapping/kalman_filter.h"
#include "francor_mapping/ego_kalman_filter_model.h"

using francor::mapping::KinematicAttribute;
using francor::mapping::KinematicAttributePack;
using francor::mapping::KinematicStateVector;
using francor::mapping::KalmanFilter;
using francor::mapping::EgoKalmanFilterModel;

using francor::base::Angle;

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
  // initial_state.velocityX() = 0.0;
  // initial_state.velocityY() = 0.0;
  initial_state.acceleration() = 0.0;
  initial_state.yaw() = Angle::createFromDegree(0.0);
  initial_state.yawRate() = (2.0 * M_PI) / needed_time_for_complete_circle;

  KalmanFilter<EgoKalmanFilterModel>::Matrix initial_covariances = KalmanFilter<EgoKalmanFilterModel>::Matrix::Identity() * 0.1;

  KalmanFilter<EgoKalmanFilterModel> kalman_filter;
  constexpr double time_step = 0.01;
  
  kalman_filter.initialize(initial_state, initial_covariances);
  std::cout << "timestamp; x; y; vel; vel_x; vel_y; acc; yaw; yaw_rate" << std::endl;
  for (double time = 0.0; time <= needed_time_for_complete_circle; time += time_step) {
    KalmanFilter<EgoKalmanFilterModel>::StateVector predicted_states;
    KalmanFilter<EgoKalmanFilterModel>::Matrix predicted_covariances;

    EXPECT_TRUE(kalman_filter.predictToTime(time, predicted_states, predicted_covariances));

    // std::cout << "time stamp = " << time << std::endl;
    // std::cout << "state:" << std::endl << static_cast<KalmanFilter<EgoKalmanFilterModel>::StateVector::Vector>(predicted_states) << std::endl;
    // std::cout << "covariances:" << std::endl << predicted_covariances << std::endl;
    std::cout << time;
    const auto states = static_cast<KalmanFilter<EgoKalmanFilterModel>::StateVector::Vector>(predicted_states);
    for (std::size_t i = 0; i < KalmanFilter<EgoKalmanFilterModel>::StateVector::size; ++i) {
      std::cout << "; " << states[i];       
    }
    std::cout << std::endl;

    // ASSERT_TRUE(kalman_filter.process(time, predicted_states, predicted_covariances, observation_matrix));
    // initialized is used to take over predicted states as current states (without calling update)
    kalman_filter.initialize(predicted_states, predicted_covariances, time);
  }
}

francor::base::Vector2d calculatePositionOnCircle(const double radius, const Angle phi)
{
  return { std::cos(phi) * radius, std::sin(phi) * radius };
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
  // initial_state.velocityX() = 0.0;
  // initial_state.velocityY() = 0.0;
  initial_state.acceleration() = 0.0;
  initial_state.yaw() = Angle::createFromDegree(0.0);
  initial_state.yawRate() = (2.0 * M_PI) / needed_time_for_complete_circle;

  KalmanFilter<EgoKalmanFilterModel>::Matrix initial_covariances = KalmanFilter<EgoKalmanFilterModel>::Matrix::Identity() * 10.0 * 10.0;

  KalmanFilter<EgoKalmanFilterModel> kalman_filter;
  constexpr double time_step = 0.1;
  constexpr Angle phi_step((velocity * time_step) / circle_radius);
  
  kalman_filter.initialize(initial_state, initial_covariances);
  std::cout << "timestamp; x; y; vel; acc; yaw; yaw_rate" << std::endl;
  for (double time = 0.0, phi = 0.0; time <= 1.0 /*needed_time_for_complete_circle*/; time += time_step, phi += phi_step) {
    const auto point = calculatePositionOnCircle(circle_radius, phi);
    std::cout << "phi = " << phi << std::endl;
    std::cout << "point " << point << std::endl;
    PositionSensorStateVector sensor_measurements;
    sensor_measurements.x() = point.x();
    sensor_measurements.y() = point.y();
    sensor_measurements.yaw() = phi + Angle::createFromDegree(90);
    francor::base::Matrix<double, 3, 3> measurement_covariances = francor::base::Matrix<double, 3, 3>::Identity() * 0.2 * 0.2;
    measurement_covariances(2, 2) = Angle::createFromDegree(5.0) * Angle::createFromDegree(5.0);


    EXPECT_TRUE(kalman_filter.process(time, sensor_measurements, measurement_covariances, observation_matrix));
    const auto states = static_cast<KalmanFilter<EgoKalmanFilterModel>::StateVector::Vector>(kalman_filter.states());
    std::cout << time;
    for (std::size_t i = 0; i < KalmanFilter<EgoKalmanFilterModel>::StateVector::size; ++i) {
      std::cout << "; " << states[i];       
    }
    std::cout << std::endl;
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}