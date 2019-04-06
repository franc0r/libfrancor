/**
 * Unit test for the data processing pipeline port blocks.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_pipeline_port_block.h"

using francor::processing::PortBlockImpl;
using francor::processing::InputPortBlock;
using francor::processing::OutputPortBlock;
using francor::processing::data::InputPort;
using francor::processing::data::OutputPort;

TEST(PortBlock, Instantiate)
{
  PortBlockImpl<InputPort, 1> block;
}

TEST(PortBlock, Disconnect)
{
  PortBlockImpl<InputPort, 3> block;

  const int value = 8;
  OutputPort output(OutputPort::create("output", &value));

  ASSERT_TRUE(block.configurePort<int>(0, "input 0"));
  ASSERT_TRUE(block.connect("input 0", output));
  ASSERT_EQ(output.numOfConnections(), 1);

  EXPECT_FALSE(block.disconnect("foo", output));
  EXPECT_EQ(output.numOfConnections(), 1);
  EXPECT_TRUE(block.disconnect("input 0", output));
  EXPECT_EQ(output.numOfConnections(), 0);
  EXPECT_EQ(block.port(0).numOfConnections(), 0);
}

TEST(InputPortBlock, Instantiate)
{
  InputPortBlock<3> block;
}

TEST(InputPortBlock, ConnectOutput)
{
  InputPortBlock<3> block;

  ASSERT_TRUE(block.configurePort<int>(0, "input 0"));
  ASSERT_TRUE(block.configurePort<int>(1, "input 1"));
  ASSERT_TRUE(block.configurePort<int>(2, "input 2"));

  const int value = 4;
  OutputPort output(OutputPort::create("output", &value));

  EXPECT_TRUE(block.connect("input 0", output));
  EXPECT_EQ(output.numOfConnections(), 1);
  EXPECT_EQ(block.port(0).data<int>(), value);
  EXPECT_EQ(&block.port(0).data<int>(), &value);

  EXPECT_FALSE(block.connect("foo", output));
  EXPECT_EQ(output.numOfConnections(), 1);
}

TEST(OutputPortBlock, Instantiate)
{
  OutputPortBlock<3> block;

  const int intValue = 7;
  const double doubleValue = 3.0;
  const char charValue = '0';

  ASSERT_TRUE(block.configurePort(0, "output 0", &intValue));
  ASSERT_TRUE(block.configurePort(1, "output 1", &doubleValue));
  ASSERT_TRUE(block.configurePort(2, "output 2", &charValue));

  InputPort input(InputPort::create<int>("input"));

  EXPECT_TRUE(block.connect("output 0", input));
  EXPECT_EQ(input.numOfConnections(), 1);
  EXPECT_EQ(input.data<int>(), intValue);
  EXPECT_EQ(&input.data<int>(), &intValue);

  EXPECT_FALSE(block.connect("foo", input));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}