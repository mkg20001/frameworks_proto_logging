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
 * Tests native auto generated code for specific vendor atom contains proper ids
 */
TEST(ApiGenVendorAtomTest, AtomIdConstantsTest) {
    EXPECT_EQ(VENDOR_ATOM1, 105501);
    EXPECT_EQ(VENDOR_ATOM2, 105502);
}

/**
 * Tests native auto generated code for specific vendor atom contains proper enums
 */
TEST(ApiGenVendorAtomTest, AtomEnumsConstantsTest) {
    EXPECT_EQ(VENDOR_ATOM1__TYPE__VENDOR_ATOM1_TYPE_UNKNOWN, 0);
    EXPECT_EQ(VENDOR_ATOM1__TYPE__VENDOR_ATOM1_TYPE_1, 1);
    EXPECT_EQ(VENDOR_ATOM1__TYPE__VENDOR_ATOM1_TYPE_2, 2);
    EXPECT_EQ(VENDOR_ATOM1__TYPE__VENDOR_ATOM1_TYPE_3, 3);
}

}  // namespace stats_log_api_gen
}  // namespace android
