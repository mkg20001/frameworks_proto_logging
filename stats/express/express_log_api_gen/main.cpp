//
// Copyright (C) 2022 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include "log.h"

#include <gflags/gflags.h>

#include <algorithm>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "catalog.h"
#include "codegen_java.h"
#include "codegen_native.h"

constexpr char DEFAULT_CONFIG_DIR[] = "frameworks/proto_logging/stats/express/catalog";
constexpr char DEFAULT_CPP_NAMESPACE[] = "android,expresslog";
constexpr char DEFAULT_CPP_HEADER_INCLUDE[] = "express-gen.h";

DEFINE_string(configDir, DEFAULT_CONFIG_DIR, "path to cfg files");
DEFINE_string(cpp, "", "path to the generated cpp file");
DEFINE_string(header, "", "path to the generated header file");
DEFINE_string(include, DEFAULT_CPP_HEADER_INCLUDE, "header file name to be included into cpp");
DEFINE_string(namespaces, DEFAULT_CPP_NAMESPACE, "C++ namespace to be used in sources");
DEFINE_string(java, "", "path to the generated Java class file");
DEFINE_string(javaPackage, "", "generated Java package name");
DEFINE_string(javaClass, "", "generated Java class name");
DEFINE_string(domain, "", "Express Metric domain name (the part which comes before the first dot)");

namespace android {
namespace express {

std::vector<std::unique_ptr<CodeGenerator>> createCodeGenerators() {
    std::vector<std::unique_ptr<CodeGenerator>> result;

    if (FLAGS_cpp.size()) {
        result.push_back(std::make_unique<CodeGeneratorNativeCpp>(
                CodeGeneratorNativeCpp(FLAGS_cpp, FLAGS_namespaces, FLAGS_include)));
    }

    if (FLAGS_header.size()) {
        result.push_back(std::make_unique<CodeGeneratorNativeHeader>(
                CodeGeneratorNativeHeader(FLAGS_header, FLAGS_namespaces)));
    }

    if (FLAGS_java.size()) {
        result.push_back(std::make_unique<CodeGeneratorJava>(
                CodeGeneratorJava(FLAGS_java, FLAGS_javaPackage, FLAGS_javaClass)));
    }

    return result;
}

bool generateLoggingCode(const MetricInfoMap& metricsIds) {
    const auto codeGenerators = createCodeGenerators();
    for (const auto& codeGen : codeGenerators) {
        if (!codeGen->generateCode(metricsIds)) return false;
    }
    return true;
}

MetricInfoMap filterMetricsIds(const MetricInfoMap& metricsIds) {
    LOGD("Domain is %s\n", FLAGS_domain.c_str());

    constexpr size_t MAX_REGEX_LEN = 256;
    char regExStr[MAX_REGEX_LEN];
    snprintf(regExStr, MAX_REGEX_LEN, ".*%s.*", FLAGS_domain.c_str());
    LOGD("RegEx is %s\n", regExStr);

    const std::regex str_expr(regExStr);
    auto pred = [&](const MetricInfoMap::value_type& item) {
        LOGD("checking metric %s\n", item.first.c_str());
        return std::regex_match(item.first, str_expr);
    };

    MetricInfoMap filteredMetricIds;
    std::copy_if(begin(metricsIds), end(metricsIds),
                 std::inserter(filteredMetricIds, filteredMetricIds.end()), pred);
    return filteredMetricIds;
}

/**
 * Do the argument parsing and execute the tasks.
 */
static int run() {
    std::unordered_map<std::string, ExpressMetric> metrics;
    if (!readCatalog(FLAGS_configDir.c_str(), metrics)) {
        return -1;
    }

    MetricInfoMap metricsIds;
    if (!generateMetricsIds(metrics, metricsIds)) {
        return -1;
    }

    MetricInfoMap filteredMetricIds = filterMetricsIds(metricsIds);
    if (filteredMetricIds.size() == 0) {
        LOGE("No metrics detected for domain %s\n", FLAGS_domain.c_str());
        return -1;
    }

    if (!generateLoggingCode(filteredMetricIds)) {
        return -1;
    }

    return 0;
}

}  // namespace express
}  // namespace android

int main(int argc, char** argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    google::ParseCommandLineFlags(&argc, &argv, false);
    return android::express::run();
}
