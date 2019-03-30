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
class Port : public PortId
{
protected:
  Port(void) = delete;
  Port(const std::string& name, const pipeline::Direction dataFlow, DataType* data = nullptr) : PortId(name), data_flow(dataFlow), _data(data) { }
  Port(const Port&) = delete;
  Port(Port&&) = delete;
  ~Port(void)
  {

  }

  Port& operator=(const Port&) = delete;
  Port& operator=(Port&&) = delete;

  DataType* _data = nullptr;

public:
  const pipeline::Direction data_flow;
  using type = DataType;

  DataType& data(void) { if (_data == nullptr) throw "data pointer is null"; return *_data; }
};

template <typename DataType>
class OutputPort;

template <typename DataType>
class InputPort : public Port<DataType>
{
public:
  InputPort(void) = delete;
  InputPort(const std::string& name) : Port<DataType>(name, pipeline::Direction::In) { }

  bool isConnected(const OutputPort<DataType>& output)
  {
    return _connected_output == &output;
  }

  bool connect(OutputPort<DataType>& output)
  {
    if (_connected_output != nullptr)
    {
      // TODO: print error
      return false;
    }

    _connected_output = &output;
    Port<DataType>::_data = &_connected_output->data();

    if (!output.isConnected(*this))
      return output.connect(*this);

    return true;
  }

  bool disconnect(OutputPort<DataType>& output)
  {
    if (!this->isConnected(output))
      return true;

    _connected_output = nullptr;
    Port<DataType>::_data = nullptr;

    if (output.isConnected(*this))
      return output.disconnect(*this);

    return true;
  }

private:
  OutputPort<DataType>* _connected_output = nullptr;
};

template <typename DataType>
class OutputPort : public Port<DataType>
{
public:
  OutputPort(void) = delete;
  OutputPort(const std::string& name
  , DataType& data) : Port<DataType>(name, pipeline::Direction::Out, &data) { }

  bool isConnected(const InputPort<DataType>& input)
  {
    return std::find(_connected_inputs.begin(), _connected_inputs.end(), &input) != _connected_inputs.end();
  }

  bool connect(InputPort<DataType>& input)
  {
    if (this->isConnected(input))
    {
      // TODO: print error
      return false;
    }

    _connected_inputs.push_back(&input);

    if (!input.isConnected(*this))
      return input.connect(*this);

    return true;
  }

  bool disconnect(InputPort<DataType>& input)
  {
    if (!this->isConnected(input))
      return true;

    _connected_inputs.erase(std::find(_connected_inputs.begin(), _connected_inputs.end(), &input));

    if (input.isConnected(*this))
      return input.disconnect(*this);

    return true;
  }

private:
  std::list<InputPort<DataType>*> _connected_inputs;
};

template <typename T, const char PortName[]>
struct PortConfig { static constexpr const char* name = PortName; using type = T; };

// Input Block
template <std::size_t i, typename Config>
struct BlockInputPort
{
public:
  BlockInputPort(void) : port(Config::name) { }

  InputPort<typename Config::type> port;
};

template <std::size_t i, typename... Configs>
struct InputBlockImpl;

template <std::size_t i>
struct InputBlockImpl<i>
{
public:
  InputBlockImpl(void) = default;
};

template <std::size_t i, typename Config, typename... Configs>
struct InputBlockImpl<i, Config, Configs...> : public InputBlockImpl<i + 1, Configs...>, public BlockInputPort<i, Config>
{
public:
  InputBlockImpl(void) : InputBlockImpl<i + 1, Configs...>(), BlockInputPort<i, Config>() { }

  static constexpr std::size_t numInputs(void) { return sizeof...(Configs) + 1; }
};

template <std::size_t i, typename Config, typename... Configs>
auto& get(InputBlockImpl<i, Config, Configs...>& block) { return block.BlockInputPort<i, Config>::port; }

template <typename... Configs>
using InputBlock = InputBlockImpl<0, Configs...>;


// Output Block


} // end namespace processing

} // end namespace francor
