/**
 * Ray caster that works on a 2d grid/image.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 3. October 2019
 */
#pragma once

#include <cstddef>

#include <francor_base/vector.h>
#include <francor_base/line.h>
#include <francor_base/log.h>

namespace francor {

namespace algorithm {

class Ray2d
{
public:
  ~Ray2d() = default;
  static Ray2d create(const std::size_t xIdx,
                    const std::size_t yIdx,
                    const base::Vector2d cellPosition,
                    const double cellSize,
                    const base::Vector2d position,
                    const base::Vector2d direction,
                    const double distance);                

  inline operator bool() const
  {
    // return true until both reached the max distance
    return _side_dist.x() < _max_distance
           ||
           _side_dist.y() < _max_distance;
  }

  inline Ray2d& operator++()
  {
    if (_side_dist.x() < _side_dist.y())
    // move in x direction
    {
      _side_dist.x() += _delta_dist.x();
      _current_idx.x()++;
    }
    else
    // move in y direction
    {
      _side_dist.y() += _delta_dist.y();
      _current_idx.y()++;
    }
    
    return *this;
  }

  inline const base::Vector2i& operator*() const { return _current_idx; }
  inline const base::Vector2i& getCurrentIndex() const { return _current_idx; }
  inline double getCurrentCellWeight() const { return 1.0; }

private:
  enum class Operation {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_RIGHT,
    MOVE_LEFT,
  };

  Ray2d() = default;

  bool initialize(const std::size_t xIdx,
                  const std::size_t yIdx,
                  const base::Vector2d cellPosition,
                  const double cellSize,
                  const base::Vector2d position,
                  const base::Vector2d direction,
                  const double distance);             


  base::Vector2i _current_idx; //> current x and y index on the map/grid
  base::Vector2d _side_dist;   //> length of ray from start position to current position
  base::Vector2d _delta_dist;  //> length of ray from one x or y-side to next x or y-side
  double _max_distance; //> the maximum length of the ray. If max is reached the caster will terminate
};

class RayCaster2d
{
public:
  RayCaster2d() = default;


};

} // end namespace algorithm

} // end namespace francor