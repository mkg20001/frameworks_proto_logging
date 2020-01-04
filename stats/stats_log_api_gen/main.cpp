

#include "Collation.h"
#if !defined(STATS_SCHEMA_LEGACY)
#include "java_writer.h"
#endif
#include "java_writer_q.h"
#include "utils.h"

#include "frameworks/base/cmds/statsd/src/atoms.pb.h"

#include <map>
#include <set>
#include <vector>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "android-base/strings.h"

using namespace google::protobuf;
using namespace std;

namespace android {
namespace stats_log_api_gen {

using android::os::statsd::Atom;

static void write_atoms_info_cpp(FILE *out, const Atoms &atoms) {
    std::set<string> kTruncatingAtomNames = {"mobile_radio_power_state_changed",
                                                 "audio_state_changed",
                                                 "call_state_changed",
                                                 "phone_signal_strength_changed",
                                                 "mobile_bytes_transfer_by_fg_bg",
                                                 "mobile_bytes_transfer"};
    fprintf(out,
            "const std::set<int> "
            "AtomsInfo::kTruncatingTimestampAtomBlackList = {\n");
    for (set<string>::const_iterator blacklistedAtom = kTruncatingAtomNames.begin();
         blacklistedAtom != kTruncatingAtomNames.end(); blacklistedAtom++) {
            fprintf(out, " %s,\n", make_constant_name(*blacklistedAtom).c_str());
    }
    fprintf(out, "};\n");
    fprintf(out, "\n");

    fprintf(out,
            "const std::set<int> AtomsInfo::kAtomsWithAttributionChain = {\n");
    for (set<AtomDecl>::const_iterator atom = atoms.decls.begin();
         atom != atoms.decls.end(); atom++) {
        for (vector<AtomField>::const_iterator field = atom->fields.begin();
             field != atom->fields.end(); field++) {
            if (field->javaType == JAVA_TYPE_ATTRIBUTION_CHAIN) {
                string constant = make_constant_name(atom->name);
                fprintf(out, " %s,\n", constant.c_str());
                break;
            }
        }
    }

    fprintf(out, "};\n");
    fprintf(out, "\n");

    fprintf(out,
            "const std::set<int> AtomsInfo::kWhitelistedAtoms = {\n");
    for (set<AtomDecl>::const_iterator atom = atoms.decls.begin();
         atom != atoms.decls.end(); atom++) {
        if (atom->whitelisted) {
            string constant = make_constant_name(atom->name);
            fprintf(out, " %s,\n", constant.c_str());
        }
    }

    fprintf(out, "};\n");
    fprintf(out, "\n");

    fprintf(out, "static std::map<int, int> getAtomUidField() {\n");
    fprintf(out, "  std::map<int, int> uidField;\n");
    for (set<AtomDecl>::const_iterator atom = atoms.decls.begin();
         atom != atoms.decls.end(); atom++) {
        if (atom->uidField == 0) {
            continue;
        }
        fprintf(out,
                "\n    // Adding uid field for atom "
                "(%d)%s\n",
                atom->code, atom->name.c_str());
        fprintf(out, "    uidField[static_cast<int>(%s)] = %d;\n",
                make_constant_name(atom->name).c_str(), atom->uidField);
    }

    fprintf(out, "    return uidField;\n");
    fprintf(out, "};\n");

    fprintf(out,
            "const std::map<int, int> AtomsInfo::kAtomsWithUidField = "
            "getAtomUidField();\n");

    fprintf(out,
            "static std::map<int, StateAtomFieldOptions> "
            "getStateAtomFieldOptions() {\n");
    fprintf(out, "    std::map<int, StateAtomFieldOptions> options;\n");
    fprintf(out, "    StateAtomFieldOptions opt;\n");
    for (set<AtomDecl>::const_iterator atom = atoms.decls.begin();
         atom != atoms.decls.end(); atom++) {
        if (atom->primaryFields.size() == 0 && atom->exclusiveField == 0) {
            continue;
        }
        fprintf(out,
                "\n    // Adding primary and exclusive fields for atom "
                "(%d)%s\n",
                atom->code, atom->name.c_str());
        fprintf(out, "    opt.primaryFields.clear();\n");
        for (const auto& field : atom->primaryFields) {
            fprintf(out, "    opt.primaryFields.push_back(%d);\n", field);
        }

        fprintf(out, "    opt.exclusiveField = %d;\n", atom->exclusiveField);
        fprintf(out, "    options[static_cast<int>(%s)] = opt;\n",
                make_constant_name(atom->name).c_str());
    }

    fprintf(out, "    return options;\n");
    fprintf(out, "}\n");

    fprintf(out,
            "const std::map<int, StateAtomFieldOptions> "
            "AtomsInfo::kStateAtomsFieldOptions = "
            "getStateAtomFieldOptions();\n");

    fprintf(out,
            "static std::map<int, std::vector<int>> "
            "getBinaryFieldAtoms() {\n");
    fprintf(out, "    std::map<int, std::vector<int>> options;\n");
    for (set<AtomDecl>::const_iterator atom = atoms.decls.begin();
         atom != atoms.decls.end(); atom++) {
        if (atom->binaryFields.size() == 0) {
            continue;
        }
        fprintf(out,
                "\n    // Adding binary fields for atom "
                "(%d)%s\n",
                atom->code, atom->name.c_str());

        for (const auto& field : atom->binaryFields) {
            fprintf(out, "    options[static_cast<int>(%s)].push_back(%d);\n",
                    make_constant_name(atom->name).c_str(), field);
        }
    }

    fprintf(out, "    return options;\n");
    fprintf(out, "}\n");

    fprintf(out,
            "const std::map<int, std::vector<int>> "
            "AtomsInfo::kBytesFieldAtoms = "
            "getBinaryFieldAtoms();\n");
}

// Writes namespaces for the cpp and header files, returning the number of namespaces written.
void write_namespace(FILE* out, const string& cppNamespaces) {
    vector<string> cppNamespaceVec = android::base::Split(cppNamespaces, ",");
    for (string cppNamespace : cppNamespaceVec) {
        fprintf(out, "namespace %s {\n", cppNamespace.c_str());
    }
}

// Writes namespace closing brackets for cpp and header files.
void write_closing_namespace(FILE* out, const string& cppNamespaces) {
    vector<string> cppNamespaceVec = android::base::Split(cppNamespaces, ",");
    for (auto it = cppNamespaceVec.rbegin(); it != cppNamespaceVec.rend(); ++it) {
        fprintf(out, "} // namespace %s\n", it->c_str());
    }
}

static int write_stats_log_cpp(FILE *out, const Atoms &atoms, const AtomDecl &attributionDecl,
                               const string& moduleName, const string& cppNamespace,
                               const string& importHeader) {
    // Print prelude
    fprintf(out, "// This file is autogenerated\n");
    fprintf(out, "\n");

    fprintf(out, "#include <mutex>\n");
    fprintf(out, "#include <chrono>\n");
    fprintf(out, "#include <thread>\n");
    fprintf(out, "#ifdef __ANDROID__\n");
    fprintf(out, "#include <cutils/properties.h>\n");
    fprintf(out, "#endif\n");
    fprintf(out, "#include <stats_event_list.h>\n");
    fprintf(out, "#include <log/log.h>\n");
    fprintf(out, "#include <%s>\n", importHeader.c_str());
    fprintf(out, "#include <utils/SystemClock.h>\n");
    fprintf(out, "\n");

    write_namespace(out, cppNamespace);
    fprintf(out, "// the single event tag id for all stats logs\n");
    fprintf(out, "const static int kStatsEventTag = 1937006964;\n");
    fprintf(out, "#ifdef __ANDROID__\n");
    fprintf(out, "const static bool kStatsdEnabled = property_get_bool(\"ro.statsd.enable\", true);\n");
    fprintf(out, "#else\n");
    fprintf(out, "const static bool kStatsdEnabled = false;\n");
    fprintf(out, "#endif\n");

    // AtomsInfo is only used by statsd internally and is not needed for other modules.
    if (moduleName == DEFAULT_MODULE_NAME) {
        write_atoms_info_cpp(out, atoms);
    }

    fprintf(out, "int64_t lastRetryTimestampNs = -1;\n");
    fprintf(out, "const int64_t kMinRetryIntervalNs = NS_PER_SEC * 60 * 20; // 20 minutes\n");
    fprintf(out, "static std::mutex mLogdRetryMutex;\n");

    // Print write methods
    fprintf(out, "\n");
    for (auto signature_to_modules_it = atoms.signatures_to_modules.begin();
        signature_to_modules_it != atoms.signatures_to_modules.end(); signature_to_modules_it++) {
        if (!signature_needed_for_module(signature_to_modules_it->second, moduleName)) {
            continue;
        }
        vector<java_type_t> signature = signature_to_modules_it->first;
        int argIndex;

        fprintf(out, "int\n");
        fprintf(out, "try_stats_write(int32_t code");
        argIndex = 1;
        for (vector<java_type_t>::const_iterator arg = signature.begin();
            arg != signature.end(); arg++) {
            if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
                for (auto chainField : attributionDecl.fields) {
                    if (chainField.javaType == JAVA_TYPE_STRING) {
                            fprintf(out, ", const std::vector<%s>& %s",
                                 cpp_type_name(chainField.javaType),
                                 chainField.name.c_str());
                    } else {
                            fprintf(out, ", const %s* %s, size_t %s_length",
                                 cpp_type_name(chainField.javaType),
                                 chainField.name.c_str(), chainField.name.c_str());
                    }
                }
            } else if (*arg == JAVA_TYPE_KEY_VALUE_PAIR) {
                fprintf(out, ", const std::map<int, int32_t>& arg%d_1, "
                             "const std::map<int, int64_t>& arg%d_2, "
                             "const std::map<int, char const*>& arg%d_3, "
                             "const std::map<int, float>& arg%d_4",
                             argIndex, argIndex, argIndex, argIndex);
            } else {
                fprintf(out, ", %s arg%d", cpp_type_name(*arg), argIndex);
            }
            argIndex++;
        }
        fprintf(out, ")\n");

        fprintf(out, "{\n");
        argIndex = 1;
        fprintf(out, "  if (kStatsdEnabled) {\n");
        fprintf(out, "    stats_event_list event(kStatsEventTag);\n");
        fprintf(out, "    event << android::elapsedRealtimeNano();\n\n");
        fprintf(out, "    event << code;\n\n");
        for (vector<java_type_t>::const_iterator arg = signature.begin();
            arg != signature.end(); arg++) {
            if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
                for (const auto &chainField : attributionDecl.fields) {
                    if (chainField.javaType == JAVA_TYPE_STRING) {
                        fprintf(out, "    if (%s_length != %s.size()) {\n",
                            attributionDecl.fields.front().name.c_str(), chainField.name.c_str());
                        fprintf(out, "        return -EINVAL;\n");
                        fprintf(out, "    }\n");
                    }
                }
                fprintf(out, "\n    event.begin();\n");
                fprintf(out, "    for (size_t i = 0; i < %s_length; ++i) {\n",
                    attributionDecl.fields.front().name.c_str());
                fprintf(out, "        event.begin();\n");
                for (const auto &chainField : attributionDecl.fields) {
                    if (chainField.javaType == JAVA_TYPE_STRING) {
                        fprintf(out, "        if (%s[i] != NULL) {\n", chainField.name.c_str());
                        fprintf(out, "           event << %s[i];\n", chainField.name.c_str());
                        fprintf(out, "        } else {\n");
                        fprintf(out, "           event << \"\";\n");
                        fprintf(out, "        }\n");
                    } else {
                        fprintf(out, "        event << %s[i];\n", chainField.name.c_str());
                    }
                }
                fprintf(out, "        event.end();\n");
                fprintf(out, "    }\n");
                fprintf(out, "    event.end();\n\n");
            } else if (*arg == JAVA_TYPE_KEY_VALUE_PAIR) {
                    fprintf(out, "    event.begin();\n\n");
                    fprintf(out, "    for (const auto& it : arg%d_1) {\n", argIndex);
                    fprintf(out, "         event.begin();\n");
                    fprintf(out, "         event << it.first;\n");
                    fprintf(out, "         event << it.second;\n");
                    fprintf(out, "         event.end();\n");
                    fprintf(out, "    }\n");

                    fprintf(out, "    for (const auto& it : arg%d_2) {\n", argIndex);
                    fprintf(out, "         event.begin();\n");
                    fprintf(out, "         event << it.first;\n");
                    fprintf(out, "         event << it.second;\n");
                    fprintf(out, "         event.end();\n");
                    fprintf(out, "    }\n");

                    fprintf(out, "    for (const auto& it : arg%d_3) {\n", argIndex);
                    fprintf(out, "         event.begin();\n");
                    fprintf(out, "         event << it.first;\n");
                    fprintf(out, "         event << it.second;\n");
                    fprintf(out, "         event.end();\n");
                    fprintf(out, "    }\n");

                    fprintf(out, "    for (const auto& it : arg%d_4) {\n", argIndex);
                    fprintf(out, "         event.begin();\n");
                    fprintf(out, "         event << it.first;\n");
                    fprintf(out, "         event << it.second;\n");
                    fprintf(out, "         event.end();\n");
                    fprintf(out, "    }\n");

                    fprintf(out, "    event.end();\n\n");
            } else if (*arg == JAVA_TYPE_BYTE_ARRAY) {
                fprintf(out,
                        "    event.AppendCharArray(arg%d.arg, "
                        "arg%d.arg_length);\n",
                        argIndex, argIndex);
            } else {
                if (*arg == JAVA_TYPE_STRING) {
                    fprintf(out, "    if (arg%d == NULL) {\n", argIndex);
                    fprintf(out, "        arg%d = \"\";\n", argIndex);
                    fprintf(out, "    }\n");
                }
                fprintf(out, "    event << arg%d;\n", argIndex);
            }
            argIndex++;
        }

