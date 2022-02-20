/**
 * @file unit_test_socketcan.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 *
 * @brief Unit tests for SocketCAN interface
 *
 * @version 0.1
 * @date 2022-02-19
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */
#include <gtest/gtest.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <stdexcept>

#include "francor_can/socketcan.h"

using namespace francor::can;

constexpr char CAN_TEST_DEVICE_NAME[] = {"can0"};

static bool VCAN_AVAILABLE = {false};

bool isCANAvailableForTest() {
    int sock = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        return false;
    }

    ifreq if_desc = {};
    strcpy(if_desc.ifr_name, CAN_TEST_DEVICE_NAME);  // NOLINT
    if (ioctl(sock, SIOCGIFINDEX, &if_desc) < 0) {   // NOLINT
        close(sock);
        return false;
    }

    close(sock);
    return true;
}

TEST(SocketCAN, constructInvldIf) {  // NOLINT
    bool except = {false};

    try {
        SocketCAN can = SocketCAN("test");
    } catch (can_exception& e) {
        except = true;
        EXPECT_EQ(EXCEP_DEVICE_NOT_FOUND, e.getType());
    }

    EXPECT_EQ(true, except);
}

TEST(SocketCAN, constructorOpenCAN) {
    if (!VCAN_AVAILABLE) GTEST_SKIP();

    try {
        SocketCAN can = SocketCAN(CAN_TEST_DEVICE_NAME);
    } catch (can_exception& e) {
        FAIL() << e.what();
    }

    SUCCEED();
}

int main(int argc, char** argv) {  // NOLINT
    VCAN_AVAILABLE = isCANAvailableForTest();

    if (!VCAN_AVAILABLE) {
        std::cout << "[ INFO     ] Test CAN Bus '" << CAN_TEST_DEVICE_NAME << "' not available! Skipping tests!"
                  << std::endl;
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}