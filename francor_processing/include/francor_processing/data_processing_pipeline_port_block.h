/**
 * Port blocks are used for input and output blocks for data processing pipeline stages.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. April 2019
 */
#pragma once

#include "francor_processing/data_processing_port.h"

#include <array>
#include <type_traits>

namespace francor
{

namespace processing
{

template <class PortType, std::size_t NumPorts>
class PortBlockImpl
{
public:
  PortBlockImpl(void) = default;
  ~PortBlockImpl(void) = default;

  bool connect(const std::string& portNameA, data::Port& portB)
  {
    const std::size_t portIndex = this->getPortIndex(portNameA);

    if (portIndex >= _ports.size())
    {
      //TODO: print error
      return false;
    }

    if (!_ports[portIndex].connect(portB))
    {
      //TODO: print error
      return false;
    }

    return true;
  }

  bool disconnect(const std::string& portNameA, data::Port& portB)
  {
    const std::size_t portIndex = this->getPortIndex(portNameA);

    if (portIndex >= _ports.size())
    {
      //TODO: print error
      return false;
    }

    if (!_ports[portIndex].disconnect(portB))
    {
      //TODO: print error
      return false;
    }

    return true;
  }

  template <typename DataType>
  bool configurePort(const std::size_t portIndex, const std::string& name)
  {
    if (portIndex >= _ports.size())
    {
      //TODO: print error
      return false;
    }

    _ports[portIndex] = std::move(data::InputPort::create<DataType>(name));
    return true;
  }

  template <typename DataType>
  bool configurePort(const std::size_t portIndex, const std::string& name, DataType const* const data)
  {
    static_assert(std::is_same<PortType, data::OutputPort>::value);
    if (portIndex >= _ports.size())
    {
      //TODO: print error
      return false;
    }

    _ports[portIndex] = std::move(data::OutputPort::create<DataType>(name, data));
    return true;
  }

  const PortType& port(const std::size_t index) const noexcept { return _ports[index]; }

private:
  std::size_t getPortIndex(const std::string& name) const
  {
    for (std::size_t i = 0; i < _ports.size(); ++i)
      if (_ports[i].name() == name)
        return i;

    return _ports.size();
  }

  std::array<PortType, NumPorts> _ports;
};


template <std::size_t NumPorts> 
using InputPortBlock = PortBlockImpl<francor::processing::data::InputPort, NumPorts>;

template <std::size_t NumPorts>
using OutputPortBlock = PortBlockImpl<data::OutputPort, NumPorts>;

} // end namespace processing

} // end namespace francor
