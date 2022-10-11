/*
 * Copyright (C) 2022 The Android Open Source Project
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
package android.test.expresslog;

import androidx.test.filters.SmallTest;
import java.util.InputMismatchException;
import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

@RunWith(JUnit4.class)
public class CounterMetricTest {
  private static final String TAG = CounterMetricTest.class.getSimpleName();

  @Test
  @SmallTest
  public void testLogValidName() {
    boolean result = true;
    try {
      ExpressLog.logCounterIncrement("tex_test.value_telemetry_express_test_counter");
    } catch (IllegalArgumentException e) {
      result = false;
    }

    Assert.assertTrue(result);
  }

  @Test
  @SmallTest
  public void testLogInvalidName() {
    boolean result = true;
    try {
      ExpressLog.logCounterIncrement("tex_test.value_telemetry_express_test_counter2");
    } catch (IllegalArgumentException e) {
      result = false;
    }

    Assert.assertFalse(result);
  }

  @Test
  @SmallTest
  public void testLogInvalidType() {
    boolean result = true;
    try {
      ExpressLog.logCounterIncrement("tex_test.value_telemetry_express_test_unknown");
    } catch (InputMismatchException e) {
      result = false;
    }

    Assert.assertFalse(result);
  }
}