        fprintf(out, "    return event.write(LOG_ID_STATS);\n");
        fprintf(out, "  } else {\n");
        fprintf(out, "    return 1;\n");
        fprintf(out, "  }\n");
        fprintf(out, "}\n");
        fprintf(out, "\n");
    }

   for (auto signature_to_modules_it = atoms.signatures_to_modules.begin();
       signature_to_modules_it != atoms.signatures_to_modules.end(); signature_to_modules_it++) {
       if (!signature_needed_for_module(signature_to_modules_it->second, moduleName)) {
           continue;
       }
       vector<java_type_t> signature = signature_to_modules_it->first;
       int argIndex;

       fprintf(out, "int\n");
       fprintf(out, "stats_write(int32_t code");
       argIndex = 1;
       for (vector<java_type_t>::const_iterator arg = signature.begin();
           arg != signature.end(); arg++) {
           if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
               for (auto chainField : attributionDecl.fields) {
                   if (chainField.javaType == JAVA_TYPE_STRING) {
                           fprintf(out, ", const std::vector<%s>& %s",
                                cpp_type_name(chainField.javaType),
                                chainField.name.c_str());
                   } else {
                           fprintf(out, ", const %s* %s, size_t %s_length",
                                cpp_type_name(chainField.javaType),
                                chainField.name.c_str(), chainField.name.c_str());
                   }
               }
           } else if (*arg == JAVA_TYPE_KEY_VALUE_PAIR) {
               fprintf(out,
                       ", const std::map<int, int32_t>& arg%d_1, "
                       "const std::map<int, int64_t>& arg%d_2, "
                       "const std::map<int, char const*>& arg%d_3, "
                       "const std::map<int, float>& arg%d_4",
                       argIndex, argIndex, argIndex, argIndex);
           } else {
               fprintf(out, ", %s arg%d", cpp_type_name(*arg), argIndex);
           }
           argIndex++;
       }
       fprintf(out, ")\n");

