import qbs

Application {
    consoleApplication: true

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion: "c++17"

    files: [
        "invocable.h",
        "overloaded.h",
        "result.h",
        "return_type.h",
        "test.cpp",
    ]

    cpp.cxxStandardLibrary: {
        if (qbs.toolchain.contains('clang')) return "libc++";
    }
    cpp.staticLibraries: {
        if (qbs.toolchain.contains('clang')) return ["c++", "c++abi"];
    }
}
