/*
 * Copyright (C) 2021, The Android Open Source Project
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

#include "rust_writer.h"

#include "utils.h"

// Note that we prepend _ to variable names to avoid using Rust language keyword.
// E.g., a variable named "type" would not compile.

namespace android {
namespace stats_log_api_gen {

const char* rust_type_name(java_type_t type) {
    switch (type) {
        case JAVA_TYPE_BOOLEAN:
            return "bool";
        case JAVA_TYPE_INT:
        case JAVA_TYPE_ENUM:
            return "i32";
        case JAVA_TYPE_LONG:
            return "i64";
        case JAVA_TYPE_FLOAT:
            return "f32";
        case JAVA_TYPE_DOUBLE:
            return "f64";
        case JAVA_TYPE_STRING:
            return "&str";
        case JAVA_TYPE_BYTE_ARRAY:
            return "&[u8]";
        default:
            return "UNKNOWN";
    }
}

static string make_camel_case_name(const string& str) {
    string result;
    const int N = str.size();
    bool justSawUnderscore = false;
    for (int i = 0; i < N; i++) {
        char c = str[i];
        if (c == '_') {
            justSawUnderscore = true;
            // Don't add the underscore to our result
        } else if (i == 0 || justSawUnderscore) {
            result += toupper(c);
            justSawUnderscore = false;
        } else {
            result += tolower(c);
        }
    }
    return result;
}

static void write_rust_method_signature(FILE* out, const char* namePrefix,
                                        const AtomDecl& atomDecl,
                                        const AtomDecl& attributionDecl,
                                        bool isCode,
                                        bool isNonChained) {
    // To make the generated code pretty, add newlines between arguments.
    const char* separator = (isCode ? "\n" : " ");
    fprintf(out, "pub fn %s%s(", namePrefix, atomDecl.name.c_str());
    if (isCode) {
        fprintf(out, "\n");
    }
    if (atomDecl.oneOfName == ONEOF_PULLED_ATOM_NAME) {
        if (isCode) {
            fprintf(out, "    ");
        }
        fprintf(out, "__pulled_data: &mut AStatsEventList,%s", separator);
    }
    for (int i = 0; i < atomDecl.fields.size(); i++) {
        const AtomField &atomField = atomDecl.fields[i];
        const java_type_t& type = atomField.javaType;
        if (type == JAVA_TYPE_ATTRIBUTION_CHAIN) {
            for (int j = 0; j < attributionDecl.fields.size(); j++) {
                const AtomField& chainField = attributionDecl.fields[j];
                if (isCode) {
                    fprintf(out, "    ");
                }
                fprintf(out, "__chain_%s: &[%s],%s", chainField.name.c_str(),
                        rust_type_name(chainField.javaType), separator);
            }
        } else {
            if (isCode) {
                fprintf(out, "    ");
            }
            // Other arguments can have the same name as these non-chained ones,
            // so prepend something.
            if (isNonChained && i < 2) {
                fprintf(out, "__non_chained");
            }
            fprintf(out, "_%s: %s,%s", atomField.name.c_str(), rust_type_name(type),
                    separator);
        }
    }
    fprintf(out, ") -> Result<(), StatsError>");
    if (isCode) {
        fprintf(out, " {");
    }
    fprintf(out, "\n");
}

static void write_rust_usage(FILE* out, const string& method_name,
                             const shared_ptr<AtomDecl> atom,
                             const AtomDecl& attributionDecl,
                             bool isNonChained) {
    // Key value pairs not supported in Rust because they're not supported in native.
    if (std::find_if(atom->fields.begin(), atom->fields.end(),
                     [](const AtomField &atomField) {
                         return atomField.javaType == JAVA_TYPE_KEY_VALUE_PAIR;
                     }) != atom->fields.end()) {
        fprintf(out, "     * Key value pairs are unsupported in Rust.\n");
        return;
    }
    fprintf(out, "     * Definition: ");
    write_rust_method_signature(out, method_name.c_str(), *atom, attributionDecl,
                                false, isNonChained);
}

static void write_rust_atom_constants(FILE* out, const Atoms& atoms,
                                      const AtomDecl& attributionDecl) {
    fprintf(out, "/**\n");
    fprintf(out, " * Constants for atom codes.\n");
    fprintf(out, " */\n");
    fprintf(out, "#[allow(dead_code)]\n");
    fprintf(out, "enum Atoms {\n");

    std::map<int, AtomDeclSet::const_iterator> atom_code_to_non_chained_decl_map;
    build_non_chained_decl_map(atoms, &atom_code_to_non_chained_decl_map);

    for (const shared_ptr<AtomDecl>& atomDecl : atoms.decls) {
        string constant = make_camel_case_name(atomDecl->name);
        fprintf(out, "\n");
        fprintf(out, "    /**\n");
        fprintf(out, "     * %s %s\n", atomDecl->message.c_str(), atomDecl->name.c_str());
        write_rust_usage(out, "stats_write_", atomDecl, attributionDecl, false);

        auto non_chained_decl = atom_code_to_non_chained_decl_map.find(atomDecl->code);
        if (non_chained_decl != atom_code_to_non_chained_decl_map.end()) {
            write_rust_usage(out, "stats_write_non_chained_", *non_chained_decl->second,
                             attributionDecl, true);
        }
        fprintf(out, "     */\n");
        fprintf(out, "    %s = %d,\n", constant.c_str(), atomDecl->code);
    }

    fprintf(out, "\n");
    fprintf(out, "}\n");
    fprintf(out, "\n");
}

