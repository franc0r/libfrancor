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
  inline void update()
  {
    this->calculateAvgDistance();
    this->calculateMedianDistance();
  }
  inline double avgDistance() const noexcept { return _avg_distance; }
  inline double medianDistance() const noexcept { return _median_distance; }

private:
  void calculateAvgDistance()
  {
    if (this->empty()) {
      _avg_distance = 0.0;
      return;
    }

    double sum = 0.0;

    for (const auto pair : *this) {
      sum += pair.distance;
    }

    _avg_distance = sum / static_cast<double>(this->size());
  }
  void calculateMedianDistance()
  {
    if (this->empty()) {
      _median_distance = 0.0;
      return;
    }

    std::vector<float> distances;
    distances.reserve(this->size());

    for (const auto& pair : *this) {
      distances.push_back(pair.distance);
    }

    std::sort(distances.begin(), distances.end());
    _median_distance = distances[distances.size() / 2];
  }

  const base::Point2dVector* _first_point_vector = nullptr;
  const base::Point2dVector* _second_point_vector = nullptr;
  double _avg_distance = 0.0; //> average distance of all pairs
  double _median_distance = 0.0; //> median distance of all pairs
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

namespace std {

inline ostream& operator<<(ostream& os, const francor::algorithm::PointPairIndex& index)
{
  os << "[" << index.first << " -> " << index.second << ", distance = " << index.distance << "]";
  return os;
}

inline ostream& operator<<(ostream& os, const francor::algorithm::PointPairIndexVector& indices)
{
  os << "### point pair index ###" << endl;
  os << "num pairs: " << indices.size() << endl;
  os << "pairs[]  : ";

  for (const auto& pair : indices)
    os << pair << " ";

  return os;
}

} // end namespace std