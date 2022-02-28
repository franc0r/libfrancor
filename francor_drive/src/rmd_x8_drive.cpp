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

#include <iostream>

using namespace francor::can;
using namespace francor::drive;

/* Defines */
static constexpr uint16_t RMD_X8_BASE_ID = {0x141U};
static constexpr float RMD_RPM_TO_DPS_FAC = {36000.0F / 60.0F};
static constexpr float RMD_DPS_TO_RMP_FAC = {60.0F / 360.0F};
static constexpr float RMD_TQ_RAW_TO_NM = {(1.5F / 4.9F) * (33.0F / 2048.0F)};
static constexpr float RMD_NM_TO_RAW_TQ = {(4.9F / 1.5F) * (2000.0F / 32.0F)};
static constexpr float RMD_RAW_TO_C = {0.1F};
static constexpr float RMD_RAW_TO_V = {0.1F};

static constexpr ssize_t RMD_BYTE_POS_TEMP = {1U};

static constexpr ssize_t RMD_BYTE_POS_TQ_H = {3U};
static constexpr ssize_t RMD_BYTE_POS_TQ_L = {2U};

static constexpr ssize_t RMD_BYTE_POS_SPD_H = {5U};
static constexpr ssize_t RMD_BYTE_POS_SPD_L = {4U};

static constexpr ssize_t RMD_BYTE_POS_ACCEL_HH = {7U};
static constexpr ssize_t RMD_BYTE_POS_ACCEL_HL = {6U};
static constexpr ssize_t RMD_BYTE_POS_ACCEL_LH = {5U};
static constexpr ssize_t RMD_BYTE_POS_ACCEL_LL = {4U};

static constexpr ssize_t RMD_BYTE_POS_VOLTAGE_L = {3U};
static constexpr ssize_t RMD_BYTE_POS_VOLTAGE_H = {4U};

/* Enumerations */
enum CANCommandList : uint8_t {
    CAN_CMD_READ_ACCEL = 0x33,
    CAN_CMD_WRITE_ACCEL = 0x34,
    CAN_CMD_READ_MOTOR_STS_1 = 0x9A,
    CAN_CMD_CLEAR_ERRORS = 0x9B,
    CAN_CMD_READ_MOTOR_STS_2 = 0x9C,
    CAN_CMD_MOTOR_OFF = 0x80,
    CAN_CMD_SET_SPEED = 0xA2,
    CAN_CMD_TORQUE_CURRENT_CTRL = 0xA1,
};

/* Private functions */
auto RMDX8Drive::transceiveCANMsg(francor::can::Msg& req_msg, francor::can::Msg& resp_msg, bool& is_resp_valid) {
    _can_if->tx(req_msg);
    resp_msg = _can_if->rx(RxSettings(_can_id, MSG_ID_MASK));

    is_resp_valid = (req_msg.getId() == resp_msg.getId() && req_msg.getDlc() == resp_msg.getDlc());
}

void RMDX8Drive::readMotorSts1() {
    Msg req = Msg(_can_id, CAN_MAX_DLC, MsgData({CAN_CMD_READ_MOTOR_STS_1}));
    Msg resp = Msg();
    bool resp_valid = {false};

    transceiveCANMsg(req, resp, resp_valid);

    if (resp_valid) {
        updateVoltageFromResp(resp);
    }
}

void RMDX8Drive::readMotorSts2() {
    Msg req = Msg(_can_id, CAN_MAX_DLC, MsgData({CAN_CMD_READ_MOTOR_STS_2}));
    Msg resp = Msg();
    bool resp_valid = {false};

    transceiveCANMsg(req, resp, resp_valid);

    if (resp_valid) {
        updateTempCFromResp(resp);
        updateTorqueNmFromResp(resp);
        updateSpeedRpmFromResp(resp);
    }
}

void RMDX8Drive::readAccel() {
    Msg req = Msg(_can_id, CAN_MAX_DLC, MsgData({CAN_CMD_READ_ACCEL}));
    Msg resp = Msg();
    bool resp_valid = {false};

    transceiveCANMsg(req, resp, resp_valid);

    if (resp_valid) {
        updateAccelFromResp(resp);
    }
}

void RMDX8Drive::disableMotor() {
    Msg req = Msg(_can_id, CAN_MAX_DLC, MsgData({CAN_CMD_MOTOR_OFF}));

    _can_if->tx(req);
    _can_if->rx(RxSettings(_can_id, MSG_ID_MASK));

    // TODO error handling if no rx is received
}

void RMDX8Drive::updateVoltageFromResp(const francor::can::Msg& resp_msg) {
    const uint16_t voltage_raw = static_cast<uint16_t>(resp_msg.getData().at(RMD_BYTE_POS_VOLTAGE_L)) |
                                 (static_cast<uint16_t>(resp_msg.getData().at(RMD_BYTE_POS_VOLTAGE_H) << 8U));

    const auto voltage = static_cast<float>(voltage_raw) * RMD_RAW_TO_V;
    _current_voltage_v.set(voltage);
}

