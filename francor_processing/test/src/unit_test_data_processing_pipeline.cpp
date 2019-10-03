/**
 * Unit test for the data processing pipeline.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. April 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_pipeline.h"

using francor::processing::ProcessingPipeline;
using francor::processing::ProcessingStageParent;
using francor::processing::ProcessingPipelineParent;
using francor::processing::data::SourcePort;
using francor::processing::data::DestinationPort;

class StageDummyIntToDouble : public ProcessingStageParent<1, 1>
{
public:
  StageDummyIntToDouble(void) : ProcessingStageParent<1, 1>("dummy int to double") { }
  virtual ~StageDummyIntToDouble(void) = default;

  bool doProcess(const std::shared_ptr<void>&) final
  {
    _value = static_cast<double>(this->getInputs()[0].data<int>());
    return true;
  }
  bool doInitialization() final
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

class Pipeline : public ProcessingPipelineParent<1, 1>
{
public:
  Pipeline() : ProcessingPipelineParent<1, 1>("pipeline") { }

private:
  bool configureStages() final
  {
    auto stage = std::make_unique<StageDummyIntToDouble>();

    bool ret = true;

    ret &= stage->initialize();
    ret &= stage->input("int").connect(this->input("input"));
    ret &= stage->output("double").connect(this->output("output"));
    ret &= this->addStage(std::move(stage));
  
    return ret;
  }
  void initializePorts() final
  {
    this->initializeInputPort<int>(0, "input");
    this->initializeOutputPort<double>(0, "output");
  }
};

TEST(ProcssingPipeline, Instantiate)
{
  Pipeline pipeline;
}

TEST(ProcssingPipeline, Process)
{
  // initialized pipeline
  Pipeline pipeline;

  // initialized source data port and destination port and connect it
  const int value = 8;

  ASSERT_TRUE(pipeline.initialize());

  pipeline.input("input").assign(&value);

  // process pipeline
  EXPECT_TRUE(pipeline.process());
  EXPECT_EQ(pipeline.output("output").data<double>(), static_cast<double>(value));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  francor::base::setLogLevel(francor::base::LogLevel::DEBUG);
  return RUN_ALL_TESTS();
}