static void write_rust_atom_constant_values(FILE* out, const Atoms& atoms) {
    fprintf(out, "//\n");
    fprintf(out, "// Constants for enum values\n");
    fprintf(out, "//\n\n");
    for (const auto& atomDecl : atoms.decls) {
        for (const AtomField& field : atomDecl->fields) {
            if (field.javaType == JAVA_TYPE_ENUM) {
                fprintf(out, "// Values for %s.%s\n", atomDecl->message.c_str(),
                        field.name.c_str());
                for (map<int, string>::const_iterator value = field.enumValues.begin();
                     value != field.enumValues.end(); value++) {
                    fprintf(out, "pub const %s__%s__%s: i32 = %d;\n",
                            make_constant_name(atomDecl->message).c_str(),
                            make_constant_name(field.name).c_str(),
                            make_constant_name(value->second).c_str(), value->first);
                }
                fprintf(out, "\n");
            }
        }
    }
}

static void write_rust_annotation_constants(FILE* out) {
    fprintf(out, "// Annotation constants.\n");
    // The annotation names are all prefixed with AnnotationId.
    // Ideally that would go into the enum name instead,
    // but I don't want to modify the actual name strings in case they change in the future.
    fprintf(out, "#[allow(clippy::enum_variant_names)]\n");
    fprintf(out, "#[repr(u8)]\n");
    fprintf(out, "enum Annotations {\n");

    const map<AnnotationId, string>& ANNOTATION_ID_CONSTANTS = get_annotation_id_constants();
    for (const auto& [id, name] : ANNOTATION_ID_CONSTANTS) {
        fprintf(out, "    %s = %hhu,\n", make_camel_case_name(name).c_str(), id);
    }
    fprintf(out, "}\n\n");
}

// This is mostly copied from the version in native_writer with some minor changes.
// Note that argIndex is 1 for the first argument.
static void write_annotations(FILE* out, int argIndex, const AtomDecl& atomDecl,
                              const string& methodPrefix, const string& methodSuffix) {
    const map<AnnotationId, string>& ANNOTATION_ID_CONSTANTS = get_annotation_id_constants();
    auto annotationsIt = atomDecl.fieldNumberToAnnotations.find(argIndex);
    if (annotationsIt == atomDecl.fieldNumberToAnnotations.end()) {
        return;
    }
    int resetState = -1;
    int defaultState = -1;
    for (const shared_ptr<Annotation>& annotation : annotationsIt->second) {
        const string& annotationConstant = ANNOTATION_ID_CONSTANTS.at(annotation->annotationId);
        switch (annotation->type) {
        case ANNOTATION_TYPE_INT:
            if (ANNOTATION_ID_TRIGGER_STATE_RESET == annotation->annotationId) {
                resetState = annotation->value.intValue;
            } else if (ANNOTATION_ID_DEFAULT_STATE == annotation->annotationId) {
                defaultState = annotation->value.intValue;
            } else {
                fprintf(out, "        %saddInt32Annotation(%sAnnotations::%s as u8, %d);\n",
                        methodPrefix.c_str(), methodSuffix.c_str(),
                        make_camel_case_name(annotationConstant).c_str(),
                        annotation->value.intValue);
            }
            break;
        case ANNOTATION_TYPE_BOOL:
            fprintf(out, "        %saddBoolAnnotation(%sAnnotations::%s as u8, %s);\n",
                    methodPrefix.c_str(), methodSuffix.c_str(),
                    make_camel_case_name(annotationConstant).c_str(),
                    annotation->value.boolValue ? "true" : "false");
            break;
        default:
            break;
        }
    }
    if (defaultState != -1 && resetState != -1) {
        const string& annotationConstant =
            ANNOTATION_ID_CONSTANTS.at(ANNOTATION_ID_TRIGGER_STATE_RESET);
        fprintf(out, "        if _%s == %d {\n",
                atomDecl.fields[argIndex - 1].name.c_str(), resetState);
        fprintf(out, "            %saddInt32Annotation(%sAnnotations::%s as u8, %d);\n",
                methodPrefix.c_str(), methodSuffix.c_str(),
                make_camel_case_name(annotationConstant).c_str(),
                defaultState);
        fprintf(out, "        }\n");
    }
}

