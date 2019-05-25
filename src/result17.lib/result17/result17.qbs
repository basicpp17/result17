import qbs

StaticLibrary {
    Depends { name: "cpp17" }

    files: [
        "Error.h",
        "Error.ops.h",
        "Error.ostream.h",
        "Ok.h",
        "Ok.ops.h",
        "Ok.ostream.h",
        "Result.ops.h",
        "Result.ostream.h",
        "Result.trait.h",
        "Overloaded.h",
        "Result.h",
    ]

    Export {
        Depends { name: "cpp17" }
        Depends { name: "cpp" }
        cpp.includePaths: [".."]
    }
}
