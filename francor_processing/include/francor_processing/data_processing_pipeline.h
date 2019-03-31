/**
 * Data processing pipeline.
 * Want to get more experience about data processing pipelines.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. February 2019
 */
#pragma once

#include <string>
#include <vector>

#include "francor_processing/data_processing_pipeline_port.h"

namespace francor
{

namespace processing
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Processing Stage
class DataProcessingStage
{
protected:
  DataProcessingStage(void) = delete;
  DataProcessingStage(const std::string& name) : _name(name) { }
  DataProcessingStage(const DataProcessingStage&) = delete;
  DataProcessingStage(DataProcessingStage&&) = delete;
  virtual ~DataProcessingStage(void) = default;

  DataProcessingStage& operator=(const DataProcessingStage&) = delete;
  DataProcessingStage& operator=(DataProcessingStage&&) = delete;

  virtual bool process(void) = 0;

  inline const std::string& name(void) const noexcept { return _name; }

private:
  const std::string _name;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Processing Stage Port including specialization
template <typename... DataTypes>
class DataProcessingStageOutput : public OutputBlock<DataTypes...>
{
public:
  DataProcessingStageOutput(const std::array<PortConfig, OutputBlock<DataTypes...>::numInputs()>& config) : OutputBlock<DataTypes...>(config) { }

  template <std::size_t Index>
  auto getOutput(void) -> decltype(get<Index>(*this)) { return get<Index>(*this); }
};

template <typename... DataTypes>
class DataProcessingStageInput : public InputBlock<DataTypes...>
{
public:
  DataProcessingStageInput(const std::array<PortConfig, InputBlock<DataTypes...>::numInputs()>& config) : InputBlock<DataTypes...>(config) { }

  template <std::size_t Index>
  auto getInput(void) -> decltype(get<Index>(*this)) { return get<Index>(*this); }
};



class DataProcssingPipeline
{
public:
  DataProcssingPipeline(void) = default;
  ~DataProcssingPipeline(void) = default;

  template <typename DataType>
  DataSourcePort<DataType>&& createDataSourcePort(const DataType& dataSource)
  {
    if (_stages.size() == 0)
      return std::move(DataSourcePort<DataType>());

    
  }  

  void addStage(std::unique_ptr<DataProcessingStage>& stage)
  {

  }

private:
  std::vector<std::unique_ptr<DataProcessingStage>> _stages;
};

} // end namespace processing

} // end namespace francor