static int write_rust_method_body(FILE* out, const AtomDecl& atomDecl,
                                  const AtomDecl& attributionDecl,
                                  const int minApiLevel) {
    fprintf(out, "    unsafe {\n");
    if (minApiLevel == API_Q) {
        fprintf(stderr, "TODO: Do we need to handle this case?");
        return 1;
    }
    if (atomDecl.oneOfName == ONEOF_PUSHED_ATOM_NAME) {
        fprintf(out, "        let event = AStatsEvent_obtain();\n");
        fprintf(out, "        let _dropper = AStatsEventDropper(event);\n");
        fprintf(out, "        AStatsEvent_setAtomId(event, Atoms::%s as u32);\n",
                make_camel_case_name(atomDecl.name).c_str());
    } else {
        fprintf(out, "        let event = AStatsEventList_addStatsEvent(__pulled_data);\n");
    }
    write_annotations(out, ATOM_ID_FIELD_NUMBER, atomDecl, "AStatsEvent_", "event, ");
    for (int i = 0; i < atomDecl.fields.size(); i++) {
        const AtomField &atomField = atomDecl.fields[i];
        const char* name = atomField.name.c_str();
        const java_type_t& type = atomField.javaType;
        switch (type) {
        case JAVA_TYPE_ATTRIBUTION_CHAIN: {
	    const char* uidName = attributionDecl.fields.front().name.c_str();
	    const char* tagName = attributionDecl.fields.back().name.c_str();
	    fprintf(out,
                    "        let uids = __chain_%s.iter().map(|n| (*n).try_into())"
                    ".collect::<Result<Vec<_>, _>>()?;\n"
                    "        let str_arr = __chain_%s.iter().map(|s| CString::new(*s))"
                    ".collect::<Result<Vec<_>, _>>()?;\n"
                    "        let ptr_arr = str_arr.iter().map(|s| s.as_ptr())"
                    ".collect::<Vec<_>>();\n"
		    "        AStatsEvent_writeAttributionChain(event, uids.as_ptr(),\n"
                    "            ptr_arr.as_ptr(), __chain_%s.len().try_into()?);\n",
		    uidName, tagName, uidName);
	    break;
        }
        case JAVA_TYPE_BYTE_ARRAY:
	    fprintf(out,
		    "        AStatsEvent_writeByteArray(event, "
                    "_%s.as_ptr(), _%s.len());\n",
		    name, name);
	    break;
        case JAVA_TYPE_BOOLEAN:
	    fprintf(out, "        AStatsEvent_writeBool(event, _%s);\n", name);
	    break;
        case JAVA_TYPE_INT:  // Fall through.
        case JAVA_TYPE_ENUM:
	    fprintf(out, "        AStatsEvent_writeInt32(event, _%s);\n", name);
	    break;
        case JAVA_TYPE_FLOAT:
	    fprintf(out, "        AStatsEvent_writeFloat(event, _%s);\n", name);
	    break;
        case JAVA_TYPE_LONG:
	    fprintf(out, "        AStatsEvent_writeInt64(event, _%s);\n", name);
	    break;
        case JAVA_TYPE_STRING:
            fprintf(out, "        let str = CString::new(_%s)?;\n", name);
	    fprintf(out, "        AStatsEvent_writeString(event, str.as_ptr());\n");
	    break;
        default:
	    // Unsupported types: OBJECT, DOUBLE, KEY_VALUE_PAIRS
	    fprintf(stderr, "Encountered unsupported type: %d.", type);
	    return 1;
        }
        // write_annotations expects the first argument to have an index of 1.
        write_annotations(out, i + 1, atomDecl, "AStatsEvent_", "event, ");
    }
    if (atomDecl.oneOfName == ONEOF_PUSHED_ATOM_NAME) {
        fprintf(out, "        let ret = AStatsEvent_write(event);\n");
        fprintf(out, "        if ret >= 0 { Ok(()) } else { Err(StatsError::Return(ret)) }\n");
    } else {
        fprintf(out, "        AStatsEvent_build(event);\n");
        fprintf(out, "        Ok(())\n");
    }
    fprintf(out, "    }\n");
    return 0;
}

