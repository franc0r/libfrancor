/**
 * Data processing pipeline that provides a basic structure for data processing. Also just a start.
 * Want to get more experience about data processing pipelines.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include <string>
#include <list>
#include <functional>
#include <algorithm>
#include <memory>

namespace francor
{

namespace processing
{

namespace pipeline
{
  enum class Direction {
    In,
    Out
  };
} // end namespace pipeline

template <typename DataType, pipeline::Direction DataFlow>
class Port;

template <typename DataType>
using PortOut = Port<DataType, pipeline::Direction::Out>;

template <typename DataType>
using PortIn = Port<DataType, pipeline::Direction::In>;

class PortId
{
protected:
  PortId(void) = delete;
  PortId(const std::string& name) : _name(name), _id(_id_counter++) { }
  PortId(const PortId&) = delete;
  PortId(PortId&&) = default;
  ~PortId(void) = default;

public:
  const std::string& name(void) const noexcept { return _name; }
  std::size_t id(void) const noexcept { return _id; }

private:
  const std::string _name;
  const std::size_t _id;
  static std::size_t _id_counter;
};

template <typename DataType>
class PortConnection
{
public:
  PortConnection(void) = delete;
  PortConnection(const PortConnection&) = delete;
  PortConnection(PortConnection&&) = default;
  PortConnection(PortIn<DataType>& input, PortOut<DataType>& output) : _input(input), _output(output) { }
  ~PortConnection(void) = default;

  PortConnection& operator=(const PortConnection&) = delete;
  PortConnection& operator=(PortConnection&&) = default;

  inline const PortIn<DataType>& getOutput(void) const { return _output; }
  inline const PortOut<DataType>& getInput(void) const { return _input; }

private:
  PortIn<DataType>& _input;
  PortOut<DataType>& _output;
};

template <typename DataType>
class PortConnectionHandler
{
protected:
  PortConnectionHandler(void) = default;
  PortConnectionHandler(const PortConnectionHandler&) = delete;
  PortConnectionHandler(PortConnectionHandler&&) = delete;
  ~PortConnectionHandler(void) = default;

  PortConnectionHandler& operator=(const PortConnectionHandler&) = delete;
  PortConnectionHandler& operator=(PortConnectionHandler&&) = delete;

  bool connect(Port<DataType, pipeline::Direction::In>& portIn, Port<DataType, pipeline::Direction::Out>& portOut)
  {
    portOut._connections.push_back(PortConnection<DataType>(portIn, portOut));
    portIn._connections.push_back(PortConnection<DataType>(portIn, portOut));
    return true;
  }

  // members
  std::list<PortConnection<DataType>> _connections;
};

template <typename DataType, pipeline::Direction DataFlow>
class Port : public PortId, public PortConnectionHandler<DataType>
{
public:
  Port(void) = delete;
  Port(const std::string& name) : PortId(name) { }
  Port(const Port&) = delete;
  Port(Port&&) = delete;
  ~Port(void)
  {

  }

  Port& operator=(const Port&) = delete;
  Port& operator=(Port&&) = delete;

  bool connect(Port<DataType, pipeline::Direction::In>& portIn)
  {
    static_assert(DataFlow == pipeline::Direction::Out);
    PortConnectionHandler<DataType>::connect(portIn, *this);

    return true;
  }

  bool connect(Port<DataType, pipeline::Direction::Out>& portOut)
  {
    static_assert(DataFlow == pipeline::Direction::Out);
    PortConnectionHandler<DataType>::connect(*this, portOut);

    return true;
  }


};

} // end namespace processing

} // end namespace francor
