/**
 * Data processing ports provide a data connection between data processing stages. This class handles a dynamic data flow
 * with type safety checks.
 * 
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 1. April 2019
 */
#pragma once

#include <string>
#include <typeinfo>
#include <functional>
#include <memory>
#include <array>

#include <francor_base/log.h>

namespace francor
{

namespace processing
{

namespace data
{

/**
 * \brief Defines a port entity class. Attributes of this class are name and a unique id.
 */
class PortId
{
protected:
  /**
   * \brief Default constructor creates an non valid entity with name "none" and id = 0.
   */
  PortId(void) = default;
  /**
   * \brief Regular constructor that should be used. Constructs a entity with "name" and
   *        a unique id.
   * 
   * \param name The name of this entity.
   */
  PortId(const std::string& name_) : _name(name_), _id(_id_counter++) { }
  // deleted copy constructor. Only allow move to secure an unique entity.
  PortId(const PortId&) = delete;
  /**
   * \brief Move constructor. The origin class will be clean up. It will become a non valid entity.
   */
  PortId(PortId&& origin);
  // defaulted destructor
  ~PortId(void) = default;

  // deleted copy assignemt operator
  PortId& operator=(const PortId&) = delete;
  /**
   * \brief Movement assign operator. Moves all attribues to this class. Origin will be clean up
   *        and will be become a non valid entity.
   */
  PortId& operator=(PortId&& origin);

public:
  /**
   * \brief Returns the name of this entity. The name can be exists multiple time. The id is unique.
   * 
   * \return The name of this entity.
   */
  inline const std::string& name(void) const noexcept { return _name; }
  /**
   * \brief Returns the id of this entity. The id is unique.
   * 
   * \return The unique id of this entity.
   */
  inline std::size_t id(void) const noexcept { return _id; }

private:
  std::string _name = "none";
  std::size_t _id = 0;
  static std::size_t _id_counter;
};


/**
 * \brief The port class is a data port for the data processing pipeline. It provides a safety data access
 *        through type checks and data flow checks. The data access is handled by a raw pointer pointing to
 *        the data. Only a output can get an external data source. The inputs can connect to an ouput of the
 *        equal data type. It is a entity with a non unique name and a unique id.
 */
class Port : public PortId
{
public:

  /**
   * \brief Data flow directions.
   */
  enum class Direction {
    IN,
    OUT,
    NONE
  };

