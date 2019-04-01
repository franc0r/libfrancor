/**
 * Data processing ports provide a data connection between data processing stages. This class handles a dynamic data flow
 * with type safety checks.
 * 
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 1. April 2019
 */
#pragma once

#include <string>
#include <list>
#include <typeinfo>
#include <functional>

namespace francor
{

namespace processing
{

class PortId
{
protected:
  PortId(void) = default;
  PortId(const std::string& name) : _name(name), _id(_id_counter++) { }
  PortId(const PortId&) = delete;
  PortId(PortId&& origin);
  ~PortId(void) = default;

  PortId& operator=(const PortId&) = delete;
  PortId& operator=(PortId&& origin);

public:
  inline const std::string& name(void) const noexcept { return _name; }
  inline std::size_t id(void) const noexcept { return _id; }

private:
  std::string _name = "none";
  std::size_t _id = 0;
  static std::size_t _id_counter;
};


class Port : public PortId
{
public:

  enum class Direction {
    IN,
    OUT,
    NONE
  };

  Port(void) = default;
  template <typename DataType>
  Port(const std::string& name, const Direction dataFlow, DataType const* const data = nullptr)
    : PortId(name),
      _data_flow(dataFlow),
      _data(data),
      _data_type_info(typeid(DataType))
  { }
  Port(const Port&) = delete;
  Port(Port&& origin);
  virtual ~Port(void);

  Port& operator=(const Port&) = delete;
  Port& operator=(Port&& origin);

  template <typename DataType>
  const DataType& data(void) const
  {
    if (_data == nullptr)
      throw "Port::data(): data pointer is null";
    if (_data_type_info.get() != typeid(DataType))
      throw std::string("Port::data(): type ") + typeid(DataType).name() + " isn't supported.";
  
    return *static_cast<DataType const*>(_data);
  }
  inline Direction dataFlow(void) const noexcept { return _data_flow; }
  inline const std::type_info& type(void) const { return _data_type_info.get(); }

private:
  Direction _data_flow = Direction::NONE;
  void const* _data = nullptr;
  std::reference_wrapper<const std::type_info> _data_type_info = typeid(void);
};

} // end namespace processing

} // end namespace francor
