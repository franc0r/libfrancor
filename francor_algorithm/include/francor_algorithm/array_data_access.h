/**
 * Defines classes to access array data in a generic way. This can be used by container classes like a grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */
#pragma once

#include <algorithm>

namespace francor {

namespace algorithm {

enum class IteratorMode {
  MOVING,
  END_INDICATOR,
};

/**
 * \brief An iterator to access a 1d array or one dimension of an 2d array.
 */
template <typename Data>
class DataAccessIterator1d : public std::iterator<std::random_access_iterator_tag, Data>
{
public:
  DataAccessIterator1d() = delete;
  DataAccessIterator1d(Data* data, const std::size_t data_step) : _data(data), _data_step(data_step) { }

  DataAccessIterator1d& operator++() { _data += _data_step; return *this; }
  DataAccessIterator1d& operator--() { _data -= _data_step; return *this; }
  bool operator==(const DataAccessIterator1d& rhs) const { return _data == rhs._data; }
  bool operator!=(const DataAccessIterator1d& rhs) const { return !(*this == rhs); }
  Data& operator*() const { return *_data; }

private:
  Data* _data;
  const std::size_t _data_step;
};    

template <typename Data, IteratorMode Mode>
class DataAccessRectangleIterator;

template <typename Data>
class DataAccessRectangleIterator<Data, IteratorMode::MOVING> : public std::iterator<std::random_access_iterator_tag, Data>
{
public:
  DataAccessRectangleIterator() = delete;
  DataAccessRectangleIterator(Data* data, const base::Size2u index, const base::Size2u rectangle_size,
                              const std::size_t max_index_x, const std::size_t stride)
    : _data(data),
      _index_local(0u, 0u),
      _rectangle_size(rectangle_size),
      _max_index_x(max_index_x),
      _stride(stride)
  {
    // check if rectangle is completly inside the array.
    if (index.x() < _rectangle_size.x() / 2) {
      _index_global.x() = 0u;
      // @todo handle member data too
    }
    else {
      _index_global.x() = index.x() - rectangle_size.x() / 2u;
      _data -= rectangle_size.x() / 2u;
    }
    if (index.y() < _rectangle_size.y() / 2u) {
      _index_global.y() = 0u;
      // @todo handle member data too
    }
    else {
      _index_global.y() = index.y() - rectangle_size.y() / 2u;
      _data -= rectangle_size.y() / 2u * _stride;
    }
  }

  DataAccessRectangleIterator& operator++()
  {
    ++_data; ++_index_local.x(); ++_index_global.x();

    if (_index_global.x() > _max_index_x || _index_local.x() >= _rectangle_size.x()) {
      _index_global.x() -= _rectangle_size.x();
      ++_index_global.y();

      _index_local.x() = 0u;
      ++_index_local.y();

      _data += _stride;
      _data -= _rectangle_size.x();
    }

    return *this;
  }
  bool operator==(const DataAccessRectangleIterator<Data, IteratorMode::END_INDICATOR>& rhs) const {
    return _data >= rhs._last_data_element
            ||
            _index_global.y() >= rhs._array_size.y()
            ||
            _index_local.y() >= _rectangle_size.y();
  }
  bool operator!=(const DataAccessRectangleIterator<Data, IteratorMode::END_INDICATOR>& rhs) const {
    return !operator==(rhs);
  }
  Data& operator*() const { return *_data; }

private:
  Data* _data;
  base::Size2u _index_global;
  base::Size2u _index_local;
  base::Size2u _rectangle_size;
  std::size_t _max_index_x;
  std::size_t _stride;
};

template <typename Data>
class DataAccessRectangleIterator<Data, IteratorMode::END_INDICATOR>
  : public std::iterator<std::random_access_iterator_tag, Data>
{
public:
  DataAccessRectangleIterator() = delete;
  DataAccessRectangleIterator(Data* last_data_element, const base::Size2u array_size)
    : _last_data_element(last_data_element),
      _array_size(array_size)
  { }

private:
  friend DataAccessRectangleIterator<Data, IteratorMode::MOVING>;

  Data* _last_data_element;
  base::Size2u _array_size;
};

// template <typename Data>
// class DataAccessEllipseIterator : public std::iterator<std::random_access_iterator_tag, Data>
// {
// public:
//   DataAccessEllipseIterator() = delete;
//   explicit DataAccessEllipseIterator(Data* data, const base::Size2u data_size,
//                                     const base::Size2d radius, const std::size_t stride)
//     : _data(data),
//       _data_size(data_size),
//       _radius(radius),
//       _index(),
//       _index_center(std::max(data_size.x(), data_size.y()) / 2u, std::max(data_size.x(), data_size.y()) / 2u),
//       _stride(stride)
//   { }                                

//   DataAccessEllipseIterator& operator++()
//   {

//   }

// private:
//   Data* _data;
//   base::Size2u _index;
//   base::Size2u _data_size;
//   base::Size2d _radius;
//   std::size_t _stride;
// };

enum class DataAccessOperationMode {
  ELEMENT,
  ELEMENT_OPERATIONS,
  LINE,
  LINE_OPERATIONS,
  RECTANGLE,
};

template <typename Data, DataAccessOperationMode Mode>
class DataAccess2dOperation;

template <typename Data>
class DataAccess2dOperation<Data, DataAccessOperationMode::LINE_OPERATIONS>
{
public:
  DataAccess2dOperation(Data* data, const std::size_t size, const std::size_t data_step)
    : _data(data), _size(size), _data_step(data_step) { }

