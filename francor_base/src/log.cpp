#include "francor_base/log.h"

namespace francor
{

namespace base
{

// global log level default set to level error
static LogLevel _log_level = LogLevel::ERROR;
static LogGroup _log_group = LogGroup::SYSTEM;

void setLogLevel(const LogLevel level)
{
  _log_level = level;
}

LogLevel getLogLevel()
{
  return _log_level;
}

void setLogGroup(const LogGroup group)
{
  _log_group = group;
}

LogGroup getLogGroup()
{
  return _log_group;
}

} // end namespace base

} // end namespace francor