       fprintf(out, "{\n");
       fprintf(out, "  int ret = 0;\n");

       fprintf(out, "  for(int retry = 0; retry < 2; ++retry) {\n");
       fprintf(out, "      ret =  try_stats_write(code");

       argIndex = 1;
       for (vector<java_type_t>::const_iterator arg = signature.begin();
           arg != signature.end(); arg++) {
           if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
               for (auto chainField : attributionDecl.fields) {
                   if (chainField.javaType == JAVA_TYPE_STRING) {
                           fprintf(out, ", %s",
                                chainField.name.c_str());
                   } else {
                           fprintf(out, ",  %s,  %s_length",
                                chainField.name.c_str(), chainField.name.c_str());
                   }
               }
           } else if (*arg == JAVA_TYPE_KEY_VALUE_PAIR) {
               fprintf(out, ", arg%d_1, arg%d_2, arg%d_3, arg%d_4", argIndex,
                       argIndex, argIndex, argIndex);
           } else {
               fprintf(out, ", arg%d", argIndex);
           }
           argIndex++;
       }
       fprintf(out, ");\n");
       fprintf(out, "      if (ret >= 0) { break; }\n");

       fprintf(out, "      {\n");
       fprintf(out, "          std::lock_guard<std::mutex> lock(mLogdRetryMutex);\n");
       fprintf(out, "          if ((android::elapsedRealtimeNano() - lastRetryTimestampNs) <= "
                                "kMinRetryIntervalNs) break;\n");
       fprintf(out, "          lastRetryTimestampNs = android::elapsedRealtimeNano();\n");
       fprintf(out, "      }\n");
       fprintf(out, "      std::this_thread::sleep_for(std::chrono::milliseconds(10));\n");
       fprintf(out, "  }\n");
       fprintf(out, "  if (ret < 0) {\n");
       fprintf(out, "      note_log_drop(ret, code);\n");
       fprintf(out, "  }\n");
       fprintf(out, "  return ret;\n");
       fprintf(out, "}\n");
       fprintf(out, "\n");
   }

    for (auto signature_it = atoms.non_chained_signatures_to_modules.begin();
            signature_it != atoms.non_chained_signatures_to_modules.end(); signature_it++) {
        if (!signature_needed_for_module(signature_it->second, moduleName)) {
            continue;
        }
        vector<java_type_t> signature = signature_it->first;
        int argIndex;

        fprintf(out, "int\n");
        fprintf(out, "try_stats_write_non_chained(int32_t code");
        argIndex = 1;
        for (vector<java_type_t>::const_iterator arg = signature.begin();
            arg != signature.end(); arg++) {
            fprintf(out, ", %s arg%d", cpp_type_name(*arg), argIndex);
            argIndex++;
        }
        fprintf(out, ")\n");

        fprintf(out, "{\n");
        argIndex = 1;
        fprintf(out, "  if (kStatsdEnabled) {\n");
        fprintf(out, "    stats_event_list event(kStatsEventTag);\n");
        fprintf(out, "    event << android::elapsedRealtimeNano();\n\n");
        fprintf(out, "    event << code;\n\n");
        for (vector<java_type_t>::const_iterator arg = signature.begin();
            arg != signature.end(); arg++) {
            if (argIndex == 1) {
                fprintf(out, "    event.begin();\n\n");
                fprintf(out, "    event.begin();\n");
            }
            if (*arg == JAVA_TYPE_STRING) {
                fprintf(out, "    if (arg%d == NULL) {\n", argIndex);
                fprintf(out, "        arg%d = \"\";\n", argIndex);
                fprintf(out, "    }\n");
            }
            if (*arg == JAVA_TYPE_BYTE_ARRAY) {
                fprintf(out,
                        "    event.AppendCharArray(arg%d.arg, "
                        "arg%d.arg_length);",
                        argIndex, argIndex);
            } else {
                fprintf(out, "    event << arg%d;\n", argIndex);
            }
            if (argIndex == 2) {
                fprintf(out, "    event.end();\n\n");
                fprintf(out, "    event.end();\n\n");
            }
            argIndex++;
        }

        fprintf(out, "    return event.write(LOG_ID_STATS);\n");
        fprintf(out, "  } else {\n");
        fprintf(out, "    return 1;\n");
        fprintf(out, "  }\n");
        fprintf(out, "}\n");
        fprintf(out, "\n");
    }

    for (auto signature_it = atoms.non_chained_signatures_to_modules.begin();
            signature_it != atoms.non_chained_signatures_to_modules.end(); signature_it++) {
       if (!signature_needed_for_module(signature_it->second, moduleName)) {
           continue;
       }
       vector<java_type_t> signature = signature_it->first;
       int argIndex;

       fprintf(out, "int\n");
       fprintf(out, "stats_write_non_chained(int32_t code");
       argIndex = 1;
       for (vector<java_type_t>::const_iterator arg = signature.begin();
           arg != signature.end(); arg++) {
           fprintf(out, ", %s arg%d", cpp_type_name(*arg), argIndex);
           argIndex++;
       }
       fprintf(out, ")\n");

       fprintf(out, "{\n");

       fprintf(out, "  int ret = 0;\n");
       fprintf(out, "  for(int retry = 0; retry < 2; ++retry) {\n");
       fprintf(out, "      ret =  try_stats_write_non_chained(code");

       argIndex = 1;
       for (vector<java_type_t>::const_iterator arg = signature.begin();
           arg != signature.end(); arg++) {
           fprintf(out, ", arg%d",   argIndex);
           argIndex++;
       }
       fprintf(out, ");\n");
       fprintf(out, "      if (ret >= 0) { break; }\n");

       fprintf(out, "      {\n");
       fprintf(out, "          std::lock_guard<std::mutex> lock(mLogdRetryMutex);\n");
       fprintf(out, "          if ((android::elapsedRealtimeNano() - lastRetryTimestampNs) <= "
                                "kMinRetryIntervalNs) break;\n");
       fprintf(out, "          lastRetryTimestampNs = android::elapsedRealtimeNano();\n");
       fprintf(out, "      }\n");

       fprintf(out, "      std::this_thread::sleep_for(std::chrono::milliseconds(10));\n");
       fprintf(out, "  }\n");
       fprintf(out, "  if (ret < 0) {\n");
       fprintf(out, "      note_log_drop(ret, code);\n");
       fprintf(out, "  }\n");
       fprintf(out, "  return ret;\n\n");
       fprintf(out, "}\n");

       fprintf(out, "\n");
   }


    // Print footer
    fprintf(out, "\n");
    write_closing_namespace(out, cppNamespace);

    return 0;
}

