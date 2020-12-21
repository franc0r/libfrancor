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
   */
  void resize(const std::size_t size)
  {
    if (_data.use_count() > 1u) {
      // data is shared and in use -> allocate new memory
      _data = std::make_shared<MemoryHandler>(size);
    }
    else if (_data.use_count() == 1u) {
      _data->resize(size);
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

};

template <typename Data>
class SharedArray2d : public SharedMemory<Data>
{
public:


  SharedArray2d(const base::Size2u size = base::Size2u(0u, 0u), const Data initial_value = Data())
    : SharedMemory<Data>(size.x() * size.y(), initial_value),
      _size(size) { }
  SharedArray2d(const SharedArray2d& rhs) = default;
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
  SharedArray2d(SharedArray2d&& rhs)
    : SharedMemory<Data>(rhs), _size(rhs._size), _offset(rhs._offset), _stride(rhs._stride)
  {
    rhs.clear();
  }
  SharedArray2d& operator=(const SharedArray2d&) = default;
  SharedArray2d& operator=(SharedArray2d&& rhs)
  {
    SharedMemory<Data>::operator=(rhs);
    _size = rhs._size;
    _offset = rhs._offset;
    _stride = rhs._stride;
    rhs.clear();
    return *this;
  }
  inline SharedArray2d createCopy() const
  {
    SharedArray2d copy = std::move(SharedMemory<Data>::createCopy());

    copy._size   = _size;
    copy._offset = _offset;
    copy._stride = _stride;

    return copy;
  }
  inline void clear()
  {
    SharedMemory<Data>::clear();
    _size = {0u, 0u};
    _offset = 0u;
    _stride = 0u;
  }
  void resize(const base::Size2u size)
  {
    SharedMemory<Data>::resize(size.x() * size.y());
    _size = size;

    // set roi related members back to initial, because with resizing a new source is created
    _offset = 0u;
    _stride = _size.x();
  }
  inline base::Size2u size() const { return _size; }
  inline Data& operator()(const std::size_t x, const std::size_t y)
  {
    return (*SharedMemory<Data>::_data)[y * _stride + x + _offset];
  }
  inline const Data& operator()(const std::size_t x, const std::size_t y) const
  {
    return (*SharedMemory<Data>::_data)[y * _stride + x + _offset];
  }
  // iterator begin() { return { SharedMemory<Data>::_data->data() + _offset }; }
  // iterator end() { return { SharedMemory<Data>::_data->data() + _size.y() * _stride + _size.x() }; }

private:
  // constructor needed for create copy. this smells a bit
  SharedArray2d(SharedMemory<Data>&& rhs) : SharedMemory<Data>(rhs) { }

  base::Size2u _size;
  std::size_t _offset{0u};
  std::size_t _stride{_size.x()};
};

} // end namespace algorithm

} // end namespace francor