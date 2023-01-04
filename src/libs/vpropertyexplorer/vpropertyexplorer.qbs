VLib {
    Depends { name: "Qt"; submodules: ["widgets"] }
    Depends { name: "VMiscLib" }

    name: "VPropertyExplorerLib"
    files: [
        "vproperty.cpp",
        "vpropertydelegate.cpp",
        "vpropertyfactorymanager.cpp",
        "vpropertyformview.cpp",
        "vpropertyformwidget.cpp",
        "vpropertymodel.cpp",
        "vpropertyset.cpp",
        "vpropertytreeview.cpp",
        "vserializedproperty.cpp",
        "vstandardpropertyfactory.cpp",
        "checkablemessagebox.cpp",
        "vpropertyexplorer_global.h",
        "vpropertyfactorymanager_p.h",
        "vpropertytreeview_p.h",
        "vpropertyset_p.h",
        "vabstractpropertyfactory.h",
        "vfileproperty_p.h",
        "vwidgetproperty_p.h",
        "vpropertymodel_p.h",
        "vstandardpropertyfactory.h",
        "vpropertyformview_p.h",
        "vpropertytreeview.h",
        "vpropertyformwidget_p.h",
        "vpropertydelegate.h",
        "vproperty_p.h",
        "vpropertyformwidget.h",
        "vpropertyformview.h",
        "vpropertyset.h",
        "vpropertymodel.h",
        "vproperty.h",
        "vpropertyfactorymanager.h",
        "vserializedproperty.h",
        "vproperties.h",
        "checkablemessagebox.h",
    ]

    buildconfig.staticBuild: false

    Group {
        name: "plugins"
        prefix: "plugins/"
        files: [
            "vtextproperty.cpp",
            "vwidgetproperty.cpp",
            "vemptyproperty.cpp",
            "vboolproperty.cpp",
            "vshortcutproperty.cpp",
            "vcolorproperty.cpp",
            "vshortcutpropertyeditor.cpp",
            "venumproperty.cpp",
            "vfileproperty.cpp",
            "vcolorpropertyeditor.cpp",
            "vfilepropertyeditor.cpp",
            "vnumberproperty.cpp",
            "Vector3d/vvector3dproperty.cpp",
            "vstringproperty.cpp",
            "vpointfproperty.cpp",
            "vobjectproperty.cpp",
            "vlinetypeproperty.cpp",
            "vlinecolorproperty.cpp",
            "vlabelproperty.cpp",
            "vtextproperty.h",
            "vwidgetproperty.h",
            "vcolorproperty.h",
            "vboolproperty.h",
            "vcolorpropertyeditor.h",
            "vshortcutpropertyeditor.h",
            "vemptyproperty.h",
            "vshortcutproperty.h",
            "venumproperty.h",
            "vfilepropertyeditor.h",
            "vfileproperty.h",
            "vnumberproperty.h",
            "Vector3d/vvector3dproperty.h",
            "vstringproperty.h",
            "vpointfproperty.h",
            "vobjectproperty.h",
            "vlinetypeproperty.h",
            "vlinecolorproperty.h",
            "vlabelproperty.h",
        ]
    }

    Group {
        name: "Precompiled headers"
        files: {
            var files = ["stable.h"];
            if (qbs.toolchain.contains("msvc"))
                files.push("stable.cpp")
            return files;
        }
        fileTags: ["cpp_pch_src"]
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [exportingProduct.sourceDirectory]
    }
}
