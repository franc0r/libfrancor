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

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
