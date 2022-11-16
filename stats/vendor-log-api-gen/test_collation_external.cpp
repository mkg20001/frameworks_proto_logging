/*
 * Copyright (C) 2017, The Android Open Source Project
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

#include <Collation.h>
#include <google/protobuf/compiler/importer.h>
#include <gtest/gtest.h>
#include <stdio.h>

#include <filesystem>

namespace android {
namespace vendor_log_api_gen {

using namespace stats_log_api_gen;

using std::map;
using std::vector;

namespace fs = std::filesystem;

/**
 * Return whether the map contains a vector of the elements provided.
 */
static bool map_contains_vector(const SignatureInfoMap& s, int count, ...) {
    va_list args;
    vector<java_type_t> v(count);

    va_start(args, count);
    for (int i = 0; i < count; i++) {
        v[i] = static_cast<java_type_t>(va_arg(args, int));
    }
    va_end(args);

    return s.find(v) != s.end();
}

/**
 * Expect that the provided map contains the elements provided.
 */
#define EXPECT_MAP_CONTAINS_SIGNATURE(s, ...)                    \
    do {                                                         \
        int count = sizeof((int[]){__VA_ARGS__}) / sizeof(int);  \
        EXPECT_TRUE(map_contains_vector(s, count, __VA_ARGS__)); \
    } while (0)

/** Expects that the provided atom has no enum values for any field. */
#define EXPECT_NO_ENUM_FIELD(atom)                                           \
    do {                                                                     \
        for (vector<AtomField>::const_iterator field = atom->fields.begin(); \
             field != atom->fields.end(); field++) {                         \
            EXPECT_TRUE(field->enumValues.empty());                          \
        }                                                                    \
    } while (0)

/** Expects that exactly one specific field has expected enum values. */
#define EXPECT_HAS_ENUM_FIELD(atom, field_name, values)                      \
    do {                                                                     \
        for (vector<AtomField>::const_iterator field = atom->fields.begin(); \
             field != atom->fields.end(); field++) {                         \
            if (field->name == field_name) {                                 \
                EXPECT_EQ(field->enumValues, values);                        \
            } else {                                                         \
                EXPECT_TRUE(field->enumValues.empty());                      \
            }                                                                \
        }                                                                    \
    } while (0)

class MFErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector {
public:
    virtual void AddError(const std::string& filename, int line, int column,
                          const std::string& message) {
        fprintf(stderr, "[Error] %s:%d:%d - %s", filename.c_str(), line, column, message.c_str());
    }
};

/**
 * Test a correct collation, with all the types.
 */
TEST(CollationTestExternal, CollateStats) {
    google::protobuf::compiler::DiskSourceTree source_tree;
    source_tree.MapPath("", fs::current_path().c_str());

    MFErrorCollector errorCollector;

    google::protobuf::compiler::Importer importer(&source_tree, &errorCollector);
    const google::protobuf::FileDescriptor* fd = importer.Import("test.proto");
    EXPECT_TRUE(fd != NULL);

    const google::protobuf::Descriptor* externalFileAtoms = fd->FindMessageTypeByName("Event");
    EXPECT_TRUE(externalFileAtoms != nullptr);

    Atoms atoms;
    const int errorCount = collate_atoms(externalFileAtoms, DEFAULT_MODULE_NAME, &atoms);

    EXPECT_EQ(0, errorCount);
    EXPECT_EQ(4ul, atoms.signatureInfoMap.size());

    // IntAtom, AnotherIntAtom
    EXPECT_MAP_CONTAINS_SIGNATURE(atoms.signatureInfoMap, JAVA_TYPE_INT);

    // OutOfOrderAtom
    EXPECT_MAP_CONTAINS_SIGNATURE(atoms.signatureInfoMap, JAVA_TYPE_INT, JAVA_TYPE_INT);

    // AllTypesAtom
    EXPECT_MAP_CONTAINS_SIGNATURE(atoms.signatureInfoMap,
                                  JAVA_TYPE_ATTRIBUTION_CHAIN,  // AttributionChain
                                  JAVA_TYPE_FLOAT,              // float
                                  JAVA_TYPE_LONG,               // int64
                                  JAVA_TYPE_LONG,               // uint64
                                  JAVA_TYPE_INT,                // int32
                                  JAVA_TYPE_BOOLEAN,            // bool
                                  JAVA_TYPE_STRING,             // string
                                  JAVA_TYPE_INT,                // uint32
                                  JAVA_TYPE_INT,                // AnEnum
                                  JAVA_TYPE_FLOAT_ARRAY,        // repeated float
                                  JAVA_TYPE_LONG_ARRAY,         // repeated int64
                                  JAVA_TYPE_INT_ARRAY,          // repeated int32
                                  JAVA_TYPE_BOOLEAN_ARRAY,      // repeated bool
                                  JAVA_TYPE_STRING_ARRAY        // repeated string
    );

    // RepeatedEnumAtom
    EXPECT_MAP_CONTAINS_SIGNATURE(atoms.signatureInfoMap, JAVA_TYPE_INT_ARRAY);

    EXPECT_EQ(5ul, atoms.decls.size());

    AtomDeclSet::const_iterator atomIt = atoms.decls.begin();
    EXPECT_EQ(1, (*atomIt)->code);
    EXPECT_EQ("int_atom", (*atomIt)->name);
    EXPECT_EQ("IntAtom", (*atomIt)->message);
    EXPECT_NO_ENUM_FIELD((*atomIt));
    atomIt++;

    EXPECT_EQ(2, (*atomIt)->code);
    EXPECT_EQ("out_of_order_atom", (*atomIt)->name);
    EXPECT_EQ("OutOfOrderAtom", (*atomIt)->message);
    EXPECT_NO_ENUM_FIELD((*atomIt));
    atomIt++;

    EXPECT_EQ(3, (*atomIt)->code);
    EXPECT_EQ("another_int_atom", (*atomIt)->name);
    EXPECT_EQ("AnotherIntAtom", (*atomIt)->message);
    EXPECT_NO_ENUM_FIELD((*atomIt));
    atomIt++;

    EXPECT_EQ(4, (*atomIt)->code);
    EXPECT_EQ("all_types_atom", (*atomIt)->name);
    EXPECT_EQ("AllTypesAtom", (*atomIt)->message);
    map<int, string> enumValues;
    enumValues[0] = "VALUE0";
    enumValues[1] = "VALUE1";
    EXPECT_HAS_ENUM_FIELD((*atomIt), "enum_field", enumValues);
    atomIt++;

    EXPECT_EQ(5, (*atomIt)->code);
    EXPECT_EQ("repeated_enum_atom", (*atomIt)->name);
    EXPECT_EQ("RepeatedEnumAtom", (*atomIt)->message);
    enumValues[0] = "VALUE0";
    enumValues[1] = "VALUE1";
    EXPECT_HAS_ENUM_FIELD((*atomIt), "repeated_enum_field", enumValues);
    atomIt++;

    EXPECT_EQ(atoms.decls.end(), atomIt);
}

}  // namespace vendor_log_api_gen
}  // namespace android
