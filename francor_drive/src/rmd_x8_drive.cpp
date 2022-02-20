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

static constexpr uint16_t CAN_BASE_ID = {0x141};

RMDX8Drive::RMDX8Drive(const unsigned int id, std::shared_ptr<CAN>& can_if)
    : _can_id(id + CAN_BASE_ID), _can_if(can_if) {}

void RMDX8Drive::reset() {}

void RMDX8Drive::enable() {}

void RMDX8Drive::disable() {}

void RMDX8Drive::setSpeedRPM(double speed_rpm) {}

double RMDX8Drive::getCurrentSpeedRPM() { return 0.0; }

bool RMDX8Drive::isConnected() { return false; }