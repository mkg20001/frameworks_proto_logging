/*
* Copyright (C) 2023 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

package com.android.test.statslogapigen;

import android.frameworks.stats.VendorAtom;

import com.android.test.statslogapigen.VendorAtomsLog;

import static com.google.common.truth.Truth.assertThat;

import org.junit.After;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

/**
 * Runs the stats-log-api-gen tests for vendor atoms java generated code
*/
@RunWith(JUnit4.class)
public class VendorAtomCodeGenTest {

    /**
     * Tests Java auto generated code for specific vendor atom contains proper ids
     */
    @Test
    public void testAtomIdConstantsGeneration() throws Exception {
        assertThat(VendorAtomsLog.VENDOR_ATOM1).isEqualTo(105501);
        assertThat(VendorAtomsLog.VENDOR_ATOM2).isEqualTo(105502);
        assertThat(VendorAtomsLog.VENDOR_ATOM4).isEqualTo(105504);
    }

    /**
     * Tests Java auto generated code for specific vendor atom contains proper enums
     */
    @Test
    public void testAtomEnumConstantsGeneration() throws Exception {
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE__TYPE_UNKNOWN).isEqualTo(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE__TYPE_1).isEqualTo(1);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE__TYPE_2).isEqualTo(2);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE__TYPE_3).isEqualTo(3);

        assertThat(VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE2__ANOTHER_TYPE_UNKNOWN).isEqualTo(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE2__ANOTHER_TYPE_1).isEqualTo(1);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE2__ANOTHER_TYPE_2).isEqualTo(2);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE2__ANOTHER_TYPE_3).isEqualTo(3);

        assertThat(VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE__TYPE_UNKNOWN).isEqualTo(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE__TYPE_1).isEqualTo(1);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE__TYPE_2).isEqualTo(2);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE__TYPE_3).isEqualTo(3);

        assertThat(VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE2__ANOTHER_TYPE_UNKNOWN).isEqualTo(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE2__ANOTHER_TYPE_1).isEqualTo(1);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE2__ANOTHER_TYPE_2).isEqualTo(2);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE2__ANOTHER_TYPE_3).isEqualTo(3);

        assertThat(VendorAtomsLog.VENDOR_ATOM4__ENUM_TYPE4__TYPE_UNKNOWN).isEqualTo(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM4__ENUM_TYPE4__TYPE_1).isEqualTo(1);
    }

    final int kTestIntValue = 100;
    final long kTestLongValue = Long.MAX_VALUE - kTestIntValue;
    final float kTestFloatValue = (float)kTestIntValue / kTestLongValue;
    final boolean kTestBoolValue = true;
    final String kTestStringValue = "test_string";
    final String kTestStringValue2 = "test_string2";

    private interface CreateVendorAtom1ApiWrapper {
        // Method signatures of pointed method
        VendorAtom methodSignature(int atomId, String rdn, int enumField1,
                int enumField2, int intField, long longField, float floatField,
                boolean booleanField, int enumField3, int enumField4);
    }

    @Test
    public void testCreateVendorAtom1ApiGen() throws Exception {
        CreateVendorAtom1ApiWrapper funcWrapper = VendorAtomsLog::createVendorAtom;

        VendorAtom atom = funcWrapper.methodSignature(VendorAtomsLog.VENDOR_ATOM1,
                kTestStringValue,
                VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE__TYPE_1,
                VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE__TYPE_2,
                kTestIntValue, kTestLongValue, kTestFloatValue, kTestBoolValue,
                VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE2__ANOTHER_TYPE_2,
                VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE2__ANOTHER_TYPE_3);

        assertThat(atom.atomId).isEqualTo(VendorAtomsLog.VENDOR_ATOM1);
        assertThat(atom.reverseDomainName).isEqualTo(kTestStringValue);
        assertThat(atom.values.length).isEqualTo(8);
        assertThat(atom.values[0].getIntValue()).isEqualTo(
                VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE__TYPE_1);
        assertThat(atom.values[1].getIntValue()).isEqualTo(
                VendorAtomsLog.VENDOR_ATOM1__ENUM_TYPE__TYPE_2);
        assertThat(atom.values[2].getIntValue()).isEqualTo(kTestIntValue);
        assertThat(atom.values[3].getLongValue()).isEqualTo(kTestLongValue);
        assertThat(atom.values[4].getFloatValue()).isEqualTo(kTestFloatValue);
        assertThat(atom.values[5].getBoolValue()).isEqualTo(kTestBoolValue);
        assertThat(atom.values[6].getIntValue()).isEqualTo(
                VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE2__ANOTHER_TYPE_2);
        assertThat(atom.values[7].getIntValue()).isEqualTo(
                VendorAtomsLog.VENDOR_ATOM2__ENUM_TYPE2__ANOTHER_TYPE_3);
    }

    private interface CreateVendorAtom3ApiWrapper {
        // Method signatures of pointed method
        VendorAtom methodSignature(int atomId, String rdn, int arg1);
    }

    @Test
    public void testCreateVendorAtom3ApiGen() throws Exception {
        CreateVendorAtom3ApiWrapper funcWrapper = VendorAtomsLog::createVendorAtom;

        VendorAtom atom = funcWrapper.methodSignature(VendorAtomsLog.VENDOR_ATOM3,
                kTestStringValue, kTestIntValue);

        assertThat(atom.atomId).isEqualTo(VendorAtomsLog.VENDOR_ATOM3);
        assertThat(atom.reverseDomainName).isEqualTo(kTestStringValue);
        assertThat(atom.values.length).isEqualTo(1);
        assertThat(atom.values[0].getIntValue()).isEqualTo(kTestIntValue);
    }

    private interface CreateVendorAtom4ApiWrapper {
        // Method signatures of pointed method
        VendorAtom methodSignature(int atomId, String rdn, float arg2, int arg3,
        long arg4, boolean arg5, int arg6, boolean[] arg7,
        float[] arg8, int[] arg9, long[] arg10, String[] arg11, int[] arg12);
    }

    @Test
    public void testCreateVendorAtom4ApiGen() throws Exception {
        CreateVendorAtom4ApiWrapper funcWrapper = VendorAtomsLog::createVendorAtom;

        final boolean[] repeatedBool = {true, false, true};
        final float[] repeatedFloat = {
                kTestFloatValue, kTestFloatValue + 1.f, kTestFloatValue + 2.f};
        final int[] repeatedInt = {kTestIntValue, kTestIntValue + 1, kTestIntValue + 2};
        final long[] repeatedLong = {kTestLongValue, kTestLongValue + 1, kTestLongValue + 2};
        final String[] repeatedString = {kTestStringValue, kTestStringValue2, kTestStringValue};
        final int[] repeatedEnum = {
                VendorAtomsLog.VENDOR_ATOM4__ENUM_TYPE4__TYPE_1,
                VendorAtomsLog.VENDOR_ATOM4__ENUM_TYPE4__TYPE_UNKNOWN,
                VendorAtomsLog.VENDOR_ATOM4__ENUM_TYPE4__TYPE_1};

        VendorAtom atom = funcWrapper.methodSignature(VendorAtomsLog.VENDOR_ATOM4, kTestStringValue,
                kTestFloatValue, kTestIntValue, kTestLongValue, kTestBoolValue,
                VendorAtomsLog.VENDOR_ATOM4__ENUM_TYPE4__TYPE_1, repeatedBool, repeatedFloat,
                repeatedInt, repeatedLong, repeatedString, repeatedEnum);

        assertThat(atom.atomId).isEqualTo(VendorAtomsLog.VENDOR_ATOM4);
        assertThat(atom.reverseDomainName).isEqualTo(kTestStringValue);
        assertThat(atom.values.length).isEqualTo(11);
        assertThat(atom.values[0].getFloatValue()).isEqualTo(kTestFloatValue);
        assertThat(atom.values[1].getIntValue()).isEqualTo(kTestIntValue);
        assertThat(atom.values[2].getLongValue()).isEqualTo(kTestLongValue);
        assertThat(atom.values[3].getBoolValue()).isEqualTo(kTestBoolValue);
        assertThat(atom.values[4].getIntValue()).isEqualTo(
                VendorAtomsLog.VENDOR_ATOM4__ENUM_TYPE4__TYPE_1);

        assertThat(atom.values[5].getRepeatedBoolValue()).isEqualTo(repeatedBool);
        assertThat(atom.values[6].getRepeatedFloatValue()).isEqualTo(repeatedFloat);
        assertThat(atom.values[7].getRepeatedIntValue()).isEqualTo(repeatedInt);
        assertThat(atom.values[8].getRepeatedLongValue()).isEqualTo(repeatedLong);
        assertThat(atom.values[9].getRepeatedStringValue()).isNotNull();
        assertThat(atom.values[9].getRepeatedStringValue().length).isEqualTo(
                  repeatedString.length);
        final String[] repeatedStringValue = atom.values[9].getRepeatedStringValue();
        for (int i = 0; i < repeatedString.length; i++) {
            assertThat(repeatedString[i]).isEqualTo(repeatedStringValue[i]);
        }
        assertThat(atom.values[10].getRepeatedIntValue()).isEqualTo(repeatedEnum);
    }

}
