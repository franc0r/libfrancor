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

/**
 * @brief Drive representation of francor drives
 *
 */
class Drive {
   public:
    Drive() = default;

    virtual void reset() = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;

    virtual void setSpeedRPM(double speed_rpm) = 0;
    virtual double getCurrentSpeedRPM() = 0;

    virtual bool isConnected() = 0;
};

};  // namespace drive
};  // namespace francor