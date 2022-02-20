/**
 * @file rmd_x8_drive.cpp
 * @author Martin Bauernschmitt

 * @brief Interface for My Actuator RMD-X8 Pro Drive (can be used for other drives too)

 * See webpage of manufacturer for details: http://www.gyems.cn/support/download

 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include <francor_drive/rmd_x8_drive.h>

using namespace francor::can;
using namespace francor::drive;

/* Defines */
static constexpr uint16_t RMD_X8_BASE_ID = {0x141U};

/* Enumerations */
enum CANCommandList : uint8_t {
    CAN_CMD_READ_MOTOR_STS_1 = 0x9A,
    CAN_CMD_CLEAR_ERRORS = 0x9B,
    CAN_CMD_READ_MOTOR_STS_2 = 0x9C,
    CAN_CMD_DISABLE_DRIVE = 0x80,
    CAN_CMD_SET_SPEED = 0xA2,
};

/* Member functions */
RMDX8Drive::RMDX8Drive(const unsigned int id, std::shared_ptr<CAN>& can_if)
    : _can_id(id + RMD_X8_BASE_ID), _can_if(can_if) {}

void RMDX8Drive::reset() {}

void RMDX8Drive::enable() {}

void RMDX8Drive::disable() {}

void RMDX8Drive::setSpeedRPM(double speed_rpm) {}

double RMDX8Drive::getCurrentSpeedRPM() { return 0.0; }

bool RMDX8Drive::isConnected() {
    Msg sts_req = Msg(_can_id, CAN_MAX_DLC, MsgData({CAN_CMD_READ_MOTOR_STS_1}));
    _can_if->tx(sts_req);
    const auto sts_resp = _can_if->rx(RxSettings(_can_id, MSG_ID_MASK));

    return (sts_resp.getData().at(0) == CAN_CMD_READ_MOTOR_STS_1);
}