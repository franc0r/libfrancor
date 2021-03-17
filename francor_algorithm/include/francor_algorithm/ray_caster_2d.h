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
#include <francor_base/size.h>

namespace francor {

namespace algorithm {

class Ray2d
{
public:
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
  ~Ray2d() = default;
  static Ray2d create(const std::size_t xIdx,
                      const std::size_t yIdx,
                      const std::size_t numCellsX,
                      const std::size_t numcellsY,
                      const double cellSize,
                      const base::Point2d position,
                      const base::Vector2d direction,
                      const double distance);                

  class iterator
  {
  public:
    iterator() = delete;
    iterator(const base::Vector2u& currentIdx, const base::Vector2d& sideDist, const base::Vector2d& deltaDist, const std::uint8_t operation)
      : _current_idx(currentIdx), _side_dist(sideDist), _delta_dist(deltaDist), _operation(operation)
    { }

  iterator& operator++()
  {
    if (_side_dist.x() < _side_dist.y()) {
    // move in x direction 
      _side_dist.x() += _delta_dist.x();

      if (_operation & Operation::MOVE_RIGHT) {
        ++_current_idx.x();
      }
      else if (_operation & Operation::MOVE_LEFT) {
        --_current_idx.x();
      }
      // else
      // no operation should be done
    }
    else {
    // move in y direction
      _side_dist.y() += _delta_dist.y();

      if (_operation & Operation::MOVE_DOWN) {
        ++_current_idx.y();
      }
      else if (_operation & Operation::MOVE_UP) {
        --_current_idx.y();
      }
      // else
      // no operation should be done
    }

    return *this;
  }

  bool operator!=(const iterator& operant) const
  {
    return (_side_dist.x() < operant._side_dist.x() || _side_dist.y() < operant._side_dist.y())
           &&
           _current_idx.x() < operant._current_idx.x()
           &&
           _current_idx.x() >= 0
           &&
           _current_idx.y() < operant._current_idx.y()
           &&
           _current_idx.y() >= 0;
  }
    base::Size2u operator*() const { return {_current_idx.x(), _current_idx.y()}; }

  private:
    base::Vector2u _current_idx;
    base::Vector2d _side_dist;
    base::Vector2d _delta_dist;
    std::uint8_t _operation = Operation::NONE;
  };

  iterator begin() const { return { _current_idx, _side_dist, _delta_dist, _operation }; }
  iterator interim() const { return { _max_idx, { _interim_distance, _interim_distance }, _delta_dist, Operation::NONE }; }
  iterator end() const { return { _max_idx, { _max_distance, _max_distance }, _delta_dist, Operation::NONE }; }

  inline operator bool() const
  {
    // return true if an operation is in progress
    return _operation != 0;
  }

  inline Ray2d& operator++()
  {
    if (_side_dist.x() < _side_dist.y()) {
    // move in x direction
      _side_dist.x() += _delta_dist.x();

      if (_operation & Operation::MOVE_RIGHT) {
        ++_current_idx.x();
      }
      else if (_operation & Operation::MOVE_LEFT) {
        --_current_idx.x();
      }
      // else
      // no operation should be done
    }
    else {
    // move in y direction
      _side_dist.y() += _delta_dist.y();

      if (_operation & Operation::MOVE_DOWN) {
        ++_current_idx.y();
      }
      else if (_operation & Operation::MOVE_UP) {
        --_current_idx.y();
      }
      // else
      // no operation should be done
    }

    // if both reached max distance clear operation
    if ((_side_dist.x() >= _max_distance && _side_dist.y() >= _max_distance)
        ||
        _current_idx.x() >= _max_idx.x()
        ||
        _current_idx.y() >= _max_idx.y()
        ||
        _current_idx.x() < 0
        ||
        _current_idx.y() < 0)
    {
      _operation = Operation::NONE;
    }
    
    return *this;
  }

  inline base::Size2u operator*() const { return {_current_idx.x(), _current_idx.y()}; }
  inline base::Size2u getCurrentIndex() const { return {_current_idx.x(), _current_idx.y()}; }
  inline double getCurrentCellWeight() const { return 1.0; }

private:
  bool initialize(const std::size_t xIdx,
                  const std::size_t yIdx,
                  const std::size_t numCellsX,
                  const std::size_t numcellsY,
                  const double cellSize,
                  const base::Point2d position,
                  const base::Vector2d direction,
                  const double distance);             


  base::Vector2u _current_idx; //> current x and y index on the map/grid
  base::Vector2u _max_idx;     //> the maximum valid index of the map/grid
  base::Vector2d _side_dist;   //> length of ray from start position to current position
  base::Vector2d _delta_dist;  //> length of ray from one x or y-side to next x or y-side
  double _max_distance;        //> the maximum length of the ray. If max is reached the caster will terminate
  double _interim_distance;    //> 
  std::uint8_t _operation = Operation::NONE;
};

} // end namespace algorithm

} // end namespace francor