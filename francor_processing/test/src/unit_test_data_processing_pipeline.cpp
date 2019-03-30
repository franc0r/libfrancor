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
  OutputPort<int> output("output", &number);
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
  OutputPort<int> output("output", &number);
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
  OutputPort<int> output("output", &number);
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
  OutputPort<int> output("output", &number);
  InputPort<int> input("input");

  ASSERT_TRUE(output.connect(input));
  EXPECT_EQ(input.data(), value);
  EXPECT_TRUE(input.disconnect(output));
  EXPECT_FALSE(input.isConnected(output));
  EXPECT_FALSE(output.isConnected(input));
  ASSERT_ANY_THROW(input.data());
}

using francor::processing::PortConfig;
using francor::processing::InputBlock;

TEST(InputBlock, Construct)
{
  constexpr std::array<PortConfig, 3> config = { PortConfig("input_0"), PortConfig("input_1"), PortConfig("input_2") };
  francor::processing::InputBlock<int, int, int> block(config);

  EXPECT_EQ(block.numInputs(), config.size());

  EXPECT_EQ(francor::processing::get<0>(block).name(), "input_0");
  EXPECT_EQ(francor::processing::get<1>(block).name(), "input_1");
  EXPECT_EQ(francor::processing::get<2>(block).name(), "input_2");
}

TEST(OutputBlock, Construct)
{
  constexpr int value = 5;
  int number0 = value + 0;
  int number1 = value + 1;
  int number2 = value + 2;
  
  const std::array<PortConfig, 3> config = { PortConfig("output_0", &number0),
                                             PortConfig("output_1", &number1),
                                             PortConfig("output_2", &number2) };

  francor::processing::OutputBlock<int, int, int> block(config);

  EXPECT_EQ(block.numInputs(), config.size());

  EXPECT_EQ(francor::processing::get<0>(block).name(), "output_0");
  EXPECT_EQ(francor::processing::get<1>(block).name(), "output_1");
  EXPECT_EQ(francor::processing::get<2>(block).name(), "output_2");

  EXPECT_EQ(francor::processing::get<0>(block).data(), number0);
  EXPECT_EQ(francor::processing::get<1>(block).data(), number1);
  EXPECT_EQ(francor::processing::get<2>(block).data(), number2);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}