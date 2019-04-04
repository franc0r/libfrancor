/**
 * Unit test for the data processing ports.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 1. April 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_port.h"

// Port
using francor::processing::Port;

TEST(Port, DefaultConstructed)
{
  Port port;

  EXPECT_EQ(port.name(), "none");
  EXPECT_EQ(port.id(), 0);
  EXPECT_EQ(port.dataFlow(), francor::processing::Port::Direction::NONE);
  EXPECT_EQ(port.type(), typeid(void));
  EXPECT_EQ(port.numOfConnections(), 0);
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
  EXPECT_EQ(port.numOfConnections(), 0);

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
  EXPECT_EQ(moved.numOfConnections(), 0);
  EXPECT_EQ(&moved.data<int>(), &data);

  // the origin port must be unconfigured after move
  EXPECT_EQ(origin.name(), "none");
  EXPECT_EQ(origin.id(), 0);
  EXPECT_EQ(origin.dataFlow(), francor::processing::Port::Direction::NONE);
  EXPECT_EQ(origin.type(), typeid(void));
  EXPECT_EQ(origin.numOfConnections(), 0);
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
  EXPECT_EQ(moved.numOfConnections(), 0);
  EXPECT_EQ(&moved.data<int>(), &data);

  // the origin port must be unconfigured after move
  EXPECT_EQ(origin.name(), "none");
  EXPECT_EQ(origin.id(), 0);
  EXPECT_EQ(origin.dataFlow(), francor::processing::Port::Direction::NONE);
  EXPECT_EQ(origin.type(), typeid(void));
  EXPECT_EQ(origin.numOfConnections(), 0);
  ASSERT_ANY_THROW(origin.data<int>());  
}

TEST(Port, ConnectInputTo)
{
  Port deviceUnderTest("source", francor::processing::Port::Direction::IN, static_cast<int*>(nullptr));
  Port input("input", francor::processing::Port::Direction::IN, static_cast<int*>(nullptr));

  // input to input is not permitted
  EXPECT_FALSE(deviceUnderTest.connect(input));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 0);
  EXPECT_EQ(input.numOfConnections(), 0);

  const int value = 5;
  Port output0("output 0", francor::processing::Port::Direction::OUT, &value);

  // input to output is valid
  EXPECT_TRUE(deviceUnderTest.connect(output0));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 1);
  EXPECT_EQ(output0.numOfConnections(), 1);
  EXPECT_EQ(deviceUnderTest.data<int>(), value);
  EXPECT_EQ(&deviceUnderTest.data<int>(), &value);

  Port output1("output 1", francor::processing::Port::Direction::OUT, &value);

  // one connection is the maximum number of connection for inputs
  EXPECT_FALSE(deviceUnderTest.connect(output1));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 1);
  EXPECT_EQ(output1.numOfConnections(), 0);
}

TEST(Port, ConnectOutputTo)
{
  std::array<Port, Port::maxNumOfConnections()> inputs;

  for (std::size_t i = 0; i < inputs.size(); ++i)
    inputs[i] = std::move(Port(std::string("input ") + std::to_string(i), francor::processing::Port::Direction::IN, static_cast<int*>(nullptr)));

  Port input88("input 88", francor::processing::Port::Direction::IN, static_cast<int*>(nullptr));

  const int value = 6;
  Port deviceUnderTest("target", francor::processing::Port::Direction::OUT, &value);
  Port output("output", francor::processing::Port::Direction::OUT, &value);

  // output to output is not permitted
  EXPECT_FALSE(deviceUnderTest.connect(output));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 0);
  EXPECT_EQ(output.numOfConnections(), 0);

  // valid connections to inputs
  for (auto& input : inputs)
    EXPECT_TRUE(deviceUnderTest.connect(input));

  EXPECT_EQ(deviceUnderTest.numOfConnections(), inputs.size());

  for (const auto& input : inputs)
  {
    EXPECT_EQ(input.data<int>(), value);
    EXPECT_EQ(&input.data<int>(), &value);
  }

  // not permitted, because maximum number of connections is reached
  EXPECT_FALSE(deviceUnderTest.connect(input88));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), inputs.size());
  EXPECT_EQ(input88.numOfConnections(), 0);
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
