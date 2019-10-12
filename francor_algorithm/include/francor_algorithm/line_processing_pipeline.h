/**

 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. October 2019
 */
#pragma once

#include <francor_base/vector.h>
#include <francor_base/line.h>
#include <francor_base/log.h>

#include <francor_processing/data_processing_pipeline_stage.h>

#include "francor_algorithm/ransac.h"

namespace francor {

namespace algorithm {

/**
 * \brief This class searches lines in a 2d point set using a line ransac.
 */
class DetectLineSegments : public processing::ProcessingStage<>
{
  using Point2dVector = francor::base::Point2dVector;

public:
  DetectLineSegments(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : processing::ProcessingStage<>("detect line segments", 2, 1)
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  ~DetectLineSegments(void) = default;

  bool doProcess(const std::shared_ptr<void>&) final
  {
    using francor::base::LogDebug;

    LogDebug() << this->name() << ": start data procssing.";
    _lines.clear();

    if (this->getInputs()[0].numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with a single set of 2d points.";
      _lines = _detector(this->getInputs()[0].data<Point2dVector>());
    }
    if (this->getInputs()[1].numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with " << this->getInputs()[1].data<std::vector<Point2dVector>>().size()
                 << " sets of 2d points.";

      for (const auto& cluster : this->getInputs()[1].data<std::vector<Point2dVector>>())
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
  bool doInitialization() final
  {
    return true;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<Point2dVector>(0, "2d points");
    this->initializeInputPort<std::vector<Point2dVector>>(1, "clustered 2d points");

    this->initializeOutputPort(0, "2d line segments", &_lines);

    return true;
  }
  bool isReady() const final
  {
    return this->input(0).numOfConnections() > 0 || this->input(1).numOfConnections() > 0;
  }

  base::LineSegmentVector _lines;
  algorithm::LineSegmentRansac _detector;
};

/**
 * \brief This class searches lines in a 2d point set using a line ransac.
 */
class DetectLines : public processing::ProcessingStage<>
{
  using Point2dVector = francor::base::Point2dVector;
  using LineVector = francor::base::LineVector;

public:
  enum Inputs {
    IN_POINT = 0,
    IN_CLUSTERED_POINT,
    COUNT_INPUTS
  };
  enum Outputs {
    OUT_LINES = 0,
    COUNT_OUTPUTS
  };

  DetectLines(const unsigned int maxIterations = 100, const std::size_t minNumPoints = 2, const double epsilon = 0.3)
    : processing::ProcessingStage<>("detect lines", COUNT_INPUTS, COUNT_OUTPUTS)
  {
    _detector.setMaxIterations(maxIterations);
    _detector.setMinNumPoints(minNumPoints);
    _detector.setEpsilon(epsilon);
  }
  ~DetectLines(void) = default;

  bool doProcess(const std::shared_ptr<void>&) final
  {
    using francor::base::LogDebug;

    LogDebug() << this->name() << ": start data procssing.";
    _lines.clear();

    if (this->input(IN_POINT).numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with a single set of 2d points.";
      _lines = _detector(this->input(IN_POINT).data<Point2dVector>());
    }
    if (this->input(IN_CLUSTERED_POINT).numOfConnections() > 0)
    {
      LogDebug() << this->name() << ": process with " << this->input(IN_CLUSTERED_POINT).data<std::vector<Point2dVector>>().size()
                 << " sets of 2d points.";

      for (const auto& cluster : this->input(IN_CLUSTERED_POINT).data<std::vector<Point2dVector>>())
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
  bool doInitialization() final
  {
    return true;
  }
  bool initializePorts() final
  {
    this->initializeInputPort<Point2dVector>(IN_POINT, "2d points");
    this->initializeInputPort<std::vector<Point2dVector>>(IN_CLUSTERED_POINT, "clustered 2d points");

    this->initializeOutputPort(OUT_LINES, "2d lines", &_lines);

    return true;
  }
  bool isReady() const final
  {
    return this->input(0).numOfConnections() > 0 || this->input(1).numOfConnections() > 0;
  }

  LineVector _lines;
  algorithm::LineRansac _detector;
};


} // end namespace algorithm

} // end namespace francor