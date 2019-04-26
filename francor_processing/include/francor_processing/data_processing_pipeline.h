/**
 * Data processing pipeline.
 * Want to get more experience about data processing pipelines.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. February 2019
 */
#pragma once

#include "francor_base/log.h"

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
    using francor::base::LogError;
    using francor::base::LogDebug;
    LogDebug() << "DataProcessingPipeline: try to add stage \"" << stage->name() << "\" to pipeline.";

    if (this->containStageByName(stage->name()))
    {
      LogError() << "DataProcessingPipeline: stage with same name is already included. Can't add to stage \""
                 << stage->name() << "\" to pipeline.";
      return false;
    }

    LogDebug() << "DataProcessingPipeline: stage \"" << stage->name() << "\" successfully added to pipeline.";
    _stages.push_back(std::move(stage));
    return true;
  }

  bool connectInputWithOutput(const std::string& stageNameA,
                              const std::string& inputName,
                              const std::string& stageNameB,
                              const std::string& outputName)
  {
    using francor::base::LogError;
    using francor::base::LogDebug;
    LogDebug() << "DataProcessingPipeline: try to connect input \"" << inputName << "\" of stage \"" << stageNameA
               << "\" with output \"" << outputName << "\" of stage \"" << stageNameB << "\".";

    // get required stages
    const std::size_t stageIndexA = this->getStageIndexByName(stageNameA);
    const std::size_t stageIndexB = this->getStageIndexByName(stageNameB);

    if (stageIndexA >= _stages.size())
    {
      LogError() << "DataProcessingPipeline: stage \"" << stageNameA << "\" isn't contained in pipeline. Can't connect data ports";
      return false;
    }
    if (stageIndexB >= _stages.size())
    {
      LogError() << "DataProcessingPipeline: stage \"" << stageNameB << "\" isn't contained in pipeline. Can't connect data ports";
      return false;
    }

    // get input index
    std::size_t inputIndex;

    for (inputIndex = 0; inputIndex < _stages[stageIndexA]->numInputs(); ++inputIndex)
      if (_stages[stageIndexA]->input(inputIndex).name() == outputName)
        break;

    if (inputIndex >= _stages[stageIndexA]->numInputs())
    {
      LogError() << "DataProcessingPipeline: input port \"" << inputName << "\" not found. Can't connect data ports.";
      return false;
    }

    // get target output index
    std::size_t outputIndex;
    
    for (outputIndex = 0; outputIndex < _stages[stageIndexB]->numOutputs(); ++outputIndex)
      if (_stages[stageIndexB]->output(outputIndex).name() == outputName)
        break;
        
    if (outputIndex >= _stages[stageIndexB]->numOutputs())
    {
      LogError() << "DataProcessingPipeline: output port \"" << outputName << "\" not found. Can't connect data ports.";
      return false;
    }

    LogDebug() << "DataProcessingPipeline: data ports successfully connected.";
    return _stages[stageIndexA]->input(inputIndex).connect(_stages[stageIndexB]->output(outputIndex));
  }

  bool connectDataSourcePort(data::SourcePort& port, const std::string& stageName, const std::string& inputName)
  {
    using francor::base::LogError;
    using francor::base::LogDebug;
    LogDebug() << "DataProcessingPipeline: try to connect input \"" << inputName << "\" of stage \"" << stageName
               << "\" with data source \"" << port.name() << "\".";

    // get required stage
    const std::size_t stageIndex = this->getStageIndexByName(stageName);

    if (stageIndex >= _stages.size())
    {
      LogError() << "DataProcessingPipeline: stage \"" << stageName << " isn't contained in pipeline. Can't connect data ports";
      return false;
    }

    // get input port index
    std::size_t inputIndex;

    for (inputIndex = 0; inputIndex < _stages[stageIndex]->numInputs(); ++inputIndex)
      if (_stages[stageIndex]->input(inputIndex).name() == inputName)
        break;

    if (inputIndex >= _stages[stageIndex]->numInputs())
    {
      LogError() << "DataProcessingPipeline: input port \"" << inputName << "\" not found. Can't connect data ports.";
      return false;
    }

    LogDebug() << "DataProcessingPipeline: data ports successfully connected.";
    return _stages[stageIndex]->input(inputIndex).connect(port);
  }

  bool connectDataDestinationPort(data::DestinationPort& port, const std::string& stageName, const std::string& outputName)
  {
    using francor::base::LogError;
    using francor::base::LogDebug;
    LogDebug() << "DataProcessingPipeline: try to connect output \"" << outputName << "\" of stage \"" << stageName
               << "\" with data destination port \"" << port.name() << "\".";    
    // get required stage
    const std::size_t stageIndex = this->getStageIndexByName(stageName);

    if (stageIndex >= _stages.size())
    {
      LogError() << "DataProcessingPipeline: stage \"" << stageName << " isn't contained in pipeline. Can't connect data ports";
      return false;
    }

    // get input port index
    std::size_t outputIndex;

    for (outputIndex = 0; outputIndex < _stages[stageIndex]->numOutputs(); ++outputIndex)
      if (_stages[stageIndex]->output(outputIndex).name() == outputName)
        break;

    if (outputIndex >= _stages[stageIndex]->numOutputs())
    {
      LogError() << "DataProcessingPipeline: output port \"" << outputName << "\" not found. Can't connect data ports.";
      return false;
    }

    LogDebug() << "DataProcessingPipeline: data ports successfully connected.";
    return _stages[stageIndex]->output(outputIndex).connect(port);
  }

  bool initialize(void)
  {
    using francor::base::LogInfo;
    using francor::base::LogError;

    LogInfo() << "DataProcessingPipeline: initialize pipeline.";
    bool ret = true;

    for (auto& stage : _stages)
      ret &= stage->initialize();

    if (!ret)
      LogError() << "DataProcessingPipeline: error occurred during initializing of pipeline.";

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
