#### This directory contains atom definitions.

For adding a new atom:

1. Find the feature subdirectory to which the atom belongs. If none of the existing feature
subdirectories are suitable, create a new one:

1. Enter the atom definition in `<feature>_atoms.proto` file in the feature subdirectory. Create
`<feature>_atoms.proto` file in the feature subdirectory if it does not already exist.

1. If there is no Android.bp file in the features subdirectory, add one with a filegroup definition for your atom definitions:\
```
filegroup {
    name: "libstats_<feature>_atom_protos",
    srcs: [
        "<feature>_atoms.proto",
    ],
}
```

1. If not already present, add `":libstats_<feature>_atom_protos"` to the list of srcs in
`"libstats_atom_protos"` filegroup in `../Android.bp`

