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

package com.android.host.statslogapigen;

import static com.google.common.truth.Truth.assertThat;

import static org.junit.Assert.assertNull;
import static org.junit.Assume.assumeTrue;

import com.android.host.statslogapigen.VendorAtomsLog;

import com.android.tradefed.testtype.DeviceJUnit4ClassRunner;
import com.android.tradefed.testtype.junit4.BaseHostJUnit4Test;

import org.junit.After;
import org.junit.Test;
import org.junit.runner.RunWith;

/**
 * Runs the stats-log-api-gen tests for vendor atoms java generated code
*/
@RunWith(DeviceJUnit4ClassRunner.class)
public class VendorAtomCodeGenTest extends BaseHostJUnit4Test {

    /**
     * Tests Java auto generated code for specific vendor atom contains proper ids
     */
    @Test
    public void testAtomIdConstantsGeneration() throws Exception {
        assertThat(VendorAtomsLog.VENDOR_ATOM1).equals(105501);
        assertThat(VendorAtomsLog.VENDOR_ATOM2).equals(105502);
        assertThat(VendorAtomsLog.VENDOR_ATOM4).equals(105504);
    }

    /**
     * Tests Java auto generated code for specific vendor atom contains proper enums
     */
    @Test
    public void testAtomEnumConstantsGeneration() throws Exception {
        assertThat(VendorAtomsLog.VENDOR_ATOM1__TYPE_UNKNOWN).equals(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__TYPE_1).equals(1);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__TYPE_2).equals(2);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__TYPE_3).equals(3);

        assertThat(VendorAtomsLog.VENDOR_ATOM1__ANOTHER_TYPE_UNKNOWN).equals(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ANOTHER_TYPE_1).equals(1);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ANOTHER_TYPE_2).equals(2);
        assertThat(VendorAtomsLog.VENDOR_ATOM1__ANOTHER_TYPE_3).equals(3);

        assertThat(VendorAtomsLog.VENDOR_ATOM2__TYPE_UNKNOWN).equals(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__TYPE_1).equals(1);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__TYPE_2).equals(2);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__TYPE_3).equals(3);

        assertThat(VendorAtomsLog.VENDOR_ATOM2__ANOTHER_TYPE_UNKNOWN).equals(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__ANOTHER_TYPE_1).equals(1);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__ANOTHER_TYPE_2).equals(2);
        assertThat(VendorAtomsLog.VENDOR_ATOM2__ANOTHER_TYPE_3).equals(3);

        assertThat(VendorAtomsLog.VENDOR_ATOM4__TYPE_UNKNOWN).equals(0);
        assertThat(VendorAtomsLog.VENDOR_ATOM4__TYPE_1).equals(1);
    }
}
