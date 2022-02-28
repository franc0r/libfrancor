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
using ValueClock = std::chrono::steady_clock;
using ValueTimepoint = ValueClock::time_point;
using ValueDurationMs = std::chrono::milliseconds;

/* Default Settings */
static constexpr ValueDurationMs DFT_VALUE_EXP_DURATION_MS = {ValueDurationMs(5U)};

/**
 * @brief Simple value representation with expiration time
 */
template <typename T>
class Value {
   public:
    Value() = default;

    explicit Value(const ValueDurationMs durability_ms) : _durability_ms(durability_ms) {}
    explicit Value(const T value, const ValueDurationMs durability_ms) : _value(value), _durability_ms(durability_ms) {
        _update_timepoint = ValueClock::now();
    }

    /**
     * @brief Sets the new value and updates the timestamp to ::now()
     */
    void set(T new_value) {
        _value = new_value;
        _update_timepoint = ValueClock::now();
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
        const auto delta_time = (ValueClock::now() - _update_timepoint);
        return (delta_time < _durability_ms);
    }

    /**
     * @brief True if value is expired (::now() - set-timestamp >= durability)
     */
    auto isExpired() const { return !isUpToDate(); }

   private:
    T _value = {};
    ValueTimepoint _update_timepoint = {};
    ValueDurationMs _durability_ms = {DFT_VALUE_EXP_DURATION_MS};
};

};  // namespace drive
};  // namespace francor