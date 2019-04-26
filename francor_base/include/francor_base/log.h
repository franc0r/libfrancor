/**
 * Represent a line and includes helper functions.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 16. February 2019
 */
#pragma once

#include <iostream>
#include <mutex>

namespace francor
{

namespace base
{

enum class LogLevel : std::uint8_t {
  DEBUG = 0,
  INFO,
  WARNING,
  ERROR,
  FATAL
};

void setLogLevel(const LogLevel level);
LogLevel getLogLevel(void);

//TODO: make thread safe
template <LogLevel Level>
class Log
{
public:
  Log(void) = default;
  ~Log(void)
  {
    if (Level >= getLogLevel())
      std::clog << std::endl;
  }

  static constexpr LogLevel level = Level;

  template<typename T>
  const Log<Level>& operator<<(const T& in) const
  {
    // if global log level lower than of *this return without
    if (Level < getLogLevel())
      return *this;

    std::clog << in;

    return *this;
  }
};

using LogDebug = Log<LogLevel::DEBUG>;
using LogInfo = Log<LogLevel::INFO>;
using LogWarn = Log<LogLevel::WARNING>;
using LogError = Log<LogLevel::ERROR>;
using LogFatal = Log<LogLevel::FATAL>;

} // end namespace base

} // end namespace francor