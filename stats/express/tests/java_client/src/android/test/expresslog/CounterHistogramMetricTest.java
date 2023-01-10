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
package android.test.expresslog;

import androidx.test.filters.SmallTest;
import com.android.internal.expresslog.CounterHistogram;
import org.junit.Assert;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

@RunWith(JUnit4.class)
public class CounterHistogramMetricTest {
  private static final String TAG = CounterHistogramMetricTest.class.getSimpleName();

  @Test
  @SmallTest
  public void testFixedRangeInt() {

    final int binCount = 10;
    final int minValue = 100;
    final int maxValue = 100000;

    CounterHistogram metric =
        new CounterHistogram(
            "tex_test.value_telemetry_express_fixed_range_histogram",
            new CounterHistogram.FixedRangeOptions(binCount, minValue, maxValue));

    // logging underflow sample
    metric.logSample(minValue - 1);

    // logging overflow sample
    metric.logSample(maxValue + 1);

    // logging valid samples per bin

    final int binSize = (maxValue - minValue + 1) / binCount;

    for(int i = 0; i < binCount; i++) {
        metric.logSample(minValue + binSize * i);
    }

    Assert.assertTrue(true);
  }

}
