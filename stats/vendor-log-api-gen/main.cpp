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

#include <Collation.h>
#include <gflags/gflags.h>
#include <google/protobuf/compiler/importer.h>

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

constexpr char DEFAULT_CPP_NAMESPACE[] = "android,vendorlog";

DEFINE_string(proto, "", "proto with atoms definitions to parse");
DEFINE_string(cpp, "", "path to the generated cpp file");
DEFINE_string(header, "", "path to the generated header file");
DEFINE_string(include, "", "header file name to be included into cpp");
DEFINE_string(namespaces, DEFAULT_CPP_NAMESPACE, "C++ namespace to be used in sources");
DEFINE_string(java, "", "path to the generated Java class file");
DEFINE_string(javaPackage, "", "generated Java package name");
DEFINE_string(javaClass, "", "generated Java class name");
DEFINE_string(module, "", "optional, module name to generate outputs for");

namespace android {
namespace vendor_log_api_gen {

using namespace stats_log_api_gen;

namespace fs = std::filesystem;

class MFErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector {
public:
    virtual void AddError(const std::string& filename, int line, int column,
                          const std::string& message) {
        printf("Error %s:%d:%d - %s\n", filename.c_str(), line, column, message.c_str());
    }
};

/**
 * Do the argument parsing and execute the tasks.
 */
static int run() {
    if(FLAGS_proto.size() == 0) {
        LOGI("Please specify the input proto file with --proto <filename> option");
        return 0;
    }

    // parse external proto file
    google::protobuf::compiler::DiskSourceTree dst;

    if (getenv("ANDROID_BUILD_TOP") != NULL) {

        fs::path top = getenv("ANDROID_BUILD_TOP");

        fs::path protobufSrc = top;
        protobufSrc /= "external/protobuf/src/";

        fs::path hwPixelstats = top;
        hwPixelstats /= "hardware/google/pixel/pixelstats/";

        LOGD("ANDROID_BUILD_TOP = %s\n", getenv("ANDROID_BUILD_TOP"));

        dst.MapPath("", top.c_str());
        dst.MapPath("", protobufSrc.c_str());
        dst.MapPath("", hwPixelstats.c_str());
    }

    MFErrorCollector errorCollector;

    google::protobuf::compiler::Importer importer(&dst, &errorCollector);
    const google::protobuf::FileDescriptor* fd = importer.Import(FLAGS_proto);

    LOGD("FileDescriptor obtained %p\n", fd);

    if (fd == NULL) {
        LOGE("Proto file parsing error");
        return 1;
    }

    const google::protobuf::Descriptor* externalFileAtoms = fd->FindMessageTypeByName("Atom");

    if (externalFileAtoms != NULL) {
        LOGE("Proto file does not contain Atom message");
        return 1;
    }

    LOGD("Atom message located in external file %p\n", externalFileAtoms);

    // Collate the parameters
    Atoms atoms;

    const int errorCount = collate_atoms(externalFileAtoms, FLAGS_module, &atoms);
    if (errorCount != 0) {
        LOGE("Atoms parsing error");
        return 1;
    }

    // Code generation

    return 0;
}

}  // namespace vendor_log_api_gen
}  // namespace android

int main(int argc, char** argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    google::ParseCommandLineFlags(&argc, &argv, false);
    return android::vendor_log_api_gen::run();
}
