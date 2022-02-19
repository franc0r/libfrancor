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
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

#include "francor_can/can.h"

using namespace francor::can;

SocketCAN::SocketCAN(const std::string& if_name) : _if_name(if_name) {
    openSocket();
    bindSocket();
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

void SocketCAN::bindSocket() {
    ifreq if_desc = {};
    strcpy(if_desc.ifr_name, _if_name.c_str());
    if (ioctl(_socket, SIOCGIFINDEX, &if_desc) < 0) {
        throw can_exception(EXCEP_DEVICE_NOT_FOUND, _if_name, "Cannot get descriptor! Device connected?");
    }

    sockaddr_can addr = {AF_CAN, if_desc.ifr_ifindex, 0};
    if (bind(_socket, (sockaddr*)(&addr), sizeof(addr)) < 0) {
        throw can_exception(EXCEP_DEVICE_ERROR, _if_name, "Cannot bind socket to device!");
    }
}

void SocketCAN::tx(Msg& tx) {}
Msg SocketCAN::rx(RxSettings& settings) { return Msg(); }

bool SocketCAN::isInterfaceRunning() { return true; }