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
LogLevel getLogLevel();

enum class LogGroup : std::uint8_t {
  FUNCTION = 0,
  ALGORITHM,
  COMPONENT,
  SUBSYSTEM,
  SYSTEM
};

void setLogGroup(const LogGroup group);
LogGroup getLogGroup();

//TODO: make thread safe
template <LogLevel Level, LogGroup Group = LogGroup::SYSTEM, const char* Name = nullptr>
class Log
{
public:
  Log()
  {
    if (Level >= getLogLevel() && Group >= getLogGroup()) {
      this->printWarnLevel(Level);

      if constexpr (Name != nullptr) {
        this->printName(Name);
      }
    }
  }
  Log(const Log&) = delete;
  Log(Log&&) = delete;
  ~Log()
  {
    if (Level >= getLogLevel() && Group >= getLogGroup())
      std::clog << std::endl;
  }

  Log& operator=(const Log&) = delete;
  Log& operator=(Log&&) = delete;

  template<typename T>
  const Log& operator<<(const T& in) const
  {
    // if global log level lower than of *this return without
    if (Level < getLogLevel())
      return *this;

    // if the group is not enabled do nothing and return
    if (Group < getLogGroup())
      return *this;

    std::clog << in;

    return *this;
  }

private:
  static void printWarnLevel(const LogLevel level)
  {
    if constexpr (Level == LogLevel::DEBUG) {
      std::clog << "[DEBUG] ";
    }
    else if constexpr (Level == LogLevel::INFO) {
      std::clog << "[INFO] ";
    }
    else if constexpr (Level == LogLevel::WARNING) {
      std::clog << "[WARNING] ";
    }
    else if constexpr (Level == LogLevel::ERROR) {
      std::clog << "[ERROR] ";
    }
    else if constexpr (Level == LogLevel::FATAL) {
      std::clog << "[FATAL] ";
    }
    else {
      std::clog << "[UNKOWN] ";
    }
  }
  static void printName(const char* name)
  {
    std::clog << "[" << name << "] ";
  }
};


using LogDebug = Log<LogLevel::DEBUG>;
using LogInfo  = Log<LogLevel::INFO>;
using LogWarn  = Log<LogLevel::WARNING>;
using LogError = Log<LogLevel::ERROR>;
using LogFatal = Log<LogLevel::FATAL>;

} // end namespace base

} // end namespace francor