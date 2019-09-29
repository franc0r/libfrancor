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

namespace francor
{

namespace processing
{

class DiagnosticData { };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Processing Stage
template <std::size_t NumOfInputs, std::size_t NumOfOutputs>
class ProcessingStage
{
protected:
  ProcessingStage() = delete;
  ProcessingStage(const std::string& stageName) : _name(stageName) { }
  ProcessingStage(const ProcessingStage&) = delete;
  ProcessingStage(ProcessingStage&&) = delete;

  ProcessingStage& operator=(const ProcessingStage&) = delete;
  ProcessingStage& operator=(ProcessingStage&&) = delete;

public:
  virtual ~ProcessingStage(void) = default;

  bool process()
  {
    this->doProcess();
  }
  virtual bool initialize() = 0;
  const data::InputPort& input(const std::string& portName) const
  {
    for (auto& port : _input_ports)
      if (port.name() == portName)
        return port;

    throw "Processing Stage: input port name is unkown.";
  }
  const data::OutputPort& output(const std::string& portName) const
  {
    for (auto& port : _output_ports)
      if (port.name() == portName)
        return port;

    throw "Processing Stage: output port name is unkown.";
  }

  inline const std::string& name() const noexcept { return _name; }

protected:
  inline std::array<data::InputPort, NumOfInputs>& getInputs() { return _input_ports; }
  inline std::array<data::OutputPort, NumOfOutputs>& getOutputs() { return _output_ports; }

  virtual bool doProcess() = 0;
  virtual DiagnosticData getDiagnostic() const = 0;
  
private:
  const std::string _name;
  std::array<data::InputPort, NumOfInputs> _input_ports;
  std::array<data::OutputPort, NumOfOutputs> _output_ports;
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Processing Stage Detect Lines
using francor::base::VectorVector2d;
using francor::base::LineVector;

/**
 * \brief This class searches lines in a 2d point set using a line ransac.
 */
class DetectLines : public ProcessingStageIO<2, 1>
{
public:
  DetectLines(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : ProcessingStageIO<2, 1>("detect lines")
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  virtual ~DetectLines(void) = default;

  virtual bool process(void) override final
  {
    using francor::base::LogDebug;

    LogDebug() << this->name() << ": start data procssing.";
    _lines.clear();

    if (this->inputs().port(0).numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with a single set of 2d points.";
      _lines = _detector(this->inputs().port(0).data<VectorVector2d>());
    }
    if (this->inputs().port(1).numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with " << this->inputs().port(1).data<std::vector<VectorVector2d>>().size()
                 << " sets of 2d points.";

      for (const auto& cluster : this->inputs().port(1).data<std::vector<VectorVector2d>>())
      {
        base::LineVector lines(_detector(cluster));
        _lines.insert(_lines.end(), lines.begin(), lines.end());
      }

      LogDebug() << this->name() << ": found " << _lines.size() << " lines.";
    }

    LogDebug() << this->name() << ": finished data processing.";
    return true;
  }

private:
  virtual bool configurePorts(InputPortBlock<num_inputs>& inputs, OutputPortBlock<num_outputs>& outputs) override final
  {
    bool ret = true;

    ret &= inputs.configurePort<VectorVector2d>(0, "2d points");
    ret &= inputs.configurePort<std::vector<VectorVector2d>>(1, "clustered 2d points");

    ret &= outputs.configurePort<LineVector>(0, "2d lines", &_lines);

    return ret;
  }
  virtual bool configureProcessing(void) override final
  {
    return true;
  }

  LineVector _lines;
  algorithm::LineRansac _detector;
};

/**
 * \brief This class searches lines in a 2d point set using a line ransac.
 */
class DetectLineSegments : public ProcessingStageIO<2, 1>
{
public:
  DetectLineSegments(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : ProcessingStageIO<2, 1>("detect line segments")
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  virtual ~DetectLineSegments(void) = default;

  virtual bool process(void) override final
  {
    using francor::base::LogDebug;

    LogDebug() << this->name() << ": start data procssing.";
    _lines.clear();

    if (this->inputs().port(0).numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with a single set of 2d points.";
      _lines = _detector(this->inputs().port(0).data<VectorVector2d>());
    }
    if (this->inputs().port(1).numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with " << this->inputs().port(1).data<std::vector<VectorVector2d>>().size()
                 << " sets of 2d points.";

      for (const auto& cluster : this->inputs().port(1).data<std::vector<VectorVector2d>>())
      {
        base::LineSegmentVector lines(_detector(cluster));
        _lines.insert(_lines.end(), lines.begin(), lines.end());
      }

      LogDebug() << this->name() << ": found " << _lines.size() << " line segments.";      
    }

    LogDebug() << this->name() << ": finished data processing.";
    return true;
  }

private:
  virtual bool configurePorts(InputPortBlock<num_inputs>& inputs, OutputPortBlock<num_outputs>& outputs) override final
  {
    bool ret = true;

    ret &= inputs.configurePort<VectorVector2d>(0, "2d points");
    ret &= inputs.configurePort<std::vector<VectorVector2d>>(1, "clustered 2d points");

    ret &= outputs.configurePort<base::LineSegmentVector>(0, "2d line segments", &_lines);

    return ret;
  }
  virtual bool configureProcessing(void) override final
  {
    return true;
  }

  base::LineSegmentVector _lines;
  algorithm::LineSegmentRansac _detector;
};

using francor::vision::Image;
  
class ExportClusteredPointsFromBitMask : public ProcessingStageIO<1, 1>
{
public:
  ExportClusteredPointsFromBitMask(void)
    : ProcessingStageIO<1, 1>("export clustered points from bit mask") {  }
  virtual ~ExportClusteredPointsFromBitMask(void) = default;

