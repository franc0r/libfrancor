#include "francor_processing/data_processing_port.h"

#include <francor_base/log.h>

#include <iostream>

namespace francor
{

namespace processing
{

namespace data
{
using francor::base::LogError;
using francor::base::LogDebug;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PortId
PortId::PortId(PortId&& origin)
  : _name(std::move(origin._name)),
    _id(origin._id)
{
  origin._name = "none"; 
  origin._id = 0; 
}

PortId& PortId::operator=(PortId&& origin)
{
  _name = std::move(origin._name);
  _id = origin._id; 
  origin._name = "none";
  origin._id = 0;

  return *this;
};

std::size_t PortId::_id_counter = 1;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Port
Port::Port(Port&& origin)
{
  *this = std::move(origin);
}

Port::~Port(void)
{
  this->reset();
}

Port& Port::operator=(Port&& origin)
{
  // first reset current state, so no active connection is left
  this->reset();

  // take attributes from origin
  PortId::operator=(std::move(origin));
  _data_flow = origin._data_flow;
  _data = origin._data;
  _data_type_info = origin._data_type_info;

  // take all connections from origin
  for (auto& connection : origin._connections)
  {
    if (connection != nullptr)
    {
      Port& targetPort = *connection; // get target port address, because after disconnect the iterator of connections is
                                      // pointing to a nullptr
      origin.disconnect(targetPort);
      this->connect(targetPort);
    }
  }

  // reset origin
  origin._data_flow = Direction::NONE;
  origin._data = nullptr;
  origin._data_type_info = typeid(void);

  return *this;
}

void Port::initializeConnections(void)
{
  for (auto& connection : _connections)
    connection = nullptr;
}

bool Port::connect(Port& port)
{
  // the data flow must be different
  if (port._data_flow == _data_flow)
  {
    LogError() << "Port (name = " << this->name() << "): can't connect to port (name = " << port.name() << "). "
               << "Data flow isn't different.";
    return false;
  }
  // the data type must be equal and not of type void
  if (port._data_type_info.get() != _data_type_info.get()
      ||
      _data_type_info.get() == typeid(void))
  {
    LogError() << "Port (name = " << this->name() << "): can't connect to port (name = " << port.name() << "). "
               << "Data type is different";
    return false;
  }
  // if a connetions is already established then cancel
  if (this->isConnectedWith(port))
  {
    LogError() << "Port (name = " << this->name() << "): can't connect to port (name = " << port.name() << "). "
               << "Ports are already connected.";
    return false;
  }

  switch (_data_flow)
  {
  case Direction::IN:
    // only one connection is allowed
    if (_connections[0] != nullptr)
    {
      LogError() << "Port (name = " << this->name() << "): can't connect to port (name = " << port.name() << "). "
                 << "Maximum number of connections reached.";
      return false;
    }

    _connections[0] = &port;
    _data = port._data;
    break;

  case Direction::OUT:
    {
      const std::size_t indexConnection = this->nextConnectionIndex();

      if (indexConnection >= _connections.size())
      // maximum number of connections is reached
      {
        LogError() << "Port (name = " << this->name() << "): can't connect to port (name = " << port.name() << "). "
                   << "Maximum number of connections reached.";
        return false;
      }

      _connections[indexConnection] = &port;
    }
    break;

  default:
    LogError() << "Port (name = " << this->name() << "): can't connect to port (name = " << port.name() << "). "
               << "Unsupported data flow.";
    return false;
  }


  if (!port.isConnectedWith(*this))
    port.connect(*this);

  return true;
}

bool Port::disconnect(Port& port)
{
  if (!this->isConnectedWith(port))
  {
    LogError() << "Port (name = " << this->name() << "): can't disconnect from port (name = " << port.name() << "). "
               << "The ports aren't connected.";
    return false;
  }

  // disconnect port from this
  switch (_data_flow)
  {
  case Direction::IN:
    // clean up data pointer, only as input!
    _data = nullptr;
    _connections[0] = nullptr;

    break;

  case Direction::OUT:

    for (auto& connection : _connections)
      if (connection == &port)
        connection = nullptr;
    
    break;

  default:
    return false;
  }

  // disconnect this from port
  if (port.isConnectedWith(*this))
    port.disconnect(*this);

  return true;
}

bool Port::isConnectedWith(const Port& port) const
{
  for (const auto connection : _connections)
    if (connection == &port)
      return true;

  return false;
}

void Port::reset(void)
{
  // clean up, disconnect from all established connections
  for (auto& connection : _connections)
    if (connection != nullptr)
      this->disconnect(*connection);

  if (_data_flow == Direction::IN)
    _data = nullptr;

  this->initializeConnections();
}

std::size_t Port::numOfConnections(void) const
{
  std::size_t counter = 0;

  for (const auto connection : _connections)
    if (connection != nullptr)
      ++counter;

  return counter;
}

std::size_t Port::nextConnectionIndex(void) const
{
  for (std::size_t i = 0; i < _connections.size(); ++i)
    if (_connections[i] == nullptr)
      return i;

  return _connections.size();
}

} // end namespace data

} // end namespace processing

} // end namespace francor