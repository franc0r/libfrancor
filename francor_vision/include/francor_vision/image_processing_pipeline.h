/**
 * Predefined processing pipeline for images.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include <francor_base/vector.h>
#include <francor_base/line.h>
#include <francor_base/log.h>

#include <francor_processing/data_processing_pipeline_stage.h>

#include "francor_vision/image.h"
#include "francor_vision/image_filter_pipeline.h"
#include "francor_vision/image_filter_criteria.h"

namespace francor
{

namespace vision
{
  using francor::processing::NoDataType;

class ExportClusteredPointsFromBitMask : public processing::ProcessingStage<NoDataType>
{
public:
  ExportClusteredPointsFromBitMask(void)
    : processing::ProcessingStage<NoDataType>("export clustered points from bit mask", 1, 1) {  }
  ~ExportClusteredPointsFromBitMask(void) = default;

  bool doProcess(NoDataType&) final
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

private:
  using VectorVector2d = francor::base::VectorVector2d;
  using LineVector = francor::base::LineVector;
  
  bool doInitialization() final
  {
    return true;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<Image>(0, "bit mask");
    this->initializeOutputPort<std::vector<VectorVector2d>>(0, "clustered 2d points", &_clustered_points);

    return true;
  }
  bool isReady() const final
  {
    return this->input(0).numOfConnections() > 0 || this->input(1).numOfConnections() > 0;
  }

  std::vector<VectorVector2d> _clustered_points;
};

class ColouredImageToBitMask : public processing::ProcessingStage<NoDataType>
{
public:
  ColouredImageToBitMask()
    : processing::ProcessingStage<NoDataType>("coloured image to bit mask", 1, 1) {  }
  ~ColouredImageToBitMask() = default;

  bool doProcess(NoDataType&) final
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

private:
  bool doInitialization() final
  {
    auto rangeFilter = std::make_unique<ImageMaskFilterColourRange>(100, 120, 70, 255, 30, 255);
    
    return _image_pipeline.addFilter("colour range", std::move(rangeFilter));
  }
  bool initializePorts() final
  {
    this->initializeInputPort<Image>(0, "coloured image");
    this->initializeOutputPort<Image>(0, "bit mask", &_bit_mask);

    return true;
  }
  bool isReady() const final
  {
    return this->input(0).numOfConnections() > 0;
  }

  Image _bit_mask;
  ImageMaskFilterPipeline _image_pipeline;
};

} // end namespace vision

} // end namespace francor