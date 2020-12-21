/**
 * Declares classes for memory management for memory blocks and arrays that are shared or subsets of it.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 19. December 2020
 */
#pragma once

#include "francor_base/size.h"
#include "francor_base/rect.h"
#include "francor_base/log.h"

#include "francor_algorithm/array_data_access.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <ostream>
#include <iomanip>

namespace francor {

namespace algorithm {

/**
 * \brief Management class for memory that is usually shared over multiple instances.
 */
template <typename Data>
class SharedMemory
{
public:
  using MemoryHandler = std::vector<Data>;

  /**
   * \brief Constructs with new allocated memory.
   * \param size Allocates size byte.
   */
  SharedMemory(const std::size_t size = 0u, const Data& initial_value = Data())
    : _data(std::make_shared<MemoryHandler>()) { _data->resize(size, initial_value); }
  /**
   * \brief Constructs with shared memory from other class.
   * \param rhs Takes from this instance a reference to its memory.
   */
  SharedMemory(const SharedMemory& rhs)
    : _data(rhs._data) { }
  /**
   * \brief Takes over content from other class.
   * \param rhs All content of rhs will be taken over.
   */
  SharedMemory(SharedMemory&& rhs)
    : _data(rhs._data)
  {
    // clear rhs so it is not longer shared with it
    rhs.clear();
  }
  /**
   * \brief Takes a refernce to the memory of rhs.
   * \param Takes from this instance a reference to its memory.
   * \return A reference to this class.
   */
  SharedMemory& operator=(const SharedMemory& rhs)
  {
    _data = rhs._data;
    return *this;
  }
  /**
   * \brief Takes over content from rhs.
   * \param rhs All content of rhs will be taken over.
   * \return A reference to this class.
   */
  SharedMemory& operator=(SharedMemory&& rhs)
  {
    _data = rhs._data;
    // clear rhs so it is not longer shared with it
    rhs.clear();
    return *this;
  }
  /**
   * \brief Creates a new instance with copied content.
   * \return A copy of this class. The memory is copied and not shared.
   */
  SharedMemory createCopy() const
  {
    SharedMemory copy(size());
    *(copy._data) = *_data;
    return copy;
  }
  /**
   * \brief Clears this class. All references to other instances are removed.
   */
  void clear()
  {
    resize(0);
  }
  /**
   * \brief Resizes the allocated memory. If the memory is shared all references will be removed and new memory
   *        will be allocated.
   * \param size The new size of the memory.
   * \param initial_value This value is set to each data element.
   */
  void resize(const std::size_t size, const Data& initial_value = Data())
  {
    if (_data.use_count() > 1u) {
      // data is shared and in use -> allocate new memory
      _data = std::make_shared<MemoryHandler>();
      _data->resize(size, initial_value);
    }
    else if (_data.use_count() == 1u) {
      _data->resize(size, initial_value);
    }
    else {
      // error: it should minimum exists one instance
      base::LogError() << "SharedMemory::resize(): unexpected path. Please inform developer.";
    }
  }
  /**
   * \brief Returns the current size.
   * \return Current size.
   */
  inline std::size_t size() const { return _data->size(); }
  /**
   * \brief Returns the address to the first byte of the allocated memory.
   */
  inline Data* data() { return _data->data(); }
  inline const Data* data() const { return _data->data(); }

protected:
  std::shared_ptr<MemoryHandler> _data;
};

/**
 * \brief A 1d array with shared content.
 */
template <typename Data>
class SharedArray : public SharedMemory<Data>
{
public:
  using iterator = DataAccessIterator1d<Data>;