static int write_rust_stats_write_methods(FILE* out, const AtomDeclSet& atomDeclSet,
                                          const AtomDecl& attributionDecl,
                                          const int minApiLevel) {
    for (const auto &atomDecl : atomDeclSet) {
        // Key value pairs not supported in Rust because they're not supported in native.
        if (std::find_if(atomDecl->fields.begin(), atomDecl->fields.end(),
                         [](const AtomField &atomField) {
                             return atomField.javaType == JAVA_TYPE_KEY_VALUE_PAIR;
                         }) != atomDecl->fields.end()) {
            continue;
        }
        if (atomDecl->oneOfName == ONEOF_PUSHED_ATOM_NAME) {
            write_rust_method_signature(out, "stats_write_", *atomDecl, attributionDecl,
                                        true, false);
        } else {
            write_rust_method_signature(out, "add_astats_event_", *atomDecl, attributionDecl,
                                        true, false);
        }
        int ret = write_rust_method_body(out, *atomDecl, attributionDecl, minApiLevel);
        if (ret != 0) {
            return ret;
        }
        fprintf(out, "}\n\n");
    }
    return 0;
}

static void write_rust_stats_write_non_chained_methods(FILE* out,
                                                       const AtomDeclSet& atomDeclSet,
                                                       const AtomDecl& attributionDecl) {
    for (const auto &atomDecl : atomDeclSet) {
        // Key value pairs not supported in Rust because they're not supported in native.
        if (std::find_if(atomDecl->fields.begin(), atomDecl->fields.end(),
                         [](const AtomField &atomField) {
                             return atomField.javaType == JAVA_TYPE_KEY_VALUE_PAIR;
                         }) != atomDecl->fields.end()) {
            continue;
        }
        write_rust_method_signature(out, "stats_write_non_chained_", *atomDecl, attributionDecl,
                                    true, true);
        fprintf(out, "    stats_write_%s(", atomDecl->name.c_str());
        for (int i = 0; i < atomDecl->fields.size(); i++) {
            if (i != 0) {
                fprintf(out, ", ");
            }
            const AtomField &atomField = atomDecl->fields[i];
            const java_type_t& type = atomField.javaType;
            if (i < 2) {
                // The first two args are attribution chains.
                fprintf(out, "&[__non_chained_%s]", atomField.name.c_str());
            } else if (type == JAVA_TYPE_ATTRIBUTION_CHAIN) {
                for (int j = 0; j < attributionDecl.fields.size(); j++) {
                    const AtomField& chainField = attributionDecl.fields[j];
                    if (i != 0 || j != 0)  {
                        fprintf(out, ", ");
                    }
                    fprintf(out, "&[__chain_%s]", chainField.name.c_str());
                }
            } else {
                fprintf(out, "_%s", atomField.name.c_str());
            }
        }
        fprintf(out, ")\n");
        fprintf(out, "}\n\n");
    }
}

int write_stats_log_rust(FILE* out, const Atoms& atoms, const AtomDecl& attributionDecl,
                         const int minApiLevel) {
    // Print prelude
    fprintf(out, "// This file is autogenerated\n");
    fprintf(out, "\n");
    fprintf(out, "use statspull_bindgen::*;\n");
    fprintf(out, "use std::convert::TryInto;\n");
    fprintf(out, "use std::ffi::CString;\n");
    fprintf(out, "\n");
    fprintf(out, "#[derive(thiserror::Error, Debug)]\n");
    fprintf(out, "pub enum StatsError {\n");
    fprintf(out, "    #[error(\"Return error {0:?}\")]\n");
    fprintf(out, "    Return(i32),\n");
    fprintf(out, "    #[error(transparent)]\n");
    fprintf(out, "    NullChar(#[from] std::ffi::NulError),\n");
    fprintf(out, "    #[error(transparent)]\n");
    fprintf(out, "    Conversion(#[from] std::num::TryFromIntError),\n");
    fprintf(out, "}\n");
    fprintf(out, "\n");
    fprintf(out, "struct AStatsEventDropper(*mut statspull_bindgen::AStatsEvent);\n");
    fprintf(out, "\n");
    fprintf(out, "impl Drop for AStatsEventDropper {\n");
    fprintf(out, "    fn drop(&mut self) {\n");
    fprintf(out, "        unsafe { AStatsEvent_release(self.0) }\n");
    fprintf(out, "    }\n");
    fprintf(out, "}\n");
    fprintf(out, "\n");

    write_rust_atom_constants(out, atoms, attributionDecl);
    write_rust_atom_constant_values(out, atoms);
    write_rust_annotation_constants(out);

    int errorCount = write_rust_stats_write_methods(out, atoms.decls, attributionDecl, minApiLevel);
    write_rust_stats_write_non_chained_methods(out, atoms.non_chained_decls, attributionDecl);

    return errorCount;
}

}  // namespace stats_log_api_gen
}  // namespace android
