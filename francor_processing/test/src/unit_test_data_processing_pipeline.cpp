/**
 * Unit test for the data processing pipeline.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. April 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_pipeline.h"

using francor::processing::ProcessingPipeline;
using francor::processing::ProcessingStage;
using francor::processing::ProcessingPipeline;
using francor::processing::data::SourcePort;
using francor::processing::data::DestinationPort;
using francor::processing::NoDataType;

class StageDummyIntToDouble : public ProcessingStage<NoDataType>
{
public:
  enum Inputs {
    IN_INT_VALUE = 0,
    COUNT_INPUTS
  };
  enum Outpus {
    OUT_DOUBLE_VALUE = 0,
    COUNT_OUTPUTS
  };

  StageDummyIntToDouble() : ProcessingStage<NoDataType>("dummy int to double", COUNT_INPUTS, COUNT_OUTPUTS) { }
  ~StageDummyIntToDouble() = default;

private:
  bool doProcess(NoDataType&) final
  {
    _value = static_cast<double>(this->getInputs()[0].data<int>());
    return true;
  }
  bool doInitialization() final
  {
    return true;
  }

  bool initializePorts() final
  {
    this->initializeInputPort<int>(0, "int");
    this->initializeOutputPort(0, "double", &_value);

    return true;
  }
  bool isReady() const final { return this->input(0).numOfConnections() > 0; }

  double _value = 0.0;
};

class Pipeline : public ProcessingPipeline<NoDataType, StageDummyIntToDouble>
{
public:
  Pipeline() : ProcessingPipeline<NoDataType, StageDummyIntToDouble>("pipeline", 1, 1) { }

private:
  bool configureStages() final
  {

    bool ret = true;

    ret &= std::get<0>(_stages).input("int").connect(this->input("input"));
    ret &= std::get<0>(_stages).output("double").connect(this->output("output"));
  
    return ret;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<int>(0, "input");
    this->initializeOutputPort<double>(0, "output");

    return true;
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
