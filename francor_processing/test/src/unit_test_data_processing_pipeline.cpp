/**
 * Unit test for the data processing pipeline.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. February 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_pipeline.h"

using francor::processing::DetectLines;

TEST(DetectLines, Instantiate)
{
  DetectLines detector;
}