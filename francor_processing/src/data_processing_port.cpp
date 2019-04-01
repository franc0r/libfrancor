#include "francor_processing/data_processing_port.h"

namespace francor
{

namespace processing
{
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
  : PortId(std::move(origin)),
    _data_flow(origin._data_flow), 
    _data(origin._data), 
    _data_type_info(origin._data_type_info)
{
  origin._data_flow = Direction::NONE;
  origin._data = nullptr;
  origin._data_type_info = typeid(void);
}

Port::~Port(void)
{

}

Port& Port::operator=(Port&& origin)
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

} // end namespace processing

} // end namespace francor