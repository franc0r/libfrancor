/**
 * @file interface.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 *
 * @brief Main class of can interface
 *
 * @version 0.1
 * @date 2022-02-19
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#pragma once

#include <array>
#include <chrono>
#include <iterator>

#include "francor_can/msg.h"

namespace francor {

namespace can {

using DurationMs = std::chrono::milliseconds;

class CAN {
   public:
    CAN() = default;

    virtual void tx(Msg& tx) = 0;
    virtual void rx(Msg& rx) = 0;
};

};  // namespace can

};  // namespace francor