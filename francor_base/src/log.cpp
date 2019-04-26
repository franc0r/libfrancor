#include "francor_base/log.h"

namespace francor
{

namespace base
{

// global log level default set to level error
static LogLevel _log_level = LogLevel::ERROR;

void setLogLevel(const LogLevel level)
{
  _log_level = level;
}

LogLevel getLogLevel(void)
{
  return _log_level;
}

} // end namespace base

} // end namespace francor