  virtual bool process(void) override final
  {
    // input data type safety check
    if (this->inputs().port(0).data<Image>().colourSpace() != vision::ColourSpace::BIT_MASK)
    {
      //TODO: print error
      return false;
    }

    // find contours using opencv function
    std::vector<std::vector<cv::Point2i>> foundClusters;

    cv::findContours(this->inputs().port(0).data<Image>().cvMat(), foundClusters, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

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

private:
  virtual bool configurePorts(InputPortBlock<num_inputs>& inputs, OutputPortBlock<num_outputs>& outputs) override final
  {
    bool ret = true;

    ret &= inputs.configurePort<Image>(0, "bit mask");
    ret &= outputs.configurePort<std::vector<VectorVector2d>>(0, "clustered 2d points", &_clustered_points);

    return ret;
  }

  virtual bool configureProcessing(void) override final
  {
    return true;
  }


  std::vector<VectorVector2d> _clustered_points;
};                                        



using francor::vision::ColourSpace;
using francor::vision::ImageMaskFilterPipeline;
using francor::vision::ImageMaskFilterColourRange;

class ColouredImageToBitMask : public ProcessingStageIO<1, 1>
{
public:
  ColouredImageToBitMask(void)
    : ProcessingStageIO<1, 1>("coloured image to bit mask") {  }
  virtual ~ColouredImageToBitMask(void) = default;

  virtual bool process(void) override final
  {
    // check colour space of input image
    if (this->inputs().port(0).data<Image>().colourSpace() != ColourSpace::BGR
        &&
        this->inputs().port(0).data<Image>().colourSpace() != ColourSpace::RGB
        &&
        this->inputs().port(0).data<Image>().colourSpace() != ColourSpace::HSV)
    {
      //TODO: print error
      return false;
    }

    return _image_pipeline(this->inputs().port(0).data<Image>(), _bit_mask);
  }

private:
  virtual bool configurePorts(InputPortBlock<num_inputs>& inputs, OutputPortBlock<num_outputs>& outputs) override final
  {
    bool ret = true;

    ret &= inputs.configurePort<Image>(0, "coloured image");
    ret &= outputs.configurePort<Image>(0, "bit mask", &_bit_mask);

    return ret;
  }

  virtual bool configureProcessing(void) override final
  {
    auto rangeFilter = std::make_unique<ImageMaskFilterColourRange>(100, 120, 70, 255, 30, 255);
    
    return _image_pipeline.addFilter("colour range", std::move(rangeFilter));
  }

  Image _bit_mask;
  ImageMaskFilterPipeline _image_pipeline;
};

} // end namespace processing

} // end namespace francor
