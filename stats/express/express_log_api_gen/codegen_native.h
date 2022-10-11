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

#include "codegen.h"

namespace android {
namespace express {

class CodeGeneratorNative : public CodeGenerator {
public:
    CodeGeneratorNative(std::string filePath, std::string namespaces)
        : CodeGenerator(filePath), mNamespaces(namespaces) {
    }

protected:
    std::string mNamespaces;
};

class CodeGeneratorNativeHeader : public CodeGeneratorNative {
public:
    CodeGeneratorNativeHeader(std::string filePath, std::string namespaces)
        : CodeGeneratorNative(filePath, namespaces) {
    }

protected:
    bool generateCodeImpl(FILE* fd, const MetricInfoMap& metricsIds) override;
};

class CodeGeneratorNativeCpp : public CodeGeneratorNative {
public:
    CodeGeneratorNativeCpp(std::string filePath, std::string namespaces, std::string includeHeader)
        : CodeGeneratorNative(filePath, namespaces), mIncludeHeader(includeHeader) {
    }

protected:
    bool generateCodeImpl(FILE* fd, const MetricInfoMap& metricsIds) override;

private:
    std::string mIncludeHeader;
};

}  // namespace express
}  // namespace android
