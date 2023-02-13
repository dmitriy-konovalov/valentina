import qbs.Probes

Project {
    name: "Valentina"
    minimumQbsVersion: "1.16"

    property bool enableConan: false

    Probes.ConanfileProbe {
        id: thirdPartyConanPackages
        condition: enableConan
        conanfilePath: project.sourceDirectory + "/conanfile.txt"
        options: {
            if (qbs.targetOS.contains("unix") && !qbs.targetOS.contains("macos"))
                return ({"xerces-c:shared": "False"});

            return ({"xerces-c:shared": "True"});
        }
    }

    references: [
        "src/src.qbs",
        "share/translations/translations.qbs",
        "share/translations/measurements.qbs",
    ]
    qbsSearchPaths: "qbs"

    AutotestRunner {}
}