void RMDX8Drive::updateAccelFromResp(const francor::can::Msg& resp_msg) {
    const auto accel_raw = static_cast<int32_t>(resp_msg.getData().at(RMD_BYTE_POS_ACCEL_LL)) |
                           (static_cast<int32_t>(resp_msg.getData().at(RMD_BYTE_POS_ACCEL_LH) << 8U)) |
                           (static_cast<int32_t>(resp_msg.getData().at(RMD_BYTE_POS_ACCEL_HL) << 16U)) |
                           (static_cast<int32_t>(resp_msg.getData().at(RMD_BYTE_POS_ACCEL_HH) << 24U));

    const auto accel = static_cast<float>(accel_raw) * RMD_DPS_TO_RMP_FAC;
    _current_accel_rpms.set(accel);
}

void RMDX8Drive::updateTempCFromResp(const francor::can::Msg& resp_msg) {
    const auto temp = static_cast<float>(resp_msg.getData().at(RMD_BYTE_POS_TEMP)) * RMD_RAW_TO_C;
    _current_temp_c.set(temp);
}

void RMDX8Drive::updateTorqueNmFromResp(const francor::can::Msg& resp_msg) {
    const int16_t torque_raw = static_cast<int16_t>(resp_msg.getData().at(RMD_BYTE_POS_TQ_L)) |
                               (static_cast<int16_t>(resp_msg.getData().at(RMD_BYTE_POS_TQ_H) << 8U));

    const auto torque = static_cast<float>(torque_raw) * RMD_TQ_RAW_TO_NM;

    _current_torque_nm.set(torque);
}

void RMDX8Drive::updateSpeedRpmFromResp(const francor::can::Msg& resp_msg) {
    const int16_t speed_raw = static_cast<int16_t>(resp_msg.getData().at(RMD_BYTE_POS_SPD_L)) |
                              (static_cast<int16_t>(resp_msg.getData().at(RMD_BYTE_POS_SPD_H) << 8U));

    const auto speed = static_cast<float>(speed_raw) * RMD_DPS_TO_RMP_FAC;

    _current_speed_rpm.set(speed);
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
        disableMotor();
    }
}

void RMDX8Drive::setAccelleration(float accel_rpms) {
    const auto accel_dpss = static_cast<int32_t>(accel_rpms * RMD_RPM_TO_DPS_FAC);

    Msg req = Msg(_can_id, CAN_MAX_DLC,
                  MsgData({
                      CAN_CMD_WRITE_ACCEL,
                      0U,
                      0U,
                      0U,
                      static_cast<uint8_t>(accel_dpss),
                      static_cast<uint8_t>(accel_dpss >> 8U),
                      static_cast<uint8_t>(accel_dpss >> 16U),
                      static_cast<uint8_t>(accel_dpss >> 24U),
                  }));
    Msg resp = Msg();
    bool resp_valid = {false};
    transceiveCANMsg(req, resp, resp_valid);
}

float RMDX8Drive::getAccelleration() {
    if (_current_accel_rpms.isExpired()) {
        readAccel();
    }

    return _current_accel_rpms.get();
}

void RMDX8Drive::setSpeedRPM(const float speed_rpm) {
    if (_actv_state == DRIVE_STS_ENABLED) {
        const auto speed_dps = static_cast<int32_t>(speed_rpm * RMD_RPM_TO_DPS_FAC);

        Msg req = Msg(_can_id, CAN_MAX_DLC,
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
        Msg resp = Msg();
        bool resp_valid = {false};

        transceiveCANMsg(req, resp, resp_valid);

        if (resp_valid) {
            updateTempCFromResp(resp);
            updateTorqueNmFromResp(resp);
            updateSpeedRpmFromResp(resp);
        }
    }
}

float RMDX8Drive::getCurrentSpeedRPM() {
    if (_current_speed_rpm.isExpired()) {
        readMotorSts2();
    }

    return _current_speed_rpm.get();
}

void RMDX8Drive::setTorqueNm(float torque_nm) {
    const auto torque_rps = static_cast<int16_t>(torque_nm * RMD_NM_TO_RAW_TQ);

    Msg req = Msg(_can_id, CAN_MAX_DLC,
                  MsgData({
                      CAN_CMD_TORQUE_CURRENT_CTRL,
                      0U,
                      0U,
                      0U,
                      static_cast<uint8_t>(torque_rps),
                      static_cast<uint8_t>(torque_rps >> 8U),
                  }));
    Msg resp = Msg();
    bool resp_valid = {false};

    transceiveCANMsg(req, resp, resp_valid);

    if (resp_valid) {
        updateTempCFromResp(resp);
        updateTorqueNmFromResp(resp);
        updateSpeedRpmFromResp(resp);
    }
}

float RMDX8Drive::getCurrentTorqueNm() {
    if (_current_torque_nm.isExpired()) {
        readMotorSts2();
    }

    return _current_torque_nm.get();
}

States RMDX8Drive::getActvState() { return _actv_state; }

float RMDX8Drive::getTempC() {
    if (_current_temp_c.isExpired()) {
        readMotorSts2();
    }

    return _current_temp_c.get();
}

float RMDX8Drive::getVoltageV() {
    if (_current_voltage_v.isExpired()) {
        readMotorSts1();
    }

    return _current_voltage_v.get();
}

bool RMDX8Drive::isConnected() noexcept {
    bool connected = {false};

    try {
        Msg req = Msg(_can_id, CAN_MAX_DLC, MsgData({CAN_CMD_READ_MOTOR_STS_1}));
        Msg resp = Msg();
        transceiveCANMsg(req, resp, connected);

    } catch (can_exception& e) {
        connected = false;
    } catch (...) {
        connected = false;
    }

    return connected;
}