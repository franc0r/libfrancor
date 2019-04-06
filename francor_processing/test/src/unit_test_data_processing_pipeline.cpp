/**
 * Unit test for the data processing pipeline.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. April 2019
 */
#include <gtest/gtest.h>

#include "francor_processing/data_processing_pipeline.h"

using francor::processing::DataProcssingPipeline;
using francor::processing::DataProcessingStageIO;
using francor::processing::InputPortBlock;
using francor::processing::OutputPortBlock;
using francor::processing::data::SourcePort;
using francor::processing::data::DestinationPort;

class StageDummyIntToDouble : public DataProcessingStageIO<1, 1>
{
public:
  StageDummyIntToDouble(void) : DataProcessingStageIO<1, 1>("dummy int to double") { }
  virtual ~StageDummyIntToDouble(void) = default;

  virtual bool process(void) override final
  {
    _value = static_cast<double>(this->inputs().port(0).data<int>());
    return true;
  }

private:
  virtual bool configurePorts(InputPortBlock<num_inputs>& inputs, OutputPortBlock<num_outputs>& outputs) override final
  {
    bool ret = true;

    ret &= inputs.configurePort<int>(0, "int");
    ret &= outputs.configurePort<double>(0, "double", &_value);

    return ret;
  }

  virtual bool configureProcessing(void)
  {
    return true;
  }

  double _value = 0.0;
};

TEST(DataProcssingPipeline, Instantiate)
{
  DataProcssingPipeline pipeline;
}

TEST(DataProcssingPipeline, Process)
{
  // initialized pipeline
  DataProcssingPipeline pipeline;
  auto test = std::make_unique<StageDummyIntToDouble>();

  ASSERT_TRUE(pipeline.addStage(std::move(test)));
  ASSERT_FALSE(pipeline.addStage(std::make_unique<StageDummyIntToDouble>()));
  ASSERT_TRUE(pipeline.initialize());

  // initialized source data port and destination port and connect it
  const int value = 8;
  SourcePort source(SourcePort::create("data source", &value));
  DestinationPort destination(DestinationPort::create<double>("data destination"));

  ASSERT_TRUE(pipeline.connectDataSourcePort(source, "dummy int to double", "int"));
  ASSERT_TRUE(pipeline.connectDataDestinationPort(destination, "dummy int to double", "double"));

  // process pipeline
  EXPECT_TRUE(pipeline.process());
  EXPECT_EQ(destination.data<double>(), static_cast<double>(value));
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
