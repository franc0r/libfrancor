/**
 * Implements the class Parameter that is used for parameter handling.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 19. December 2020
 */
#pragma once

#include "francor_base/log.h"

#include <string>
#include <exception>
#include <memory>
#include <tuple>

namespace francor {

namespace base {

namespace impl {

template <typename Data>
Data convertValueString(const std::string&);

template <>
int convertValueString<int>(const std::string& value_string)
{
  return std::stoi(value_string);
}

template <>
double convertValueString<double>(const std::string& value_string)
{
  return std::stod(value_string);
}

template <>
std::string convertValueString<std::string>(const std::string& value_string)
{
  if (value_string.empty()) {
    throw std::invalid_argument("Value string is empty.");
  }
  return value_string;
}

} // end namespace impl

class ParameterAdapterInterface
{
public:
  ParameterAdapterInterface() = default;
  virtual ~ParameterAdapterInterface() = default;

  virtual std::string getValueString(const std::string& parameter_name) const = 0;
};

class ParameterBase
{
public:
  ParameterBase() = delete;
  ParameterBase(const std::string& name, const char* const description)
    : _name(name),
      _description(description)
  {

  }
  virtual ~ParameterBase() = default;

  virtual void updateValue(const std::shared_ptr<const ParameterAdapterInterface>& interface) = 0;
  const std::string& name() const { return _name; }
  const char* const description() const { return _description; }

private:
  const std::string _name;
  const char* const _description;
};

template <typename Data, class... Constrain>
class Parameter : public ParameterBase
{
public:
  Parameter(const std::string& name,
            const Data& default_value,
            const char* const description,
            const Constrain&... constrain_args)
    : ParameterBase(name, description),
      _constrains{constrain_args...}
  {
    if (false == this->setValue(default_value)) {
      LogError() << "Parameter class " << this->name() << ": can't set " << default_value << " as default value."
                 << " Parameter value is not initialized! Current value is " << _value;
    }
  }
  Parameter(const std::string& name,
            const Data& default_value,
            const char* const description,
            const std::shared_ptr<const ParameterAdapterInterface>& interface,
            const Constrain&... constrain_args)
    : Parameter(name, default_value, description, constrain_args...)
  {
    this->updateValue(interface);
  }
  ~Parameter() override = default;

  void updateValue(const std::shared_ptr<const ParameterAdapterInterface>& interface) final
  {
    if (nullptr == interface) {
      LogError() << "Parameter class " << this->name() << ": can't update value. Given interface is null.";
      throw std::runtime_error("Given ParameterAdapterInterface is null");
    }

    try {
      const auto& new_value = impl::convertValueString<Data>(interface->getValueString(this->name()));

      if (false == this->setValue(new_value)) {
        LogError() << "Parameter class " << this->name() << " can't set value. Keep previous value.";
        throw std::invalid_argument("New value is not valid");
      }      
    }
    catch (...) {
      LogError() << "Parameter class " << this->name() << ": value string convertion failed.";
      throw;
    }
  }
  inline const Data& value() const { return _value; }
  inline operator const Data&() const { return _value; }

private:
  bool setValue(const Data& value)
  {
    if (false == this->isValueValid(value)) {
      return false;
    }

    _value = value;
    return true;
  }
  template <std::size_t... I>
  bool isValueValidExecuter(const Data& value, std::index_sequence<I...>) const
  {
    bool result = true;

    ((result &= std::get<I>(_constrains).isFullFiled(value)), ...);

    return result;
  }
  bool isValueValid(const Data& value) const
  {
    if constexpr (sizeof...(Constrain) == 0) {
      return true;
    }
    else {
      return this->isValueValidExecuter(value, std::make_index_sequence<sizeof...(Constrain)>{});
    }
  }

  Data _value;
  std::tuple<Constrain...> _constrains;
};

} // end namespace base

} // end namespace francor