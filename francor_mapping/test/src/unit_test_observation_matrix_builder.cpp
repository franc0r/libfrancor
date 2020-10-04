/**
 * Unit test for observation matrix builder.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 03. October 2020
 */
#include <gtest/gtest.h>

#include "francor_mapping/observation_matrix_builder.h"

using francor::mapping::StateAttribute;
using francor::mapping::StateAttributeVector;
using francor::mapping::ObservationMatrix;
using francor::mapping::ObservationMatrixBuilder;

using AttributeVectorA = StateAttributeVector<StateAttribute::ACC_X,
                                              StateAttribute::ACC_Y,
                                              StateAttribute::POS_X,
                                              StateAttribute::POS_Y,
                                              StateAttribute::VEL_X,
                                              StateAttribute::VEL_Y>;

using AttributeVectorB = StateAttributeVector<StateAttribute::POS_Y,
                                              StateAttribute::POS_X,
                                              StateAttribute::VEL_Y,
                                              StateAttribute::VEL_X>;

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