  // use constructor of base class
  using SharedMemory<Data>::SharedMemory;
  /**
   * \brief Creates a copy of this class without sharing.
   * \return A new instance with copied content.
   */
  inline SharedArray createCopy() const
  {
    SharedArray copy;
    static_cast<SharedMemory<Data>&>(copy) = SharedMemory<Data>::createCopy();
    return copy;
  }
  /**
   * \brief Accesses the memory at given index.
   * \param index Byte index of memory.
   * \return Value at given index.
   */
  inline Data& operator[](const std::size_t index) { return SharedMemory<Data>::_data->operator[](index); }
  inline const Data& operator[](const std::size_t index) const { return SharedMemory<Data>::_data->operator[](index); }
  /**
   * \brief Creates and returns an iterator pointing to the first data element.
   * \return Iterator pointing to first data element.
   */
  iterator begin() { return { SharedMemory<Data>::_data->data(), 1u }; }
  /**
   * \brief Creates and returns an iterator pointing to one after last data element.
   * \return Iterator pointing to one after last data element.
   */
  iterator end() { return { SharedMemory<Data>::_data->data() + this->size(), 1u }; }
};

template <typename Data>
class SharedArray2d : protected SharedMemory<Data>
{
public:
  // use data method of base class
  using SharedMemory<Data>::data;

