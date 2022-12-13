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
#include <statslog.h>

namespace android {
namespace stats_log_api_gen {

/**
 * Tests auto generated code for specific atom contains proper ids
 */
TEST(ApiGenAtomTest, AtomIdConstantsTest) {
    // For reference from the atoms.proto
    // BleScanStateChanged ble_scan_state_changed = 2
    //         [(module) = "bluetooth", (module) = "statsdtest"];
    // ProcessStateChanged process_state_changed = 3 [(module) = "framework", deprecated = true];
    EXPECT_EQ(android::util::BLE_SCAN_STATE_CHANGED, 2);
    EXPECT_EQ(android::util::PROCESS_STATE_CHANGED, 3);
    EXPECT_EQ(android::util::BOOT_SEQUENCE_REPORTED, 57);
}

/**
 * Tests auto generated code for specific atom contains proper enums
 */
TEST(ApiGenAtomTest, AtomEnumsConstantsTest) {
    // For reference from the pixelatoms.proto
    // message BleScanStateChanged {
    //     repeated AttributionNode attribution_node = 1
    //             [(state_field_option).primary_field_first_uid = true];

    //     enum State {
    //         OFF = 0;
    //         ON = 1;
    //         // RESET indicates all ble stopped. Used when it (re)starts (e.g. after it crashes).
    //         RESET = 2;
    //     }

    EXPECT_EQ(android::util::BLE_SCAN_STATE_CHANGED__STATE__OFF, 0);
    EXPECT_EQ(android::util::BLE_SCAN_STATE_CHANGED__STATE__ON, 1);
    EXPECT_EQ(android::util::BLE_SCAN_STATE_CHANGED__STATE__RESET, 2);
}

/**
 * Tests auto generated code for write api
 */
TEST(ApiGenAtomTest, AtomWriteBootSequenceReportedTest) {
    // For reference from the pixelatoms.proto
    // message BootSequenceReported {
    //     // Reason for bootloader boot. Eg. reboot. See bootstat.cpp for larger list
    //     // Default: "<EMPTY>" if not available.
    //     optional string bootloader_reason = 1;
    //     // Reason for system boot. Eg. bootloader, reboot,userrequested
    //     // Default: "<EMPTY>" if not available.
    //     optional string system_reason = 2;
    //     // End of boot time in ms from unix epoch using system wall clock.
    //     optional int64 end_time_millis = 3;
    //     // Total boot duration in ms.
    //     optional int64 total_duration_millis = 4;
    //     // Bootloader duration in ms.
    //     optional int64 bootloader_duration_millis = 5;
    //     // Time since last boot in ms. Default: 0 if not available.
    //     optional int64 time_since_last_boot = 6;
    // }

    // This is going to be build time test which confirms the API signature
    // android::util::stats_write(android::util::BOOT_SEQUENCE_REPORTED, reason.c_str(),
    //                          system_reason.c_str(), end_time.count(), total_duration.count(),
    //                          (int64_t)bootloader_duration_ms,
    //                          (int64_t)time_since_last_boot_sec * 1000);

    typedef int (*WriteApi)(int32_t code, char const* arg1, char const* arg2, int64_t arg3,
                             int64_t arg4, int64_t arg5, int64_t arg6);

    WriteApi atomWriteApi = &android::util::stats_write;

    EXPECT_NE(atomWriteApi, nullptr);
}

}  // namespace stats_log_api_gen
}  // namespace android
