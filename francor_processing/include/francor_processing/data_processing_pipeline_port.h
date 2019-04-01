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
class PortId
{
protected:
  PortId(void) = default;
  PortId(const std::string& name) : _name(name), _id(_id_counter++) { }
  PortId(const PortId&) = delete;
  PortId(PortId&& origin) : _name(std::move(origin._name)), _id(origin._id) { origin._name = "none"; origin._id = 0; }
  ~PortId(void) = default;

  PortId& operator=(const PortId&) = delete;
  PortId& operator=(PortId&& origin)
  {
    _name = std::move(origin._name);
    _id = origin._id; 
    origin._name = "none";
    origin._id = 0;

    return *this;
  };

public:
  const std::string& name(void) const noexcept { return _name; }
  std::size_t id(void) const noexcept { return _id; }

private:
  std::string _name = "none";
  std::size_t _id = 0;
  static std::size_t _id_counter;
};

std::size_t PortId::_id_counter = 1;

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
  Port(Port&& origin)
    : PortId(std::move(origin)),
      _data_flow(origin._data_flow), 
      _data(origin._data), 
      _data_type_info(origin._data_type_info)
  {
    origin._data_flow = Direction::NONE;
    origin._data = nullptr;
    origin._data_type_info = typeid(void);
  }
  virtual ~Port(void)
  {

  }

  Port& operator=(const Port&) = delete;
  Port& operator=(Port&& origin)
  {
    PortId::operator=(std::move(origin));
    _data_flow = origin._data_flow;
    _data = origin._data;
    _data_type_info = origin._data_type_info;

    origin._data_flow = Direction::NONE;
    origin._data = nullptr;
    origin._data_type_info = typeid(void);

    return *this;
  }

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Input Port
// template <typename DataType>
// class OutputPort;

// template <typename DataType>
// class InputPort : public Port<DataType, pipeline::Direction::In>
// {
// public:
//   InputPort(void) = delete;
//   InputPort(const std::string& name) : Port<DataType, pipeline::Direction::In>(name) { }
//   InputPort(const std::string& name, const DataType*) : InputPort<DataType>(name) { }
//   ~InputPort(void)
//   {
//     if (_connected_output != nullptr)
//       this->disconnect(*_connected_output);
//   }

//   bool isConnected(const OutputPort<DataType>& output) const
//   {
//     return _connected_output == &output;
//   }

//   bool isConnected(void) const
//   {
//     return _connected_output != nullptr;
//   }

//   bool connect(OutputPort<DataType>& output)
//   {
//     if (_connected_output != nullptr)
//     {
//       // TODO: print error
//       return false;
//     }

//     _connected_output = &output;
//     Port<DataType, pipeline::Direction::In>::_data = &_connected_output->data();

//     if (!output.isConnected(*this))
//       return output.connect(*this);

//     return true;
//   }

//   bool disconnect(OutputPort<DataType>& output)
//   {
//     if (!this->isConnected(output))
//       return true;

//     _connected_output = nullptr;
//     Port<DataType, pipeline::Direction::In>::_data = nullptr;

//     if (output.isConnected(*this))
//       return output.disconnect(*this);

//     return true;
//   }

// private:
//   OutputPort<DataType>* _connected_output = nullptr;
// };

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Output Port
// template <typename DataType>
// class OutputPort : public Port<DataType, pipeline::Direction::Out>
// {
// public:
//   OutputPort(void) = delete;
//   OutputPort(const std::string& name, const DataType* data) : Port<DataType, pipeline::Direction::Out>(name, data) { }
//   ~OutputPort(void)
//   {
//     while (_connected_inputs.size())
//       this->disconnect(*_connected_inputs.front());
//   }

//   bool isConnected(const InputPort<DataType>& input)
//   {
//     return std::find(_connected_inputs.begin(), _connected_inputs.end(), &input) != _connected_inputs.end();
//   }

//   bool connect(InputPort<DataType>& input)
//   {
//     if (this->isConnected(input))
//     {
//       // TODO: print error
//       return false;
//     }

//     _connected_inputs.push_back(&input);

//     if (!input.isConnected(*this))
//       return input.connect(*this);

//     return true;
//   }

//   bool disconnect(InputPort<DataType>& input)
//   {
//     if (!this->isConnected(input))
//       return true;

//     _connected_inputs.erase(std::find(_connected_inputs.begin(), _connected_inputs.end(), &input));

//     if (input.isConnected(*this))
//       return input.disconnect(*this);

//     return true;
//   }

// private:
//   std::list<InputPort<DataType>*> _connected_inputs;
// };

// template <typename DataType>
// class DataSourcePort : public OutputPort<DataType>
// {
// public:
//   DataSourcePort(const std::string& name) : OutputPort<DataType>(name, nullptr) { }
//   DataSourcePort(const std::string& name, const DataType& dataSource) : OutputPort<DataType>(name, &dataSource) { }
// };

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Port Config used for all port types
// struct PortConfig
// {
//   PortConfig(void) = delete;
//   constexpr PortConfig(const char* name_, const void* data_ = nullptr) : name(name_), data(data_) { }
  
//   const char* name = "none";
//   const void* data = nullptr;
// };

// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// // Port Block in and out
// template <std::size_t i, typename DataType, template<typename> class PortType>
// struct BlockPort
// {
// public:
//   BlockPort(const char* name, const DataType* data = nullptr) : port(name, data) { }

//   PortType<DataType> port;
// };

// template <std::size_t i, template<typename> class PortType, typename... DataTypes>
// struct BlockPortImpl;

// template <std::size_t i, template<typename> class PortType>
// struct BlockPortImpl<i, PortType>
// {
// public:
//   BlockPortImpl(const std::array<PortConfig, i>& config) { }

//   static constexpr std::size_t numInputs(void) { return i; }
// };

// template <std::size_t i, template<typename> class PortType, typename HeadDataType, typename... DataTypes>
// struct BlockPortImpl<i, PortType, HeadDataType, DataTypes...> : public BlockPortImpl<i + 1, PortType, DataTypes...>,
//                                                                 public BlockPort<i, HeadDataType, PortType>
// {
// public:
//   BlockPortImpl(const std::array<PortConfig, BlockPortImpl<i, PortType, HeadDataType, DataTypes...>::numInputs()>& config)
//     : BlockPortImpl<i + 1, PortType, DataTypes...>(config),
//       BlockPort<i, HeadDataType, PortType>(std::get<i>(config).name, reinterpret_cast<const HeadDataType*>(std::get<i>(config).data))
//   {

//   }

//   static constexpr pipeline::Direction data_flow = PortType<HeadDataType>::data_flow;
// };

// //TODO: move this function into class BlockPortImpl
// template <std::size_t i, template<typename> class PortType, typename HeadDataType, typename... DataTypes>
// auto& get(BlockPortImpl<i, PortType, HeadDataType, DataTypes...>& block) { return block.BlockPort<i, HeadDataType, PortType>::port; }

// template <typename... DataTypes>
// using InputBlock = BlockPortImpl<0, InputPort, DataTypes...>;

// template <typename... DataTypes>
// using OutputBlock = BlockPortImpl<0, OutputPort, DataTypes...>;

} // end namespace processing

} // end namespace francor
