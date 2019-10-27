/**
 * Implements a transform, rotation and tranlation classes. This classes works on points.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#pragma once

#include "francor_base/point.h"
#include "francor_base/vector.h"
#include "francor_base/angle.h"

namespace francor {

namespace base {

struct Rotation2d
{
  constexpr inline Point2d operator*(const Point2d& point) const
  {
    return { std::cos(phi) * point.x() - std::sin(phi) * point.y(),
             std::sin(phi) * point.x() + std::cos(phi) * point.y() };
  }

  inline Rotation2d inverse() const { return { phi * -1.0 }; }

  Angle phi = 0.0;
};

class Transform2d
{
public:
  Transform2d() = default;
  Transform2d(const Rotation2d& rotation, const Vector2d& translation)
    : _rotation(rotation),
      _translation(translation)
  { }

  inline void setRotation(const Angle angle) { _rotation.phi = angle; }
  inline void setRotation(const Rotation2d& rot) { _rotation = rot; }
  inline void setTranslation(const Vector2d& trans) { _translation = trans; }
  inline const Rotation2d& rotation() const noexcept { return _rotation; }
  inline const Vector2d& translation() const noexcept { return _translation; }

  Transform2d inverse() const { return { _rotation.inverse(), _translation * -1.0 }; }

  inline Point2d operator*(const Point2d& point) const 
  {
    // first move then rotate
    return _rotation * (point + _translation);
  }

private:
  Rotation2d _rotation;
  Vector2d _translation;
};

} // end namespace base

} // end namespace francor