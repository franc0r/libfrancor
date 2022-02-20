/**
 * @file can_exception.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 *
 * @brief Exception definition for can module
 *
 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#pragma once

#include <sstream>
#include <string>

namespace francor {

namespace can {

/**
 * @brief CAN exception types
 */
enum ExceptionType {
    EXCEP_NONE = 0,
    EXCEP_DEVICE_ERROR,
    EXCEP_DEVICE_NOT_FOUND,
    EXCEP_DEVICE_NOT_RUNNING,
    EXCEP_DEVICE_CFG_ERROR,
    EXCEP_TX_ERROR,
    EXCEP_RX_TIMEOUT,
};

/**
 * @brief CAN exception class
 */
class can_exception : public std::exception {
   public:
    explicit can_exception(ExceptionType type, std::string if_name, std::string desc) : _type(type), _if_name(if_name) {
        std::stringstream full_desc;
        full_desc << "Exception-ID['" << getExceptionDescription(type) << "'], Interface['" << _if_name
                  << "'] Description: " << desc << std::endl;
        _desc = full_desc.str();
    }

    virtual ~can_exception() = default;

    virtual const char* what() const throw() { return _desc.c_str(); }

    virtual ExceptionType getType() const throw() { return _type; }

    std::string getExceptionDescription(const ExceptionType type) {
        std::string desc;
        switch (type) {
            case EXCEP_NONE:
                desc = "EXECP_NONE";
                break;
            case EXCEP_DEVICE_ERROR:
                desc = "EXCEP_DEVICE_ERROR";
                break;
            case EXCEP_DEVICE_NOT_FOUND:
                desc = "EXCEP_DEVICE_NOT_FOUND";
                break;
            case EXCEP_DEVICE_NOT_RUNNING:
                desc = "EXCEP_DEVICE_NOT_RUNNING";
                break;
            case EXCEP_DEVICE_CFG_ERROR:
                desc = "EXCEP_DEVICE_CFG_ERROR";
                break;
            case EXCEP_TX_ERROR:
                desc = "EXCEP_TX_ERROR";
                break;
            case EXCEP_RX_TIMEOUT:
                desc = "EXCEP_RX_TIMEOUT";
                break;
        }
        return desc;
    }

   protected:
    ExceptionType _type;
    std::string _if_name;
    std::string _desc;
};

};  // namespace can

};  // namespace francor
