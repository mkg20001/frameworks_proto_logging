/*
 * Copyright (C) 2022, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "test_vendor_atoms.h"

namespace android {
namespace stats_log_api_gen {

using namespace android::VendorAtoms;

/**
 * Tests auto generated code for specific vendor atom contains proper ids
 */
TEST(ApiGenVendorAtomTest, AtomIdConstantsTest) {
    // For reference from the pixelatoms.proto
    //   VendorBatteryHealthSnapshot vendor_battery_health_snapshot =
    //             105026 [(android.os.statsd.module) = "health"]; // moved from atoms.proto
    //   VendorBatteryCausedShutdown vendor_battery_caused_shutdown =
    //             105027 [(android.os.statsd.module) = "health"]; // moved from atoms.proto

    EXPECT_EQ(VENDOR_BATTERY_HEALTH_SNAPSHOT, 105026);
    EXPECT_EQ(VENDOR_BATTERY_CAUSED_SHUTDOWN, 105027);
}

/**
 * Tests auto generated code for specific vendor atom contains proper enums
 */
TEST(ApiGenVendorAtomTest, AtomEnumsConstantsTest) {
    // For reference from the pixelatoms.proto
    // message VendorBatteryHealthSnapshot {
    //   enum BatterySnapshotType {
    //     BATTERY_SNAPSHOT_TYPE_UNKNOWN = 0;
    //     BATTERY_SNAPSHOT_TYPE_MIN_TEMP = 1;         // Snapshot at min batt temp over 24hrs.
    //     BATTERY_SNAPSHOT_TYPE_MAX_TEMP = 2;         // Snapshot at max batt temp over 24hrs.
    //     BATTERY_SNAPSHOT_TYPE_MIN_RESISTANCE = 3;   // Snapshot at min batt resistance over
    //     24hrs. BATTERY_SNAPSHOT_TYPE_MAX_RESISTANCE = 4;   // Snapshot at max batt resistance
    //     over 24hrs. BATTERY_SNAPSHOT_TYPE_MIN_VOLTAGE = 5;      // Snapshot at min batt voltage
    //     over 24hrs. BATTERY_SNAPSHOT_TYPE_MAX_VOLTAGE = 6;      // Snapshot at max batt voltage
    //     over 24hrs. BATTERY_SNAPSHOT_TYPE_MIN_CURRENT = 7;      // Snapshot at min batt current
    //     over 24hrs. BATTERY_SNAPSHOT_TYPE_MAX_CURRENT = 8;      // Snapshot at max batt current
    //     over 24hrs. BATTERY_SNAPSHOT_TYPE_MIN_BATT_LEVEL = 9;   // Snapshot at min battery level
    //     (SoC) over 24hrs. BATTERY_SNAPSHOT_TYPE_MAX_BATT_LEVEL = 10;  // Snapshot at max battery
    //     level (SoC) over 24hrs. BATTERY_SNAPSHOT_TYPE_AVG_RESISTANCE = 11;  // Snapshot at
    //     average battery resistance over 24hrs.
    //   }

    EXPECT_EQ(VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_UNKNOWN, 0);
    EXPECT_EQ(VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MIN_TEMP, 1);
    EXPECT_EQ(VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MAX_TEMP, 2);
    EXPECT_EQ(
            VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MIN_RESISTANCE,
            3);
    EXPECT_EQ(
            VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MAX_RESISTANCE,
            4);
    EXPECT_EQ(VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MIN_VOLTAGE,
              5);
    EXPECT_EQ(VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MAX_VOLTAGE,
              6);
    EXPECT_EQ(VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MIN_CURRENT,
              7);
    EXPECT_EQ(VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MAX_CURRENT,
              8);
    EXPECT_EQ(
            VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MIN_BATT_LEVEL,
            9);
    EXPECT_EQ(
            VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_MAX_BATT_LEVEL,
            10);
    EXPECT_EQ(
            VENDOR_BATTERY_HEALTH_SNAPSHOT__TYPE__BATTERY_SNAPSHOT_TYPE_AVG_RESISTANCE,
            11);
}

}  // namespace stats_log_api_gen
}  // namespace android
