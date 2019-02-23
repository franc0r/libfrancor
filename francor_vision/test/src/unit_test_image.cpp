/**
 * Unit test for the image class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <gtest/gtest.h>

#include "francor_vision/image.h"

TEST(ImageTest, ConstructEmptyImage)
{
  francor::vision::Image image;

  EXPECT_EQ(image.cols(), 0);
  EXPECT_EQ(image.rows(), 0);
  EXPECT_EQ(image.colourSpace(), francor::vision::ColourSpace::NONE);
}

TEST(ImageTest, InstantiateImage)
{
  constexpr std::size_t rows = 640;
  constexpr std::size_t cols = 480;
  francor::vision::Image image(rows, cols, francor::vision::ColourSpace::BGR);

  EXPECT_EQ(image.cols(), cols);
  EXPECT_EQ(image.rows(), rows);
  EXPECT_EQ(image.colourSpace(), francor::vision::ColourSpace::BGR);
}

TEST(ImageTest, CopyConstructor)
{
  constexpr std::size_t rows = 1;
  constexpr std::size_t cols = 2;
  francor::vision::Image origin(rows, cols, francor::vision::ColourSpace::GRAY);

  // init origin with 10s
  origin(0, 0).gray() = 10;
  origin(0, 1).gray() = 10;

  francor::vision::Image copied(origin);

  // change values of origin
  origin(0, 0).gray() = 20;
  origin(0, 1).gray() = 20;

  // size and type must be equal
  ASSERT_EQ(origin.cols(), copied.cols());
  ASSERT_EQ(origin.rows(), copied.rows());
  EXPECT_EQ(origin.colourSpace(), copied.colourSpace());

  // values of copied image must no be changed
  EXPECT_EQ(origin(0, 0).gray(), 20);
  EXPECT_EQ(origin(0, 1).gray(), 20);
  EXPECT_EQ(copied(0, 0).gray(), 10);
  EXPECT_EQ(copied(0, 1).gray(), 10);

  EXPECT_EQ(copied.colourSpace(), francor::vision::ColourSpace::GRAY);
}

TEST(ImageTest, MoveConstructor)
{
  constexpr std::size_t rows = 1;
  constexpr std::size_t cols = 2;
  francor::vision::Image origin(rows, cols, francor::vision::ColourSpace::GRAY);

  // init origin with 10s
  origin(0, 0).gray() = 10;
  origin(0, 1).gray() = 10;

  francor::vision::Image moved(std::move(origin));

  // size and type must be moved
  ASSERT_EQ(moved.cols(), cols);
  ASSERT_EQ(moved.rows(), rows);
  EXPECT_EQ(moved.colourSpace(), francor::vision::ColourSpace::GRAY);

  // moved image contains all attributes from origin
  EXPECT_EQ(moved(0, 0).gray(), 10);
  EXPECT_EQ(moved(0, 1).gray(), 10);

  // origin is reinitialized as empty image
  EXPECT_EQ(origin.cols(), 0);
  EXPECT_EQ(origin.rows(), 0);
  EXPECT_EQ(origin.colourSpace(), francor::vision::ColourSpace::NONE);
}

// TEST(ImageTest, MoveFromCvMat)
// {
//   constexpr std::size_t rows = 1;
//   constexpr std::size_t cols = 2;
//   cv::Mat origin(rows, cols, CV_8UC1);

//   // init origin with 10s
//   origin(0, 0).gray() = 10;
//   origin(0, 1).gray() = 10;

//   francor::vision::Image moved(std::move(origin), francor::vision::ColourSpace::GRAY);

//   // size and type must be moved
//   ASSERT_EQ(moved.cols(), cols);
//   ASSERT_EQ(moved.rows(), rows);
//   EXPECT_EQ(moved.colourSpace(), francor::vision::ColourSpace::GRAY);

//   // moved image contains all attributes from origin
//   EXPECT_EQ(moved(0, 0).gray(), 10);
//   EXPECT_EQ(moved(0, 1).gray(), 10);

//   // origin is reinitialized as empty image
//   EXPECT_EQ(origin.cols(), 0);
//   EXPECT_EQ(origin.rows(), 0);  
// }

TEST(ImageTest, CopyFromCvMat)
{
  constexpr std::size_t rows = 1;
  constexpr std::size_t cols = 2;
  cv::Mat origin(rows, cols, CV_8UC1);

  // init origin with 10s
  origin.at<std::uint8_t>(0, 0) = 10;
  origin.at<std::uint8_t>(0, 1) = 10;

  francor::vision::Image copied(origin, francor::vision::ColourSpace::GRAY);

  // change values of origin
  origin.at<std::uint8_t>(0, 0) = 20;
  origin.at<std::uint8_t>(0, 1) = 20;

  // size and type must be equal
  ASSERT_EQ(origin.cols, copied.cols());
  ASSERT_EQ(origin.rows, copied.rows());

  // values of copied image must no be changed
  EXPECT_EQ(origin.at<std::uint8_t>(0, 0), 20);
  EXPECT_EQ(origin.at<std::uint8_t>(0, 1), 20);
  EXPECT_EQ(copied(0, 0).gray(), 10);
  EXPECT_EQ(copied(0, 1).gray(), 10);

  EXPECT_EQ(copied.colourSpace(), francor::vision::ColourSpace::GRAY);
}

TEST(ImageTest, AssignCopyOperator)
{
  constexpr std::size_t rows = 1;
  constexpr std::size_t cols = 2;
  francor::vision::Image origin(rows, cols, francor::vision::ColourSpace::GRAY);

  // init origin with 10s
  origin(0, 0).gray() = 10;
  origin(0, 1).gray() = 10;

  francor::vision::Image copied;

  copied = origin;

  // change values of origin
  origin(0, 0).gray() = 20;
  origin(0, 1).gray() = 20;

  // size and type must be equal
  ASSERT_EQ(origin.cols(), copied.cols());
  ASSERT_EQ(origin.rows(), copied.rows());
  EXPECT_EQ(origin.colourSpace(), copied.colourSpace());

  // values of copied image must no be changed
  EXPECT_EQ(origin(0, 0).gray(), 20);
  EXPECT_EQ(origin(0, 1).gray(), 20);
  EXPECT_EQ(copied(0, 0).gray(), 10);
  EXPECT_EQ(copied(0, 1).gray(), 10);

  EXPECT_EQ(copied.colourSpace(), francor::vision::ColourSpace::GRAY);
}

TEST(ImageTest, AssignMoveOperator)
{
  constexpr std::size_t rows = 1;
  constexpr std::size_t cols = 2;
  francor::vision::Image origin(rows, cols, francor::vision::ColourSpace::GRAY);

  // init origin with 10s
  origin(0, 0).gray() = 10;
  origin(0, 1).gray() = 10;

  francor::vision::Image moved;
  
  moved = std::move(origin);

  // size and type must be moved
  ASSERT_EQ(moved.cols(), cols);
  ASSERT_EQ(moved.rows(), rows);
  EXPECT_EQ(moved.colourSpace(), francor::vision::ColourSpace::GRAY);

  // moved image contains all attributes from origin
  EXPECT_EQ(moved(0, 0).gray(), 10);
  EXPECT_EQ(moved(0, 1).gray(), 10);

  // origin is reinitialized as empty image
  EXPECT_EQ(origin.cols(), 0);
  EXPECT_EQ(origin.rows(), 0);
  EXPECT_EQ(origin.colourSpace(), francor::vision::ColourSpace::NONE);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}