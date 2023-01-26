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

TEST(ApiGenVendorAtomTest, buildVendorAtom3ApiTest) {
    typedef void (*VendorAtom3BuildFunc)(VendorAtom & atom, int32_t code, char const* arg1,
                                         int32_t arg2);
    VendorAtom3BuildFunc func = &buildVendorAtom;

    EXPECT_NE(func, nullptr);

    VendorAtom atom;
    func(atom, 105503, "test_string", 105503);

    EXPECT_EQ(atom.atomId, 105503);
    EXPECT_EQ(atom.reverseDomainName, "");
    EXPECT_EQ(atom.values.size(), static_cast<size_t>(1));
    EXPECT_EQ(atom.values[0].get<VendorAtomValue::intValue>(), 105503);
}

}  // namespace stats_log_api_gen
}  // namespace android
