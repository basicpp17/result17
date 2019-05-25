import qbs

Project {
    name: "Result17"

    references: [
        "thirdparty",
        "src/result17.lib",
    ]

    AutotestRunner {}

    // note: if do not use this .qbs project you need a similar setup
    Product {
        name: "cpp17"

        Export {
            Depends { name: "cpp" }
            cpp.cxxLanguageVersion: "c++17"
            cpp.treatWarningsAsErrors: true

            Properties {
                condition: qbs.toolchain.contains('msvc')
                cpp.cxxFlags: base.concat(
                    "/permissive-", "/Zc:__cplusplus", // best C++ compatibility
                    "/diagnostics:caret", // better errors
                    "/wd4068", // ignore unknown pragmas
                    "/D_ENABLE_EXTENDED_ALIGNED_STORAGE" // use real alignments
                )
            }
            Properties {
                condition: qbs.toolchain.contains('clang')
                cpp.cxxFlags: base.concat(
                    "--pedantic", // best C++ compatibility
                    "-Wall", "-Wextra" // enable more warnings
                )
                cpp.cxxStandardLibrary: "libc++"
                cpp.staticLibraries: ["c++", "c++abi"]
            }
        }
    }

    Product {
        name: "[Extra Files]"
        files: [
            ".clang-format",
            ".clang-tidy",
            ".editorconfig",
            ".gitignore",
            ".travis.yml",
            "LICENSE",
            "README.md",
        ]
    }
}
