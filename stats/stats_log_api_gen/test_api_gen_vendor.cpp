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

#include <aidl/android/frameworks/stats/VendorAtom.h>
#include <gtest/gtest.h>
#include <test_vendor_atoms.h>
#include <test_vendor_atoms_with_module.h>

#include <limits>

namespace android {
namespace stats_log_api_gen {

using namespace android::VendorAtoms;
using namespace aidl::android::frameworks::stats;

/**
 * Tests native auto generated code for specific vendor atom contains proper ids
 */
TEST(ApiGenVendorAtomTest, AtomIdConstantsTest) {
    EXPECT_EQ(VENDOR_ATOM1, 105501);
    EXPECT_EQ(VENDOR_ATOM2, 105502);
    EXPECT_EQ(VENDOR_ATOM4, 105504);
}

/**
 * Tests native auto generated code for specific vendor atom contains proper enums
 */
TEST(ApiGenVendorAtomTest, AtomEnumTest) {
    EXPECT_EQ(VendorAtom1::TYPE_UNKNOWN, 0);
    EXPECT_EQ(VendorAtom1::TYPE_1, 1);
    EXPECT_EQ(VendorAtom1::TYPE_2, 2);
    EXPECT_EQ(VendorAtom1::TYPE_3, 3);

    EXPECT_EQ(VendorAtom1::ANOTHER_TYPE_UNKNOWN, 0);
    EXPECT_EQ(VendorAtom1::ANOTHER_TYPE_1, 1);
    EXPECT_EQ(VendorAtom1::ANOTHER_TYPE_2, 2);
    EXPECT_EQ(VendorAtom1::ANOTHER_TYPE_3, 3);

    EXPECT_EQ(VendorAtom2::TYPE_UNKNOWN, 0);
    EXPECT_EQ(VendorAtom2::TYPE_1, 1);
    EXPECT_EQ(VendorAtom2::TYPE_2, 2);
    EXPECT_EQ(VendorAtom2::TYPE_3, 3);

    EXPECT_EQ(VendorAtom2::ANOTHER_TYPE_UNKNOWN, 0);
    EXPECT_EQ(VendorAtom2::ANOTHER_TYPE_1, 1);
    EXPECT_EQ(VendorAtom2::ANOTHER_TYPE_2, 2);
    EXPECT_EQ(VendorAtom2::ANOTHER_TYPE_3, 3);

    EXPECT_EQ(VendorAtom4::TYPE_UNKNOWN, 0);
    EXPECT_EQ(VendorAtom4::TYPE_1, 1);

    typedef void (*Atom1FuncWithEnum)(VendorAtom1::EnumType arg);
    typedef void (*Atom1FuncWithEnum2)(VendorAtom1::EnumType2 arg);
    typedef void (*Atom2FuncWithEnum)(VendorAtom2::EnumType arg);
    typedef void (*Atom2FuncWithEnum2)(VendorAtom2::EnumType2 arg);

    Atom1FuncWithEnum f1 = nullptr;
    Atom1FuncWithEnum2 f2 = nullptr;
    Atom2FuncWithEnum f3 = nullptr;
    Atom2FuncWithEnum2 f4 = nullptr;

    EXPECT_EQ(f1, nullptr);
    EXPECT_EQ(f2, nullptr);
    EXPECT_EQ(f3, nullptr);
    EXPECT_EQ(f4, nullptr);
}

TEST(ApiGenVendorAtomTest, AtomIdModuleTest) {
    EXPECT_EQ(VendorAtomsModule::VENDOR_ATOM4, 105504);
    EXPECT_EQ(VendorAtomsModule::VendorAtom4::TYPE_UNKNOWN, 0);
    EXPECT_EQ(VendorAtomsModule::VendorAtom4::TYPE_1, 1);
}

static const int32_t kTestIntValue = 100;
static const int64_t kTestLongValue = std::numeric_limits<int64_t>::max() - kTestIntValue;
static const float kTestFloatValue = (float)kTestIntValue / kTestLongValue;
static const bool kTestBoolValue = true;
static const char* kTestStringValue = "test_string";

TEST(ApiGenVendorAtomTest, buildVendorAtom1ApiTest) {
    typedef void (*VendorAtom1BuildFunc)(
            VendorAtom & atom, int32_t code, char const* reverse_domain_name, int32_t enumField1,
            int32_t enumField2, int32_t int_value32, int64_t int_value64, float float_value,
            bool bool_value, int32_t enumField3, int32_t enumField4);
    VendorAtom1BuildFunc func = &buildVendorAtom;

    EXPECT_NE(func, nullptr);

    VendorAtom atom;
    func(atom, VENDOR_ATOM1, kTestStringValue, VendorAtom1::TYPE_1, VendorAtom1::TYPE_2,
         kTestIntValue, kTestLongValue, kTestFloatValue, kTestBoolValue,
         VendorAtom1::ANOTHER_TYPE_2, VendorAtom1::ANOTHER_TYPE_3);

    EXPECT_EQ(atom.atomId, VENDOR_ATOM1);
    EXPECT_EQ(atom.reverseDomainName, kTestStringValue);
    EXPECT_EQ(atom.values.size(), static_cast<size_t>(8));
    EXPECT_EQ(atom.values[0].get<VendorAtomValue::intValue>(), VendorAtom1::TYPE_1);
    EXPECT_EQ(atom.values[1].get<VendorAtomValue::intValue>(), VendorAtom1::TYPE_2);
    EXPECT_EQ(atom.values[2].get<VendorAtomValue::intValue>(), kTestIntValue);
    EXPECT_EQ(atom.values[3].get<VendorAtomValue::longValue>(), kTestLongValue);
    EXPECT_EQ(atom.values[4].get<VendorAtomValue::floatValue>(), kTestFloatValue);
    EXPECT_EQ(atom.values[5].get<VendorAtomValue::boolValue>(), kTestBoolValue);
    EXPECT_EQ(atom.values[6].get<VendorAtomValue::intValue>(), VendorAtom1::ANOTHER_TYPE_2);
    EXPECT_EQ(atom.values[7].get<VendorAtomValue::intValue>(), VendorAtom1::ANOTHER_TYPE_3);
}

TEST(ApiGenVendorAtomTest, buildVendorAtom3ApiTest) {
    typedef void (*VendorAtom3BuildFunc)(VendorAtom & atom, int32_t code, char const* arg1,
                                         int32_t arg2);
    VendorAtom3BuildFunc func = &buildVendorAtom;

    EXPECT_NE(func, nullptr);

    VendorAtom atom;
    func(atom, VENDOR_ATOM3, kTestStringValue, kTestIntValue);

    EXPECT_EQ(atom.atomId, VENDOR_ATOM3);
    EXPECT_EQ(atom.reverseDomainName, kTestStringValue);
    EXPECT_EQ(atom.values.size(), static_cast<size_t>(1));
    EXPECT_EQ(atom.values[0].get<VendorAtomValue::intValue>(), kTestIntValue);
}

}  // namespace stats_log_api_gen
}  // namespace android
