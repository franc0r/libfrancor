/**
 * Declares classes for memory management for memory blocks and arrays that are shared or subsets of it.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 19. December 2020
 */
#pragma once

#include "francor_base/size.h"
#include "francor_base/log.h"

#include <memory>
#include <vector>
#include <algorithm>

namespace francor {

namespace base {

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
  SharedMemory(const std::size_t size = 0u)
    : _data(std::make_shared<MemoryHandler>(size)) { }
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
  template <class Target>
  Target createCopy() const
  {
    Target copy(size());
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
      LogError() << "SharedMemory::resize(): unexpected path. Please inform developer.";
    }
  }
  /**
   * \brief Returns the size of the allocated memory in bytes.
   * \return Size of allocated memory in bytes.
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

template <typename Data>
class SharedArray : public SharedMemory<Data>
{
public:
  using SharedMemory<Data>::SharedMemory;
  inline SharedArray createCopy() const { return SharedMemory<Data>::template createCopy<SharedArray>(); }
  /**
   * \brief Accesses the memory at given index.
   * \param index Byte index of memory.
   * \return Value at given index.
   */
  inline Data& operator[](const std::size_t index) { return (*SharedMemory<Data>::_data)[index]; }
  inline const Data& operator[](const std::size_t index) const { return (*SharedMemory<Data>::_data)[index]; }
};

template <typename Data>
class SharedArray2d : public SharedMemory<Data>
{
public:
  class iterator : public std::iterator<std::random_access_iterator_tag, Data>
  {
  public:
    explicit iterator(Data* data) : _data(data) { }

    iterator& operator++() { ++_data; return *this; }
    iterator& operator--() { --_data; return *this; }
    bool operator==(const iterator& rhs) const { return _data == rhs._data; }
    bool operator!=(const iterator& rhs) const { return !(*this == rhs); }
    Data& operator*() const { return *_data; }

  private:
    Data* _data;
  };   

  SharedArray2d(const Size2u size, const Data initial_value = Data())
    : SharedMemory<Data>(size.x() * size.y()),
      _size(size)
  {
    
  }

  void resize(const Size2u size)
  {
    SharedMemory<Data>::resize(size.x() * size.y());
    _size = size;
  }
  inline Size2u size() const { return _size; }

  iterator begin() { return { SharedMemory<Data>::_data }; }
  iterator end() { return { SharedMemory<Data>::_data + SharedMemory<Data>::size() }; }

private:
  Size2u _size;
};

} // end namespace base

} // end namespace francor