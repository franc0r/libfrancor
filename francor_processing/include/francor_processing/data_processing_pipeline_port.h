/**
 * Data processing pipeline ports provide a generic data interface for the processing pipeline.
 * Want to get more experience about data processing pipelines.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 30. February 2019
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Port Base
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

std::size_t PortId::_id_counter = 1;

template <typename DataType, pipeline::Direction Direction>
class Port : public PortId
{
protected:
  Port(void) = delete;
  Port(const std::string& name, const DataType* data = nullptr) : PortId(name), _data(data) { }
  Port(const Port&) = delete;
  Port(Port&&) = delete;
  ~Port(void)
  {

  }

  Port& operator=(const Port&) = delete;
  Port& operator=(Port&&) = delete;

  const DataType* _data = nullptr;

public:
  static constexpr pipeline::Direction data_flow = Direction;
  using type = DataType;

  const DataType& data(void) const { if (_data == nullptr) throw "Port::data(): data pointer is null"; return *_data; }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input Port
template <typename DataType>
class OutputPort;

template <typename DataType>
class InputPort : public Port<DataType, pipeline::Direction::In>
{
public:
  InputPort(void) = delete;
  InputPort(const std::string& name) : Port<DataType, pipeline::Direction::In>(name) { }
  InputPort(const std::string& name, const DataType*) : InputPort<DataType>(name) { }

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
    Port<DataType, pipeline::Direction::In>::_data = &_connected_output->data();

    if (!output.isConnected(*this))
      return output.connect(*this);

    return true;
  }

  bool disconnect(OutputPort<DataType>& output)
  {
    if (!this->isConnected(output))
      return true;

    _connected_output = nullptr;
    Port<DataType, pipeline::Direction::In>::_data = nullptr;

    if (output.isConnected(*this))
      return output.disconnect(*this);

    return true;
  }

private:
  OutputPort<DataType>* _connected_output = nullptr;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Output Port
template <typename DataType>
class OutputPort : public Port<DataType, pipeline::Direction::Out>
{
public:
  OutputPort(void) = delete;
  OutputPort(const std::string& name, const DataType* data) : Port<DataType, pipeline::Direction::Out>(name, data) { }

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Port Config used for all port types
struct PortConfig
{
  PortConfig(void) = delete;
  constexpr PortConfig(const char* name_, const void* data_ = nullptr) : name(name_), data(data_) { }
  
  const char* name = "none";
  const void* data = nullptr;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Port Block in and out
template <std::size_t i, typename DataType, template<typename> class PortType>
struct BlockPort
{
public:
  BlockPort(const char* name, const DataType* data = nullptr) : port(name, data) { }

  PortType<DataType> port;
};

template <std::size_t i, template<typename> class PortType, typename... DataTypes>
struct BlockPortImpl;

template <std::size_t i, template<typename> class PortType>
struct BlockPortImpl<i, PortType>
{
public:
  BlockPortImpl(const std::array<PortConfig, i>& config) { }

  static constexpr std::size_t numInputs(void) { return i; }
};

template <std::size_t i, template<typename> class PortType, typename HeadDataType, typename... DataTypes>
struct BlockPortImpl<i, PortType, HeadDataType, DataTypes...> : public BlockPortImpl<i + 1, PortType, DataTypes...>, public BlockPort<i, HeadDataType, PortType>
{
public:
  BlockPortImpl(const std::array<PortConfig, BlockPortImpl<i, PortType, HeadDataType, DataTypes...>::numInputs()>& config)
    : BlockPortImpl<i + 1, PortType, DataTypes...>(config),
      BlockPort<i, HeadDataType, PortType>(std::get<i>(config).name, reinterpret_cast<const HeadDataType*>(std::get<i>(config).data))
  {

  }

  static constexpr pipeline::Direction data_flow = PortType<HeadDataType>::data_flow;
};

//TODO: move this function into class BlockPortImpl
template <std::size_t i, template<typename> class PortType, typename HeadDataType, typename... DataTypes>
auto& get(BlockPortImpl<i, PortType, HeadDataType, DataTypes...>& block) { return block.BlockPort<i, HeadDataType, PortType>::port; }

template <typename... DataTypes>
using InputBlock = BlockPortImpl<0, InputPort, DataTypes...>;

template <typename... DataTypes>
using OutputBlock = BlockPortImpl<0, OutputPort, DataTypes...>;

} // end namespace processing

} // end namespace francor