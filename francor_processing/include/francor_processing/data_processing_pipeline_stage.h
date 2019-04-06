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
  DataProcessingStage(const std::string& name) : _name(name) { }
  DataProcessingStage(const DataProcessingStage&) = delete;
  DataProcessingStage(DataProcessingStage&&) = delete;
  virtual ~DataProcessingStage(void) = default;

  DataProcessingStage& operator=(const DataProcessingStage&) = delete;
  DataProcessingStage& operator=(DataProcessingStage&&) = delete;

public:
  virtual bool process(void) = 0;
  virtual bool initialize(void) = 0;

  inline const std::string& name(void) const noexcept { return _name; }

private:
  const std::string _name;
};


template <std::size_t NumInputs, std::size_t NumOutputs>
class DataProcessingStageIO : public DataProcessingStage
{
public:
  DataProcessingStageIO(const std::string& name) : DataProcessingStage(name) { }

  virtual bool initialize(void) override final
  {
    if (!this->configurePorts(_inputs, _outputs))
    {
      //TODO: print error
      return false;
    }

    return true;
  }
  virtual ~DataProcessingStageIO(void) = default;

  InputPortBlock<NumInputs>& inputs(void) { return _inputs; }
  OutputPortBlock<NumOutputs>& outputs(void) { return _outputs; }

protected:
  virtual bool configurePorts(InputPortBlock<NumInputs>& inputs, OutputPortBlock<NumOutputs>& outputs) = 0;

private:
  InputPortBlock<NumInputs> _inputs;
  OutputPortBlock<NumOutputs> _outputs;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Processing Stage Detect Lines

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
      _lines = _detector(this->inputs().port(0).data<base::VectorVector2d>());
    }
    if (this->inputs().port(1).numOfConnections() > 0)
    {
      for (const auto& cluster : this->inputs().port(1).data<std::vector<base::VectorVector2d>>())
      {
        base::LineVector lines(_detector(cluster));
        _lines.insert(_lines.end(), lines.begin(), lines.end());
      }
    }

    return true;
  }

private:
  virtual bool configurePorts(InputPortBlock<2>& inputs, OutputPortBlock<1>& outputs) override final
  {
    return true;
  }

  base::LineVector _lines;
  algorithm::LineRansac _detector;
};

class ExportClusteredPointsFromBitMask : public DataProcessingStageIO<1, 1>
{
public:
  ExportClusteredPointsFromBitMask(void)
    : DataProcessingStageIO<1, 1>("export clustered points from bit mask") {  }
  virtual ~ExportClusteredPointsFromBitMask(void) = default;

  virtual bool process(void) override final
  {
    // input data type safety check
    if (this->inputs().port(0).data<vision::Image>().colourSpace() != vision::ColourSpace::BIT_MASK)
    {
      //TODO: print error
      return false;
    }

    // find contours using opencv function
    std::vector<std::vector<cv::Point2i>> foundClusters;

    cv::findContours(this->inputs().port(0).data<vision::Image>().cvMat(), foundClusters, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

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
  virtual bool configurePorts(InputPortBlock<1>& inputs, OutputPortBlock<1>& outputs) override final
  {
    return true;
  }

  std::vector<base::VectorVector2i> _clustered_points;
};                                        

} // end namespace processing

} // end namespace francor
