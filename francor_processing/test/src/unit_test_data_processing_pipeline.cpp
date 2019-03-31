/**
 * Unit test for the data processing pipeline.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. February 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_pipeline_stage.h"

using francor::processing::DetectLines;
using francor::processing::ExportClusteredPointsFromBitMask;

TEST(DetectLines, Instantiate)
{
  DetectLines detector;
}

TEST(ExportClusteredPointsFromBitMask, Instantiate)
{
  ExportClusteredPointsFromBitMask stage;
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
