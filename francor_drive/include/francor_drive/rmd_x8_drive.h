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

#include <memory>

namespace francor {

namespace drive {
class RMDX8Drive : public Drive {
   public:
    RMDX8Drive(unsigned int id, std::shared_ptr<francor::can::CAN>& can_if);

    void reset() final;
    void enable() final;
    void disable() final;

    void setSpeedRPM(const float speed_rpm) final;
    float getCurrentSpeedRPM() final;

    States getActvState() final;
    float getTempC() final;

    bool isConnected() noexcept final;

   private:
    static auto getTempCFromResp(const francor::can::Msg& resp_msg);
    static auto getTorqueNmFromResp(const francor::can::Msg& resp_msg);
    static auto getSpeedRpmFromResp(const francor::can::Msg& resp_msg);
    static auto isRespValid(const francor::can::Msg& req_msg, const francor::can::Msg& resp_msg);

    const unsigned int _can_id = {0};
    std::shared_ptr<francor::can::CAN> _can_if = {};

    States _actv_state = {DRIVE_STS_INIT};

    float _tgt_speed_rpm = {0.0F};

    float _current_torque_nm = {0.0F};
    float _current_speed_rpm = {0.0F};

    float _current_temp_c = {0.0F};
};

};  // namespace drive

};  // namespace francor