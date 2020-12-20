/**
 * Unit test for the class ParameterBase and Parameter.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 20. December 2020
 */
#include <gtest/gtest.h>

#include "francor_base/memory.h"

using francor::base::SharedMemory;

void setTestData(SharedMemory& memory)
{
  for (std::size_t i = 0; i < memory.size(); ++i) {
    memory[i] = static_cast<std::uint8_t>(i + 1u);
  }
}

TEST(SharedMemory, NewInstance)
{
  constexpr std::size_t size = 100;

  // default construction
  SharedMemory empty;

  EXPECT_EQ(0u, empty.size());

  // construct with a size so that memory is allocated
  SharedMemory normal(size);

  EXPECT_EQ(size, normal.size());
}

TEST(SharedMemory, AccessData)
{
  constexpr std::size_t size = 100;

  // construct and allocate memory and assign test data
  SharedMemory memory(size);

  ASSERT_EQ(size, memory.size());
  setTestData(memory);

  // check if data was correct assigned
  for (std::size_t i = 0; i < memory.size(); ++i) {
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), memory[i]);
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), *(memory.data() + i));
  }
}

TEST(SharedMemory, SharedContent)
{
  constexpr std::size_t size = 100;

  // construct and allocate memory and assign test data
  SharedMemory memory(size);
  
  setTestData(memory);

  // construct new object and share content with it
  // copy constructor is used
  SharedMemory shared(memory);

  // check if content is shared by accessing memory
  EXPECT_EQ(size, shared.size());
  for (std::size_t i = 0; i < shared.size(); ++i) {
    // expect values from object memory
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), shared[i]);
    // override current value and check if it is also changed in object memory
    shared[i] = size - i;
    EXPECT_EQ(size - i, memory[i]);
  }

  // copy assignment operator is used
  shared.clear();
  setTestData(memory);
  EXPECT_EQ(0u, shared.size());

  shared = memory;

  // check if content is shared by accessing memory
  EXPECT_EQ(size, shared.size());
  for (std::size_t i = 0; i < shared.size(); ++i) {
    // expect values from object memory
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), shared[i]);
    // override current value and check if it is also changed in object memory
    shared[i] = size - i;
    EXPECT_EQ(size - i, memory[i]);
  }
}

TEST(SharedMemory, CopiedContent)
{
  constexpr std::size_t size = 100;

  // construct and allocate memory and assign test data
  SharedMemory memory(size);
  
  setTestData(memory);

  // construct new object as a copy of object memory
  SharedMemory copied = memory.createCopy();

  // check if were copied correctly by accessing memory
  EXPECT_EQ(size, copied.size());
  for (std::size_t i = 0; i < copied.size(); ++i) {
    // expect values from object memory
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), copied[i]);
    // override current value and check if not reference to object memory does exist
    copied[i] = 0u;
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), memory[i]);
  }
}

TEST(SharedMemory, MovedContent)
{
  constexpr std::size_t size = 100;

  // construct and allocate memory and assign test data
  SharedMemory memory(size);

  setTestData(memory);

  // move content using move constructor
  SharedMemory moved(std::move(memory));

  EXPECT_EQ(0u, memory.size());
  EXPECT_EQ(size, moved.size());

  // check if content of object memory is moved
  for (std::size_t i = 0; i < moved.size(); ++i) {
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), moved[i]);
  }

  // move content using move assignment operator
  memory.resize(size);
  setTestData(memory);
  moved.clear();

  moved = std::move(memory);

  EXPECT_EQ(0u, memory.size());
  EXPECT_EQ(size, moved.size());

  // check if content of object memory is moved
  for (std::size_t i = 0; i < moved.size(); ++i) {
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), moved[i]);
  }
}

TEST(SharedMemory, ClearAndResize)
{
  constexpr std::size_t size = 100;

  // construct and allocate memory and assign test data
  SharedMemory memory(size);

  // check if memory is cleared
  EXPECT_EQ(size, memory.size());
  memory.clear();
  EXPECT_EQ(0u, memory.size());

  // check method resize
  constexpr std::size_t half_size = size / 2u;
  memory.resize(size);

  EXPECT_EQ(size, memory.size());

  // assign test data and reduce size to check if content is kept
  // that is the case if the memory is not shared with another instance
  setTestData(memory);
  memory.resize(half_size);

  EXPECT_EQ(half_size, memory.size());

  for (std::size_t i = 0u; i < memory.size(); ++i) {
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), memory[i]);
  }

  // check if new memory is allocate when the memory is being shared with another instance
  SharedMemory shared(memory);
  EXPECT_EQ(half_size, shared.size());

  memory.resize(size);
  EXPECT_EQ(size, memory.size());
  EXPECT_EQ(half_size, shared.size());

  // check if memory reference was removed
  for (std::size_t i = 0u; i < half_size; ++i) {
    memory[i] = 200u;
    EXPECT_EQ(static_cast<std::uint8_t>(i + 1), shared[i]);
  }
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}