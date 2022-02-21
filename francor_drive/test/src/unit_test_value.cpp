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

#include "francor_drive/value.h"

using namespace francor::drive;

TEST(Value, constructorDft) {  // NOLINT
}

int main(int argc, char** argv) {  // NOLINT
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}