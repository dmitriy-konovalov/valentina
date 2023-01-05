VLib {
    Depends { name: "Qt"; submodules: ["concurrent", "svg"] }
    Depends { name: "VMiscLib" }
    Depends { name: "VGeometryLib" }
    Depends { name: "VPatternDBLib" }
    Depends { name: "VObjLib" }
    Depends { name: "QMUParserLib" }
    Depends { name: "VDXFLib" }
    Depends { name: "IFCLib" }
    Depends { name: "VWidgetsLib" }
    Depends { name: "VFormatLib" }

    name: "VLayoutLib"
    files: [
        "vlayoutexporter.h",
        "vlayoutgenerator.h",
        "vlayoutdef.h",
        "vlayoutpaper.h",
        "vlayoutpaper_p.h",
        "vbank.h",
        "vcontour.h",
        "vcontour_p.h",
        "vbestsquare.h",
        "vlayoutpoint.h",
        "vposition.h",
        "vrawlayout.h", 
        "vprintlayout.h",
        "vsapoint.h",
        "vtextmanager.h",
        "vposter.h",
        "vgraphicsfillitem.h",
        "vabstractpiece.h",
        "vabstractpiece_p.h",
        "vlayoutpiece.h",
        "vlayoutpiece_p.h",
        "vlayoutpiecepath.h",
        "vlayoutpiecepath_p.h",
        "vbestsquare_p.h",
        "vrawsapoint.h",
        "vlayoutexporter.cpp",
        "vlayoutgenerator.cpp",
        "vlayoutpaper.cpp",
        "vbank.cpp",
        "vcontour.cpp",
        "vbestsquare.cpp",
        "vlayoutpoint.cpp",
        "vposition.cpp",
        "vrawlayout.cpp",
        "vsapoint.cpp",
        "vprintlayout.cpp",
        "vtextmanager.cpp",
        "vposter.cpp",
        "vgraphicsfillitem.cpp",
        "vabstractpiece.cpp",
        "vlayoutpiece.cpp",
        "vlayoutpiecepath.cpp",
        "vrawsapoint.cpp",
    ]

    Group {
        name: "dialogs"
        prefix: "dialogs/"
        files: [
            "dialoglayoutscale.h",
            "vabstractlayoutdialog.h",
            "watermarkwindow.h",
            "dialoglayoutscale.cpp",
            "vabstractlayoutdialog.cpp",
            "watermarkwindow.cpp",
            "dialoglayoutscale.ui",
            "watermarkwindow.ui",
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

    Group {
        name: "Resources"
        prefix: "share/"
        files: [
            "icons.qrc",
        ]
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: [exportingProduct.sourceDirectory]
    }
}
