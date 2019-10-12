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
                      const base::Point2d cellPosition,
                      const double cellSize,
                      const base::Point2d position,
                      const base::Vector2d direction,
                      const double distance);                

  inline operator bool() const
  {
    // return true until a operation is in process
    return _operation != 0;
  }

  inline Ray2d& operator++()
  {
    if (_side_dist.x() < _side_dist.y())
    // move in x direction
    {
      _side_dist.x() += _delta_dist.x();

      if      (_operation & Operation::MOVE_RIGHT)
        ++_current_idx.x();
      else if (_operation & Operation::MOVE_LEFT)
        --_current_idx.x();
      // else
      // no operation should be done
    }
    else
    // move in y direction
    {
      _side_dist.y() += _delta_dist.y();

      if      (_operation & Operation::MOVE_DOWN)
        ++_current_idx.y();
      else if (_operation & Operation::MOVE_UP)
        --_current_idx.y();
      // else
      // no operation should be done
    }

    // if both reached max distance clear operation
    if (_side_dist.x() >= _max_distance && _side_dist.y() >= _max_distance)
    {
      _operation = Operation::NONE;
    }

    return *this;
  }

  inline const base::Vector2i& operator*() const { return _current_idx; }
  inline const base::Vector2i& getCurrentIndex() const { return _current_idx; }
  inline double getCurrentCellWeight() const { return 1.0; }

private:
  struct Operation {
    enum {
      MOVE_UP    = (1 << 0),
      MOVE_DOWN  = (1 << 1),
      MOVE_RIGHT = (1 << 2),
      MOVE_LEFT  = (1 << 3),
      NONE       = 0,
    };
  };

  Ray2d() = default;

  bool initialize(const std::size_t xIdx,
                  const std::size_t yIdx,
                  const base::Point2d cellPosition,
                  const double cellSize,
                  const base::Point2d position,
                  const base::Vector2d direction,
                  const double distance);             


  base::Vector2i _current_idx; //> current x and y index on the map/grid
  base::Vector2d _side_dist;   //> length of ray from start position to current position
  base::Vector2d _delta_dist;  //> length of ray from one x or y-side to next x or y-side
  double _max_distance; //> the maximum length of the ray. If max is reached the caster will terminate
  std::uint8_t _operation = Operation::NONE;
};

class RayCaster2d
{
public:
  RayCaster2d() = default;


};

} // end namespace algorithm

} // end namespace francor