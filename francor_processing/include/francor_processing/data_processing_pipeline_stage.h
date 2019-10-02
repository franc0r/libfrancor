/**
 * Data processing pipeline stages.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 31. February 2019
 */
#pragma once

#include "francor_base/vector.h"
#include "francor_base/line.h"
#include "francor_base/log.h"

#include "francor_algorithm/ransac.h"

#include "francor_vision/image.h"
#include "francor_vision/image_filter_pipeline.h"
#include "francor_vision/image_filter_criteria.h"

#include "francor_processing/data_processing_pipeline_port_block.h"

#include <string>
#include <stdexcept>

namespace francor
{

namespace processing
{

class ProcessingStage
{
protected:
  ProcessingStage() = delete;
  ProcessingStage(const std::string& stageName) : _name(stageName) { }

public:
  virtual ~ProcessingStage() = default;

  bool process()
  {
    bool ret = this->doProcess();
    // TODO: add diagnostic

    return ret;
  }
  bool initialize()
  {
    return this->doInitialization();
  }

  inline const std::string& name() const noexcept { return _name; }

protected:
  virtual bool doProcess() = 0;
  virtual bool doInitialization() = 0;
  
private:
  const std::string _name;
};

template <typename InputType, std::size_t NumOfInputs, typename OutputType, std::size_t NumOfOutputs>
class DataInputOutput
{
public:
  DataInputOutput() {  };
  DataInputOutput(const DataInputOutput&) = delete;
  virtual ~DataInputOutput() = default;

  DataInputOutput& operator=(const DataInputOutput&) = delete;

  InputType& input(const std::string& portName)
  {
    return this->findInput(portName);
  }
  OutputType& output(const std::string& portName)
  {
    return this->findOutput(portName);
  }
  void connectToInput(const std::string& name, OutputType& output)
  {
    auto& input = this->findInput(name);
    input.connect(output);
  }
  void connectToOutput(const std::string& name, InputType& input)
  {
    auto& output = this->findOutput(name);
    output.connect(input);
  }

protected:
  inline std::array<InputType, NumOfInputs>& getInputs() { return _input_ports; }
  inline std::array<OutputType, NumOfOutputs>& getOutputs() { return _output_ports; }

  virtual void initializePorts() = 0;

  template <typename DataType>
  inline void initializeInputPort(const std::size_t index, const std::string& name)
  {
    _input_ports[index] = InputType::template create<DataType>(name);
  }
  template <typename DataType>
  void initializeOutputPort(const std::size_t index, const std::string& name, DataType const* const data)
  {
    _output_ports[index] = OutputType::template create(name, data);
  }
  bool initialize()
  {
    this->initializePorts();

    for (const auto& input : _input_ports)
      ; // TODO: check if input is initialized

    for (const auto& output : _output_ports)
      ; // TODO: check if output is initialized

    return true;
  }

private:
  InputType& findInput(const std::string& portName)
  {
    for (auto& port : _input_ports)
      if (port.name() == portName)
        return port;

    base::LogError() << "Processing Stage: input port name is unkown.";
    throw std::invalid_argument("Processing Stage: input port name is unkown.");
  }
  OutputType& findOutput(const std::string portName)
  {
    for (auto& port : _output_ports)
      if (port.name() == portName)
        return port;

    base::LogError() << "Processing Stage: output port name is unkown.";
    throw std::invalid_argument("Processing Stage: output port name is unkown.");
  }

