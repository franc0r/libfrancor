/**
 * @file msg.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 *
 * @brief CAN message representation
 *
 * @version 0.1
 * @date 2022-02-19
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#pragma once

#include <linux/can.h>
#include <string.h>

#include <array>
#include <iterator>
#include <stdexcept>

namespace francor {

namespace can {

/* Settings */
constexpr bool MSG_EN_RANGE_CHECK_EXP = {true};  //!< Config range check exception

/* Defines */
constexpr uint16_t MSG_MAX_ID = {2047U};
constexpr uint8_t MSG_MAX_DLC = {8U};
constexpr uint16_t MSG_ID_MASK = {0x7FFU};

/* Error messages */
constexpr char MSG_EXP_ID_OUT_OF_RANGE[] = "FRANCOR_CAN_MSG_ID_OUT_OF_RANGE";    // NOLINT
constexpr char MSG_EXP_DLC_OUT_OF_RANGE[] = "FRANCOR_CAN_MSG_DLC_OUT_OF_RANGE";  // NOLINT

/* Typedefs */
using MsgData = std::array<uint8_t, MSG_MAX_DLC>;

/**
 * @brief CAN message representation class
 */
class Msg {
   public:
    Msg() = default;
    explicit Msg(uint16_t id, uint8_t dlc) : _id(id), _dlc(dlc), _data({0}) { checkHeader(); }
    explicit Msg(uint16_t id, uint8_t dlc, MsgData data) : _id(id), _dlc(dlc), _data(data) { checkHeader(); }

    explicit Msg(can_frame frame) : _id(frame.can_id), _dlc(frame.can_dlc) {
        memcpy(_data.data(), &frame.data[0U], sizeof(frame.data));
        checkHeader();
    }

    [[nodiscard]] auto getId() const { return _id; }
    [[nodiscard]] auto getDlc() const { return _dlc; }
    [[nodiscard]] auto getData() const { return _data; }

    [[nodiscard]] auto getCCANFrame() const {
        struct can_frame frame = {};
        frame.can_id = _id;
        frame.can_dlc = _dlc;
        memcpy(&frame.data[0U], _data.data(), sizeof(frame.data));
        return frame;
    }

   private:
    void checkHeader() const {
        if (MSG_EN_RANGE_CHECK_EXP) {
            if (_id > MSG_MAX_ID) {
                throw std::invalid_argument(static_cast<const char*>(MSG_EXP_ID_OUT_OF_RANGE));
            }
            if (_dlc > MSG_MAX_DLC) {
                throw std::invalid_argument(static_cast<const char*>(MSG_EXP_DLC_OUT_OF_RANGE));
            }
        }
    }

    uint16_t _id = {0U};
    uint8_t _dlc = {0U};
    MsgData _data = {0U};
};

};  // namespace can

};  // namespace francor