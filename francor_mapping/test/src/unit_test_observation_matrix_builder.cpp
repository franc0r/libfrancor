/**
 * Unit test for observation matrix builder.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 03. October 2020
 */
#include <gtest/gtest.h>

#include "francor_mapping/observation_matrix_builder.h"
#include "francor_mapping/observation_matrix.h"

using francor::mapping::KinematicAttribute;
using francor::mapping::KinematicAttributePack;
using francor::mapping::ObservationMatrix;
using francor::mapping::impl::ObservationMatrix2;
using francor::mapping::ObservationMatrixBuilder;
using francor::mapping::KinematicStateVector;

using AttributeVectorA = KinematicAttributePack<KinematicAttribute::ACC_X,
                                                KinematicAttribute::ACC_Y,
                                                KinematicAttribute::POS_X,
                                                KinematicAttribute::POS_Y,
                                                // KinematicAttribute::VEL_X,
                                                KinematicAttribute::VEL,
                                                KinematicAttribute::YAW>;

using AttributeVectorB = KinematicAttributePack<KinematicAttribute::POS_Y,
                                                KinematicAttribute::POS_X,
                                                KinematicAttribute::VEL_X,
                                                KinematicAttribute::VEL_Y,
                                                KinematicAttribute::YAW>;

TEST(ObservationMatrix, Instansiate)
{
  ObservationMatrix2<AttributeVectorA, AttributeVectorB> matrix;
  // francor::base::Matrix<double, 6, 6> covariance = francor::base::Matrix<double, 6, 6>::Identity();

  // const auto result = matrix * covariance;

  // std::cout << "result:" << std::endl << covariance << std::endl;
  KinematicStateVector<AttributeVectorA> state_vector;
  state_vector.accelerationX() = 1.0;
  state_vector.accelerationY() = 2.0;
  state_vector.x() = 3.0;
  state_vector.y() = 4.0;
  state_vector.velocity() = 5.0;
  // state_vector.velocityY() = 6.0;
  state_vector.yaw() = francor::base::Angle::createFromDegree(0);

  KinematicStateVector<AttributeVectorB> sensor_measurement;

  sensor_measurement = matrix * state_vector;

  std::cout << "sensor measurement:" << std::endl << static_cast<KinematicStateVector<AttributeVectorB>::Vector>(sensor_measurement) << std::endl;
}

TEST(ObservationMatrixBuilder, CreateObservationMatrix)
{
  using BuilderType = ObservationMatrixBuilder<AttributeVectorA, AttributeVectorB>;
  using MatrixType = ObservationMatrix<AttributeVectorA, AttributeVectorB>;


  for (std::size_t row = 0; row < MatrixType::rows; ++row) {
    for (std::size_t col = 0; col < MatrixType::cols; ++col) {
      std::cout << MatrixType::element(row, col) << " ";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}