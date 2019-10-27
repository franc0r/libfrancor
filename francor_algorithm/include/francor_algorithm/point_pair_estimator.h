/**
 * Point pair estimator interface definition.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#pragma once

#include <francor_base/point.h>

namespace francor {

namespace algorithm {

struct PointPairIndex
{
  std::size_t first;
  std::size_t second;
  float distance;
};

class PointPairIndexVector : public std::vector<PointPairIndex>
{
public:
  PointPairIndexVector() = default;

  inline void setFristPointVector(const base::Point2dVector& vector) { _first_point_vector = &vector; }
  inline void setSecondPointVector(const base::Point2dVector& vector) { _second_point_vector = &vector; }
  void clear()
  {
    std::vector<PointPairIndex>::clear();
    _first_point_vector = nullptr;
    _second_point_vector = nullptr;
  }
  inline bool isValid() const noexcept
  {
    return _first_point_vector != nullptr && _second_point_vector != nullptr;
  }

private:
  const base::Point2dVector* _first_point_vector = nullptr;
  const base::Point2dVector* _second_point_vector = nullptr;
};
class PointPairEstimator
{
protected:
  PointPairEstimator() = default;

public:
  virtual ~PointPairEstimator() = default;

  virtual bool setPointDataset(const base::Point2dVector& points) = 0;
  virtual bool findPairs(const base::Point2dVector& points, PointPairIndexVector& pairs) = 0;
};

} // end namespace algorithm

} // end namespace francor