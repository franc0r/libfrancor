/**
 * Base class and math models for the RANSAC class. These classes define are used by the RANSAC to find
 * geometry shapes.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 20. April 2019
 */
#pragma once

#include "francor_base/line.h"
#include "francor_base/line_segment.h"
#include "francor_algorithm/geometry_fitting.h"

namespace francor {

namespace algorithm {

/**
 * Abstract class for the underlaying model of the ransac. This class provides the interface that class ransac can use it.
 */
template <typename InputDataType, typename ModelDataType, const std::size_t NumberOfRequiredData>
class RansacTargetModel
{
public:
  // defines input and output type
  struct Input
  {
    using type = InputDataType;
    static constexpr std::size_t count = NumberOfRequiredData;
  };
  struct Output
  {
    using type = ModelDataType;
  };
  
  // defaulted constructor and destructor
  RansacTargetModel(void) = default;
  virtual ~RansacTargetModel(void) = default;

  virtual double error(const typename Input::type& data) const = 0;
  virtual bool estimate(const std::array<typename Input::type, Input::count>& modelData) = 0;
  virtual typename Output::type fitData(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData,
                                        const std::vector<std::size_t>& indices) const = 0;
  inline constexpr const typename Output::type& model(void) const noexcept { return _model; }

protected:
  typename Output::type _model;
};

/**
 * Ransac model for lines.
 */
class RansacLineModel : public RansacTargetModel<base::Vector2d, base::Line, 2>
{
public:
  RansacLineModel(void) = default;
  ~RansacLineModel(void) = default;

  double error(const Input::type& data) const final
  {
    return _model.distanceTo(data);
  }

  bool estimate(const std::array<Input::type, Input::count>& modelData) final
  {
    static_assert(Input::count == 2);
    _model = base::Line((modelData[1] - modelData[0]).normalized(), modelData[0]);
    return true;
  }

  typename Output::type fitData(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData,
                                        const std::vector<std::size_t>& indices) const final
  {
    return { fittingLineFromPoints(inputData, indices) };
  }                             
};

/**
 * Ransac model for line segments.
 */
class RansacLineSegmentModel : public RansacTargetModel<base::Vector2d, base::LineSegment, 2>
{
public:
  RansacLineSegmentModel(void) = default;
  ~RansacLineSegmentModel(void) = default;

  double error(const Input::type& data) const final
  {
    return _model.line().distanceTo(data);
  }

  bool estimate(const std::array<Input::type, Input::count>& modelData) final
  {
    static_assert(Input::count == 2);
    _model = base::LineSegment(modelData[1], modelData[0]);
    return true;
  }

  typename Output::type fitData(const std::vector<typename Input::type, Eigen::aligned_allocator<typename Input::type>>& inputData,
                                const std::vector<std::size_t>& indices) const final
  {
    return { fittingLineSegmentFromPoints(inputData, indices) };
  }                                          
};

} // end namespace algorithm

} // end namespace francor