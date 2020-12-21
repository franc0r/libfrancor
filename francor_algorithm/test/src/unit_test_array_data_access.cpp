/**
 * Unit test for the array access iterators.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 20. December 2020
 */
#include <gtest/gtest.h>

#include "francor_algorithm/shared_array.h"

using SharedArray2d = francor::algorithm::SharedArray2d<unsigned int>;
using francor::base::Size2u;

TEST(DataAccess2dOperation, Rectangle)
{
  constexpr Size2u array_size(21u, 21u);
  constexpr Size2u rectangle_size(9u, 9u);

  {
    SharedArray2d array(array_size, 0u);

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(array_size.x() / 2u, array_size.y() / 2u).rectangle(rectangle_size)) {
      element = 1u;
    }
    std::cout << array << std::endl;
  }

  {
    SharedArray2d array(array_size, 0u);

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(0u, 0u).rectangle(rectangle_size)) {
      element = 1u;
    }
    std::cout << array << std::endl;    
  }

  {
    SharedArray2d array(array_size, 0u);

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(array_size.x() - 1u, 0u).rectangle(rectangle_size)) {
      element = 1u;
    }
    std::cout << array << std::endl;    
  }  

  {
    SharedArray2d array(array_size, 0u);

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(array_size.x() - 1u, array_size.y() - 1u).rectangle(rectangle_size)) {
      element = 1u;
    }
    std::cout << array << std::endl;    
  }

  {
    SharedArray2d array(array_size, 0u);

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(0u, array_size.y() - 1u).rectangle(rectangle_size)) {
      element = 1u;
    }
    std::cout << array << std::endl;    
  } 
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
