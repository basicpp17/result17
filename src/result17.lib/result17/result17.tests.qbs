import qbs

Project {
    name: "tests"

    Application {
        name: "result17.tests"
        condition: googletest.present
        consoleApplication: true
        type: ["application", "autotest"]

        Depends { name: "result17" }
        Depends { name: "googletest" }

        files: [
            "Result.test.cpp",
        ]
    }

//    Application {
//        name: "result17.example"
//        consoleApplication: true

//        Depends { name: "result17" }

//        files: [
//            "Result.example.cpp",
//        ]
//    }
}
