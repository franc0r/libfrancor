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

#include "francor_processing/data_processing_pipeline.h"

namespace francor
{

namespace processing
{

/**
 * \brief This class searches lines in a 2d point set using a line ransac.
 */
class DetectLines : public DataProcessingStage,
                    public DataProcessingStageInput<base::VectorVector2d, std::vector<base::VectorVector2d>>,
                    public DataProcessingStageOutput<base::LineVector>
{
public:
  DetectLines(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : DataProcessingStage("detect lines"),
      DataProcessingStageInput<base::VectorVector2d, std::vector<base::VectorVector2d>>({ PortConfig("input points"),
                                                                                          PortConfig("input clustered points") }),
      DataProcessingStageOutput<base::LineVector>({ PortConfig("output lines", &_lines) })
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  virtual ~DetectLines(void) = default;

  virtual bool process(void) override final
  {
    _lines.clear();

    if (this->getInput<0>().isConnected())
    {
      _lines = _detector(this->getInput<0>().data());
    }
    if (this->getInput<1>().isConnected())
    {
      for (const auto& cluster : this->getInput<1>().data())
      {
        base::LineVector lines(_detector(cluster));
        _lines.insert(_lines.end(), lines.begin(), lines.end());
      }
    }

    return true;
  }

private:
  base::LineVector _lines;
  algorithm::LineRansac _detector;
};

class ExportClusteredPointsFromBitMask : public DataProcessingStage,
                                         public DataProcessingStageInput<vision::Image>,
                                         public DataProcessingStageOutput<std::vector<base::VectorVector2d>>
{
public:
  ExportClusteredPointsFromBitMask(void)
    : DataProcessingStage("export clustered points from bit mask"),
      DataProcessingStageInput<vision::Image>({ PortConfig("input bit mask") }),
      DataProcessingStageOutput<std::vector<base::VectorVector2d>>({ PortConfig("output clustered points", &_clustered_points) })
  {

  }
  virtual ~ExportClusteredPointsFromBitMask(void) = default;

  virtual bool process(void) override final
  {
    // input data type safety check
    if (this->getInput<0>().data().colourSpace() != vision::ColourSpace::BIT_MASK)
    {
      //TODO: print error
      return false;
    }

    // find contours using opencv function
    std::vector<std::vector<cv::Point2i>> foundClusters;

    cv::findContours(this->getInput<0>().data().cvMat(), foundClusters, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

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
  std::vector<base::VectorVector2i> _clustered_points;
};                                        

} // end namespace processing

} // end namespace francor
