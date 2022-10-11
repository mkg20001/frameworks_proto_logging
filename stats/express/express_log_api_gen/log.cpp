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

#include <openssl/sha.h>
#include <string.h>

#include <algorithm>

namespace android {
namespace express {

void sha256(const std::string& str, unsigned char hash[SHA256_DIGEST_LENGTH]) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
}

uint64_t hash64(const std::string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    sha256(str, hash);
    int64_t result = (hash[0] & 0xFF);
    for (int i = 1; i < 8; i++) {
        result |= (hash[i] & 0xFFL) << (i * 8);
    }
    return result;
}

ExecutionMeasure::ExecutionMeasure(std::string name) : mName(name) {
    mStart = std::chrono::high_resolution_clock::now();
}

ExecutionMeasure::~ExecutionMeasure() {
    const auto stop = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - mStart);
    LOGD("%s took to complete %lld microseconds\n", mName.c_str(), duration.count());
}

}  // namespace express
}  // namespace android
