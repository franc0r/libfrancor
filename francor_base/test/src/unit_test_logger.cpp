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

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}