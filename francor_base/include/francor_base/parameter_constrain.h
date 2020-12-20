/**
 * Implements the class ParameterConstrain. This class is used to check if an parameter value is valid.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 19. December 2020
 */
#pragma once

namespace francor {

namespace base {

template <typename DataType>
class ParameterConstrain
{
protected:
  explicit ParameterConstrain(const char* const name) : _name(name) { }

public:
  virtual ~ParameterConstrain() = default;

  inline const char* const name() const { return _name; }
  virtual bool isFullFiled(const DataType& parameter_value) const = 0;

private:
  const char* const _name;
};

template <typename DataType>
class ConstrainMaximumValue : public ParameterConstrain<DataType>
{
public:
  ConstrainMaximumValue(const DataType& max_value) : ParameterConstrain<DataType>("maximum_value"), _max_value(max_value) { }
  bool isFullFiled(const DataType& parameter_value) const final
  {
    return parameter_value <= _max_value;
  }

private:
  const DataType _max_value;
};

template <typename DataType>
class ConstrainMinimumValue : public ParameterConstrain<DataType>
{
public:
  ConstrainMinimumValue(const DataType& min_value) : ParameterConstrain<DataType>("minimum_value"), _min_value(min_value) { }
  bool isFullFiled(const DataType& parameter_value) const final
  {
    return parameter_value >= _min_value;
  }

private:
  const DataType _min_value;
};



} // end namespace base

} // end namespace francor