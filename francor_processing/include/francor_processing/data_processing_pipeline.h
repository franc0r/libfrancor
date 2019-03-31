/**
 * Data processing pipeline.
 * Want to get more experience about data processing pipelines.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. February 2019
 */
#pragma once

#include <string>

#include "francor_base/vector.h"
#include "francor_base/line.h"

#include "francor_algorithm/ransac.h"

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
template <template<typename...> class PortBlock, typename... DataTypes>
class DataProcessingStagePort
{
protected:
  DataProcessingStagePort(void) = delete;
  DataProcessingStagePort(const std::array<PortConfig, PortBlock<DataTypes...>::numInputs()>& config) : _ports(config) { }

protected:
  PortBlock<DataTypes...> _ports;
};

template <typename... DataTypes>
class DataProcessingStageOutput : public DataProcessingStagePort<OutputBlock, DataTypes...>
{
public:
  DataProcessingStageOutput(const std::array<PortConfig, OutputBlock<DataTypes...>::numInputs()>& config) : DataProcessingStagePort<OutputBlock, DataTypes...>(config) { }

  template <std::size_t Index>
  auto& getOutput(void) { return get<Index>(DataProcessingStagePort<OutputBlock, DataTypes...>::_ports); }
};

template <typename... DataTypes>
class DataProcessingStageInput : public DataProcessingStagePort<InputBlock, DataTypes...>
{
public:
  DataProcessingStageInput(const std::array<PortConfig, InputBlock<DataTypes...>::numInputs()>& config) : DataProcessingStagePort<InputBlock, DataTypes...>(config) { }

  template <std::size_t Index>
  auto& getInput(void) { return get<Index>(DataProcessingStagePort<InputBlock, DataTypes...>::_ports); }
};


//TODO: move detect line class to a seperate file
/**
 * \brief This class searches lines in a 2d point set using a line ransac.
 */
class DetectLines : public DataProcessingStage,
                    public DataProcessingStageInput<base::VectorVector2d>,
                    public DataProcessingStageOutput<base::LineVector>
{
public:
  DetectLines(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : DataProcessingStage("detect lines"),
      DataProcessingStageInput<base::VectorVector2d>({ PortConfig("input points") }),
      DataProcessingStageOutput<base::LineVector>({ PortConfig("output lines", &_lines) })
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  virtual ~DetectLines(void) = default;

  virtual bool process(void)
  {
    _lines = _detector(this->getInput<0>().data());
    return true;
  }

private:
  base::LineVector _lines;
  algorithm::LineRansac _detector;
};

} // end namespace processing

} // end namespace francor