static void write_cpp_usage(
    FILE* out, const string& method_name, const string& atom_code_name,
    const AtomDecl& atom, const AtomDecl &attributionDecl) {
    fprintf(out, "     * Usage: %s(StatsLog.%s", method_name.c_str(),
            atom_code_name.c_str());

    for (vector<AtomField>::const_iterator field = atom.fields.begin();
            field != atom.fields.end(); field++) {
        if (field->javaType == JAVA_TYPE_ATTRIBUTION_CHAIN) {
            for (auto chainField : attributionDecl.fields) {
                if (chainField.javaType == JAVA_TYPE_STRING) {
                    fprintf(out, ", const std::vector<%s>& %s",
                         cpp_type_name(chainField.javaType),
                         chainField.name.c_str());
                } else {
                    fprintf(out, ", const %s* %s, size_t %s_length",
                         cpp_type_name(chainField.javaType),
                         chainField.name.c_str(), chainField.name.c_str());
                }
            }
        } else if (field->javaType == JAVA_TYPE_KEY_VALUE_PAIR) {
            fprintf(out, ", const std::map<int, int32_t>& %s_int"
                         ", const std::map<int, int64_t>& %s_long"
                         ", const std::map<int, char const*>& %s_str"
                         ", const std::map<int, float>& %s_float",
                         field->name.c_str(),
                         field->name.c_str(),
                         field->name.c_str(),
                         field->name.c_str());
        } else {
            fprintf(out, ", %s %s", cpp_type_name(field->javaType), field->name.c_str());
        }
    }
    fprintf(out, ");\n");
}

static void write_cpp_method_header(
        FILE* out,
        const string& method_name,
        const map<vector<java_type_t>, set<string>>& signatures_to_modules,
        const AtomDecl &attributionDecl, const string& moduleName) {

    for (auto signature_to_modules_it = signatures_to_modules.begin();
            signature_to_modules_it != signatures_to_modules.end(); signature_to_modules_it++) {
        // Skip if this signature is not needed for the module.
        if (!signature_needed_for_module(signature_to_modules_it->second, moduleName)) {
            continue;
        }

        vector<java_type_t> signature = signature_to_modules_it->first;
        fprintf(out, "int %s(int32_t code", method_name.c_str());
        int argIndex = 1;
        for (vector<java_type_t>::const_iterator arg = signature.begin();
                arg != signature.end(); arg++) {
            if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
                for (auto chainField : attributionDecl.fields) {
                    if (chainField.javaType == JAVA_TYPE_STRING) {
                        fprintf(out, ", const std::vector<%s>& %s",
                            cpp_type_name(chainField.javaType), chainField.name.c_str());
                    } else {
                        fprintf(out, ", const %s* %s, size_t %s_length",
                            cpp_type_name(chainField.javaType),
                            chainField.name.c_str(), chainField.name.c_str());
                    }
                }
            } else if (*arg == JAVA_TYPE_KEY_VALUE_PAIR) {
                fprintf(out, ", const std::map<int, int32_t>& arg%d_1, "
                             "const std::map<int, int64_t>& arg%d_2, "
                             "const std::map<int, char const*>& arg%d_3, "
                             "const std::map<int, float>& arg%d_4",
                             argIndex, argIndex, argIndex, argIndex);
            } else {
                fprintf(out, ", %s arg%d", cpp_type_name(*arg), argIndex);
            }
            argIndex++;
        }
        fprintf(out, ");\n");

    }
}

static int
write_stats_log_header(FILE* out, const Atoms& atoms, const AtomDecl &attributionDecl,
        const string& moduleName, const string& cppNamespace)
{
    // Print prelude
    fprintf(out, "// This file is autogenerated\n");
    fprintf(out, "\n");
    fprintf(out, "#pragma once\n");
    fprintf(out, "\n");
    fprintf(out, "#include <stdint.h>\n");
    fprintf(out, "#include <vector>\n");
    fprintf(out, "#include <map>\n");
    fprintf(out, "#include <set>\n");
    fprintf(out, "\n");

    write_namespace(out, cppNamespace);
    fprintf(out, "\n");
    fprintf(out, "/*\n");
    fprintf(out, " * API For logging statistics events.\n");
    fprintf(out, " */\n");
    fprintf(out, "\n");
    fprintf(out, "/**\n");
    fprintf(out, " * Constants for atom codes.\n");
    fprintf(out, " */\n");
    fprintf(out, "enum {\n");

    std::map<int, set<AtomDecl>::const_iterator> atom_code_to_non_chained_decl_map;
    build_non_chained_decl_map(atoms, &atom_code_to_non_chained_decl_map);

    size_t i = 0;
    int maxPushedAtomId = 2;
    // Print atom constants
    for (set<AtomDecl>::const_iterator atom = atoms.decls.begin();
        atom != atoms.decls.end(); atom++) {
        // Skip if the atom is not needed for the module.
        if (!atom_needed_for_module(*atom, moduleName)) {
            continue;
        }
        string constant = make_constant_name(atom->name);
        fprintf(out, "\n");
        fprintf(out, "    /**\n");
        fprintf(out, "     * %s %s\n", atom->message.c_str(), atom->name.c_str());
        write_cpp_usage(out, "stats_write", constant, *atom, attributionDecl);

        auto non_chained_decl = atom_code_to_non_chained_decl_map.find(atom->code);
        if (non_chained_decl != atom_code_to_non_chained_decl_map.end()) {
            write_cpp_usage(out, "stats_write_non_chained", constant, *non_chained_decl->second,
                attributionDecl);
        }
        fprintf(out, "     */\n");
        char const* const comma = (i == atoms.decls.size() - 1) ? "" : ",";
        fprintf(out, "    %s = %d%s\n", constant.c_str(), atom->code, comma);
        if (atom->code < PULL_ATOM_START_ID && atom->code > maxPushedAtomId) {
            maxPushedAtomId = atom->code;
        }
        i++;
    }
    fprintf(out, "\n");
    fprintf(out, "};\n");
    fprintf(out, "\n");

    // Print constants for the enum values.
    fprintf(out, "//\n");
    fprintf(out, "// Constants for enum values\n");
    fprintf(out, "//\n\n");
    for (set<AtomDecl>::const_iterator atom = atoms.decls.begin();
        atom != atoms.decls.end(); atom++) {
        // Skip if the atom is not needed for the module.
        if (!atom_needed_for_module(*atom, moduleName)) {
            continue;
        }

        for (vector<AtomField>::const_iterator field = atom->fields.begin();
            field != atom->fields.end(); field++) {
            if (field->javaType == JAVA_TYPE_ENUM) {
                fprintf(out, "// Values for %s.%s\n", atom->message.c_str(),
                    field->name.c_str());
                for (map<int, string>::const_iterator value = field->enumValues.begin();
                    value != field->enumValues.end(); value++) {
                    fprintf(out, "const int32_t %s__%s__%s = %d;\n",
                        make_constant_name(atom->message).c_str(),
                        make_constant_name(field->name).c_str(),
                        make_constant_name(value->second).c_str(),
                        value->first);
                }
                fprintf(out, "\n");
            }
        }
    }

    fprintf(out, "struct BytesField {\n");
    fprintf(out,
            "  BytesField(char const* array, size_t len) : arg(array), "
            "arg_length(len) {}\n");
    fprintf(out, "  char const* arg;\n");
    fprintf(out, "  size_t arg_length;\n");
    fprintf(out, "};\n");
    fprintf(out, "\n");

    // This metadata is only used by statsd, which uses the default libstatslog.
    if (moduleName == DEFAULT_MODULE_NAME) {

        fprintf(out, "struct StateAtomFieldOptions {\n");
        fprintf(out, "  std::vector<int> primaryFields;\n");
        fprintf(out, "  int exclusiveField;\n");
        fprintf(out, "};\n");
        fprintf(out, "\n");

        fprintf(out, "struct AtomsInfo {\n");
        fprintf(out,
                "  const static std::set<int> "
                "kTruncatingTimestampAtomBlackList;\n");
        fprintf(out, "  const static std::map<int, int> kAtomsWithUidField;\n");
        fprintf(out,
                "  const static std::set<int> kAtomsWithAttributionChain;\n");
        fprintf(out,
                "  const static std::map<int, StateAtomFieldOptions> "
                "kStateAtomsFieldOptions;\n");
        fprintf(out,
                "  const static std::map<int, std::vector<int>> "
                "kBytesFieldAtoms;");
        fprintf(out,
                "  const static std::set<int> kWhitelistedAtoms;\n");
        fprintf(out, "};\n");

        fprintf(out, "const static int kMaxPushedAtomId = %d;\n\n",
                maxPushedAtomId);
    }

    // Print write methods
    fprintf(out, "//\n");
    fprintf(out, "// Write methods\n");
    fprintf(out, "//\n");
    write_cpp_method_header(out, "stats_write", atoms.signatures_to_modules, attributionDecl,
            moduleName);

    fprintf(out, "//\n");
    fprintf(out, "// Write flattened methods\n");
    fprintf(out, "//\n");
    write_cpp_method_header(out, "stats_write_non_chained", atoms.non_chained_signatures_to_modules,
        attributionDecl, moduleName);

    fprintf(out, "\n");
    write_closing_namespace(out, cppNamespace);

    return 0;
}

