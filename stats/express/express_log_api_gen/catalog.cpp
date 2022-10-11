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

#include "log.h"

#include "catalog.h"

#include <google/protobuf/text_format.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <regex>
#include <sstream>
#include <unordered_set>
#include <vector>

#include "utils.h"

using std::string;
using std::unordered_map;
using std::unordered_set;
using std::vector;

namespace fs = std::filesystem;
namespace pb = google::protobuf;

namespace android {
namespace express {

static bool validateMetricId(const string& metricId) {
    // validation is done according to regEx
    static const std::regex str_expr("[a-z]+[a-z_0-9]*[.](value_)[a-z]+[a-z_0-9]*");

    if (!std::regex_match(metricId, str_expr)) {
        LOGE("Metric Id does not follow naming convention: %s\n", metricId.c_str());
        return false;
    }

    return true;
}

static vector<fs::path> buildConfigFilesList(fs::path configDirPath) {
    vector<fs::path> result;

    for (const auto& entry : fs::directory_iterator(configDirPath)) {
        LOGD("Checking %s\n", entry.path().c_str());
        LOGD("  is_regular_file %d\n", fs::is_regular_file(entry.path()));
        LOGD("  extension %s\n", entry.path().extension().c_str());

        if (fs::is_regular_file(entry.path()) &&
            strcmp(entry.path().extension().c_str(), ".cfg") == 0) {
            LOGD("located config: %s\n", entry.path().c_str());
            result.push_back(entry.path());
        }
    }

    return result;
}

static bool readMetrics(const fs::path& cfgFile, unordered_map<string, ExpressMetric>& metrics) {
    std::ifstream fileStream(cfgFile.c_str());
    std::stringstream buffer;
    buffer << fileStream.rdbuf();

    LOGD("Metrics config content:\n %s\n", buffer.str().c_str());

    ExpressMetricConfigFile cfMessage;
    if (!pb::TextFormat::ParseFromString(buffer.str(), &cfMessage)) {
        LOGE("Can not process config file %s\n", cfgFile.c_str());
        return false;
    }

    LOGD("Metrics amount in the file %d\n", cfMessage.express_metric_size());

    for (int i = 0; i < cfMessage.express_metric_size(); i++) {
        const ExpressMetric& metric = cfMessage.express_metric(i);

        if (!metric.has_id()) {
            LOGE("No id is defined for metric index %d. Skip\n", i);
            continue;
        }

        LOGI("Metric: %s\n", metric.id().c_str());

        if (!validateMetricId(metric.id())) {
            LOGE("Metric id does not follow naming convention. Skip\n");
            continue;
        }

        if (metrics.find(metric.id()) != metrics.end()) {
            LOGE("Metric id redefinition error, broken uniqueness rule. Skip\n");
            continue;
        }

        metrics[metric.id()] = metric;
    }

    return true;
}

bool readCatalog(const char* configDir, unordered_map<string, ExpressMetric>& metrics) {
    MEASURE_FUNC();
    auto configDirPath = fs::current_path();
    configDirPath /= configDir;

    LOGD("Config dir %s\n", configDirPath.c_str());

    const vector<fs::path> configFilesList = buildConfigFilesList(configDirPath);

    for (const auto& configFilePath : configFilesList) {
        readMetrics(configFilePath, metrics);
    }
    LOGD("Catalog dir %s processed\n", configDirPath.c_str());

    return true;
}

bool generateMetricsIds(const unordered_map<string, ExpressMetric>& metrics,
                        MetricInfoMap& metricsIds) {
    MEASURE_FUNC();
    unordered_set<int64_t> currentHashes;
    for (const auto& metricInfo : metricsIds) {
        currentHashes.insert(metricInfo.second.hash());
    }

    vector<string> updateNamesList;
    for (const auto& name : metrics) {
        updateNamesList.push_back(name.first);
    }
    sort(updateNamesList.begin(), updateNamesList.end());

    for (const auto& metricId : updateNamesList) {
        // check if it is a new name which needs generated Id
        if (metricsIds.find(metricId) != metricsIds.end()) {
            LOGE("Detected existing metric id %s.\n", metricId.c_str());
            return false;
        } else {
            int64_t hashId = hash64(metricId);
            // check if there is a collision
            if (currentHashes.find(hashId) != currentHashes.end()) {
                LOGE("Detected collision for new meric %s\n", metricId.c_str());
                do {
                    hashId = hashId++ % std::numeric_limits<int64_t>::max();
                } while (currentHashes.find(hashId) != currentHashes.end());
            }

            currentHashes.insert(hashId);

            // generating new MetricInfo
            MetricInfo pbMessage;
            pbMessage.set_id(metricId);
            pbMessage.set_hash(hashId);
            pbMessage.set_type(metrics.find(metricId)->second.type());

            metricsIds[metricId] = pbMessage;
        }
    }

    return true;
}

}  // namespace express
}  // namespace android
