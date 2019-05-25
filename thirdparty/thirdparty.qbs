import qbs
import qbs.File
import qbs.FileInfo

Project {
    minimumQbsVersion: "1.7.1"

    Product {
        name: "Extra Thirdparty Files"
        files: [
            "git_clone_googletest.sh",
        ]
    }

    StaticLibrary {
        id: googletest
        name: "googletest"

        Probe {
            id: googletestProbe
            property pathList searchPathes: ["./googletest", "../../googletest"]

            property path basePath // valid if found

            configure: {
                for (var i=0; i < searchPathes.length; i++) {
                    var c = searchPathes[i];
                    if (File.exists(FileInfo.joinPaths(googletest.sourceDirectory, c, "googletest/src/gtest-all.cc"))) {
                        found = true;
                        basePath = FileInfo.joinPaths(googletest.sourceDirectory, c);
                        return;
                    }
                }
            }
        }
        condition: googletestProbe.found

        files: [
            FileInfo.joinPaths(googletestProbe.basePath, "googlemock/src/gmock-all.cc"),
            FileInfo.joinPaths(googletestProbe.basePath, "googletest/src/gtest-all.cc")
        ]

        Depends { name: "cpp" }
        cpp.cxxLanguageVersion: "c++17"
        cpp.includePaths: [
            FileInfo.joinPaths(googletestProbe.basePath, "googlemock"),
            FileInfo.joinPaths(googletestProbe.basePath, "googlemock/include"),
            FileInfo.joinPaths(googletestProbe.basePath, "googletest"),
            FileInfo.joinPaths(googletestProbe.basePath, "googletest/include"),
        ]
        cpp.defines: ["GTEST_LANG_CXX11"]

        Properties {
            condition: qbs.toolchain.contains('clang')
            cpp.cxxFlags: base.concat("-Wno-unused-parameter")
            cpp.cxxStandardLibrary: "libc++"
        }

        Export {
            Depends { name: "cpp" }
            cpp.includePaths: [
                FileInfo.joinPaths(googletestProbe.basePath, "googlemock/include"),
                FileInfo.joinPaths(googletestProbe.basePath, "googletest/include")
            ]
            cpp.defines: ["GTEST_LANG_CXX11"]

            Properties {
                condition: qbs.toolchain.contains('clang')
                cpp.cxxFlags: base.concat("-Wno-unused-parameter")
                cpp.linkerFlags: base.concat("-lpthread")
            }

            property bool useMain: true
            Group {
                name: "Main"
                condition: product.googletest.useMain

                files: FileInfo.joinPaths(googletestProbe.basePath, "googlemock/src/gmock_main.cc")
            }
        }
    }
}
