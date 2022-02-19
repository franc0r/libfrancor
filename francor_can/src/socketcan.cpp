/**
 * @file socketcan_interface.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 *
 * @brief SocketCAN interface class
 *
 * @version 0.1
 * @date 2022-02-19
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include "francor_can/socketcan.h"

using namespace francor::can;

SocketCAN::SocketCAN(const std::string& if_name) : _if_name(if_name) {}