// Hide the JNI write helpers that are not used in the new schema.
// TODO(b/145100015): Remove this and other JNI related functionality once StatsEvent migration is
// complete.
#if defined(STATS_SCHEMA_LEGACY)
// JNI helpers.
static const char*
jni_type_name(java_type_t type)
{
    switch (type) {
        case JAVA_TYPE_BOOLEAN:
            return "jboolean";
        case JAVA_TYPE_INT:
        case JAVA_TYPE_ENUM:
            return "jint";
        case JAVA_TYPE_LONG:
            return "jlong";
        case JAVA_TYPE_FLOAT:
            return "jfloat";
        case JAVA_TYPE_DOUBLE:
            return "jdouble";
        case JAVA_TYPE_STRING:
            return "jstring";
        case JAVA_TYPE_BYTE_ARRAY:
            return "jbyteArray";
        default:
            return "UNKNOWN";
    }
}

static const char*
jni_array_type_name(java_type_t type)
{
    switch (type) {
        case JAVA_TYPE_INT:
            return "jintArray";
        case JAVA_TYPE_FLOAT:
            return "jfloatArray";
        case JAVA_TYPE_STRING:
            return "jobjectArray";
        default:
            return "UNKNOWN";
    }
}

static string
jni_function_name(const string& method_name, const vector<java_type_t>& signature)
{
    string result("StatsLog_" + method_name);
    for (vector<java_type_t>::const_iterator arg = signature.begin();
        arg != signature.end(); arg++) {
        switch (*arg) {
            case JAVA_TYPE_BOOLEAN:
                result += "_boolean";
                break;
            case JAVA_TYPE_INT:
            case JAVA_TYPE_ENUM:
                result += "_int";
                break;
            case JAVA_TYPE_LONG:
                result += "_long";
                break;
            case JAVA_TYPE_FLOAT:
                result += "_float";
                break;
            case JAVA_TYPE_DOUBLE:
                result += "_double";
                break;
            case JAVA_TYPE_STRING:
                result += "_String";
                break;
            case JAVA_TYPE_ATTRIBUTION_CHAIN:
              result += "_AttributionChain";
              break;
            case JAVA_TYPE_KEY_VALUE_PAIR:
              result += "_KeyValuePairs";
              break;
            case JAVA_TYPE_BYTE_ARRAY:
                result += "_bytes";
                break;
            default:
                result += "_UNKNOWN";
                break;
        }
    }
    return result;
}

static const char*
java_type_signature(java_type_t type)
{
    switch (type) {
        case JAVA_TYPE_BOOLEAN:
            return "Z";
        case JAVA_TYPE_INT:
        case JAVA_TYPE_ENUM:
            return "I";
        case JAVA_TYPE_LONG:
            return "J";
        case JAVA_TYPE_FLOAT:
            return "F";
        case JAVA_TYPE_DOUBLE:
            return "D";
        case JAVA_TYPE_STRING:
            return "Ljava/lang/String;";
        case JAVA_TYPE_BYTE_ARRAY:
            return "[B";
        default:
            return "UNKNOWN";
    }
}

static string
jni_function_signature(const vector<java_type_t>& signature, const AtomDecl &attributionDecl)
{
    string result("(I");
    for (vector<java_type_t>::const_iterator arg = signature.begin();
        arg != signature.end(); arg++) {
        if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
            for (auto chainField : attributionDecl.fields) {
                result += "[";
                result += java_type_signature(chainField.javaType);
            }
        } else if (*arg == JAVA_TYPE_KEY_VALUE_PAIR) {
            result += "Landroid/util/SparseArray;";
        } else {
            result += java_type_signature(*arg);
        }
    }
    result += ")I";
    return result;
}

