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


template <typename DataStructureType, class... Stages>
class ProcessingPipeline : public DataInputOutput<data::SourcePort, data::DestinationPort>
{
  static constexpr std::size_t _num_stages = sizeof...(Stages);

public:
  ProcessingPipeline(void) = delete;
  ProcessingPipeline(const std::string& name, const std::size_t numOfInputs, const std::size_t numOfOutputs)
    : DataInputOutput<data::SourcePort, data::DestinationPort>(numOfInputs, numOfOutputs),
      _name(name)
  { }
  ~ProcessingPipeline(void) = default;

  bool initialize()
  {
    using francor::base::LogInfo;
    using francor::base::LogError;

    LogInfo() << "DataProcessingPipeline (name = " << _name << "): initialize pipeline."; 

    // initialize input and output ports of this pipeline
    this->initializePorts();

    if (!this->initializeStages())
    {
      LogError() << "DataProcessingPipeline (name = " << _name << "): error occurred during initialization of stages.";
      return false;
    }

    // configure all stages.
    if (!this->configureStages())
    {
      LogError() << "DataProcessingPipeline (name = " << _name << "): initializing of processing stages failed.";
      return false;
    }

    LogInfo() << "DataProcessingPipeline (name = " << _name << "): pipeline successfully initialized.";
    return true;
  }

  bool process()
  {
    NoDataType dummy;
    return this->process<>(dummy);
  }
  template<typename... ArgumentTypes>
  bool process(DataStructureType& model, ArgumentTypes&... data)
  {
    static_assert(_num_stages > 0, "ProcessingPipeline: no processing stage is added. Minimum one is required to process the pipeline");

    if constexpr (sizeof...(ArgumentTypes) == 0) {
      NoDataType dummy;
      return this->processStage<0>(model, dummy);
    }
    else {
      return this->processStage<0>(model, data...);
    }
  }

  const std::string& name() const noexcept { return _name; }

private:
  virtual bool configureStages() = 0;

  // initialize stages
  bool initializeStages() { return this->initializeStage<0>(); }
  template<std::size_t StageIndex, std::enable_if_t<(StageIndex < _num_stages), int> = 0>
  inline bool initializeStage()
  {
    return std::get<StageIndex>(_stages).initialize() && this->initializeStage<StageIndex + 1>();
  }
  template<std::size_t StageIndex, std::enable_if_t<(StageIndex >= _num_stages), int> = 0>
  inline bool initializeStage() { return true; }

  // process stages
  template<std::size_t StageIndex, typename ArgumentType = NoDataType>
  inline bool processStage(DataStructureType& model, ArgumentType& arg)
  {
    if constexpr (StageIndex >= _num_stages)
      return false;

    bool ret = true;
    
    if constexpr (std::is_same<typename std::tuple_element_t<StageIndex, std::tuple<Stages...>>::data_structure_type, NoDataType>::value) {
      NoDataType dummy;
      ret &= std::get<StageIndex>(_stages).process(dummy);
    }
    if constexpr (std::is_same<typename std::tuple_element_t<StageIndex, std::tuple<Stages...>>::data_structure_type, DataStructureType>::value) {
      ret &= std::get<StageIndex>(_stages).process(model);
    }
    if constexpr (std::is_same<typename std::tuple_element_t<StageIndex, std::tuple<Stages...>>::data_structure_type, ArgumentType>::value) {
      ret &= std::get<StageIndex>(_stages).process(arg);
    }
    if constexpr (StageIndex + 1 < _num_stages) {
      ret &= this->processStage<StageIndex + 1>(model, arg);
    }

    return ret;
  }
  
  // get stage index by name
  template<std::size_t StageIndex, std::enable_if_t<(StageIndex < _num_stages), int> = 0>
  inline constexpr std::size_t getStageIndexByName(const char* name) const
  {
    if constexpr (std::get<StageIndex>(_stages).name() == name)
      return StageIndex;

    return this->getStageIndexByName(name);
  }
  template<std::size_t StageIndex, std::enable_if_t<(StageIndex >= _num_stages), int> = 0>
  inline constexpr std::size_t getStageIndexByName(const char*) const { return 0; /* \todo do error handling here */ }


  const std::string _name; //> pipeline name

protected:
  std::tuple<Stages...> _stages; //> stages of this pipeline
};

} // end namespace processing

} // end namespace francor
