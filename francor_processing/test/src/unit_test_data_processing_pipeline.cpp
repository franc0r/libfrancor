/**
 * Unit test for the data processing pipeline.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. April 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_pipeline.h"

using francor::processing::ProcssingPipeline;
using francor::processing::ProcessingStageParent;
using francor::processing::InputPortBlock;
using francor::processing::OutputPortBlock;
using francor::processing::data::SourcePort;
using francor::processing::data::DestinationPort;

class StageDummyIntToDouble : public ProcessingStageParent<1, 1>
{
public:
  StageDummyIntToDouble(void) : ProcessingStageParent<1, 1>("dummy int to double") { }
  virtual ~StageDummyIntToDouble(void) = default;

  bool doProcess(void) final
  {
    _value = static_cast<double>(this->getInputs()[0].data<int>());
    return true;
  }
  bool initialize() final
  {
    return true;
  }

private:
  void initializePorts() final
  {
    this->initializeInputPort<int>(0, "int");
    this->initializeOutputPort(0, "double", &_value);
  }

  double _value = 0.0;
};

TEST(ProcssingPipeline, Instantiate)
{
  ProcssingPipeline pipeline;
}

TEST(ProcssingPipeline, Process)
{
  // initialized pipeline
  ProcssingPipeline pipeline;
  auto test = std::make_unique<StageDummyIntToDouble>();

  // initialized source data port and destination port and connect it
  const int value = 8;
  SourcePort source(SourcePort::create("data source", &value));
  DestinationPort destination(DestinationPort::create<double>("data destination"));

  ASSERT_TRUE(pipeline.addStage(std::move(test)));

  auto stage = std::make_unique<StageDummyIntToDouble>();
  stage->connectToInput("int", source);
  stage->connectToOutput("double", destination);
  ASSERT_FALSE(pipeline.addStage(std::move(stage)));

  ASSERT_TRUE(pipeline.initialize());

  // process pipeline
  EXPECT_TRUE(pipeline.process());
  EXPECT_EQ(destination.data<double>(), static_cast<double>(value));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
