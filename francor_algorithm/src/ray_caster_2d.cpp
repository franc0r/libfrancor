#include "francor_algorithm/ray_caster_2d.h"

namespace francor {

namespace algorithm {

Ray2d Ray2d::create(const std::size_t xIdx,
                const std::size_t yIdx,
                const base::Point2d cellPosition,
                const double cellSize,
                const base::Point2d position,
                const base::Vector2d direction,
                const double distance)
{
  Ray2d ray;

  if (!ray.initialize(xIdx, yIdx, cellPosition, cellSize, position, direction, distance))
  {
    base::LogError() << "Ray: error occurred during initialization. Return uninitialized ray.";
    return { };
  }

  return ray;
}                

bool Ray2d::initialize(const std::size_t xIdx,
                     const std::size_t yIdx,
                     const base::Point2d cellPosition,
                     const double cellSize,
                     const base::Point2d position,
                     const base::Vector2d direction,
                     const double distance)
{
  using francor::base::Line;
  using francor::base::Vector2d;

  assert(direction.norm() >= 0.99 && direction.norm() <= 1.01);

  // take start index and max distance
  _current_idx.x() = static_cast<int>(xIdx);
  _current_idx.y() = static_cast<int>(yIdx);
  _max_distance = distance;


  // select right operation according the direction vector
  _operation = Operation::NONE;

  if (direction.x() >= 0.0) _operation |= Operation::MOVE_RIGHT; else _operation |= Operation::MOVE_LEFT;
  if (direction.y() >= 0.0) _operation |= Operation::MOVE_DOWN;  else _operation |= Operation::MOVE_UP;
  
  // if (_operation & Operation::MOVE_LEFT) --_current_idx.x();
  // if (_operation & Operation::MOVE_UP) --_current_idx.y();


  // calculate next x and y position of next y and x grid line.
  const double posNextGridLineX = cellPosition.x() + cellSize * 0.5;
  const double posNextGridLineY = cellPosition.y() + cellSize * 0.5;
  const Line ray(Line::createFromVectorAndPoint(direction, position));
  std::cout << "pos next grid line x = " << posNextGridLineX << std::endl;
  std::cout << "pos next grid line y = " << posNextGridLineY << std::endl;
  const auto intersectionNextCellX = ray.intersectionPoint(Line::createFromVectorAndPoint( { 0.0, 1.0 }, { posNextGridLineX, 0.0 } ));
  const auto intersectionNextCellY = ray.intersectionPoint(Line::createFromVectorAndPoint( { 1.0, 0.0 }, { 0.0, posNextGridLineY } ));
  std::cout << "intersection x:" << std::endl << intersectionNextCellX << std::endl;
  std::cout << "intersection y:" << std::endl << intersectionNextCellY << std::endl;
  // already get distances (m) of next iteration in x and y direction
  _side_dist.x() = (intersectionNextCellX - position).norm();
  _side_dist.y() = (intersectionNextCellY - position).norm();
  std::cout << "side dist:" << std::endl << _side_dist << std::endl;

  // calculate for each direction the step length
  _delta_dist.x() = (Vector2d(direction) / direction.x()).norm() * cellSize;
  _delta_dist.y() = (Vector2d(direction) / direction.y()).norm() * cellSize;
  std::cout << "delta dist:" << std::endl << _delta_dist << std::endl;

  return true;  
}                     

} // end namespace algorithm

} // end namespace francor