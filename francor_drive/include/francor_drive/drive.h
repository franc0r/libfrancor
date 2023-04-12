/**
 * @file drive.h
 * @author Martin Bauernschmitt
 *
 * @brief Main representation of a FRANCOR drive
 *
 * @version 0.1
 * @date 2022-02-20
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#pragma once

namespace francor {

namespace drive {

enum States {
    DRIVE_STS_INIT,
    DRIVE_STS_DISABLED,
    DRIVE_STS_ENABLED,
    DRIVE_STS_ERROR,
};

/**
 * @brief Drive representation of francor drives
 */
class Drive {
   public:
    Drive() = default;

    virtual void reset() = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;

    virtual void setAccelleration(float accel_rpms) = 0;
    virtual float getAccelleration() = 0;

    virtual void setSpeedRPM(float speed_rpm) = 0;
    virtual float getCurrentSpeedRPM() = 0;

    virtual void setTorqueNm(float torque_nm) = 0;
    virtual float getCurrentTorqueNm() = 0;

    virtual States getActvState() = 0;
    virtual float getTempC() = 0;
    virtual float getVoltageV() = 0;

    virtual bool isConnected() = 0;
};

};  // namespace drive
};  // namespace francor