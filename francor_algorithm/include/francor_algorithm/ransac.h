/**
 * Ransac classes. Just started with lines to get experience. So please respect that.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include "francor_base/line.h"
#include "francor_algorithm/geometry_fitting.h"

#include <random>
#include <vector>

namespace francor {

namespace algorithm {

/**
 * Interface class for the underlaying model of the ransac.
 */
template <typename InputData>
class RansacTargetModel
{
public:
  RansacTargetModel(void) = default;
  virtual ~RansacTargetModel(void) = default;

  virtual double error(const InputData& data) const = 0;
  virtual bool estimate(const std::vector<InputData, Eigen::aligned_allocator<InputData>>& modelData) = 0;
};

/**
 * Ransac model for lines.
 */
class RansacLineModel : public RansacTargetModel<base::Vector2d>
{
public:
  RansacLineModel(void) = default;
  virtual ~RansacLineModel(void) = default;

  virtual double error(const base::Vector2d& data) const override final
  {
    return _model.distanceTo(data);
  }

  virtual bool estimate(const std::vector<base::Vector2d, Eigen::aligned_allocator<base::Vector2d>>& modelData) override final
  {
    if (modelData.size() < 2)
    {
      // TODO: print error
      return false;
    }

    _model = fittingLineFromPoints(modelData);

    return true;
  }

private:
  base::Line _model;
};

/**
 * Base ransac class.
 */
template <class ModelType, typename InputType, typename ReturnType>
class Ransac
{
protected:
  Ransac(void) = default;

public:
  // prevent copying and moving of the base class.
  Ransac(const Ransac&) = delete;
  Ransac(Ransac&&) = delete;
  virtual ~Ransac(void) = default;

  Ransac& operator=(const Ransac&) = delete;
  Ransac& operator=(Ransac&&) = delete;

  ReturnType operator()(const std::vector<InputType, Eigen::aligned_allocator<InputType>>& inputData)
  {
    return { this->process(inputData) };
  }

  virtual ReturnType process(const std::vector<InputType, Eigen::aligned_allocator<InputType>>& inputData) = 0;

private:
  std::random_device _rd;
  std::mt19937 _gen;
};

class LineRansac : public Ransac<RansacLineModel, base::Vector2d, base::Line>
{
public:
  LineRansac(void) = default;
  LineRansac(const LineRansac&) = default;
  LineRansac(LineRansac&&) = default;
  virtual ~LineRansac(void) = default;

  LineRansac& operator=(const LineRansac&) = default;
  LineRansac& operator=(LineRansac&&) = default;

  virtual base::Line process(const std::vector<base::Vector2d, Eigen::aligned_allocator<base::Vector2d>>& inputData) override final
  {

  }
};

} // end namespace algorithm

} // end namespace francor