  // after a operation was selected (Iterator != void) this becomes enabled


  DataAccess2dOperation<Data, DataAccessOperationMode::LINE>
  all_elements() const { return { _data, _size, _data_step }; }
  DataAccess2dOperation<Data, DataAccessOperationMode::LINE>
  every_second_element() const { return { _data, _size / 2u, _data_step * 2u }; }

private:
  Data* _data;
  const std::size_t _size;
  const std::size_t _data_step;
};

template <typename Data>
class DataAccess2dOperation<Data, DataAccessOperationMode::LINE>
{
public:
  DataAccess2dOperation(Data* data, const std::size_t size, const std::size_t data_step)
    : _data(data), _size(size), _data_step(data_step) { }

  DataAccessIterator1d<Data> begin() { return DataAccessIterator1d<Data>(_data, _data_step); }
  DataAccessIterator1d<Data> end() { return DataAccessIterator1d<Data>(_data + _data_step * _size, _data_step); }

private:
  Data* _data;
  const std::size_t _size;
  const std::size_t _data_step;
};

template <typename Data>
class DataAccess2dOperation<Data, DataAccessOperationMode::ELEMENT_OPERATIONS>
{
public:
  DataAccess2dOperation() = default;
  DataAccess2dOperation(Data* data, const base::Size2u index, const base::Size2u array_size, const std::size_t stride)
    : _data(data), _index(index), _array_size(array_size), _stride(stride) { }

  DataAccess2dOperation<Data, DataAccessOperationMode::RECTANGLE>
  rectangle(const base::Size2u rectangle_size) const { return { _data, _index, rectangle_size, _array_size, _stride }; }

private:
  Data* _data;
  base::Size2u _index;
  base::Size2u _array_size;
  std::size_t _stride;
};

template <typename Data>
class DataAccess2dOperation<Data, DataAccessOperationMode::RECTANGLE>
{
public:
  DataAccess2dOperation() = default;
  DataAccess2dOperation(Data* data, const base::Size2u index, const base::Size2u rectangle_size,
                        base::Size2u array_size, const std::size_t stride)
    : _data(data), _index(index), _rectangle_size(rectangle_size), _array_size(array_size), _stride(stride) { }

  DataAccessRectangleIterator<Data, IteratorMode::MOVING>
  begin() const { return { _data, _index, _rectangle_size, _array_size.x() - 1, _stride }; }
  DataAccessRectangleIterator<Data, IteratorMode::END_INDICATOR>
  end() const { return { _data + _rectangle_size.x() / 2u + _rectangle_size.y() * _stride / 2u, _array_size }; }

private:
  Data* _data;
  base::Size2u _index;
  base::Size2u _rectangle_size;
  base::Size2u _array_size;
  std::size_t _stride;
};

} // end namespace algorithm

} // end namespace francor