/**
 * Declares classes for memory management for memory blocks and arrays that are shared or subsets of it.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 19. December 2020
 */
#pragma once

#include "francor_base/log.h"

#include <memory>
#include <vector>

namespace francor {

namespace base {

/**
 * \brief Management class for memory that is usually shared over multiple instances.
 */
class SharedMemory
{
public:
  /**
   * \brief Constructs with new allocated memory.
   * \param size Allocates size byte.
   */
  SharedMemory(const std::size_t size = 0u)
    : _data(std::make_shared<std::vector<std::uint8_t>>(size)) { }
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
      _data = std::make_shared<std::vector<std::uint8_t>>(size);
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
  inline std::uint8_t* data() { return _data->data(); }
  inline const std::uint8_t* data() const { return _data->data(); }
  /**
   * \brief Accesses the memory at given index.
   * \param index Byte index of memory.
   * \return Value at given index.
   */
  inline std::uint8_t& operator[](const std::size_t index) { return (*_data)[index]; }
  inline const std::uint8_t& operator[](const std::size_t index) const { return (*_data)[index]; }

private:
  std::shared_ptr<std::vector<std::uint8_t>> _data;
};

} // end namespace base

} // end namespace francor