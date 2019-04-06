/**
 * Unit test for the data processing ports.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 1. April 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_port.h"

// Port
using francor::processing::data::Port;
// using francor::processing::data::Port::Direction;

TEST(Port, DefaultConstructed)
{
  Port port;

  EXPECT_EQ(port.name(), "none");
  EXPECT_EQ(port.id(), 0);
  EXPECT_EQ(port.dataFlow(), Port::Direction::NONE);
  EXPECT_EQ(port.type(), typeid(void));
  EXPECT_EQ(port.numOfConnections(), 0);
  ASSERT_ANY_THROW(port.data<int>());
}

TEST(Port, InitializedWithDataType)
{
  int data;
  Port port("port int", Port::Direction::OUT, &data);

  // valid access
  EXPECT_EQ(port.name(), "port int");
  EXPECT_EQ(port.dataFlow(), Port::Direction::OUT);
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
  Port origin("port int", Port::Direction::OUT, &data);
  const std::size_t idOrigin = origin.id();

  // move to new port and check if the attributes were moved properly
  Port moved(std::move(origin));

  EXPECT_EQ(moved.name(), "port int");
  EXPECT_EQ(moved.id(), idOrigin);
  EXPECT_EQ(moved.dataFlow(), Port::Direction::OUT);
  EXPECT_EQ(moved.type(), typeid(data));
  EXPECT_EQ(moved.numOfConnections(), 0);
  EXPECT_EQ(&moved.data<int>(), &data);

  // the origin port must be unconfigured after move
  EXPECT_EQ(origin.name(), "none");
  EXPECT_EQ(origin.id(), 0);
  EXPECT_EQ(origin.dataFlow(), Port::Direction::NONE);
  EXPECT_EQ(origin.type(), typeid(void));
  EXPECT_EQ(origin.numOfConnections(), 0);
  ASSERT_ANY_THROW(origin.data<int>());
}

TEST(Port, MovedAssigned)
{
  // instantiate initialized output port
  int data;
  Port origin("port int", Port::Direction::OUT, &data);
  const std::size_t idOrigin = origin.id();

  // move to new port and check if the attributes were moved properly
  Port moved;
  moved = std::move(origin);

  EXPECT_EQ(moved.name(), "port int");
  EXPECT_EQ(moved.id(), idOrigin);
  EXPECT_EQ(moved.dataFlow(), Port::Direction::OUT);
  EXPECT_EQ(moved.type(), typeid(data));
  EXPECT_EQ(moved.numOfConnections(), 0);
  EXPECT_EQ(&moved.data<int>(), &data);

  // the origin port must be unconfigured after move
  EXPECT_EQ(origin.name(), "none");
  EXPECT_EQ(origin.id(), 0);
  EXPECT_EQ(origin.dataFlow(), Port::Direction::NONE);
  EXPECT_EQ(origin.type(), typeid(void));
  EXPECT_EQ(origin.numOfConnections(), 0);
  ASSERT_ANY_THROW(origin.data<int>());  
}

TEST(Port, ConnectInputTo)
{
  Port deviceUnderTest("source", Port::Direction::IN, static_cast<int*>(nullptr));
  Port input("input", Port::Direction::IN, static_cast<int*>(nullptr));

  // input to input is not permitted
  EXPECT_FALSE(deviceUnderTest.connect(input));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 0);
  EXPECT_EQ(input.numOfConnections(), 0);

  // try to connect output with wrong data type
  const double wrongType = 1.0;
  Port output0("output 0", Port::Direction::OUT, &wrongType);

  EXPECT_FALSE(deviceUnderTest.connect(output0));  
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 0);
  EXPECT_EQ(output0.numOfConnections(), 0);

  const int value = 5;
  Port output1("output 1", Port::Direction::OUT, &value);

  // input to output is valid
  EXPECT_TRUE(deviceUnderTest.connect(output1));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 1);
  EXPECT_EQ(output1.numOfConnections(), 1);
  EXPECT_EQ(deviceUnderTest.data<int>(), value);
  EXPECT_EQ(&deviceUnderTest.data<int>(), &value);

  Port output2("output 2", Port::Direction::OUT, &value);

  // one connection is the maximum number of connection for inputs
  EXPECT_FALSE(deviceUnderTest.connect(output2));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 1);
  EXPECT_EQ(output2.numOfConnections(), 0);
}

TEST(Port, ConnectOutputTo)
{
  // connect maximum number of connections
  std::array<Port, Port::maxNumOfConnections()> inputs;

  for (std::size_t i = 0; i < inputs.size(); ++i)
    inputs[i] = std::move(Port(std::string("input ") + std::to_string(i), Port::Direction::IN, static_cast<int*>(nullptr)));

  const int value = 6;
  Port deviceUnderTest("target", Port::Direction::OUT, &value);
  Port output("output", Port::Direction::OUT, &value);

  // output to output is not permitted
  EXPECT_FALSE(deviceUnderTest.connect(output));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 0);
  EXPECT_EQ(output.numOfConnections(), 0);

  // not permitted, because data types do not match
  Port input42("input 42", Port::Direction::IN, static_cast<double*>(nullptr));

  EXPECT_FALSE(deviceUnderTest.connect(input42));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), 0);
  EXPECT_EQ(input42.numOfConnections(), 0);
  
  // valid connections to inputs
  for (auto& input : inputs)
  {
    EXPECT_TRUE(deviceUnderTest.connect(input));
    // is already connected --> false
    EXPECT_FALSE(deviceUnderTest.connect(input));
  }

  EXPECT_EQ(deviceUnderTest.numOfConnections(), inputs.size());

  for (const auto& input : inputs)
  {
    EXPECT_TRUE(input.isConnectedWith(deviceUnderTest));
    EXPECT_TRUE(deviceUnderTest.isConnectedWith(input));
    EXPECT_EQ(input.data<int>(), value);
    EXPECT_EQ(&input.data<int>(), &value);
  }

  // not permitted, because maximum number of connections is reached
  Port input88("input 88", Port::Direction::IN, static_cast<int*>(nullptr));

  EXPECT_FALSE(deviceUnderTest.connect(input88));
  EXPECT_EQ(deviceUnderTest.numOfConnections(), inputs.size());
  EXPECT_EQ(input88.numOfConnections(), 0);
}

TEST(Port, DisconnectFromInput)
{
  const int value = 5;
  Port input("input", Port::Direction::IN, static_cast<int*>(nullptr));
  Port output0("output 0", Port::Direction::OUT, &value);
  Port output1("output 1", Port::Direction::OUT, &value);

  ASSERT_TRUE(input.connect(output0));

  // try to disconnect a not connected output
  EXPECT_FALSE(input.disconnect(output1));

  // disconnect a connected output
  EXPECT_TRUE(input.disconnect(output0));
  EXPECT_ANY_THROW(input.data<int>());
}

TEST(Port, DisconnectFromOutput)
{
  // connect maximum number of connections
  std::array<Port, Port::maxNumOfConnections()> inputs;

  for (std::size_t i = 0; i < inputs.size(); ++i)
    inputs[i] = std::move(Port(std::string("input ") + std::to_string(i), Port::Direction::IN, static_cast<int*>(nullptr)));

  const int value = 6;
  Port output("output", Port::Direction::OUT, &value);

  // establish connections
  for (auto& input : inputs)
    ASSERT_TRUE(output.connect(input));

  ASSERT_EQ(output.numOfConnections(), inputs.size());

  // disconnect and reconnect every connections
  for (auto& input : inputs)
  {
    EXPECT_TRUE(output.disconnect(input));
    EXPECT_EQ(output.numOfConnections(), inputs.size() - 1);
    EXPECT_ANY_THROW(input.data<int>());

    EXPECT_TRUE(output.connect(input));
    EXPECT_EQ(output.numOfConnections(), inputs.size());
    EXPECT_EQ(input.data<int>(), value);
    EXPECT_EQ(&input.data<int>(), &value);
  }

  // destroy object by call the destructor of class Port
  output.~Port();

  // all inputs must be disconnected
  for (const auto& input : inputs)
  {
    EXPECT_EQ(input.numOfConnections(), 0);
    EXPECT_ANY_THROW(input.data<int>());
  }
}

TEST(Port, MoveConnetions)
{
  // connect maximum number of connections
  std::array<Port, Port::maxNumOfConnections()> inputs;

  for (std::size_t i = 0; i < inputs.size(); ++i)
    inputs[i] = std::move(Port(std::string("input ") + std::to_string(i), Port::Direction::IN, static_cast<int*>(nullptr)));

  const int value = 6;
  Port origin("origin", Port::Direction::OUT, &value);

  // establish connections
  for (auto& input : inputs)
    ASSERT_TRUE(origin.connect(input));

  ASSERT_EQ(origin.numOfConnections(), inputs.size());

  // move origin including all connections
  const int newValue = 7;
  Port input88("input 88", Port::Direction::IN, static_cast<int*>(nullptr));
  Port moved("moved", Port::Direction::OUT, &newValue);

  // before established connections must be disconnected
  ASSERT_TRUE(moved.connect(input88));
  EXPECT_EQ(moved.numOfConnections(), 1);
  EXPECT_EQ(input88.numOfConnections(), 1);

  moved = std::move(origin);

  EXPECT_EQ(input88.numOfConnections(), 0);
  EXPECT_EQ(moved.numOfConnections(), inputs.size());

  for (const auto& input : inputs)
  {
    EXPECT_FALSE(input.isConnectedWith(origin));
    EXPECT_TRUE(input.isConnectedWith(moved));
    EXPECT_NE(&input.data<int>(), &newValue);
    EXPECT_EQ(&input.data<int>(), &value);
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
