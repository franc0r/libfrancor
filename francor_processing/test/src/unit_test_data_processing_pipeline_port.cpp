/**
 * Unit test for the data processing pipeline ports.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_pipeline_port.h"

// Port
using francor::processing::Port;

TEST(Port, DefaultConstructed)
{
  Port port;

  EXPECT_EQ(port.name(), "none");
  EXPECT_EQ(port.id(), 0);
  EXPECT_EQ(port.dataFlow(), francor::processing::Port::Direction::NONE);
  EXPECT_EQ(port.type(), typeid(void));
  ASSERT_ANY_THROW(port.data<int>());
}

TEST(Port, InitializedWithDataType)
{
  int data;
  Port port("port int", francor::processing::Port::Direction::OUT, &data);

  // valid access
  EXPECT_EQ(port.name(), "port int");
  EXPECT_EQ(port.dataFlow(), francor::processing::Port::Direction::OUT);
  EXPECT_EQ(port.type(), typeid(data));
  EXPECT_EQ(&port.data<int>(), &data);

  // wrong data type access
  ASSERT_ANY_THROW(port.data<float>());
  ASSERT_ANY_THROW(port.data<unsigned int>());
  ASSERT_ANY_THROW(port.data<char>());
}

TEST(Port, MoveConstructed)
{
  // instantiate initialized output port
  int data;
  Port origin("port int", francor::processing::Port::Direction::OUT, &data);
  const std::size_t idOrigin = origin.id();

  // move to new port and check if the attributes were moved properly
  Port moved(std::move(origin));

  EXPECT_EQ(moved.name(), "port int");
  EXPECT_EQ(moved.id(), idOrigin);
  EXPECT_EQ(moved.dataFlow(), francor::processing::Port::Direction::OUT);
  EXPECT_EQ(moved.type(), typeid(data));
  EXPECT_EQ(&moved.data<int>(), &data);

  // the origin port must be unconfigured after move
  EXPECT_EQ(origin.name(), "none");
  EXPECT_EQ(origin.id(), 0);
  EXPECT_EQ(origin.dataFlow(), francor::processing::Port::Direction::NONE);
  EXPECT_EQ(origin.type(), typeid(void));
  ASSERT_ANY_THROW(origin.data<int>());
}

TEST(Port, MovedAssigned)
{
  // instantiate initialized output port
  int data;
  Port origin("port int", francor::processing::Port::Direction::OUT, &data);
  const std::size_t idOrigin = origin.id();

  // move to new port and check if the attributes were moved properly
  Port moved;
  moved = std::move(origin);

  EXPECT_EQ(moved.name(), "port int");
  EXPECT_EQ(moved.id(), idOrigin);
  EXPECT_EQ(moved.dataFlow(), francor::processing::Port::Direction::OUT);
  EXPECT_EQ(moved.type(), typeid(data));
  EXPECT_EQ(&moved.data<int>(), &data);

  // the origin port must be unconfigured after move
  EXPECT_EQ(origin.name(), "none");
  EXPECT_EQ(origin.id(), 0);
  EXPECT_EQ(origin.dataFlow(), francor::processing::Port::Direction::NONE);
  EXPECT_EQ(origin.type(), typeid(void));
  ASSERT_ANY_THROW(origin.data<int>());  
}

// TEST(Port, OutputConnectInput)
// {
//   constexpr int value = 5;
//   int number = value;
//   OutputPort<int> output("output", &number);
//   InputPort<int> input("input");

//   ASSERT_TRUE(output.connect(input));
//   EXPECT_TRUE(input.isConnected(output));
//   EXPECT_TRUE(output.isConnected(input));
//   EXPECT_EQ(&output.data(), &number);
//   EXPECT_EQ(&input.data(), &number);
//   EXPECT_EQ(input.data(), value);

//   // second time must fail
//   EXPECT_FALSE(output.connect(input));
// }

// TEST(Port, InputConnectOutput)
// {
//   constexpr int value = 5;
//   int number = value;
//   OutputPort<int> output("output", &number);
//   InputPort<int> input("input");

//   ASSERT_TRUE(input.connect(output));
//   EXPECT_TRUE(input.isConnected(output));
//   EXPECT_TRUE(output.isConnected(input));
//   EXPECT_EQ(&output.data(), &number);
//   EXPECT_EQ(&input.data(), &number);
//   EXPECT_EQ(input.data(), value);

//   // second time must fail
//   EXPECT_FALSE(input.connect(output));
// }

// TEST(Port, DisconnectInput)
// {
//   constexpr int value = 5;
//   int number = value;
//   OutputPort<int> output("output", &number);
//   InputPort<int> input("input");

//   ASSERT_TRUE(output.connect(input));
//   EXPECT_EQ(input.data(), value);
//   EXPECT_TRUE(output.disconnect(input));
//   EXPECT_FALSE(input.isConnected(output));
//   EXPECT_FALSE(output.isConnected(input));
//   ASSERT_ANY_THROW(input.data());
// }

// TEST(Port, DisconnectOutput)
// {
//   constexpr int value = 5;
//   int number = value;
//   OutputPort<int> output("output", &number);
//   InputPort<int> input("input");

//   ASSERT_TRUE(output.connect(input));
//   EXPECT_EQ(input.data(), value);
//   EXPECT_TRUE(input.disconnect(output));
//   EXPECT_FALSE(input.isConnected(output));
//   EXPECT_FALSE(output.isConnected(input));
//   ASSERT_ANY_THROW(input.data());
// }

// // Port Block
// using francor::processing::PortConfig;
// using francor::processing::InputBlock;
// using francor::processing::OutputBlock;

// TEST(InputBlock, Construct)
// {
//   constexpr std::array<PortConfig, 3> config = { PortConfig("input_0"), PortConfig("input_1"), PortConfig("input_2") };
//   InputBlock<int, int, int> block(config);

//   EXPECT_EQ(block.numInputs(), config.size());

//   EXPECT_EQ(francor::processing::get<0>(block).name(), "input_0");
//   EXPECT_EQ(francor::processing::get<1>(block).name(), "input_1");
//   EXPECT_EQ(francor::processing::get<2>(block).name(), "input_2");
// }

// TEST(OutputBlock, Construct)
// {
//   constexpr int value = 5;
//   int number0 = value + 0;
//   int number1 = value + 1;
//   int number2 = value + 2;
  
//   const std::array<PortConfig, 3> config = { PortConfig("output_0", &number0),
//                                              PortConfig("output_1", &number1),
//                                              PortConfig("output_2", &number2) };

//   OutputBlock<int, int, int> block(config);

//   EXPECT_EQ(block.numInputs(), config.size());

//   EXPECT_EQ(francor::processing::get<0>(block).name(), "output_0");
//   EXPECT_EQ(francor::processing::get<1>(block).name(), "output_1");
//   EXPECT_EQ(francor::processing::get<2>(block).name(), "output_2");

//   EXPECT_EQ(francor::processing::get<0>(block).data(), number0);
//   EXPECT_EQ(francor::processing::get<1>(block).data(), number1);
//   EXPECT_EQ(francor::processing::get<2>(block).data(), number2);
// }

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}