  /**
   * \brief Default constructor that creates an port without an data flow direction and data type.
   */
  Port(void)
  {
    this->initializeConnections();
  }
  /**
   * \brief Constructs an valid port with an data flow and name, valid for the whole lifetime.
   *        If this object is an input the data pointer should be initialized with nullptr. The
   *        data type is choosen by the parsed data object. So in case of an input it can be done
   *        by "static_cast<DataType*>(nullptr)". An output must be initialized with an valid
   *        data pointer that is valid the whole lifetime of this port.
   * 
   * \param name The name of this port.
   * \param dataFlow The data flow direction of this port.
   * \param data Pointer to the data that can be accessed using this port. The DataType will be deducted
   *        from the type of the pointer. In case of an input please use "static_cast<DataType*>(nulltpr)."
   */
  template <typename DataType>
  Port(const std::string& name_, const Direction dataFlow, DataType const* const data = nullptr)
    : PortId(name_),
      _data_flow(dataFlow),
      _data(data),
      _data_type_info(typeid(DataType))
  {
    this->initializeConnections();
  }
  // deleted copy constructor, it is only permitted to move this class
  Port(const Port&) = delete;
  /**
   * \brief Move origin into this object. All attributes and connections are moved. The origin object will be reset.
   *        The original data type will be reset, too.
   * 
   * \param origin The object that will be moved.
   */
  Port(Port&& origin);
  /**
   * \brief Virtual destructor. Disconnect all established connections.
   */
  virtual ~Port(void);
  // deleted copy assignemt operator, it is only permitted to move this class
  Port& operator=(const Port&) = delete;
  /**
   * \brief Move origin into this object. All attributes and connections are moved. The origin object will be reset.
   *        The original data type will be reset, too.
   * 
   * \param origin The object that will be moved.
   */
  Port& operator=(Port&& origin);
  /**
   * \brief Access the data this port is representing. If this port isn't pointer to valid data a expection is thrown.
   *        Also if the data type doesn't match.
   */
  template <typename DataType>
  const DataType& data(void) const
  {
    using base::LogError;

    if (_data == nullptr) {
      LogError() << "Port::data(): data pointer is null";
      throw std::runtime_error("Port::data(): data pointer is null");
    }
    if (_data_type_info.get() != typeid(DataType)) {
      LogError() << "Port::data(): type " << typeid(DataType).name() << " isn't supported.";
      throw std::runtime_error(std::string("Port::data(): type ") + typeid(DataType).name() + " isn't supported.");
    }

    return *static_cast<DataType const*>(_data);
  }
  /**
   * \brief Returns the data flow direction of this class.
   * 
   * \return the data flow direction of this class.
   */
  inline Direction dataFlow(void) const noexcept { return _data_flow; }
  /**
   * \brief Returns the data type of this class.
   * 
   * \return the data type of this class.
   */
  inline const std::type_info& type(void) const { return _data_type_info.get(); }
  /**
   * \brief Connects this port to the given port. Follwing must be fulfilled for a successfull connection:
   *          * this->dataFlow() != port.dataFlow()
   *          * this->dataFlow() != Direction::NONE
   *          * port.dataFlow()  != Direction::NONE
   *          * this->dataType() == port.dataType()
   *          * this->numOfConnections() < maxNumOfConnections()
   *          * !this->isConnectedWith(port)
   * 
   *  If this is of an input the maximum allowed number of connnections is one. An output can be connected with more the
   *  one inputs.
   * 
   * \param port The port this shall connect to.
   * \return true if the connection is established.
   */
  bool connect(Port& port);
  /**
   * \brief Disconnect from port. If this is an input the data pointer isn't valid after port is disconnected.
   * 
   * \param port This port will be disconnected.
   * \return true is port is successfully disconnected. The port is not connected with this false will be returned.
   */
  bool disconnect(Port& port);
  /**
   * \brief Checks if port is connected with this.
   * 
   * \return true if port and this is connected.
   */
  bool isConnectedWith(const Port& port) const;
  /**
   * \brief Resets this port. The data type and data flow isn't reset. Only this is an input the data pointer will be reset.
   *        All established connections will be disconnected.
   */
  void reset(void);
  /**
   * Returns the number of established connections.
   * 
   * \return number of established connecttions.
   */
  std::size_t numOfConnections(void) const;
  /**
   * Returns the maximum number of connections for each output port.
   * 
   * \return The maximum number of connections for each output port.
   */
  static constexpr std::size_t maxNumOfConnections(void) { return MAX_CONNECTIONS; }

protected:
  template <typename DataType>
  void updateDataPtrOfConnections(DataType const* const data)
  {
    using francor::base::LogError;
    if (_data_type_info.get() != typeid(DataType)) {
      LogError() << "Port: type " << typeid(DataType).name() << " isn't supported.";
      throw std::string("Port: type ") + typeid(DataType).name() + " isn't supported.";
    }

    // only update data pointer if this port is an output
    if (_data_flow == Direction::OUT)
    {
      for (auto& connection : _connections)
        if (connection != nullptr)
          connection->_data = data;
    }
    // else
    // do nothing
  }

private:
  void initializeConnections(void);
  std::size_t nextConnectionIndex(void) const;

  static constexpr std::size_t MAX_CONNECTIONS = 10;

  Direction _data_flow = Direction::NONE;
  void const* _data = nullptr;
  std::reference_wrapper<const std::type_info> _data_type_info = typeid(void);
  std::array<Port*, MAX_CONNECTIONS> _connections;
};


// special port classes
class InputPort : public Port
{
public:
  template <typename DataType>
  static InputPort create(const std::string& name)
  {
    return { name, static_cast<DataType*>(nullptr) };
  }

  InputPort() = default;

protected:
  template <typename DataType>
  InputPort(const std::string& name, DataType const* const data) : Port(name, Direction::IN, data) { }
};

class OutputPort : public Port
{
public:
  template <typename DataType>
  static OutputPort create(const std::string& name, DataType const* const data)
  {
    return { name, data };
  }

  OutputPort() = default;

protected:
  template <typename DataType>
  OutputPort(const std::string& name, DataType const* const data) : Port(name, Direction::OUT, data) { }
};

class SourcePort : public OutputPort
{
public:
  template <typename DataType>
  static SourcePort create(const std::string& name, DataType const* const data = nullptr)
  {
    return { name, data };
  }

  SourcePort() = default;

  template <typename DataType>
  void assign(DataType const* const data)
  {
    this->updateDataPtrOfConnections(data);
  }

private:
  template <typename DataType>
  SourcePort(const std::string& name, DataType const* const data) : OutputPort(name, data) { }
};

class DestinationPort : public InputPort
{
public:
  template <typename DataType>
  static DestinationPort create(const std::string& name, DataType const* const data = nullptr)
  {
    return { name, data };
  }

  DestinationPort() = default;

private:
  template <typename DataType>
  DestinationPort(const std::string& name, DataType const* const data) : InputPort(name, data) { }
};


} // end namespace data

} // end namespace processing

} // end namespace francor
