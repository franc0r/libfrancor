/**
 * Unit test for the data processing pipeline.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#include <gtest/gtest.h>

#include <type_traits>

#include "francor_processing/data_processing_pipeline.h"

using francor::processing::PortIn;
using francor::processing::PortOut;

TEST(Port, OutputConnectInput)
{
  PortOut<int> output("output");
  PortIn<int> input("input");

  ASSERT_TRUE(output.connect(input));
}

TEST(Port, InputConnectOutput)
{
  PortOut<int> output("output");
  PortIn<int> input("input");

  ASSERT_TRUE(input.connect(output));
}

// TEST(Port, InputConnectInput)
// {
//   PortIn<int> inputA("input a");  
//   PortIn<int> inputB("input b");

//   ASSERT_FALSE(inputA.connect(inputB));  
// }

// TEST(Port, OutputConnectOutput)
// {
//   PortOut<int> outputA("output a");  
//   PortOut<int> outputB("output b");

//   ASSERT_FALSE(outputA.connect(outputB));  
// }

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}