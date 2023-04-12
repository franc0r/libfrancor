/**
 * @file can.h
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
#include <exception>
#include <iterator>
#include <sstream>

#include "francor_can/can_exception.h"
#include "francor_can/msg.h"

namespace francor {

namespace can {

/* Typedefs */
using DurationMs = std::chrono::milliseconds;

/* Default Settings */
constexpr uint16_t DFT_RX_FILTER_ID = {0x0000};
constexpr uint16_t DFT_RX_FILTER_MASK = {0x7FFU};
constexpr DurationMs DFT_RX_TIMEOUT_MS = {DurationMs(10)};

/**
 * @brief Reception settings
 */
struct RxSettings {
    RxSettings() = default;
    RxSettings(uint16_t filter_id) : filter_id(filter_id & MSG_ID_MASK) {}
    RxSettings(uint16_t filter_id, uint16_t filter_mask)
        : filter_id(filter_id & MSG_ID_MASK), filter_mask(filter_mask & MSG_ID_MASK) {}
    RxSettings(uint16_t filter_id, uint16_t filter_mask, DurationMs timeout_ms)
        : filter_id(filter_id & MSG_ID_MASK), filter_mask(filter_mask & MSG_ID_MASK), timeout_ms(timeout_ms) {}

    uint16_t filter_id = {DFT_RX_FILTER_ID};
    uint16_t filter_mask = {DFT_RX_FILTER_MASK};
    DurationMs timeout_ms = {DFT_RX_TIMEOUT_MS};
};

/**
 * @brief CAN interface class
 */
class CAN {
   public:
    CAN() = default;

    virtual void tx(Msg tx) = 0;
    virtual Msg rx(const RxSettings settings) = 0;

    virtual bool isDeviceUp() = 0;
};

};  // namespace can

};  // namespace francor