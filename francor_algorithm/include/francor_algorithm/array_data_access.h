/**
 * Defines classes to access array data in a generic way. This can be used by container classes like a grid.
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 2. November 2019
 */
#pragma once

#include <francor_base/angle.h>

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
    // @todo it smells here
    if (index.x() < _rectangle_size.x() / 2u) {
      const std::size_t difference = _rectangle_size.x() / 2u - index.x();
      _index_global.x() = 0u;
      _index_local.x() = difference;
      _data -= index.x();
    }
    else {
      _index_global.x() = index.x() - rectangle_size.x() / 2u;
      _data -= rectangle_size.x() / 2u;
    }
    if (index.y() < _rectangle_size.y() / 2u) {
      const std::size_t difference = _rectangle_size.y() / 2u - index.y();
      _index_global.y() = 0u;
      _index_local.y() = difference;
      _data -= index.y() * _stride;
    }
    else {
      _index_global.y() = index.y() - rectangle_size.y() / 2u;
      _data -= rectangle_size.y() / 2u * _stride;
    }
  }

  DataAccessRectangleIterator& operator++()
  {
    ++_data; ++_index_local.x(); ++_index_global.x();

    // detect if the row must be increamented
    if (_index_global.x() > _max_index_x || _index_local.x() >= _rectangle_size.x()) {
      // calculate new x index in new row
      const std::size_t steps_back = _rectangle_size.x() - (_rectangle_size.x() - std::min(_index_local.x(), _index_global.x()));
      _index_global.x() -= steps_back;
      _data -= steps_back;
      _index_local.x() -= steps_back;

      ++_index_global.y();
      ++_index_local.y();
      _data += _stride;
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
  inline base::Size2u arrayIndex() const { return _index_global; }
  inline base::Size2u localIndex() const { return _index_local; }
  inline const base::Size2u& rectangleSize() const { return _rectangle_size; }

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

template <typename Data, IteratorMode Mode>
class DataAccessCircleIterator;

template <typename Data>
class DataAccessCircleIterator<Data, IteratorMode::MOVING>
  : public DataAccessRectangleIterator<Data, IteratorMode::MOVING>
{
public:
  DataAccessCircleIterator(Data* data, const base::Size2u index, const std::size_t radius,
                           const std::size_t max_index_x, const std::size_t stride)
    : DataAccessRectangleIterator<Data, IteratorMode::MOVING>(data, index, {radius * 2u, radius * 2u}, max_index_x, stride),
      _radius_squared(static_cast<int>(radius * radius)),
      _index_center(radius, radius)
  {
    operator++();
  }

  DataAccessCircleIterator& operator++()
  {
    DataAccessRectangleIterator<Data, IteratorMode::MOVING>::operator++();
    const float dx = static_cast<float>(DataAccessRectangleIterator<Data, IteratorMode::MOVING>::localIndex().x()) - _index_center.x() + 0.5f;
    const float dy = static_cast<float>(DataAccessRectangleIterator<Data, IteratorMode::MOVING>::localIndex().y()) - _index_center.y() + 0.5f;
    const auto distance_squared = dx * dx + dy * dy;

    // estimate if current index is inside circle
    if (distance_squared <= _radius_squared) {
      // index is inside
      return *this;
    }

    // recursive call until a valid index was found; at least the center point should be found
    // or the maximum y index was reached
    // this I don't like and actually the iterator should be desgined to go exactly to the next element
    if (DataAccessRectangleIterator<Data, IteratorMode::MOVING>::localIndex().y()
        >=
        DataAccessRectangleIterator<Data, IteratorMode::MOVING>::rectangleSize().y()) {
      return *this;
    }
    else {
      return this->operator++();
    }
  }

private:
  float _radius_squared;
  base::Size2u _index_center;
};

template <typename Data, IteratorMode Mode>
class DataAccessEllipseIterator;

template <typename Data>
class DataAccessEllipseIterator<Data, IteratorMode::MOVING> : public DataAccessRectangleIterator<Data, IteratorMode::MOVING>
{
public:
  DataAccessEllipseIterator(Data* data, const base::Size2u index, const base::Size2f radius, const base::Angle phi,
                            const std::size_t max_index_x, const std::size_t stride)
    : DataAccessRectangleIterator<Data, IteratorMode::MOVING>(data, index, {static_cast<std::size_t>(std::max(radius.x(), radius.y()) + 0.5f) * 2u,
                                                                            static_cast<std::size_t>(std::max(radius.x(), radius.y()) + 0.5f) * 2u}, max_index_x, stride),
      _sin_phi(std::sin(-phi)),
      _cos_phi(std::cos(-phi)),
      _inv_a_2(1.0f / (radius.x() * radius.x())),
      _inv_b_2(1.0f / (radius.y() * radius.y())),
      _center(std::max(radius.x(), radius.y()) )
  {
    operator++();
  }                                

  DataAccessEllipseIterator& operator++()
  {
    DataAccessRectangleIterator<Data, IteratorMode::MOVING>::operator++();
    const auto p_x = static_cast<float>(DataAccessRectangleIterator<Data, IteratorMode::MOVING>::localIndex().x()) + 0.5f;
    const auto p_y = static_cast<float>(DataAccessRectangleIterator<Data, IteratorMode::MOVING>::localIndex().y()) + 0.5f;

    _dx = p_x - _center;
    _dy = p_y - _center;

    const auto dx_transformed = _cos_phi * _dx - _sin_phi * _dy;
    const auto dy_transformed = _sin_phi * _dx + _cos_phi * _dy;

    // estimate if current index is inside ellipse
    if ((dx_transformed * dx_transformed) * _inv_a_2 + (dy_transformed * dy_transformed) * _inv_b_2 < 1.0f) {
      // index is inside
      return *this;      
    }

    // recursive call until a valid index was found; at least the center point should be found
    // or the maximum y index was reached
    // this I don't like and actually the iterator should be desgined to go exactly to the next element
    if (DataAccessRectangleIterator<Data, IteratorMode::MOVING>::localIndex().y()
        >=
        DataAccessRectangleIterator<Data, IteratorMode::MOVING>::rectangleSize().y()) {
      return *this;
    }
    else {
      return this->operator++();
    }
  }

  inline float dx() const { return _dx; }
  inline float dy() const { return _dy; }

private:
  float _sin_phi;
  float _cos_phi;
  float _inv_a_2;
  float _inv_b_2;
  float _center;
  float _dx;
  float _dy;
};

enum class DataAccessOperationMode {
  ELEMENT,
  ELEMENT_OPERATIONS,
  LINE,
  LINE_OPERATIONS,
  RECTANGLE,
  CIRCLE,
  ELLIPSE,
};

template <typename Data, DataAccessOperationMode Mode>
class DataAccess2dOperation;

template <typename Data>
class DataAccess2dOperation<Data, DataAccessOperationMode::LINE_OPERATIONS>
{
public:
  DataAccess2dOperation(Data* data, const std::size_t size, const std::size_t data_step)
    : _data(data), _size(size), _data_step(data_step) { }

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
  DataAccess2dOperation<Data, DataAccessOperationMode::CIRCLE>
  circle(const std::size_t radius) const { return { _data, _index, radius, _array_size, _stride }; }
  DataAccess2dOperation<Data, DataAccessOperationMode::ELLIPSE>
  ellipse(const base::Size2f radius, const base::Angle phi) const { return { _data, _index, radius, phi, _array_size, _stride }; }

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

template <typename Data>
class DataAccess2dOperation<Data, DataAccessOperationMode::CIRCLE>
{
public:
  DataAccess2dOperation() = default;
  DataAccess2dOperation(Data* data, const base::Size2u index, const std::size_t radius,
                        const base::Size2u array_size, const std::size_t stride)
    : _data(data), _index(index), _radius(radius), _array_size(array_size), _stride(stride) { }

  DataAccessCircleIterator<Data, IteratorMode::MOVING>
  begin() const { return { _data, _index, _radius, _array_size.x() - 1u, _stride }; }
  DataAccessRectangleIterator<Data, IteratorMode::END_INDICATOR>
  end() const { return { _data + _radius + _radius * _stride, _array_size }; }

private:
  Data* _data;
  base::Size2u _index;
  std::size_t _radius;
  base::Size2u _array_size;
  std::size_t _stride;
};

template <typename Data>
class DataAccess2dOperation<Data, DataAccessOperationMode::ELLIPSE>
{
public:
  DataAccess2dOperation() = default;
  DataAccess2dOperation(Data* data, const base::Size2u index, const base::Size2f radius, const base::Angle phi,
                        const base::Size2u array_size, const std::size_t stride)
    : _data(data), _index(index), _radius(radius), _phi(phi), _array_size(array_size), _stride(stride) { }

  DataAccessEllipseIterator<Data, IteratorMode::MOVING> begin() const {
    return { _data, _index, _radius, _phi, _array_size.x() - 1u, _stride };
  }
  DataAccessRectangleIterator<Data, IteratorMode::END_INDICATOR> end() const {
    const std::size_t max_radius = static_cast<std::size_t>(std::max(_radius.x(), _radius.y()) + 0.5f);
    return { _data + max_radius + max_radius * _stride, _array_size };
  }

private:
  Data* _data;
  base::Size2u _index;
  base::Size2f _radius;
  base::Angle _phi;
  base::Size2u _array_size;
  std::size_t _stride;
};

} // end namespace algorithm

} // end namespace francor