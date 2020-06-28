/**
 * Defines custom execeptions that are used in this project.
 *
 * \author Christian Merkl (knueppl@gmx.de)
 * \date 26. June 2020
 */
#pragma once

#include <stdexcept>

namespace francor {

namespace base {

class DataProcessingError : public std::runtime_error
{
public:
  DataProcessingError(const char* error_message)
    : std::runtime_error(error_message),
      _error_message(error_message)
  { }

  const char* what() const noexcept override { return _error_message; }

private:
  const char* _error_message;
};



} // end namespace base

} // end namespace francor