  std::array<InputType, NumOfInputs> _input_ports;
  std::array<OutputType, NumOfOutputs> _output_ports;
};


template <std::size_t NumOfInputs, std::size_t NumOfOutputs>
class ProcessingStageParent : public ProcessingStage,
                              public DataInputOutput<data::InputPort, NumOfInputs, data::OutputPort, NumOfOutputs>
{
public:
  ProcessingStageParent(const std::string& name)
    : ProcessingStage(name),
      DataInputOutput<data::InputPort, NumOfInputs, data::OutputPort, NumOfOutputs>() { }

  bool initialize()
  {
    bool ret = true;

    ret &= DataInputOutput<data::InputPort, NumOfInputs, data::OutputPort, NumOfOutputs>::initialize();
    ret &= ProcessingStage::initialize();

    return ret;
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Processing Stage Detect Lines
using francor::base::VectorVector2d;
using francor::base::LineVector;

/**
 * \brief This class searches lines in a 2d point set using a line ransac.
 */
class DetectLines : public ProcessingStageParent<2, 1>
{
public:
  DetectLines(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : ProcessingStageParent<2, 1>("detect lines")
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  ~DetectLines(void) = default;

  bool doProcess(void) final
  {
    using francor::base::LogDebug;

    LogDebug() << this->name() << ": start data procssing.";
    _lines.clear();

    if (this->getInputs()[0].numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with a single set of 2d points.";
      _lines = _detector(this->getInputs()[0].data<VectorVector2d>());
    }
    if (this->getInputs()[1].numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with " << this->getInputs()[1].data<std::vector<VectorVector2d>>().size()
                 << " sets of 2d points.";

      for (const auto& cluster : this->getInputs()[1].data<std::vector<VectorVector2d>>())
      {
        base::LineVector lines(_detector(cluster));
        _lines.insert(_lines.end(), lines.begin(), lines.end());
      }

      LogDebug() << this->name() << ": found " << _lines.size() << " lines.";
    }

    LogDebug() << this->name() << ": finished data processing.";
    return true;
  }
  bool doInitialization() final
  {
    return true;
  }

private:
  void initializePorts() final
  {
    this->initializeInputPort<VectorVector2d>(0, "2d points");
    this->initializeInputPort<std::vector<VectorVector2d>>(1, "clustered 2d points");

    this->initializeOutputPort(0, "2d lines", &_lines);
  }

  LineVector _lines;
  algorithm::LineRansac _detector;
};

/**
 * \brief This class searches lines in a 2d point set using a line ransac.
 */
class DetectLineSegments : public ProcessingStageParent<2, 1>
{
public:
  DetectLineSegments(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : ProcessingStageParent<2, 1>("detect line segments")
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  ~DetectLineSegments(void) = default;

  bool doProcess(void) final
  {
    using francor::base::LogDebug;

    LogDebug() << this->name() << ": start data procssing.";
    _lines.clear();

    if (this->getInputs()[0].numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with a single set of 2d points.";
      _lines = _detector(this->getInputs()[0].data<VectorVector2d>());
    }
    if (this->getInputs()[1].numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with " << this->getInputs()[1].data<std::vector<VectorVector2d>>().size()
                 << " sets of 2d points.";

      for (const auto& cluster : this->getInputs()[1].data<std::vector<VectorVector2d>>())
      {
        base::LineSegmentVector lines(_detector(cluster));
        _lines.insert(_lines.end(), lines.begin(), lines.end());
      }

      LogDebug() << this->name() << ": found " << _lines.size() << " line segments.";      
    }

    LogDebug() << this->name() << ": finished data processing.";
    return true;
  }
  bool doInitialization() final
  {
    return true;
  }

private:
  void initializePorts() final
  {
    this->initializeInputPort<VectorVector2d>(0, "2d points");
    this->initializeInputPort<std::vector<VectorVector2d>>(1, "clustered 2d points");

    this->initializeOutputPort(0, "2d line segments", &_lines);
  }

  base::LineSegmentVector _lines;
  algorithm::LineSegmentRansac _detector;
};

using francor::vision::Image;
  
class ExportClusteredPointsFromBitMask : public ProcessingStageParent<1, 1>
{
public:
  ExportClusteredPointsFromBitMask(void)
    : ProcessingStageParent<1, 1>("export clustered points from bit mask") {  }
  ~ExportClusteredPointsFromBitMask(void) = default;

  bool doProcess(void) final
  {
    // input data type safety check
    if (this->getInputs()[0].data<Image>().colourSpace() != vision::ColourSpace::BIT_MASK)
    {
      //TODO: print error
      return false;
    }

    // find contours using opencv function
    std::vector<std::vector<cv::Point2i>> foundClusters;

    cv::findContours(this->getInputs()[0].data<Image>().cvMat(), foundClusters, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

    // copy data to francor data types
    _clustered_points.resize(foundClusters.size());
    
    for (std::size_t cluster = 0; cluster < foundClusters.size(); ++cluster)
    {
      _clustered_points[cluster].resize(foundClusters[cluster].size());

      for (std::size_t point = 0; point < _clustered_points[cluster].size(); ++point)
      {
        _clustered_points[cluster][point].x() = static_cast<double>(foundClusters[cluster][point].x);
        _clustered_points[cluster][point].y() = static_cast<double>(foundClusters[cluster][point].y);
      }
    }

    return true;
  }
  bool doInitialization() final
  {
    return true;
  }

private:
  void initializePorts() final
  {
    this->initializeInputPort<Image>(0, "bit mask");
    this->initializeOutputPort<std::vector<VectorVector2d>>(0, "clustered 2d points", &_clustered_points);
  }

  std::vector<VectorVector2d> _clustered_points;
};                                        



using francor::vision::ColourSpace;
using francor::vision::ImageMaskFilterPipeline;
using francor::vision::ImageMaskFilterColourRange;

class ColouredImageToBitMask : public ProcessingStageParent<1, 1>
{
public:
  ColouredImageToBitMask(void)
    : ProcessingStageParent<1, 1>("coloured image to bit mask") {  }
  ~ColouredImageToBitMask(void) = default;

  bool doProcess(void) final
  {
    // check colour space of input image
    if (this->getInputs()[0].data<Image>().colourSpace() != ColourSpace::BGR
        &&
        this->getInputs()[0].data<Image>().colourSpace() != ColourSpace::RGB
        &&
        this->getInputs()[0].data<Image>().colourSpace() != ColourSpace::HSV)
    {
      //TODO: print error
      return false;
    }

    return _image_pipeline(this->getInputs()[0].data<Image>(), _bit_mask);
  }

  bool doInitialization() final
  {
    auto rangeFilter = std::make_unique<ImageMaskFilterColourRange>(100, 120, 70, 255, 30, 255);
    
    return _image_pipeline.addFilter("colour range", std::move(rangeFilter));
  }
  
private:
  void initializePorts() final
  {
    this->initializeInputPort<Image>(0, "coloured image");
    this->initializeOutputPort<Image>(0, "bit mask", &_bit_mask);
  }

  Image _bit_mask;
  ImageMaskFilterPipeline _image_pipeline;
};

} // end namespace processing

} // end namespace francor
