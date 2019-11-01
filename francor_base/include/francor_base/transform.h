/**
 * Implements a transform, rotation and tranlation classes. This classes works on points.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. October 2019
 */

#pragma once

#include "francor_base/point.h"
#include "francor_base/vector.h"
#include "francor_base/matrix.h"
#include "francor_base/angle.h"

#include <ostream>

namespace francor {

namespace base {

class Rotation2d
{
public:
  Rotation2d(const Angle& angle = 0.0)
    : _phi(angle)
  {
    this->calculateMat(_phi, _mat);
  }
  inline Point2d operator*(const Point2d& point) const
  {
    return { _mat(0, 0) * point.x() + _mat(0, 1) * point.y(), _mat(1, 0) * point.x() + _mat(1, 1) * point.y() };
  }
  inline Vector2d operator*(const Vector2d& vector) const
  {
    return _mat * vector;
  }
  inline Rotation2d operator*(const Rotation2d& operant) const { return { _phi + operant._phi }; }
  inline Rotation2d& operator=(const Angle& angle) { _phi = angle; this->calculateMat(_phi, _mat); return *this; }
  inline Rotation2d inverse() const { return { _phi * -1.0 }; }
  inline const Matrix2d& mat() const noexcept { return _mat; }
  inline Angle phi() const { return _phi; }

private:
  void calculateMat(const Angle& phi, Matrix2d& mat) const
  {
    mat << std::cos(_phi), -std::sin(_phi),
           std::sin(_phi),  std::cos(_phi);
  }

  Angle _phi = 0.0;
  Matrix2d _mat;
};

class Transform2d
{
public:
  Transform2d() = default;
  Transform2d(const Rotation2d& rotation, const Vector2d& translation)
    : _rotation(rotation),
      _translation(translation)
  { }

  inline void setRotation(const Angle angle) { _rotation = angle; }
  inline void setRotation(const Rotation2d& rot) { _rotation = rot; }
  inline void setTranslation(const Vector2d& trans) { _translation = trans; }
  inline const Rotation2d& rotation() const noexcept { return _rotation; }
  inline const Vector2d& translation() const noexcept { return _translation; }

  inline Transform2d inverse() const
  {
    const Rotation2d inv = _rotation.inverse();
    return { inv, (inv * _translation) * -1.0 };
  }
  inline Transform2d operator*(const Transform2d& operant) const
  {
    return { _rotation * operant._rotation, _translation + _rotation * operant._translation };
  }
  inline Point2d operator*(const Point2d& point) const 
  {
    // first rotate then move
    return _rotation * point + _translation;
  }
  operator Matrix3d() const
  {
    Matrix3d mat;

    mat << _rotation.mat()(0, 0), _rotation.mat()(0, 1), _translation.x(), 
           _rotation.mat()(1, 0), _rotation.mat()(1, 1), _translation.y(),
                             0.0,                   0.0,              1.0;

    return mat;                              
  } 

private:
  Rotation2d _rotation;
  Vector2d _translation;
};

} // end namespace base

} // end namespace francor

namespace std {

inline ostream& operator<<(ostream& os, const francor::base::Transform2d& transform)
{
  os << "[ rot: " << transform.rotation().phi() << ", t: [ " << transform.translation().x() << ", "
     << transform.translation().y() << " ] ]";

  return os;
}

} // end namespace std