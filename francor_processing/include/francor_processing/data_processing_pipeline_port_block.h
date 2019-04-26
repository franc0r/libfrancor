/**
 * Port blocks are used for input and output blocks for data processing pipeline stages.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 6. April 2019
 */
#pragma once

#include "francor_base/log.h"

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
    using francor::base::LogError;
    using francor::base::LogDebug;

    LogDebug() << "PortBlock: try to connect to data port \"" << portB.name() << "\".";
    const std::size_t portIndex = this->getPortIndex(portNameA);

    if (portIndex >= _ports.size())
    {
      LogError() << "PortBlock: data port \"" << portNameA << " isn't contained. Can't connect to data port \""
                 << portB.name() << "\".";
      return false;
    }

    if (!_ports[portIndex].connect(portB))
    {
      LogError() << "PortBlock: error occurred during connecting to data port \"" << portB.name() << "\".";
      return false;
    }

    LogDebug() << "PortBlock: connection to data port \"" << portB.name() << "\" has been established.";
    return true;
  }

  bool disconnect(const std::string& portNameA, data::Port& portB)
  {
    using francor::base::LogError;
    using francor::base::LogDebug;

    LogDebug() << "PortBlock: try to disconnect from data port \"" << portB.name() << "\".";
    const std::size_t portIndex = this->getPortIndex(portNameA);

    if (portIndex >= _ports.size())
    {
      LogError() << "PortBlock: data port \"" << portNameA << " isn't contained. Can't disconnect from data port \""
                 << portB.name() << "\".";      
      return false;
    }

    if (!_ports[portIndex].disconnect(portB))
    {
      LogError() << "PortBlock: error occurred during disconnecting from data port \"" << portB.name() << "\".";
      return false;
    }

    LogDebug() << "PortBlock: data port \"" << portB.name() << "\" successfully disconnected from \"" << portNameA
               << "\".";
    return true;
  }

  template <typename DataType>
  bool configurePort(const std::size_t portIndex, const std::string& name)
  {
    using francor::base::LogError;
    using francor::base::LogDebug;
    LogDebug() << "PortBlock: try to configure data port with index = " << portIndex << ".";

    if (portIndex >= _ports.size())
    {
      LogError() << "PortBlock: port index " << portIndex << " is out of range. Can't configure data port.";
      return false;
    }

    _ports[portIndex] = std::move(data::InputPort::create<DataType>(name));

    LogDebug() << "PortBlock: data port with index = " << portIndex << " successfully configured. Used port name \""
               << name << "\" and data type \"" << typeid(DataType).name() << "\".";
    return true;
  }

  template <typename DataType>
  bool configurePort(const std::size_t portIndex, const std::string& name, DataType const* const data)
  {
    using francor::base::LogError;
    using francor::base::LogDebug;
    LogDebug() << "PortBlock: try to configure data port with index = " << portIndex << ".";

    static_assert(std::is_same<PortType, data::OutputPort>::value);
    if (portIndex >= _ports.size())
    {
      LogError() << "PortBlock: port index " << portIndex << " is out of range. Can't configure data port.";      
      return false;
    }

    _ports[portIndex] = std::move(data::OutputPort::create<DataType>(name, data));

    LogDebug() << "PortBlock: data port with index = " << portIndex << " successfully configured. Used port name \""
               << name << "\" and data type \"" << typeid(DataType).name() << "\".";
    return true;
  }

  PortType& port(const std::size_t index) noexcept { return _ports[index]; }

  std::size_t getPortIndex(const std::string& name) const
  {
    for (std::size_t i = 0; i < _ports.size(); ++i)
      if (_ports[i].name() == name)
        return i;

    return _ports.size();
  }

  inline static constexpr std::size_t numPorts(void) { return NumPorts; }

private:
  std::array<PortType, NumPorts> _ports;
};


template <std::size_t NumPorts> 
using InputPortBlock = PortBlockImpl<francor::processing::data::InputPort, NumPorts>;

template <std::size_t NumPorts>
using OutputPortBlock = PortBlockImpl<data::OutputPort, NumPorts>;

} // end namespace processing

} // end namespace francor
