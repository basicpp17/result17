import qbs

Application {
    consoleApplication: true

    Depends { name: "cpp" }
    cpp.cxxLanguageVersion: "c++17"

    files: [
        "overloaded.h",
        "result.h",
        "return_type.h",
        "test.cpp",
    ]

}
