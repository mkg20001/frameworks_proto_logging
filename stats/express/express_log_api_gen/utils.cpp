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

#include "utils.h"

#include <openssl/sha.h>
#include <stdio.h>

#include <string>
#include <vector>

namespace android {
namespace express {

using namespace std;

vector<string> split(const string& s, const string& delimiters) {
    vector<string> result;

    size_t base = 0;
    size_t found;
    while (true) {
        found = s.find_first_of(delimiters, base);
        result.push_back(s.substr(base, found - base));
        if (found == s.npos) break;
        base = found + 1;
    }

    return result;
}

void sha256(const string& str, unsigned char hash[SHA256_DIGEST_LENGTH]) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);
}

uint64_t hash64(const string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    sha256(str, hash);
    int64_t result = (hash[0] & 0xFF);
    for (int i = 1; i < 8; i++) {
        result |= (hash[i] & 0xFFL) << (i * 8);
    }
    return result;
}

}  // namespace express
}  // namespace android
