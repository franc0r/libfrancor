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

static constexpr char name_input_0[] = "input 0";
static constexpr char name_input_1[] = "input 1";
static constexpr char name_input_2[] = "input 2";

using francor::processing::PortConfig;
using francor::processing::InputBlock;

TEST(InputBlock, Construct)
{
  francor::processing::InputBlock<PortConfig<int, name_input_0>,
                                  PortConfig<int, name_input_1>,
                                  PortConfig<int, name_input_2>> block;

  EXPECT_EQ(block.numInputs(), 3);

  // EXPECT_EQ(block.get<0>().name(), name_input_0);
  // EXPECT_EQ(block.get<1>().name(), name_input_1);
  // EXPECT_EQ(block.get<2>().name(), name_input_2);
  EXPECT_EQ(francor::processing::get<0>(block).name(), name_input_0);
  EXPECT_EQ(francor::processing::get<1>(block).name(), name_input_1);
  EXPECT_EQ(francor::processing::get<2>(block).name(), name_input_2);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}