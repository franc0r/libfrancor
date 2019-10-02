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

class ProcssingPipeline
{
public:
  ProcssingPipeline(void) = default;
  ~ProcssingPipeline(void) = default;

  bool addStage(std::unique_ptr<ProcessingStage> stage)
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

    LogInfo() << "DataProcessingPipeline: pipeline successfully initialized.";
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
                        [&] (const std::unique_ptr<ProcessingStage>& stage) { return stage->name() == stageName; } )
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

  std::vector<std::unique_ptr<ProcessingStage>> _stages;
};

} // end namespace processing

} // end namespace francor
