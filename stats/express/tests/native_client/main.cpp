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

#include <expresslog.h>
#include <getopt.h>

#include <iostream>

void expect_message(int32_t action);

void show_help();

void make_test_logging();

int main(int argc, char* argv[]) {
    static struct option opts[] = {
            {"help", no_argument, 0, 'h'},
            {"test", no_argument, 0, 't'},
    };

    int c = 0;
    while ((c = getopt_long(argc, argv, "ht", opts, nullptr)) != -1) {
        switch (c) {
            case 'h': {
                show_help();
                break;
            }
            case 't': {
                make_test_logging();
                break;
            }
            default: {
                show_help();
                return 1;
            }
        }
    }

    std::cout << "try: logcat | grep \"statsd.*0x1000\"\n";

    return 0;
}

void expect_message(int32_t action) {
    std::cout << "expect the following log in logcat:\n";
    std::cout << "statsd.*(" << action << ")0x10000->\n";
}

void show_help() {
    std::cout << "Telemetry Express Test client\n";
    std::cout << " arguments:\n";
    std::cout << " -h or --help - shows help information\n";
    std::cout << "Please enable statsd logging using 'cmd stats print-logs'";
    std::cout << "\n\n you can use multiple arguments to trigger multiple events.\n";
}

void make_test_logging() {
    // TODO: add validation of metric name on the library side - skip and error log for invalid
    android::expresslog::Counter::logIncrement("tex_test.value_telemetry_express_test_counter");
}
