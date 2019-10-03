/**
 * Ray caster that works on a 2d grid/image.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 3. October 2019
 */
#pragma once

#include <cstddef>

#include <francor_base/vector.h>

namespace francor {

namespace algorithm {

class RayCaster2d
{
public:
  enum class RayDirection {
    UP_RIGHT,
    UP_LEFT,
    DOWN_RIGHT,
    DOWN_LEFT
  };

  RayCaster2d() = default;

  bool initialize(const std::size_t xIdx,
                  const std::size_t yIdx,
                  const base::Vector2d cellPosition,
                  const double cellSize,
                  const base::Vector2d position,
                  const base::Vector2d direction,
                  const double distance)
  {
    _current_idx.x() = static_cast<int>(xIdx);
    _current_idx.y() = static_cast<int>(yIdx);
    _c

    return true;
  }                  
  bool operator bool() const;
  RayCaster2d& operator++();
  inline const base::Vector2i& getCurrentIndex() const { return _current_idx; }

private:
  base::Vector2i _current_idx; //> current x and y index on the map/grid
  base::Vector2d _side_dist;   //> length of ray from current position to next x or y-side
  base::Vector2d _delta_dist;  //> length of ray from one x or y-side to next x or y-side
};

} // end namespace algorithm

} // end namespace francor