static void write_key_value_map_jni(FILE* out) {
   fprintf(out, "    std::map<int, int32_t> int32_t_map;\n");
   fprintf(out, "    std::map<int, int64_t> int64_t_map;\n");
   fprintf(out, "    std::map<int, float> float_map;\n");
   fprintf(out, "    std::map<int, char const*> string_map;\n\n");

   fprintf(out, "    jclass jmap_class = env->FindClass(\"android/util/SparseArray\");\n");

   fprintf(out, "    jmethodID jget_size_method = env->GetMethodID(jmap_class, \"size\", \"()I\");\n");
   fprintf(out, "    jmethodID jget_key_method = env->GetMethodID(jmap_class, \"keyAt\", \"(I)I\");\n");
   fprintf(out, "    jmethodID jget_value_method = env->GetMethodID(jmap_class, \"valueAt\", \"(I)Ljava/lang/Object;\");\n\n");


   fprintf(out, "    std::vector<std::unique_ptr<ScopedUtfChars>> scoped_ufs;\n\n");

   fprintf(out, "    jclass jint_class = env->FindClass(\"java/lang/Integer\");\n");
   fprintf(out, "    jclass jlong_class = env->FindClass(\"java/lang/Long\");\n");
   fprintf(out, "    jclass jfloat_class = env->FindClass(\"java/lang/Float\");\n");
   fprintf(out, "    jclass jstring_class = env->FindClass(\"java/lang/String\");\n");
   fprintf(out, "    jmethodID jget_int_method = env->GetMethodID(jint_class, \"intValue\", \"()I\");\n");
   fprintf(out, "    jmethodID jget_long_method = env->GetMethodID(jlong_class, \"longValue\", \"()J\");\n");
   fprintf(out, "    jmethodID jget_float_method = env->GetMethodID(jfloat_class, \"floatValue\", \"()F\");\n\n");

   fprintf(out, "    jint jsize = env->CallIntMethod(value_map, jget_size_method);\n");
   fprintf(out, "    for(int i = 0; i < jsize; i++) {\n");
   fprintf(out, "        jint key = env->CallIntMethod(value_map, jget_key_method, i);\n");
   fprintf(out, "        jobject jvalue_obj = env->CallObjectMethod(value_map, jget_value_method, i);\n");
   fprintf(out, "        if (jvalue_obj == NULL) { continue; }\n");
   fprintf(out, "        if (env->IsInstanceOf(jvalue_obj, jint_class)) {\n");
   fprintf(out, "            int32_t_map[key] = env->CallIntMethod(jvalue_obj, jget_int_method);\n");
   fprintf(out, "        } else if (env->IsInstanceOf(jvalue_obj, jlong_class)) {\n");
   fprintf(out, "            int64_t_map[key] = env->CallLongMethod(jvalue_obj, jget_long_method);\n");
   fprintf(out, "        } else if (env->IsInstanceOf(jvalue_obj, jfloat_class)) {\n");
   fprintf(out, "            float_map[key] = env->CallFloatMethod(jvalue_obj, jget_float_method);\n");
   fprintf(out, "        } else if (env->IsInstanceOf(jvalue_obj, jstring_class)) {\n");
   fprintf(out, "            std::unique_ptr<ScopedUtfChars> utf(new ScopedUtfChars(env, (jstring)jvalue_obj));\n");
   fprintf(out, "            if (utf->c_str() != NULL) { string_map[key] = utf->c_str(); }\n");
   fprintf(out, "            scoped_ufs.push_back(std::move(utf));\n");
   fprintf(out, "        }\n");
   fprintf(out, "    }\n");
}

static int
write_stats_log_jni_method(FILE* out, const string& java_method_name, const string& cpp_method_name,
        const map<vector<java_type_t>, set<string>>& signatures_to_modules,
        const AtomDecl &attributionDecl) {
    // Print write methods
    for (auto signature_to_modules_it = signatures_to_modules.begin();
            signature_to_modules_it != signatures_to_modules.end(); signature_to_modules_it++) {
        vector<java_type_t> signature = signature_to_modules_it->first;
        int argIndex;

        fprintf(out, "static int\n");
        fprintf(out, "%s(JNIEnv* env, jobject clazz UNUSED, jint code",
                jni_function_name(java_method_name, signature).c_str());
        argIndex = 1;
        for (vector<java_type_t>::const_iterator arg = signature.begin();
                arg != signature.end(); arg++) {
            if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
                for (auto chainField : attributionDecl.fields) {
                    fprintf(out, ", %s %s", jni_array_type_name(chainField.javaType),
                        chainField.name.c_str());
                }
            } else if (*arg == JAVA_TYPE_KEY_VALUE_PAIR) {
                fprintf(out, ", jobject value_map");
            } else {
                fprintf(out, ", %s arg%d", jni_type_name(*arg), argIndex);
            }
            argIndex++;
        }
        fprintf(out, ")\n");

        fprintf(out, "{\n");

        // Prepare strings
        argIndex = 1;
        bool hadStringOrChain = false;
        bool isKeyValuePairAtom = false;
        for (vector<java_type_t>::const_iterator arg = signature.begin();
                arg != signature.end(); arg++) {
            if (*arg == JAVA_TYPE_STRING) {
                hadStringOrChain = true;
                fprintf(out, "    const char* str%d;\n", argIndex);
                fprintf(out, "    if (arg%d != NULL) {\n", argIndex);
                fprintf(out, "        str%d = env->GetStringUTFChars(arg%d, NULL);\n",
                        argIndex, argIndex);
                fprintf(out, "    } else {\n");
                fprintf(out, "        str%d = NULL;\n", argIndex);
                fprintf(out, "    }\n");
            } else if (*arg == JAVA_TYPE_BYTE_ARRAY) {
                hadStringOrChain = true;
                fprintf(out, "    jbyte* jbyte_array%d;\n", argIndex);
                fprintf(out, "    const char* str%d;\n", argIndex);
                fprintf(out, "    int str%d_length = 0;\n", argIndex);
                fprintf(out,
                        "    if (arg%d != NULL && env->GetArrayLength(arg%d) > "
                        "0) {\n",
                        argIndex, argIndex);
                fprintf(out,
                        "        jbyte_array%d = "
                        "env->GetByteArrayElements(arg%d, NULL);\n",
                        argIndex, argIndex);
                fprintf(out,
                        "        str%d_length = env->GetArrayLength(arg%d);\n",
                        argIndex, argIndex);
                fprintf(out,
                        "        str%d = "
                        "reinterpret_cast<char*>(env->GetByteArrayElements(arg%"
                        "d, NULL));\n",
                        argIndex, argIndex);
                fprintf(out, "    } else {\n");
                fprintf(out, "        jbyte_array%d = NULL;\n", argIndex);
                fprintf(out, "        str%d = NULL;\n", argIndex);
                fprintf(out, "    }\n");

                fprintf(out,
                        "    android::util::BytesField bytesField%d(str%d, "
                        "str%d_length);",
                        argIndex, argIndex, argIndex);

            } else if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
                hadStringOrChain = true;
                for (auto chainField : attributionDecl.fields) {
                    fprintf(out, "    size_t %s_length = env->GetArrayLength(%s);\n",
                        chainField.name.c_str(), chainField.name.c_str());
                    if (chainField.name != attributionDecl.fields.front().name) {
                        fprintf(out, "    if (%s_length != %s_length) {\n",
                            chainField.name.c_str(),
                            attributionDecl.fields.front().name.c_str());
                        fprintf(out, "        return -EINVAL;\n");
                        fprintf(out, "    }\n");
                    }
                    if (chainField.javaType == JAVA_TYPE_INT) {
                        fprintf(out, "    jint* %s_array = env->GetIntArrayElements(%s, NULL);\n",
                            chainField.name.c_str(), chainField.name.c_str());
                    } else if (chainField.javaType == JAVA_TYPE_STRING) {
                        fprintf(out, "    std::vector<%s> %s_vec;\n",
                            cpp_type_name(chainField.javaType), chainField.name.c_str());
                        fprintf(out, "    std::vector<ScopedUtfChars*> scoped_%s_vec;\n",
                            chainField.name.c_str());
                        fprintf(out, "    for (size_t i = 0; i < %s_length; ++i) {\n",
                            chainField.name.c_str());
                        fprintf(out, "        jstring jstr = "
                            "(jstring)env->GetObjectArrayElement(%s, i);\n",
                             chainField.name.c_str());
                        fprintf(out, "        if (jstr == NULL) {\n");
                        fprintf(out, "            %s_vec.push_back(NULL);\n",
                            chainField.name.c_str());
                        fprintf(out, "        } else {\n");
                        fprintf(out, "            ScopedUtfChars* scoped_%s = "
                            "new ScopedUtfChars(env, jstr);\n",
                             chainField.name.c_str());
                        fprintf(out, "            %s_vec.push_back(scoped_%s->c_str());\n",
                                chainField.name.c_str(), chainField.name.c_str());
                        fprintf(out, "            scoped_%s_vec.push_back(scoped_%s);\n",
                                chainField.name.c_str(), chainField.name.c_str());
                        fprintf(out, "        }\n");
                        fprintf(out, "    }\n");
                    }
                    fprintf(out, "\n");
                }
            } else if (*arg == JAVA_TYPE_KEY_VALUE_PAIR) {
                isKeyValuePairAtom = true;
            }
            argIndex++;
        }
        // Emit this to quiet the unused parameter warning if there were no strings or attribution
        // chains.
        if (!hadStringOrChain && !isKeyValuePairAtom) {
            fprintf(out, "    (void)env;\n");
        }
        if (isKeyValuePairAtom) {
            write_key_value_map_jni(out);
        }

        // stats_write call
        argIndex = 1;
        fprintf(out, "\n    int ret =  android::util::%s(code",
                cpp_method_name.c_str());
        for (vector<java_type_t>::const_iterator arg = signature.begin();
                arg != signature.end(); arg++) {
            if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
                for (auto chainField : attributionDecl.fields) {
                    if (chainField.javaType == JAVA_TYPE_INT) {
                        fprintf(out, ", (const %s*)%s_array, %s_length",
                            cpp_type_name(chainField.javaType),
                            chainField.name.c_str(), chainField.name.c_str());
                    } else if (chainField.javaType == JAVA_TYPE_STRING) {
                        fprintf(out, ", %s_vec", chainField.name.c_str());
                    }
                }
            } else if (*arg == JAVA_TYPE_KEY_VALUE_PAIR) {
                fprintf(out, ", int32_t_map, int64_t_map, string_map, float_map");
            } else if (*arg == JAVA_TYPE_BYTE_ARRAY) {
                fprintf(out, ", bytesField%d", argIndex);
            } else {
                const char* argName =
                        (*arg == JAVA_TYPE_STRING) ? "str" : "arg";
                fprintf(out, ", (%s)%s%d", cpp_type_name(*arg), argName, argIndex);
            }
            argIndex++;
        }
        fprintf(out, ");\n");
        fprintf(out, "\n");

        // Clean up strings
        argIndex = 1;
        for (vector<java_type_t>::const_iterator arg = signature.begin();
                arg != signature.end(); arg++) {
            if (*arg == JAVA_TYPE_STRING) {
                fprintf(out, "    if (str%d != NULL) {\n", argIndex);
                fprintf(out, "        env->ReleaseStringUTFChars(arg%d, str%d);\n",
                        argIndex, argIndex);
                fprintf(out, "    }\n");
            } else if (*arg == JAVA_TYPE_BYTE_ARRAY) {
                fprintf(out, "    if (str%d != NULL) { \n", argIndex);
                fprintf(out,
                        "        env->ReleaseByteArrayElements(arg%d, "
                        "jbyte_array%d, 0);\n",
                        argIndex, argIndex);
                fprintf(out, "    }\n");
            } else if (*arg == JAVA_TYPE_ATTRIBUTION_CHAIN) {
                for (auto chainField : attributionDecl.fields) {
                    if (chainField.javaType == JAVA_TYPE_INT) {
                        fprintf(out, "    env->ReleaseIntArrayElements(%s, %s_array, 0);\n",
                            chainField.name.c_str(), chainField.name.c_str());
                    } else if (chainField.javaType == JAVA_TYPE_STRING) {
                        fprintf(out, "    for (size_t i = 0; i < scoped_%s_vec.size(); ++i) {\n",
                            chainField.name.c_str());
                        fprintf(out, "        delete scoped_%s_vec[i];\n", chainField.name.c_str());
                        fprintf(out, "    }\n");
                    }
                }
            }
            argIndex++;
        }

        fprintf(out, "    return ret;\n");

        fprintf(out, "}\n");
        fprintf(out, "\n");
    }


    return 0;
}

