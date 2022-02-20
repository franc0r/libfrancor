/**
 * @file socketcan_interface.cpp
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 *
 * @brief SocketCAN interface class
 *
 * @version 0.1
 * @date 2022-02-19
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#include "francor_can/socketcan.h"

#include <linux/can/raw.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "francor_can/can.h"

using namespace francor::can;

SocketCAN::SocketCAN(const std::string& if_name) : _if_name(if_name) {
    openSocket();
    createIfReqStruct();
    bindSocket();
    checkDeviceUp();
}

SocketCAN::~SocketCAN() {
    if (_socket >= 0) {
        close(_socket);
    }
}

void SocketCAN::openSocket() {
    _socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socket < 0) {
        throw can_exception(EXCEP_DEVICE_ERROR, _if_name, "Cannot create socket!");
    }
}

void SocketCAN::createIfReqStruct() {
    memset(&_if_req, 0, sizeof(_if_req));
    strcpy(_if_req.ifr_name, _if_name.c_str());  // NOLINT
}

void SocketCAN::bindSocket() {
    if (ioctl(_socket, SIOCGIFINDEX, &_if_req) < 0) {  // NOLINT
        throw can_exception(EXCEP_DEVICE_NOT_FOUND, _if_name, "Cannot get descriptor! Device connected?");
    }

    sockaddr_can addr = {AF_CAN, _if_req.ifr_ifindex, 0};       // NOLINT
    if (bind(_socket, (sockaddr*)(&addr), sizeof(addr)) < 0) {  // NOLINT
        throw can_exception(EXCEP_DEVICE_ERROR, _if_name, "Cannot bind socket to device!");
    }
}

void SocketCAN::checkDeviceUp() {
    if (!isDeviceUp()) {
        throw can_exception(EXCEP_DEVICE_NOT_RUNNING, _if_name, "Device is not up and running!");
    }
}

auto SocketCAN::isIoRWSuccess(const ssize_t res) { return static_cast<bool>(CAN_MTU == res); }

void SocketCAN::setRxFilter(const RxSettings& settings) {
    can_filter filter = {.can_id = settings.filter_id, .can_mask = settings.filter_mask};

    if (0 != setsockopt(_socket, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter))) {
        throw can_exception(EXCEP_DEVICE_CFG_ERROR, _if_name, "Failed to set rx filter!");
    }
}

void SocketCAN::setRxTimeout(const RxSettings& settings) {
    constexpr __time_t US_PER_SEC = {1000000U};

    struct timeval tv = {
        .tv_sec = std::chrono::duration_cast<std::chrono::seconds>(settings.timeout_ms).count(),
        .tv_usec = (std::chrono::duration_cast<std::chrono::microseconds>(settings.timeout_ms).count() % US_PER_SEC),
    };

    if (0 != setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<void*>(&tv), sizeof(tv))) {
        throw can_exception(EXCEP_DEVICE_CFG_ERROR, _if_name, "Failed to set rx timeout!");
    }
}

void SocketCAN::tx(Msg tx) {
    struct can_frame frm = tx.getCCANFrame();

    if (!isIoRWSuccess(write(_socket, &frm, CAN_MTU))) {
        std::stringstream desc;
        desc << "CAN transmission failed! UNIX errno = " << +errno << "!";
        throw can_exception(EXCEP_TX_ERROR, _if_name, desc.str());
    }
}

Msg SocketCAN::rx(const RxSettings settings) {
    setRxFilter(settings);
    setRxTimeout(settings);

    can_frame can_msg = {};
    Msg msg = Msg();
    if (isIoRWSuccess(read(_socket, &can_msg, CAN_MTU))) {
        msg = Msg(can_msg);
    } else {
        std::stringstream desc;
        desc << "RX timeout of msg [ID: '" << std::hex << +settings.filter_id << "']";
        throw can_exception(EXCEP_RX_TIMEOUT, _if_name, "RX timeout");
    }

    return msg;
}

bool SocketCAN::isDeviceUp() {
    if (ioctl(_socket, SIOCGIFFLAGS, &_if_req) < 0) {  // NOLINT
        throw can_exception(EXCEP_DEVICE_ERROR, _if_name, "Cannot get device flags!");
    }

    return ((_if_req.ifr_flags & IFF_UP) != 0);  // NOLINT
}