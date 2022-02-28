/**
 * @file unit_test_value.h
 * @author Martin Bauernschmitt (martin.bauernschmitt@francor.de)
 *
 * @brief Unit tests for drive Value representation
 *
 * @version 0.1
 * @date 2022-02-21
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */
#include <gtest/gtest.h>

#include <stdexcept>
#include <thread>

#include "francor_drive/value.h"

using namespace francor::drive;

TEST(Value, ConstructDft) {  // NOLINT
    constexpr int DURABILITY_MS = {10};

    auto test_value = Value<int>(ValueDurationMs(DURABILITY_MS));

    EXPECT_EQ(DURABILITY_MS, test_value.getDurabilityMs().count());
    EXPECT_EQ(0, test_value.get());
}

TEST(Value, ConstructValue) {  // NOLINT
    constexpr int DURABILITY_MS = {5};
    constexpr int VALUE = {1234};

    auto test_value = Value<int>(VALUE, ValueDurationMs(DURABILITY_MS));
    EXPECT_EQ(false, test_value.isExpired());
    EXPECT_EQ(true, test_value.isUpToDate());
    EXPECT_EQ(VALUE, test_value.get());

    std::this_thread::sleep_for(ValueDurationMs(DURABILITY_MS));

    EXPECT_EQ(true, test_value.isExpired());
    EXPECT_EQ(false, test_value.isUpToDate());
    EXPECT_EQ(VALUE, test_value.get());
}

TEST(Value, ValueExpireTest) {  // NOLINT
    constexpr int DURABILITY_MS = {10};
    constexpr int VALUE = {-2202};

    auto test_value = Value<int>(ValueDurationMs(DURABILITY_MS));

    EXPECT_EQ(true, test_value.isExpired());
    EXPECT_EQ(false, test_value.isUpToDate());

    test_value.set(VALUE);

    EXPECT_EQ(false, test_value.isExpired());
    EXPECT_EQ(true, test_value.isUpToDate());
    EXPECT_EQ(VALUE, test_value.get());

    std::this_thread::sleep_for(ValueDurationMs(DURABILITY_MS - 1U));

    EXPECT_EQ(false, test_value.isExpired());
    EXPECT_EQ(true, test_value.isUpToDate());
    EXPECT_EQ(VALUE, test_value.get());

    std::this_thread::sleep_for(ValueDurationMs(1U));

    EXPECT_EQ(true, test_value.isExpired());
    EXPECT_EQ(false, test_value.isUpToDate());
    EXPECT_EQ(VALUE, test_value.get());
}

int main(int argc, char** argv) {  // NOLINT
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}