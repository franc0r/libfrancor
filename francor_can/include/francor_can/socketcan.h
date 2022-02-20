/**
 * @file socketcan_interface.h
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

#pragma once

#include <net/if.h>

#include <array>
#include <chrono>
#include <iterator>

#include "francor_can/can.h"

namespace francor {

namespace can {

class SocketCAN : public CAN {
   public:
    explicit SocketCAN(const std::string& if_name);
    ~SocketCAN();

    void tx(Msg tx) final;
    Msg rx(const RxSettings settings) final;

    bool isDeviceUp() final;

   private:
    void openSocket();
    void createIfReqStruct();
    void bindSocket();
    void checkDeviceUp();

    static auto isIoRWSuccess(ssize_t res);

    void setRxFilter(const RxSettings& settings);
    void setRxTimeout(const RxSettings& settings);

    const std::string _if_name;

    int _socket = {-1};
    ifreq _if_req = {};
};

};  // namespace can

};  // namespace francor
