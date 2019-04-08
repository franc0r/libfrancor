/**
 * Data processing pipeline.
 * Want to get more experience about data processing pipelines.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. February 2019
 */
#pragma once

#include "francor_processing/data_processing_pipeline_stage.h"

#include <vector>
#include <memory>

namespace francor
{

namespace processing
{


class DataProcssingPipeline
{
public:
  DataProcssingPipeline(void) = default;
  ~DataProcssingPipeline(void) = default;

  bool addStage(std::unique_ptr<DataProcessingStage> stage)
  {
    if (this->containStageByName(stage->name()))
    {
      //TODO: print error
      return false;
    }

    _stages.push_back(std::move(stage));
    return true;
  }

  bool connectInputWithOutput(const std::string& stageNameA, const std::string& inputName, const std::string& stageNameB, const std::string& outputName)
  {
    // get required stages
    const std::size_t stageIndexA = this->getStageIndexByName(stageNameA);
    const std::size_t stageIndexB = this->getStageIndexByName(stageNameB);

    if (stageIndexA >= _stages.size())
    {
      //TODO: print error
      return false;
    }
    if (stageIndexB >= _stages.size())
    {
      //TODO: print error
      return false;
    }

    // get input index
    std::size_t inputIndex;

    for (inputIndex = 0; inputIndex < _stages[stageIndexA]->numInputs(); ++inputIndex)
      if (_stages[stageIndexA]->input(inputIndex).name() == outputName)
        break;

    if (inputIndex >= _stages[stageIndexA]->numInputs())
    {
      //TODO: print error
      return false;
    }

    // get target output index
    std::size_t outputIndex;
    
    for (outputIndex = 0; outputIndex < _stages[stageIndexB]->numOutputs(); ++outputIndex)
      if (_stages[stageIndexB]->output(outputIndex).name() == outputName)
        break;
        
    if (outputIndex >= _stages[stageIndexB]->numOutputs())
    {
      //TODO: print error
      return false;
    }

    return _stages[stageIndexA]->input(inputIndex).connect(_stages[stageIndexB]->output(outputIndex));
  }

  bool connectDataSourcePort(data::SourcePort& port, const std::string& stageName, const std::string& inputName)
  {
    // get required stage
    const std::size_t stageIndex = this->getStageIndexByName(stageName);

    if (stageIndex >= _stages.size())
    {
      //TODO: print error
      return false;
    }

    // get input port index
    std::size_t inputIndex;

    for (inputIndex = 0; inputIndex < _stages[stageIndex]->numInputs(); ++inputIndex)
      if (_stages[stageIndex]->input(inputIndex).name() == inputName)
        break;

    if (inputIndex >= _stages[stageIndex]->numInputs())
    {
      //TODO: print error
      return false;
    }

    return _stages[stageIndex]->input(inputIndex).connect(port);
  }

  bool connectDataDestinationPort(data::DestinationPort& port, const std::string& stageName, const std::string& outputName)
  {
    // get required stage
    const std::size_t stageIndex = this->getStageIndexByName(stageName);

    if (stageIndex >= _stages.size())
    {
      //TODO: print error
      return false;
    }

    // get input port index
    std::size_t outputIndex;

    for (outputIndex = 0; outputIndex < _stages[stageIndex]->numOutputs(); ++outputIndex)
      if (_stages[stageIndex]->output(outputIndex).name() == outputName)
        break;

    if (outputIndex >= _stages[stageIndex]->numOutputs())
    {
      //TODO: print error
      return false;
    }

    return _stages[stageIndex]->output(outputIndex).connect(port);
  }

  bool initialize(void)
  {
    bool ret = true;

    for (auto& stage : _stages)
      ret &= stage->initialize();

    return ret;
  }

  bool process(void)
  {
    bool ret = true;

    for (auto& stage : _stages)
      ret &= stage->process();

    return ret;
  }


private:
  bool containStageByName(const std::string& stageName) const
  {
    return std::find_if(_stages.begin(),
                        _stages.end(),
                        [&] (const std::unique_ptr<DataProcessingStage>& stage) { return stage->name() == stageName; } )
           !=
           _stages.end();
  }
  std::size_t getStageIndexByName(const std::string& stageName) const
  {
    for (std::size_t stageIndex = 0; stageIndex < _stages.size(); ++stageIndex)
      if (_stages[stageIndex]->name() == stageName)
        return stageIndex;

    return _stages.size();
  }

  std::vector<std::unique_ptr<DataProcessingStage>> _stages;
};

} // end namespace processing

} // end namespace francor
