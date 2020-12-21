/**
 * Defines classes to access array data in a generic way. This can be used by container classes like a grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */
#pragma once

#include <algorithm>

namespace francor {

namespace algorithm {

/**
 * \brief An iterator to access a 1d array or one dimension of an 2d array.
 */
template <typename Data>
class DataAccessIterator1d : public std::iterator<std::random_access_iterator_tag, Data>
{
public:
  DataAccessIterator1d() = delete;
  explicit DataAccessIterator1d(Data* data, const std::size_t data_step) : _data(data), _data_step(data_step) { }

  DataAccessIterator1d& operator++() { _data += _data_step; return *this; }
  DataAccessIterator1d& operator--() { _data -= _data_step; return *this; }
  bool operator==(const DataAccessIterator1d& rhs) const { return _data == rhs._data; }
  bool operator!=(const DataAccessIterator1d& rhs) const { return !(*this == rhs); }
  Data& operator*() const { return *_data; }

private:
  Data* _data;
  const std::size_t _data_step;
};    


template <typename Data, class Iterator = void>
class DataAccess2dOperation
{
public:
  using iterator = Iterator;

  DataAccess2dOperation(Data* data, const std::size_t size, const std::size_t data_step)
    : _data(data), _size(size), _data_step(data_step) { }

  // after a operation was selected (Iterator != void) this becomes enabled
  template <std::enable_if_t<false == std::is_same<Iterator, void>::value>* = nullptr>
  iterator begin() { return iterator(_data, _data_step); }
  template <std::enable_if_t<false == std::is_same<Iterator, void>::value>* = nullptr>
  iterator end() { return iterator(_data + _data_step * _size, _data_step); }

  // if this class is constructed without an iterator (Iterator == void) this operations are available
  template <std::enable_if_t<std::is_same<Iterator, void>::value>* = nullptr>
  DataAccess2dOperation<Data, DataAccessIterator1d<Data>>
  all_elements() { return { _data, _size, _data_step }; }
  template <std::enable_if_t<std::is_same<Iterator, void>::value>* = nullptr>
  DataAccess2dOperation<Data, DataAccessIterator1d<Data>>
  every_second_element() { return { _data, _size / 2u, _data_step * 2u }; }

private:
  Data* _data;
  const std::size_t _size;
  const std::size_t _data_step;
};

} // end namespace algorithm

} // end namespace francor