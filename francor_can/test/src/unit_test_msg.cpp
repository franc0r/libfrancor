/**
 * @file unit_test_msg.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 *
 * @brief Unit tests for CAN message
 *
 * @version 0.1
 * @date 2022-02-19
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */
#include <gtest/gtest.h>

#include <stdexcept>

#include "francor_can/msg.h"

using namespace francor::can;

TEST(CANMsg, constructorEmptyMsg) {  // NOLINT
    Msg msg = Msg();

    EXPECT_EQ(0U, msg.getId());
    EXPECT_EQ(0U, msg.getDlc());

    for (auto& byte : msg.getData()) {
        EXPECT_EQ(0, byte);
    }
}

TEST(CANMsg, constructorID) {  // NOLINT
    constexpr uint16_t TEST_ID = {2047U};

    Msg msg = Msg(TEST_ID, 0U);
    EXPECT_EQ(TEST_ID, msg.getId());
}

TEST(CANMsg, constructorIDAndDLC) {  // NOLINT
    constexpr uint16_t TEST_ID = {2047U};
    constexpr uint16_t TEST_DLC = {7U};

    Msg msg = Msg(TEST_ID, TEST_DLC);
    EXPECT_EQ(TEST_ID, msg.getId());
    EXPECT_EQ(TEST_DLC, msg.getDlc());
}

TEST(CANMsg, constructorInvldID) {  // NOLINT
    constexpr uint16_t TEST_ID_INVLD = {2048};
    bool error_catched = {false};

    try {
        Msg msg = Msg(TEST_ID_INVLD, 0U);
        (void)msg;
    } catch (std::invalid_argument& e) {
        error_catched = true;
    }

    EXPECT_EQ(true, error_catched);
}

TEST(CANMsg, constructorInvldDlc) {  // NOLINT
    constexpr uint16_t TEST_DLC_INVLD = {9U};
    bool error_catched = {false};

    try {
        Msg msg = Msg(0U, TEST_DLC_INVLD);
        (void)msg;
    } catch (std::invalid_argument& e) {
        error_catched = true;
    }

    EXPECT_EQ(true, error_catched);
}

TEST(CANMsg, constructorBytes1) {  // NOLINT
    constexpr uint16_t TEST_ID = {2047U};
    constexpr uint16_t TEST_DLC = {7U};
    constexpr MsgData TEST_DATA = {1U, 2U, 3U};

    Msg msg = Msg(TEST_ID, TEST_DLC, TEST_DATA);
    EXPECT_EQ(TEST_ID, msg.getId());
    EXPECT_EQ(TEST_DLC, msg.getDlc());

    unsigned int idx = {0U};
    for (auto& byte : msg.getData()) {
        EXPECT_EQ(TEST_DATA.at(idx++), byte);
    }
}

TEST(CANMsg, constructorBytes2) {  // NOLINT
    constexpr uint16_t TEST_ID = {2047U};
    constexpr uint16_t TEST_DLC = {7U};
    constexpr MsgData TEST_DATA = {1U, 2U, 3U, 4U, 0U, 6U, 7U, 8U};

    Msg msg = Msg(TEST_ID, TEST_DLC, TEST_DATA);
    EXPECT_EQ(TEST_ID, msg.getId());
    EXPECT_EQ(TEST_DLC, msg.getDlc());

    unsigned int idx = {0U};
    for (auto& byte : msg.getData()) {
        EXPECT_EQ(TEST_DATA.at(idx++), byte);
    }
}

TEST(CANMsg, getCCANFrame) {  // NOLINT
    constexpr uint16_t TEST_ID = {2047U};
    constexpr uint16_t TEST_DLC = {7U};
    constexpr MsgData TEST_DATA = {1U, 2U, 3U, 4U, 0U, 6U, 7U, 8U};

    Msg msg = Msg(TEST_ID, TEST_DLC, TEST_DATA);
    struct can_frame frame = msg.getCCANFrame();

    EXPECT_EQ(TEST_ID, frame.can_id);
    EXPECT_EQ(TEST_DLC, frame.can_dlc);

    for (auto idx = 0U; idx < MSG_MAX_DLC; idx++) {
        EXPECT_EQ(TEST_DATA.at(idx), frame.data[idx]);  // NOLINT
    }
}

TEST(CANMsg, constructorCCANFrame) {  // NOLINT
    constexpr uint16_t TEST_ID = {2047U};
    constexpr uint16_t TEST_DLC = {7U};
    constexpr MsgData TEST_DATA = {1U, 2U, 3U, 4U, 0U, 6U, 7U, 8U};

    struct can_frame frame = {
        .can_id = TEST_ID,
        .can_dlc = TEST_DLC,
    };

    memcpy(&frame.data[0U], &TEST_DATA[0U], sizeof(frame.data));
    Msg msg = Msg(frame);

    EXPECT_EQ(TEST_ID, msg.getId());
    EXPECT_EQ(TEST_DLC, msg.getDlc());

    for (auto idx = 0U; idx < MSG_MAX_DLC; idx++) {
        EXPECT_EQ(TEST_DATA.at(idx), msg.getData()[idx]);  // NOLINT
    }
}

int main(int argc, char** argv) {  // NOLINT
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}