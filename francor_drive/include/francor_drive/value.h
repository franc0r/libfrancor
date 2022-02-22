/**
 * @file value.h
 * @author Martin Bauernschmitt
 *
 * @brief Drive value with update timestamp representation
 *
 * @version 0.1
 * @date 2022-02-21
 *
 * @copyright Copyright (c) 2022 - BSD-3-clause - FRANCOR e.V.
 *
 */

#pragma once

#include <chrono>

namespace francor {

namespace drive {

/* Typedefs */
using Clock = std::chrono::steady_clock;
using ValueTimepoint = std::chrono::steady_clock::time_point;
using DurationMs = std::chrono::milliseconds;

/* Default Settings */
static constexpr DurationMs DFT_VALUE_EXP_DURATION_MS = {DurationMs(5U)};

/**
 * @brief Simple value representation with expiration time
 */
template <typename T>
class Value {
   public:
    Value() = default;

    explicit Value(const DurationMs durability_ms) : _durability_ms(durability_ms) {}

    /**
     * @brief Sets the new value and updates the timestamp to ::now()
     */
    void set(T new_value) {
        _value = new_value;
        _update_timepoint = Clock::now();
    }

    /**
     * @brief Gets the current value
     */
    auto get() const { return _value; }

    /**
     * @brief Gets the current expire duration
     */
    auto getDurabilityMs() const { return _durability_ms; }

    /**
     * @brief True if data is up to date (::now() - set-timestamp < durability)
     */
    auto isUpToDate() const {
        const auto delta_time = (Clock::now() - _update_timepoint);
        return (delta_time < _durability_ms);
    }

    /**
     * @brief True if value is expired (::now() - set-timestamp >= durability)
     */
    auto isExpired() const { return !isUpToDate(); }

   private:
    T _value = {};
    ValueTimepoint _update_timepoint = {};
    DurationMs _durability_ms = {DFT_VALUE_EXP_DURATION_MS};
};

};  // namespace drive
};  // namespace francor