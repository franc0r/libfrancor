/**
 * A class that represents a angle and respects angle specific conditions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 5. October 2019
 */
#pragma once

#include <cmath>
#include <ostream>

namespace francor {

namespace base {

class Angle
{
public:
  constexpr Angle(const double radian = 0.0) : _value(radian) { }
  ~Angle() = default;

  inline constexpr double radian() const noexcept { return _value; }
  inline constexpr double degree() const noexcept { return this->convertToDegree(_value); }

  inline constexpr void setRadian(const double value) noexcept { _value = value; }
  inline constexpr void setDegree(const double value) noexcept { _value = this->convertToRadian(value); }

  inline constexpr Angle operator+(const Angle& operant) const noexcept { return { _value + operant._value }; }
  inline constexpr Angle operator+(const double operant) const noexcept { return { _value + operant        }; }
  inline constexpr Angle operator-(const Angle& operant) const noexcept { return { _value - operant._value }; }
  inline constexpr Angle operator-(const double operant) const noexcept { return { _value - operant        }; }

  inline constexpr Angle& operator+=(const Angle& operant) noexcept { _value += operant._value; return *this; }
  inline constexpr Angle& operator+=(const double operant) noexcept { _value += operant       ; return *this; }
  inline constexpr Angle& operator-=(const Angle& operant) noexcept { _value -= operant._value; return *this; }
  inline constexpr Angle& operator-=(const double operant) noexcept { _value -= operant       ; return *this; }
  inline constexpr Angle& operator= (const double operant) noexcept { _value  = operant       ; return *this; }

  inline constexpr operator double() const noexcept { return _value; }

  static constexpr Angle createFromDegree(const double angle) { return { convertToRadian(angle) }; }

private:
  static constexpr double convertToDegree(const double value) { return value * 180.0 / M_PI; }
  static constexpr double convertToRadian(const double value) { return value * M_PI / 180.0; }

protected:
  double _value;
};


class NormalizedAngle : public Angle
{
public:
  constexpr NormalizedAngle(const double radian = 0.0) : Angle(radian) { this->normalize(); }
  constexpr NormalizedAngle(const Angle& angle) : Angle(angle) { this->normalize(); }
  ~NormalizedAngle() = default;

  constexpr void normalize()
  {
    // add or sub pi/2 from angle until it is in range of [-pi/2, pi/2]
    while (_value >   M_PI_2) _value -= M_PI;
    while (_value <= -M_PI_2) _value += M_PI;
  }

  inline constexpr void setRadian(const double value) noexcept { Angle::setRadian(value); this->normalize(); }
  inline constexpr void setDegree(const double value) noexcept { Angle::setDegree(value); this->normalize(); }

  inline constexpr NormalizedAngle operator+(const Angle& operant) const noexcept { return { Angle::operator+(operant) }; }
  inline constexpr NormalizedAngle operator+(const double operant) const noexcept { return { Angle::operator+(operant) }; }
  inline constexpr NormalizedAngle operator-(const Angle& operant) const noexcept { return { Angle::operator-(operant) }; }
  inline constexpr NormalizedAngle operator-(const double operant) const noexcept { return { Angle::operator-(operant) }; }

  inline constexpr NormalizedAngle& operator+=(const Angle& operant) { Angle::operator+=(operant); this->normalize(); return *this;}
  inline constexpr NormalizedAngle& operator+=(const double operant) { Angle::operator+=(operant); this->normalize(); return *this;}
  inline constexpr NormalizedAngle& operator-=(const Angle& operant) { Angle::operator-=(operant); this->normalize(); return *this;}
  inline constexpr NormalizedAngle& operator-=(const double operant) { Angle::operator-=(operant); this->normalize(); return *this;}

  inline constexpr NormalizedAngle& operator=(const Angle& operant) { Angle::operator=(operant); this->normalize(); return *this; }
  inline constexpr NormalizedAngle& operator=(const double operant) { Angle::operator=(operant); this->normalize(); return *this; }
};

/**
 * @brief Hack for +- PI
 * 
 * @note @knueppl plz fix
 * 
 */
class NormalizedAngleExtended : public Angle
{
public:
  constexpr NormalizedAngleExtended(const double radian = 0.0) : Angle(radian) { this->normalize(); }
  constexpr NormalizedAngleExtended(const Angle& angle) : Angle(angle) { this->normalize(); }
  ~NormalizedAngleExtended() = default;

  constexpr void normalize()
  {
    // add or sub pi/2 from angle until it is in range of [-pi/2, pi/2]
    while (_value >   M_PI) _value -= 2 * M_PI;
    while (_value <= -M_PI) _value += 2 * M_PI;
  }

  inline constexpr void setRadian(const double value) noexcept { Angle::setRadian(value); this->normalize(); }
  inline constexpr void setDegree(const double value) noexcept { Angle::setDegree(value); this->normalize(); }

  inline constexpr NormalizedAngleExtended operator+(const Angle& operant) const noexcept { return { Angle::operator+(operant) }; }
  inline constexpr NormalizedAngleExtended operator+(const double operant) const noexcept { return { Angle::operator+(operant) }; }
  inline constexpr NormalizedAngleExtended operator-(const Angle& operant) const noexcept { return { Angle::operator-(operant) }; }
  inline constexpr NormalizedAngleExtended operator-(const double operant) const noexcept { return { Angle::operator-(operant) }; }

  inline constexpr NormalizedAngleExtended& operator+=(const Angle& operant) { Angle::operator+=(operant); this->normalize(); return *this;}
  inline constexpr NormalizedAngleExtended& operator+=(const double operant) { Angle::operator+=(operant); this->normalize(); return *this;}
  inline constexpr NormalizedAngleExtended& operator-=(const Angle& operant) { Angle::operator-=(operant); this->normalize(); return *this;}
  inline constexpr NormalizedAngleExtended& operator-=(const double operant) { Angle::operator-=(operant); this->normalize(); return *this;}

  inline constexpr NormalizedAngleExtended& operator=(const Angle& operant) { Angle::operator=(operant); this->normalize(); return *this; }
  inline constexpr NormalizedAngleExtended& operator=(const double operant) { Angle::operator=(operant); this->normalize(); return *this; }
};

} // end namespace base

} // end namespace francor



namespace std
{

inline ostream& operator<<(ostream& os, const francor::base::Angle& angle)
{
  os << "angle [radian = " << angle.radian() << ", degree = " << angle.degree() << "]";

  return os;
}

} // end namespace std