  /**
   * \brief Constructs with allocating new memory and initial all data elements with initial value.
   * \param size The size of the new constructed array.
   * \param initial_value With this value all data elements are initialized.
   */
  SharedArray2d(const base::Size2u size = base::Size2u(0u, 0u), const Data initial_value = Data())
    : SharedMemory<Data>(size.x() * size.y(), initial_value),
      _size(size) { }
  /**
   * \brief Copy constructor used to share content. No data is copied.
   * \param rhs The source array object.
   */
  SharedArray2d(const SharedArray2d& rhs) = default;
  /**
   * \brief Constructs with shared memory and given ROI. This instance works only on ROI, but data is shared with source array.
   *   
   * \param rhs The source array object.
   * \param roi The ROI this instance will limited to. The given ROI must be completly inside the array otherwise no data is shared.
   */
  SharedArray2d(const SharedArray2d& rhs, const base::Rectu& roi)
    : SharedMemory<Data>(rhs),
      _size(roi.size()),
      _offset(roi.origin().y() * rhs._size.x() + roi.origin().x()),
      _stride(rhs._size.x())
  {
    // roi must be complete inside the array otherwise cancel operation and clear array
    if (roi.origin().x() + roi.size().x() >= rhs._size.x()
        ||
        roi.origin().y() + roi.size().y() >= rhs._size.y()) {
      base::LogError() << "SharedArray2d: given roi is not complete inside the array. source array = " << rhs._size
                 << ", roi = " << roi;
      clear();
    }
  }
  /**
   * \brief Constructs and takes over content from rhs object. The rhs is cleared after.
   * \param rhs Source array object.
   */
  SharedArray2d(SharedArray2d&& rhs)
    : SharedMemory<Data>(rhs), _size(rhs._size), _offset(rhs._offset), _stride(rhs._stride)
  {
    rhs.clear();
  }
  /**
   * \brief Gets a reference to source array object. No data is copied.
   */
  SharedArray2d& operator=(const SharedArray2d&) = default;
  /**
   * \brief Takes over content of source array object. After the source array object is cleared.
   * \param rhs The source array object.
   * \return A reference to this class.
   */
  SharedArray2d& operator=(SharedArray2d&& rhs)
  {
    SharedMemory<Data>::operator=(rhs);
    _size = rhs._size;
    _offset = rhs._offset;
    _stride = rhs._stride;
    rhs.clear();
    return *this;
  }
  /**
   * \brief Creates a new instance and copies the content. No data is shared.
   * \return A new object with copied content.
   */
  inline SharedArray2d createCopy() const
  {
    SharedArray2d copy;
    
    static_cast<SharedMemory<Data>&>(copy) = SharedMemory<Data>::createCopy();
    copy._size   = _size;
    copy._offset = _offset;
    copy._stride = _stride;

    return copy;
  }
  /**
   * \brief Clears the content of this class.
   */
  inline void clear()
  {
    SharedMemory<Data>::clear();
    _size = {0u, 0u};
    _offset = 0u;
    _stride = 0u;
  }
  /**
   * \brief Resizes the array to given size and initialize each data element with an initial value.
   * \param size The new size of this class.
   * \param initial_value All data elements are initialized with this value.
   */
  void resize(const base::Size2u size, const Data& initial_value = Data())
  {
    SharedMemory<Data>::resize(size.x() * size.y(), initial_value);
    _size = size;

    // set roi related members back to initial, because with resizing a new source is created
    _offset = 0u;
    _stride = _size.x();
  }
  /**
   * \brief Returns the current size of this class.
   * \return The size in x and y direction.
   */
  inline base::Size2u size() const { return _size; }
  /**
   * \brief Accesses the data element at x and y.
   * \param x The index x of the data access.
   * \param y The index y of the data access.
   * \return A reference to the addressed data element.
   */
  inline Data& operator()(const std::size_t x, const std::size_t y) {
    return (*SharedMemory<Data>::_data)[y * _stride + x + _offset];
  }
  inline const Data& operator()(const std::size_t x, const std::size_t y) const {
    return (*SharedMemory<Data>::_data)[y * _stride + x + _offset];
  }
  /**
   * \brief Creates and returns an array access operation object that represents a row of this array. Different iterator types
   *        are available to walk through the row. For example every second data element of row 2 can be accessed using this:
   * 
   *        for (auto& element : array.row(2).every_second_element())
   * 
   * \param index The index of the row.
   * \return Operation collection for accessing the row data elements.
   */
  DataAccess2dOperation<Data, DataAccessOperationMode::LINE_OPERATIONS> row(const std::size_t index) {
    return { &(*SharedMemory<Data>::_data)[index * _size.y()], _size.x(), 1u };
  }
  DataAccess2dOperation<const Data, DataAccessOperationMode::LINE_OPERATIONS> row(const std::size_t index) const {
    return { &(*SharedMemory<Data>::_data)[index * _size.y()], _size.x(), 1u };
  }
  /**
   * \brief Creates and returns an array access operation object that represents a column of this array. Different iterator types
   *        are available to walk through the row. For example every second data element of column 2 can be accessed using this:
   * 
   *        for (auto& element : array.col(2).every_second_element())
   * 
   * \param index The index of the col.
   * \return Operation collection for accessing the col data elements.
   */
  DataAccess2dOperation<Data, DataAccessOperationMode::LINE_OPERATIONS> col(const std::size_t index) {
    return { &(*SharedMemory<Data>::_data)[index], _size.y(), _size.x() };
  }
  DataAccess2dOperation<const Data, DataAccessOperationMode::LINE_OPERATIONS> col(const std::size_t index) const {
    return { &(*SharedMemory<Data>::_data)[index], _size.y(), _size.x() };
  }

  DataAccess2dOperation<Data, DataAccessOperationMode::ELEMENT_OPERATIONS>
  at(const std::size_t x, const std::size_t y) {
    return { &(*SharedMemory<Data>::_data)[y * _stride + x], base::Size2u(x, y), _size, _stride };
  }

private:
  base::Size2u _size;
  std::size_t _offset{0u};
  std::size_t _stride{_size.x()};
};

} // end namespace algorithm

} // end namespace francor

namespace std {

template <typename Data>
inline ostream& operator<<(ostream& os, const francor::algorithm::SharedArray2d<Data>& array)
{
  os << "array: [" << std::endl;
  os << "  size: " << array.size() << std::endl;
  os << "  data: [" << std::endl;

  for (std::size_t row = 0; row < array.size().y(); ++row) {
    os << "    ";
    for (const auto element : array.row(row).all_elements()) {
      os << element << " ";
    }
    os << std::endl;
  }

  os << "  ]" << std::endl;
  os << "]" << std::endl;

  return os;
}

} // end namespace std