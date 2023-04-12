/**
 * @file rmd_x8_drive.h.h
 * @author Martin Bauernschmitt

 * @brief Interface for My Actuator RMD-X8 Pro Drive (can be used for other drives too)

 * See webpage of manufacturer for details: http://www.gyems.cn/support/download

 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#pragma once

#include <francor_can/can.h>
#include <francor_drive/drive.h>
#include <francor_drive/value.h>

#include <memory>

namespace francor {

namespace drive {

/* Defines */
static constexpr ValueDurationMs RMD_X8_DFT_DURABILITY_MS = {ValueDurationMs(2U)};
static constexpr unsigned int RMD_X8_DFT_RETRY_LIMIT = {1U};

class RMDX8Drive : public Drive {
   public:
    RMDX8Drive(unsigned int id, std::shared_ptr<francor::can::CAN>& can_if);

    void reset() final;
    void enable() final;
    void disable() final;

    void setAccelleration(float accel_rpms) final;
    float getAccelleration() final;

    void setSpeedRPM(float speed_rpm) final;
    float getCurrentSpeedRPM() final;

    void setTorqueNm(float torque_nm);
    float getCurrentTorqueNm() final;

    States getActvState() final;
    float getTempC() final;
    float getVoltageV() final;

    bool isConnected() final;

   private:
    auto transceiveCANMsg(francor::can::Msg& req_msg, francor::can::Msg& resp_msg, bool& is_resp_valid);

    void readMotorSts1();
    void readMotorSts2();
    void readAccel();
    void disableMotor();

    void updateVoltageFromResp(const francor::can::Msg& resp_msg);
    void updateAccelFromResp(const francor::can::Msg& resp_msg);
    void updateTempCFromResp(const francor::can::Msg& resp_msg);
    void updateTorqueNmFromResp(const francor::can::Msg& resp_msg);
    void updateSpeedRpmFromResp(const francor::can::Msg& resp_msg);

    const unsigned int _can_id = {0};
    std::shared_ptr<francor::can::CAN> _can_if = {};
    const unsigned int _retry_limit = {RMD_X8_DFT_RETRY_LIMIT};

    States _actv_state = {DRIVE_STS_INIT};

    float _tgt_speed_rpm = {0.0F};

    Value<float> _current_voltage_v = {Value<float>(0.0F, RMD_X8_DFT_DURABILITY_MS)};
    Value<float> _current_torque_nm = {Value<float>(0.0F, RMD_X8_DFT_DURABILITY_MS)};
    Value<float> _current_speed_rpm = {Value<float>(0.0F, RMD_X8_DFT_DURABILITY_MS)};
    Value<float> _current_temp_c = {Value<float>(0.0F, RMD_X8_DFT_DURABILITY_MS)};
    Value<float> _current_accel_rpms = {Value<float>(0.0F, RMD_X8_DFT_DURABILITY_MS)};
};

};  // namespace drive

};  // namespace francor