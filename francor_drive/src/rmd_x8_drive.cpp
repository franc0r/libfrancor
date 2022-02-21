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
static constexpr float RMD_RPM_TO_DPS_FAC = {3600.0F / 60.0F};
static constexpr float RMD_TQ_RAW_TO_NM = {(1.5F / 4.9F) * (33.0F / 2048.0F)};

static constexpr ssize_t RMD_BYTE_POS_TEMP = {1U};
static constexpr ssize_t RMD_BYTE_POS_TQ_H = {3U};
static constexpr ssize_t RMD_BYTE_POS_TQ_L = {2U};

/* Enumerations */
enum CANCommandList : uint8_t {
    CAN_CMD_READ_MOTOR_STS_1 = 0x9A,
    CAN_CMD_CLEAR_ERRORS = 0x9B,
    CAN_CMD_READ_MOTOR_STS_2 = 0x9C,
    CAN_CMD_DISABLE_DRIVE = 0x80,
    CAN_CMD_SET_SPEED = 0xA2,
};

/* Private functions */
auto RMDX8Drive::getTempCFromResp(const francor::can::Msg& resp_msg) {
    return static_cast<float>(resp_msg.getData().at(RMD_BYTE_POS_TEMP));
}

auto RMDX8Drive::getTorqueNmFromResp(const francor::can::Msg& resp_msg) {
    const int16_t torque_raw = static_cast<int16_t>(resp_msg.getData().at(RMD_BYTE_POS_TQ_L)) |
                               (static_cast<int16_t>(resp_msg.getData().at(RMD_BYTE_POS_TQ_H) << 8U));

    return static_cast<float>(torque_raw) * RMD_TQ_RAW_TO_NM;
}

auto RMDX8Drive::isRespValid(const francor::can::Msg& req_msg, const francor::can::Msg& resp_msg) {
    return (req_msg.getId() == resp_msg.getId() && req_msg.getDlc() == resp_msg.getDlc());
}

/* Member functions */
RMDX8Drive::RMDX8Drive(const unsigned int id, std::shared_ptr<CAN>& can_if)
    : _can_id(id + RMD_X8_BASE_ID), _can_if(can_if) {}

void RMDX8Drive::reset() {
    if (_actv_state != DRIVE_STS_INIT) {
        _actv_state = DRIVE_STS_INIT;
    }
}

void RMDX8Drive::enable() {
    if (_actv_state != DRIVE_STS_ENABLED) {
        _actv_state = DRIVE_STS_ENABLED;
        setSpeedRPM(0.0F);
    }
}

void RMDX8Drive::disable() {
    if (_actv_state != DRIVE_STS_DISABLED) {
        _actv_state = DRIVE_STS_DISABLED;
    }
}

void RMDX8Drive::setSpeedRPM(const float speed_rpm) {
    if (_actv_state == DRIVE_STS_ENABLED) {
        const auto speed_dps = static_cast<int32_t>(speed_rpm * RMD_RPM_TO_DPS_FAC);

        Msg spd_req = Msg(_can_id, CAN_MAX_DLC,
                          MsgData({
                              CAN_CMD_SET_SPEED,
                              0U,
                              0U,
                              0U,
                              static_cast<uint8_t>(speed_dps),
                              static_cast<uint8_t>(speed_dps >> 8U),
                              static_cast<uint8_t>(speed_dps >> 16U),
                              static_cast<uint8_t>(speed_dps >> 24U),
                          }));

        _can_if->tx(spd_req);

        const auto spd_resp = _can_if->rx(RxSettings(_can_id, MSG_ID_MASK));

        if (isRespValid(spd_req, spd_resp)) {
            _current_temp_c = getTempCFromResp(spd_resp);
            _current_torque_nm = getTorqueNmFromResp(spd_resp);
        }
    }
}

float RMDX8Drive::getCurrentSpeedRPM() { return 0.0F; }

States RMDX8Drive::getActvState() { return _actv_state; }

float RMDX8Drive::getTempC() { return 0.0F; }

bool RMDX8Drive::isConnected() noexcept {
    bool connected = {false};

    try {
        Msg sts_req = Msg(_can_id, CAN_MAX_DLC, MsgData({CAN_CMD_READ_MOTOR_STS_1}));
        _can_if->tx(sts_req);
        const auto sts_resp = _can_if->rx(RxSettings(_can_id, MSG_ID_MASK));
        connected = isRespValid(sts_req, sts_resp);
    } catch (can_exception& e) {
        connected = false;
    } catch (...) {
        connected = false;
    }

    return connected;
}