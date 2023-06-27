/*
 * Copyright (C) 2023, The Android Open Source Project
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

#include <google/protobuf/message.h>
#include <stdint.h>

#include <map>
#include <string>
#include <unordered_map>

#include "frameworks/proto_logging/stats/express/express_config.pb.h"

namespace android {
namespace express {

/**
 * @brief Recursively parses folder, locates all cfg files and reads ExpressMetric configs
 */
bool readCatalog(const char* path, std::map<std::string, ExpressMetric>& metrics);

bool generateMetricsIds(const std::map<std::string, ExpressMetric>& metrics,
                        std::unordered_map<std::string, int64_t>& metricsIds);

}  // namespace express
}  // namespace android
