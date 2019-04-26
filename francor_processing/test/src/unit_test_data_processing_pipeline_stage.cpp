/**
 * Unit test for the data processing pipeline stage.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. April 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_pipeline_stage.h"

using francor::processing::DetectLines;
using francor::processing::DetectLineSegments;
using francor::processing::ExportClusteredPointsFromBitMask;

TEST(DetectLines, InstantiateAndInitialize)
{
  DetectLines detector;

  EXPECT_TRUE(detector.initialize());
}

TEST(DetectLineSegments, InstantiateAndInitialize)
{
  DetectLineSegments detector;

  EXPECT_TRUE(detector.initialize());
}

TEST(ExportClusteredPointsFromBitMask, InstantiateAndInitialize)
{
  ExportClusteredPointsFromBitMask stage;

  EXPECT_TRUE(stage.initialize());
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
