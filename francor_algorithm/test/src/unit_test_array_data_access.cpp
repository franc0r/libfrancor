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
using francor::base::Size2f;
using francor::base::Angle;

/**
 * \brief Unit test for line iterator for array data access.
 */
TEST(DataAccess2dOperation, LineIterator)
{
  constexpr Size2u array_size(21u, 21u);

  // test row iterator access
  SharedArray2d array(array_size, 0u);
  unsigned int counter = 0;

  for (auto& element : array.row(2).all_elements()) {
    element = ++counter;
  }
  for (std::size_t row = 0; row < array_size.y(); ++row) {
    for (std::size_t col = 0; col < array_size.x(); ++col) {
      if (2u == row) {
        EXPECT_EQ(col + 1u, array(col, row));
      }
      else {
        EXPECT_EQ(0u, array(col, row));
      }
    }
  }

  // test column iterator access
  array.clear();
  array.resize(array_size, 0u);
  counter = 0u;

  for (auto& element : array.col(2).all_elements()) {
    element = ++counter;
  }
  for (std::size_t row = 0; row < array_size.y(); ++row) {
    for (std::size_t col = 0; col < array_size.x(); ++col) {
      if (2u == col) {
        EXPECT_EQ(row + 1u, array(col, row));
      }
      else {
        EXPECT_EQ(0u, array(col, row));
      }
    }
  }
}

/**
 * \brief Unit test for iterator rectangle.
 */