void write_jni_registration(FILE* out, const string& java_method_name,
        const map<vector<java_type_t>, set<string>>& signatures_to_modules,
        const AtomDecl &attributionDecl) {
    for (auto signature_to_modules_it = signatures_to_modules.begin();
            signature_to_modules_it != signatures_to_modules.end(); signature_to_modules_it++) {
        vector<java_type_t> signature = signature_to_modules_it->first;
        fprintf(out, "    { \"%s\", \"%s\", (void*)%s },\n",
            java_method_name.c_str(),
            jni_function_signature(signature, attributionDecl).c_str(),
            jni_function_name(java_method_name, signature).c_str());
    }
}
#endif // JNI helpers.

static int
#if defined(STATS_SCHEMA_LEGACY)
write_stats_log_jni(FILE* out, const Atoms& atoms, const AtomDecl &attributionDecl)
#else
// Write empty JNI file that doesn't contain any JNI methods.
// TODO(b/145100015): remove this function and all JNI autogen code once StatsEvent migration is
// complete.
write_stats_log_jni(FILE* out)
#endif
{
    // Print prelude
    fprintf(out, "// This file is autogenerated\n");
    fprintf(out, "\n");

#if defined(STATS_SCHEMA_LEGACY)
    fprintf(out, "#include <statslog.h>\n");
    fprintf(out, "\n");
    fprintf(out, "#include <nativehelper/JNIHelp.h>\n");
    fprintf(out, "#include <nativehelper/ScopedUtfChars.h>\n");
    fprintf(out, "#include <utils/Vector.h>\n");
#endif
    fprintf(out, "#include \"core_jni_helpers.h\"\n");
    fprintf(out, "#include \"jni.h\"\n");
    fprintf(out, "\n");
#if defined(STATS_SCHEMA_LEGACY)
    fprintf(out, "#define UNUSED  __attribute__((__unused__))\n");
    fprintf(out, "\n");
#endif

    fprintf(out, "namespace android {\n");
    fprintf(out, "\n");

#if defined(STATS_SCHEMA_LEGACY)
    write_stats_log_jni_method(out, "write", "stats_write", atoms.signatures_to_modules, attributionDecl);
    write_stats_log_jni_method(out, "write_non_chained", "stats_write_non_chained",
            atoms.non_chained_signatures_to_modules, attributionDecl);
#endif

    // Print registration function table
    fprintf(out, "/*\n");
    fprintf(out, " * JNI registration.\n");
    fprintf(out, " */\n");
    fprintf(out, "static const JNINativeMethod gRegisterMethods[] = {\n");
#if defined(STATS_SCHEMA_LEGACY)
    write_jni_registration(out, "write", atoms.signatures_to_modules, attributionDecl);
    write_jni_registration(out, "write_non_chained", atoms.non_chained_signatures_to_modules,
            attributionDecl);
#endif
    fprintf(out, "};\n");
    fprintf(out, "\n");

    // Print registration function
    fprintf(out, "int register_android_util_StatsLogInternal(JNIEnv* env) {\n");
    fprintf(out, "    return RegisterMethodsOrDie(\n");
    fprintf(out, "            env,\n");
    fprintf(out, "            \"android/util/StatsLogInternal\",\n");
    fprintf(out, "            gRegisterMethods, NELEM(gRegisterMethods));\n");
    fprintf(out, "}\n");

    fprintf(out, "\n");
    fprintf(out, "} // namespace android\n");
    return 0;
}

