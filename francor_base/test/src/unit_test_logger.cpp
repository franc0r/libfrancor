/**
 * Unit test for the line class.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. April 2019
 */
#include <gtest/gtest.h>

#include "francor_base/log.h"

using francor::base::LogLevel;
using francor::base::LogDebug;
using francor::base::LogInfo;
using francor::base::LogWarn;
using francor::base::LogError;
using francor::base::LogFatal;

TEST(Log, LogOnEachLevel)
{
  francor::base::setLogLevel(LogLevel::DEBUG);

  constexpr int value = 23;

  LogDebug() << value << " " << value + 1;
  LogInfo() << value;
  LogWarn() << value;
  LogError() << value;
  LogFatal() << value;
}

TEST(Log, BenchmarkLogging)
{
  francor::base::setLogLevel(LogLevel::INFO);
  const std::string log_message = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyz";
  const std::size_t num_of_executions = 100000;

  auto start = std::chrono::system_clock::now();  
  for (std::size_t i = 0; i < num_of_executions; ++i) {
    LogFatal() << log_message;
  }
  auto end = std::chrono::system_clock::now();
  auto elapsed_enabled = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  std::cout << "elapsed enabled = " << elapsed_enabled.count() << " us" << std::endl;

  start = std::chrono::system_clock::now();  
  for (std::size_t i = 0; i < num_of_executions; ++i) {
    LogDebug() << log_message;
  }
  end = std::chrono::system_clock::now();
  auto elapsed_disabled = std::chrono::duration_cast<std::chrono::microseconds>(end - start);  
  std::cout << "elapsed disabled = " << elapsed_disabled.count() << " us" << std::endl;

  EXPECT_LE(elapsed_disabled.count(), elapsed_enabled.count());
}

TEST(Log, LogName)
{
  using francor::base::LogGroup;
  using francor::base::LogLevel;
  using francor::base::Log;
  francor::base::setLogLevel(LogLevel::DEBUG); 
  static constexpr const char name[] = "LogNameTest";
  Log<LogLevel::DEBUG, LogGroup::SYSTEM, name>() << "abcdefghijklmnopqrstuvwxyz";
}

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}