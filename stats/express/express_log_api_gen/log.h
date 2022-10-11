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

#pragma once

#include <chrono>
#include <string>

#define DEBUG true

#define LOGI(...)                     \
    do {                              \
        fprintf(stdout, "[INFO] ");   \
        fprintf(stdout, __VA_ARGS__); \
    } while (0)

#define LOGD(...)                               \
    do {                                        \
        if (DEBUG) fprintf(stdout, "[DEBUG] "); \
        fprintf(stdout, __VA_ARGS__);           \
    } while (0)

#define LOGE(...)                     \
    do {                              \
        fprintf(stdout, "[ERROR] ");  \
        fprintf(stdout, __VA_ARGS__); \
    } while (0)

namespace android {
namespace express {

uint64_t hash64(const std::string& str);

#define MEASURE_FUNC() ExecutionMeasure func_execution_measure(__func__);

class ExecutionMeasure final {
public:
    ExecutionMeasure(std::string name);
    ~ExecutionMeasure();

private:
    std::string mName;
    std::chrono::time_point<std::chrono::high_resolution_clock> mStart;
};

}  // namespace express
}  // namespace android