TEST(DataAccess2dOperation, RectangleIterator)
{
  constexpr Size2u array_size(21u, 21u);
  constexpr Size2u rectangle_size(9u, 9u);

  // check normal case; rectangle is completly located inside the array 
  {
    SharedArray2d array(array_size, 0u);
    constexpr Size2u index_center(array_size.x() / 2u, array_size.y() / 2u);
    constexpr std::size_t min_index_x = index_center.x() - rectangle_size.x() / 2u;
    constexpr std::size_t min_index_y = index_center.y() - rectangle_size.y() / 2u;
    constexpr std::size_t max_index_x = index_center.x() + rectangle_size.x() / 2u;
    constexpr std::size_t max_index_y = index_center.y() + rectangle_size.y() / 2u;

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(index_center.x(), index_center.y()).rectangle(rectangle_size)) {
      element = 1u;
    }

    for (std::size_t row = 0; row < array_size.y(); ++row) {
      for (std::size_t col = 0; col < array_size.x(); ++col) {
        if (row >= min_index_y && row <= max_index_y && col >= min_index_x && col <= max_index_x) {
          EXPECT_EQ(1u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;;      
        }
        else {
          EXPECT_EQ(0u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;
        }
      }
    }
  }

  // check behaivour in top left corner; rectangle is not completly located inside the array;
  // iterator must handle this situation
  {
    SharedArray2d array(array_size, 0u);
    constexpr Size2u index_center(0u, 0u);
    constexpr std::size_t min_index_x = 0u;
    constexpr std::size_t min_index_y = 0u;
    constexpr std::size_t max_index_x = min_index_x + rectangle_size.x() / 2u;
    constexpr std::size_t max_index_y = min_index_y + rectangle_size.y() / 2u;

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(index_center.x(), index_center.y()).rectangle(rectangle_size)) {
      element = 1u;
    }

    for (std::size_t row = 0; row < array_size.y(); ++row) {
      for (std::size_t col = 0; col < array_size.x(); ++col) {
        if (row >= min_index_y && row <= max_index_y && col >= min_index_x && col <= max_index_x) {
          EXPECT_EQ(1u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;;      
        }
        else {
          EXPECT_EQ(0u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;
        }
      }
    }
  }

  // check behaivour in top right corner; rectangle is not completly located inside the array;
  // iterator must handle this situation
  {
    SharedArray2d array(array_size, 0u);
    constexpr Size2u index_center(array_size.x() - 1u, 0u);
    constexpr std::size_t min_index_x = index_center.x() - rectangle_size.x() / 2u;
    constexpr std::size_t min_index_y = 0u;
    constexpr std::size_t max_index_x = array_size.x() - 1u;
    constexpr std::size_t max_index_y = index_center.y() + rectangle_size.y() / 2u;

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(index_center.x(), index_center.y()).rectangle(rectangle_size)) {
      element = 1u;
    }

    for (std::size_t row = 0; row < array_size.y(); ++row) {
      for (std::size_t col = 0; col < array_size.x(); ++col) {
        if (row >= min_index_y && row <= max_index_y && col >= min_index_x && col <= max_index_x) {
          EXPECT_EQ(1u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;;      
        }
        else {
          EXPECT_EQ(0u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;
        }
      }
    }   
  }  

  // check behaivour in bottom right corner; rectangle is not completly located inside the array;
  // iterator must handle this situation
  {
    SharedArray2d array(array_size, 0u);
    constexpr Size2u index_center(array_size.x() - 1u, array_size.y() - 1u);
    constexpr std::size_t min_index_x = index_center.x() - rectangle_size.x() / 2u;
    constexpr std::size_t min_index_y = index_center.y() - rectangle_size.y() / 2u;
    constexpr std::size_t max_index_x = array_size.x() - 1u;
    constexpr std::size_t max_index_y = array_size.y() - 1u;

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(index_center.x(), index_center.y()).rectangle(rectangle_size)) {
      element = 1u;
    }

    for (std::size_t row = 0; row < array_size.y(); ++row) {
      for (std::size_t col = 0; col < array_size.x(); ++col) {
        if (row >= min_index_y && row <= max_index_y && col >= min_index_x && col <= max_index_x) {
          EXPECT_EQ(1u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;;      
        }
        else {
          EXPECT_EQ(0u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;
        }
      }
    }   
  }

  // check behaivour in bottom left corner; rectangle is not completly located inside the array;
  // iterator must handle this situation
  {
    SharedArray2d array(array_size, 0u);
    constexpr Size2u index_center(0u, array_size.y() - 1u);
    constexpr std::size_t min_index_x = 0u;
    constexpr std::size_t min_index_y = index_center.y() - rectangle_size.y() / 2u;
    constexpr std::size_t max_index_x = index_center.x() + rectangle_size.x() / 2u;
    constexpr std::size_t max_index_y = array_size.y() - 1u;

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(index_center.x(), index_center.y()).rectangle(rectangle_size)) {
      element = 1u;
    }

    for (std::size_t row = 0; row < array_size.y(); ++row) {
      for (std::size_t col = 0; col < array_size.x(); ++col) {
        if (row >= min_index_y && row <= max_index_y && col >= min_index_x && col <= max_index_x) {
          EXPECT_EQ(1u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;;      
        }
        else {
          EXPECT_EQ(0u, array(col, row)) << "value wrong for index x = " << col << " y = " << row;
        }
      }
    }  
  } 
}

/**
 * \brief Unit test for circle iterator data access.
 */
TEST(DataAccess2dOperation, CircleIterator)
{
  constexpr Size2u array_size(20u, 20u);
  constexpr std::size_t radius = 7u;

  // check normal case; circle is completly located inside the array 
  {
    SharedArray2d array(array_size, 0u);
    constexpr Size2u index_center(array_size.x() / 2u, array_size.y() / 2u);
    // constexpr std::size_t min_index_x = index_center.x() - radius;
    // constexpr std::size_t min_index_y = index_center.y() - radius;
    // constexpr std::size_t max_index_x = index_center.x() + radius;
    // constexpr std::size_t max_index_y = index_center.y() + radius;

    ASSERT_EQ(array_size, array.size());

    for (auto& element : array.at(index_center).circle(radius)) {
      element = 1u;
    }

    // std::cout << array << std::endl;
  }
}

/**
 * \brief Unit test for ellipse iterator data access.
 */
TEST(DataAccess2dOperation, EllispeIterator)
{
  constexpr Size2u array_size(20u, 20u);
  constexpr Size2f radius(2.0f, 4.0f);

  // check normal case; circle is completly located inside the array 
  {
    constexpr Size2u index_center(array_size.x() / 2u, array_size.y() / 2u);
    // constexpr std::size_t min_index_x = index_center.x() - radius;
    // constexpr std::size_t min_index_y = index_center.y() - radius;
    // constexpr std::size_t max_index_x = index_center.x() + radius;
    // constexpr std::size_t max_index_y = index_center.y() + radius;

    for (Angle phi = Angle::createFromDegree(0.0); phi < Angle::createFromDegree(180.0); phi += Angle::createFromDegree(22.5)) {
      SharedArray2d array(array_size, 0u);
      for (auto& element : array.at(index_center).ellipse(radius, phi)) {
        element = 1u;
      }
      std::cout << array << std::endl;
    }
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