static void
print_usage()
{
    fprintf(stderr, "usage: stats-log-api-gen OPTIONS\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "OPTIONS\n");
    fprintf(stderr, "  --cpp FILENAME       the header file to output\n");
    fprintf(stderr, "  --header FILENAME    the cpp file to output\n");
    fprintf(stderr, "  --help               this message\n");
    fprintf(stderr, "  --java FILENAME      the java file to output\n");
    fprintf(stderr, "  --jni FILENAME       the jni file to output\n");
    fprintf(stderr, "  --module NAME        optional, module name to generate outputs for\n");
    fprintf(stderr, "  --namespace COMMA,SEP,NAMESPACE   required for cpp/header with module\n");
    fprintf(stderr, "                                    comma separated namespace of the files\n");
    fprintf(stderr, "  --importHeader NAME  required for cpp/jni to say which header to import\n");
    fprintf(stderr, "  --javaPackage PACKAGE             the package for the java file.\n");
    fprintf(stderr, "                                    required for java with module\n");
    fprintf(stderr, "  --javaClass CLASS    the class name of the java class.\n");
    fprintf(stderr, "                       Optional for Java with module.\n");
    fprintf(stderr, "                       Default is \"StatsLogInternal\"\n");}

/**
 * Do the argument parsing and execute the tasks.
 */
static int
run(int argc, char const*const* argv)
{
    string cppFilename;
    string headerFilename;
    string javaFilename;
    string jniFilename;

    string moduleName = DEFAULT_MODULE_NAME;
    string cppNamespace = DEFAULT_CPP_NAMESPACE;
    string cppHeaderImport = DEFAULT_CPP_HEADER_IMPORT;
    string javaPackage = DEFAULT_JAVA_PACKAGE;
    string javaClass = DEFAULT_JAVA_CLASS;

    int index = 1;
    while (index < argc) {
        if (0 == strcmp("--help", argv[index])) {
            print_usage();
            return 0;
        } else if (0 == strcmp("--cpp", argv[index])) {
            index++;
            if (index >= argc) {
                print_usage();
                return 1;
            }
            cppFilename = argv[index];
        } else if (0 == strcmp("--header", argv[index])) {
            index++;
            if (index >= argc) {
                print_usage();
                return 1;
            }
            headerFilename = argv[index];
        } else if (0 == strcmp("--java", argv[index])) {
            index++;
            if (index >= argc) {
                print_usage();
                return 1;
            }
            javaFilename = argv[index];
        } else if (0 == strcmp("--jni", argv[index])) {
            index++;
            if (index >= argc) {
                print_usage();
                return 1;
            }
            jniFilename = argv[index];
        } else if (0 == strcmp("--module", argv[index])) {
            index++;
            if (index >= argc) {
                print_usage();
                return 1;
            }
            moduleName = argv[index];
        } else if (0 == strcmp("--namespace", argv[index])) {
            index++;
            if (index >= argc) {
                print_usage();
                return 1;
            }
            cppNamespace = argv[index];
        } else if (0 == strcmp("--importHeader", argv[index])) {
            index++;
            if (index >= argc) {
                print_usage();
                return 1;
            }
            cppHeaderImport = argv[index];
        } else if (0 == strcmp("--javaPackage", argv[index])) {
            index++;
            if (index >= argc) {
                print_usage();
                return 1;
            }
            javaPackage = argv[index];
        } else if (0 == strcmp("--javaClass", argv[index])) {
            index++;
            if (index >= argc) {
                print_usage();
                return 1;
            }
            javaClass = argv[index];
        }
        index++;
    }

    if (cppFilename.size() == 0
            && headerFilename.size() == 0
            && javaFilename.size() == 0
            && jniFilename.size() == 0) {
        print_usage();
        return 1;
    }

    // Collate the parameters
    Atoms atoms;
    int errorCount = collate_atoms(Atom::descriptor(), &atoms);
    if (errorCount != 0) {
        return 1;
    }

    AtomDecl attributionDecl;
    vector<java_type_t> attributionSignature;
    collate_atom(android::os::statsd::AttributionNode::descriptor(),
                 &attributionDecl, &attributionSignature);

    // Write the .cpp file
    if (cppFilename.size() != 0) {
        FILE* out = fopen(cppFilename.c_str(), "w");
        if (out == NULL) {
            fprintf(stderr, "Unable to open file for write: %s\n", cppFilename.c_str());
            return 1;
        }
        // If this is for a specific module, the namespace must also be provided.
        if (moduleName != DEFAULT_MODULE_NAME && cppNamespace == DEFAULT_CPP_NAMESPACE) {
            fprintf(stderr, "Must supply --namespace if supplying a specific module\n");
            return 1;
        }
        // If this is for a specific module, the header file to import must also be provided.
        if (moduleName != DEFAULT_MODULE_NAME && cppHeaderImport == DEFAULT_CPP_HEADER_IMPORT) {
            fprintf(stderr, "Must supply --headerImport if supplying a specific module\n");
            return 1;
        }
        errorCount = android::stats_log_api_gen::write_stats_log_cpp(
            out, atoms, attributionDecl, moduleName, cppNamespace, cppHeaderImport);
        fclose(out);
    }

    // Write the .h file
    if (headerFilename.size() != 0) {
        FILE* out = fopen(headerFilename.c_str(), "w");
        if (out == NULL) {
            fprintf(stderr, "Unable to open file for write: %s\n", headerFilename.c_str());
            return 1;
        }
        // If this is for a specific module, the namespace must also be provided.
        if (moduleName != DEFAULT_MODULE_NAME && cppNamespace == DEFAULT_CPP_NAMESPACE) {
            fprintf(stderr, "Must supply --namespace if supplying a specific module\n");
        }
        errorCount = android::stats_log_api_gen::write_stats_log_header(
            out, atoms, attributionDecl, moduleName, cppNamespace);
        fclose(out);
    }

    // Write the .java file
    if (javaFilename.size() != 0) {
        FILE* out = fopen(javaFilename.c_str(), "w");
        if (out == NULL) {
            fprintf(stderr, "Unable to open file for write: %s\n", javaFilename.c_str());
            return 1;
        }
        // If this is for a specific module, the java package must also be provided.
        if (moduleName != DEFAULT_MODULE_NAME && javaPackage== DEFAULT_JAVA_PACKAGE) {
            fprintf(stderr, "Must supply --javaPackage if supplying a specific module\n");
            return 1;
        }

#if defined(STATS_SCHEMA_LEGACY)
        if (moduleName == DEFAULT_MODULE_NAME) {
            errorCount = android::stats_log_api_gen::write_stats_log_java_q(
                    out, atoms, attributionDecl);
        } else {
            errorCount = android::stats_log_api_gen::write_stats_log_java_q_for_module(
                    out, atoms, attributionDecl, moduleName, javaClass, javaPackage);

        }
#else
        if (moduleName == DEFAULT_MODULE_NAME) {
            javaClass = "StatsLogInternal";
            javaPackage = "android.util";
        }
        errorCount = android::stats_log_api_gen::write_stats_log_java(
                out, atoms, attributionDecl, moduleName, javaClass, javaPackage);
#endif

        fclose(out);
    }

    // Write the jni file
    if (jniFilename.size() != 0) {
        FILE* out = fopen(jniFilename.c_str(), "w");
        if (out == NULL) {
            fprintf(stderr, "Unable to open file for write: %s\n", jniFilename.c_str());
            return 1;
        }

#if defined(STATS_SCHEMA_LEGACY)
        errorCount = android::stats_log_api_gen::write_stats_log_jni(
            out, atoms, attributionDecl);
#else
        errorCount = android::stats_log_api_gen::write_stats_log_jni(out);
#endif

        fclose(out);
    }

    return errorCount;
}

}  // namespace stats_log_api_gen
}  // namespace android

/**
 * Main.
 */
int
main(int argc, char const*const* argv)
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    return android::stats_log_api_gen::run(argc, argv);
}
