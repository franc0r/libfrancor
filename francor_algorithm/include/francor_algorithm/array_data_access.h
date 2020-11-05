/**
 * Defines classes to access array data in a generic way. This can be used by container classes like a grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */
#pragma once

#include <algorithm>

namespace francor {

namespace algorithm {

template <typename DataType, std::size_t Stride = 1>
class ArrayDataIterator1D : public std::iterator<std::random_access_iterator_tag, DataType>
{
public:
  explicit ArrayDataIterator1D(DataType* data) : _data(data) { }

  ArrayDataIterator1D& operator++() { if constexpr (Stride == 0) _data++; else _data += Stride; return *this; }
  ArrayDataIterator1D& operator--() { if constexpr (Stride == 0) _data--; else _data -= Stride; return *this; }
  bool operator==(const ArrayDataIterator1D& rhs) const { return _data == rhs._data; }
  bool operator!=(const ArrayDataIterator1D& rhs) const { return !(*this == rhs); }
  DataType& operator*() const { return *_data; }

private:
  DataType* _data;
};    

template <typename DataType, std::size_t Stride, class Iterator = void>
class ArrayDataAccess1D
{
public:
  using iterator = Iterator;

  ArrayDataAccess1D(DataType* data, const std::size_t size) : _data(data), _size(size) { }

  template <std::enable_if_t<false == std::is_same<Iterator, void>::value>* = nullptr>
  iterator begin() { return iterator(_data); }
  template <std::enable_if_t<false == std::is_same<Iterator, void>::value>* = nullptr>
  iterator end() { return iterator(_data + Stride * _size); }

  template <std::enable_if_t<std::is_same<Iterator, void>::value>* = nullptr>
  ArrayDataAccess1D<DataType, Stride, ArrayDataIterator1D<DataType, Stride>>
  all_elements() { return { _data, _size }; }
  template <std::enable_if_t<std::is_same<Iterator, void>::value>* = nullptr>
  ArrayDataAccess1D<DataType, Stride * 2, ArrayDataIterator1D<DataType, Stride * 2>>
  every_second_element() { return { _data, _size / 2 }; }

private:
  DataType* _data;
  std::size_t _size;
};

} // end namespace algorithm

} // end namespace francor