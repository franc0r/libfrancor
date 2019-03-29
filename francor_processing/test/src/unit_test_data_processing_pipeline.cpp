/**
 * Unit test for the data processing pipeline.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <gtest/gtest.h>

#include <type_traits>

#include "francor_processing/data_processing_pipeline.h"

using francor::processing::InputPort;
using francor::processing::OutputPort;

TEST(Port, OutputConnectInput)
{
  constexpr int value = 5;
  int number = value;
  OutputPort<int> output("output", number);
  InputPort<int> input("input");

  ASSERT_TRUE(output.connect(input));
  EXPECT_TRUE(input.isConnected(output));
  EXPECT_TRUE(output.isConnected(input));
  EXPECT_EQ(&output.data(), &number);
  EXPECT_EQ(&input.data(), &number);
  EXPECT_EQ(input.data(), value);

  // second time must fail
  EXPECT_FALSE(output.connect(input));
}

TEST(Port, InputConnectOutput)
{
  constexpr int value = 5;
  int number = value;
  OutputPort<int> output("output", number);
  InputPort<int> input("input");

  ASSERT_TRUE(input.connect(output));
  EXPECT_TRUE(input.isConnected(output));
  EXPECT_TRUE(output.isConnected(input));
  EXPECT_EQ(&output.data(), &number);
  EXPECT_EQ(&input.data(), &number);
  EXPECT_EQ(input.data(), value);

  // second time must fail
  EXPECT_FALSE(input.connect(output));
}

TEST(Port, DisconnectInput)
{
  constexpr int value = 5;
  int number = value;
  OutputPort<int> output("output", number);
  InputPort<int> input("input");

  ASSERT_TRUE(output.connect(input));
  EXPECT_EQ(input.data(), value);
  EXPECT_TRUE(output.disconnect(input));
  EXPECT_FALSE(input.isConnected(output));
  EXPECT_FALSE(output.isConnected(input));
  ASSERT_ANY_THROW(input.data());
}

TEST(Port, DisconnectOutput)
{
  constexpr int value = 5;
  int number = value;
  OutputPort<int> output("output", number);
  InputPort<int> input("input");

  ASSERT_TRUE(output.connect(input));
  EXPECT_EQ(input.data(), value);
  EXPECT_TRUE(input.disconnect(output));
  EXPECT_FALSE(input.isConnected(output));
  EXPECT_FALSE(output.isConnected(input));
  ASSERT_ANY_THROW(input.data());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}