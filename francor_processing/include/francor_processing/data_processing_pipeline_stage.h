/**
 * Data processing pipeline stages.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 31. February 2019
 */
#pragma once

#include "francor_base/vector.h"
#include "francor_base/line.h"

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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Processing Stage
class DataProcessingStage
{
protected:
  DataProcessingStage(void) = delete;
  DataProcessingStage(const std::string& stageName) : _name(stageName) { }
  DataProcessingStage(const DataProcessingStage&) = delete;
  DataProcessingStage(DataProcessingStage&&) = delete;

  DataProcessingStage& operator=(const DataProcessingStage&) = delete;
  DataProcessingStage& operator=(DataProcessingStage&&) = delete;

public:
  virtual ~DataProcessingStage(void) = default;

  virtual bool process(void) = 0;
  virtual bool initialize(void) = 0;
  virtual data::InputPort& input(const std::size_t index) = 0;
  virtual data::OutputPort& output(const std::size_t index) = 0;
  virtual std::size_t numInputs(void) const = 0;
  virtual std::size_t numOutputs(void) const = 0;

  inline const std::string& name(void) const noexcept { return _name; }

private:
  const std::string _name;
};


template <std::size_t NumInputs, std::size_t NumOutputs>
class DataProcessingStageIO : public DataProcessingStage
{
public:
  DataProcessingStageIO(const std::string& stageName) : DataProcessingStage(stageName) { }

  virtual bool initialize(void) override final
  {
    if (!this->configurePorts(_inputs, _outputs))
    {
      //TODO: print error
      return false;
    }
    if (!this->configureProcessing())
    {
      //TODO: print error
      return false;
    }

    return true;
  }
  virtual ~DataProcessingStageIO(void) = default;

  virtual data::InputPort& input(const std::size_t index) override final
  {
    return _inputs.port(index);
  }
  virtual data::OutputPort& output(const std::size_t index) override final
  {
    return _outputs.port(index);
  }
  virtual std::size_t numInputs(void) const override final { return num_inputs; }
  virtual std::size_t numOutputs(void) const override final { return num_outputs; }

protected:
  InputPortBlock<NumInputs>& inputs(void) { return _inputs; }
  OutputPortBlock<NumOutputs>& outputs(void) { return _outputs; }
  
  virtual bool configurePorts(InputPortBlock<NumInputs>& inputs, OutputPortBlock<NumOutputs>& outputs) = 0;
  virtual bool configureProcessing(void) = 0;

  static constexpr std::size_t num_inputs = NumInputs;
  static constexpr std::size_t num_outputs = NumOutputs;

private:
  InputPortBlock<NumInputs> _inputs;
  OutputPortBlock<NumOutputs> _outputs;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Processing Stage Detect Lines
using francor::base::VectorVector2d;
using francor::base::LineVector;

/**
 * \brief This class searches lines in a 2d point set using a line ransac.
 */
class DetectLines : public DataProcessingStageIO<2, 1>
{
public:
  DetectLines(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : DataProcessingStageIO<2, 1>("detect lines")
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  virtual ~DetectLines(void) = default;

  virtual bool process(void) override final
  {
    _lines.clear();

    if (this->inputs().port(0).numOfConnections() > 0)
    {
      _lines = _detector(this->inputs().port(0).data<VectorVector2d>());
    }
    if (this->inputs().port(1).numOfConnections() > 0)
    {
      for (const auto& cluster : this->inputs().port(1).data<std::vector<VectorVector2d>>())
      {
        base::LineVector lines(_detector(cluster));
        _lines.insert(_lines.end(), lines.begin(), lines.end());
      }
    }

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
class DetectLineSegments : public DataProcessingStageIO<2, 1>
{
public:
  DetectLineSegments(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : DataProcessingStageIO<2, 1>("detect lines")
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  virtual ~DetectLineSegments(void) = default;

  virtual bool process(void) override final
  {
    _lines.clear();

    if (this->inputs().port(0).numOfConnections() > 0)
    {
      _lines = _detector(this->inputs().port(0).data<VectorVector2d>());
    }
    if (this->inputs().port(1).numOfConnections() > 0)
    {
      for (const auto& cluster : this->inputs().port(1).data<std::vector<VectorVector2d>>())
      {
        base::LineVector lines(_detector(cluster));
        _lines.insert(_lines.end(), lines.begin(), lines.end());
      }
    }

    return true;
  }

private:
  virtual bool configurePorts(InputPortBlock<num_inputs>& inputs, OutputPortBlock<num_outputs>& outputs) override final
  {
    bool ret = true;

    ret &= inputs.configurePort<VectorVector2d>(0, "2d points");
    ret &= inputs.configurePort<std::vector<VectorVector2d>>(1, "clustered 2d points");

    ret &= outputs.configurePort<LineVector>(0, "2d line segments", &_lines);

    return ret;
  }
  virtual bool configureProcessing(void) override final
  {
    return true;
  }

  LineVector _lines;
  algorithm::LineRansac _detector;
};

using francor::vision::Image;
  
class ExportClusteredPointsFromBitMask : public DataProcessingStageIO<1, 1>
{
public:
  ExportClusteredPointsFromBitMask(void)
    : DataProcessingStageIO<1, 1>("export clustered points from bit mask") {  }
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

class ColouredImageToBitMask : public DataProcessingStageIO<1, 1>
{
public:
  ColouredImageToBitMask(void)
    : DataProcessingStageIO<1, 1>("coloured image to bit mask") {  }
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
