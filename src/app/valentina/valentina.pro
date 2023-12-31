#-------------------------------------------------
#
# Project created by QtCreator 2013-06-18T12:36:43
#
#-------------------------------------------------

# Compilation main binary file

# File with common stuff for whole project
include(../../../common.pri)

# Here we don't see "network" library, but, i think, "printsupport" depend on this library, so we still need this
# library in installer.
QT       += core gui widgets xml svg printsupport xmlpatterns concurrent

# Use winextras only for Windows 7+
win32:greaterThan(QT_MAJOR_VERSION, 4):greaterThan(QT_MINOR_VERSION, 6) {
    QT += winextras
}

# We want create executable file
TEMPLATE = app

# Name of binary file
macx{
    TARGET = Valentina
} else {
    TARGET = valentina
}

VERSION = 0.7.52

# Use out-of-source builds (shadow builds)
CONFIG -= debug_and_release debug_and_release_target

# Since Q5.12 available support for C++17
equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 11) {
    CONFIG += c++17
} else {
    CONFIG += c++14
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Since Qt 5.4.0 the source code location is recorded only in debug builds.
# We need this information also in release builds. For this need define QT_MESSAGELOGCONTEXT.
DEFINES += QT_MESSAGELOGCONTEXT

# Directory for executable file
DESTDIR = bin

# Directory for files created moc
MOC_DIR = moc

# Directory for objecs files
OBJECTS_DIR = obj

# Directory for files created rcc
RCC_DIR = rcc

# Directory for files created uic
UI_DIR = uic

# Suport subdirectories. Just better project code tree.
include(valentina.pri)

# Resource files. This files will be included in binary.
RESOURCES += \
    share/resources/toolcursor.qrc \ # Tools cursor icons.
    share/resources/toolicon.qrc

# Compilation will fail without this files after we added them to this section.
OTHER_FILES += \
    share/resources/icon/64x64/icon64x64.ico # Valentina's logo.

# Set using ccache. Function enable_ccache() defined in common.pri.
$$enable_ccache()

include(warnings.pri)

CONFIG(release, debug|release){
    # Release mode
    !*msvc*:CONFIG += silent
    DEFINES += V_NO_ASSERT
    !unix:*g++*{
        QMAKE_CXXFLAGS += -fno-omit-frame-pointer # Need for exchndl.dll
    }

    noDebugSymbols{ # For enable run qmake with CONFIG+=noDebugSymbols
        DEFINES += V_NO_DEBUG
    } else {
        # Turn on debug symbols in release mode on Unix systems.
        # On Mac OS X temporarily disabled. Need find way how to strip binary file.
        !macx:!*msvc*{
            QMAKE_CXXFLAGS_RELEASE += -g -gdwarf-3
            QMAKE_CFLAGS_RELEASE += -g -gdwarf-3
            QMAKE_LFLAGS_RELEASE =
        }
    }
} else {
# Breakpoints do not work if debug the app inside of bundle. In debug mode we turn off creating a bundle.
# Probably it will breake some dependencies. Version for Mac designed to work inside an app bundle.
    CONFIG -= app_bundle
}

DVCS_HESH=$$FindBuildRevision()
message("Build revision:" $${DVCS_HESH})
DEFINES += "BUILD_REVISION=$${DVCS_HESH}" # Make available build revision number in sources.

# Some extra information about Qt. Can be usefull.
message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Qt resources can be found in the following locations:)
message(Documentation: $$[QT_INSTALL_DOCS])
message(Header files: $$[QT_INSTALL_HEADERS])
message(Libraries: $$[QT_INSTALL_LIBS])
message(Binary files (executables): $$[QT_INSTALL_BINS])
message(Plugins: $$[QT_INSTALL_PLUGINS])
message(Data files: $$[QT_INSTALL_DATA])
message(Translation files: $$[QT_INSTALL_TRANSLATIONS])
message(Settings: $$[QT_INSTALL_SETTINGS])
message(Examples: $$[QT_INSTALL_EXAMPLES])

# Path to recource file.
win32:RC_FILE = share/resources/valentina.rc

include(../svgfont.pri)

win32 {
    INSTALL_PDFTOPS += ../../../dist/win/pdftops.exe
}

include(../translations.pri)

# Set "make install" command for Unix-like systems.
unix{
    !macx{
        # Path to bin file after installation
        target.path = $$BINDIR

        tape.path = $$BINDIR
        tape.files += $${OUT_PWD}/../tape/$${DESTDIR}/tape

        puzzle.path = $$BINDIR
        puzzle.files += $${OUT_PWD}/../puzzle/$${DESTDIR}/puzzle

        # .desktop file
        desktop.path = $$DATADIR/applications/
        desktop.files += ../../../dist/ua.com.smart-pattern.$${TARGET}.desktop \
        desktop.files += ../../../dist/ua.com.smart-pattern.tape.desktop \
        desktop.files += ../../../dist/ua.com.smart-pattern.puzzle.desktop

        # .metainfo.xml file
        metainfo.path = $$DATADIR/metainfo/
        metainfo.files += ../../../dist/ua.com.smart-pattern.$${TARGET}.metainfo.xml

        # logo
        hicolor_48_apps.path = $$DATADIR/icons/hicolor/48x48/apps/
        hicolor_48_apps.files += \
            ../../../share/icons/48x48/apps/$${TARGET}.png \
            ../../../share/icons/48x48/apps/tape.png \
            ../../../share/icons/48x48/apps/puzzle.png

        hicolor_48_mimetypes.path = $$DATADIR/icons/hicolor/48x48/mimetypes/
        hicolor_48_mimetypes.files += \
            ../../../share/icons/48x48/mimetypes/application-x-valentina-pattern.png \
            ../../../share/icons/48x48/mimetypes/application-x-valentina-i-measurements.png \
            ../../../share/icons/48x48/mimetypes/application-x-valentina-s-measurements.png \
            ../../../share/icons/48x48/mimetypes/application-x-valentina-layout.png

        hicolor_64_apps.path = $$DATADIR/icons/hicolor/64x64/apps/
        hicolor_64_apps.files += \
            ../../../share/icons/64x64/apps/$${TARGET}.png \
            ../../../share/icons/64x64/apps/tape.png \
            ../../../share/icons/64x64/apps/puzzle.png

        hicolor_64_mimetypes.path = $$DATADIR/icons/hicolor/64x64/mimetypes/
        hicolor_64_mimetypes.files += \
            ../../../share/icons/64x64/mimetypes/application-x-valentina-pattern.png \
            ../../../share/icons/64x64/mimetypes/application-x-valentina-i-measurements.png \
            ../../../share/icons/64x64/mimetypes/application-x-valentina-s-measurements.png \
            ../../../share/icons/64x64/mimetypes/application-x-valentina-layout.png

        hicolor_128_apps.path = $$DATADIR/icons/hicolor/128x128/apps/
        hicolor_128_apps.files += \
            ../../../share/icons/128x128/apps/$${TARGET}.png \
            ../../../share/icons/128x128/apps/tape.png \
            ../../../share/icons/128x128/apps/puzzle.png

        hicolor_128_mimetypes.path = $$DATADIR/icons/hicolor/128x128/mimetypes/
        hicolor_128_mimetypes.files += \
            ../../../share/icons/128x128/mimetypes/application-x-valentina-pattern.png \
            ../../../share/icons/128x128/mimetypes/application-x-valentina-i-measurements.png \
            ../../../share/icons/128x128/mimetypes/application-x-valentina-s-measurements.png \
            ../../../share/icons/128x128/mimetypes/application-x-valentina-layout.png

        hicolor_256_apps.path = $$DATADIR/icons/hicolor/256x256/apps/
        hicolor_256_apps.files += \
            ../../../share/icons/256x256/apps/$${TARGET}.png \
            ../../../share/icons/256x256/apps/tape.png \
            ../../../share/icons/256x256/apps/puzzle.png

        hicolor_256_mimetypes.path = $$DATADIR/icons/hicolor/256x256/mimetypes/
        hicolor_256_mimetypes.files += \
            ../../../share/icons/256x256/mimetypes/application-x-valentina-pattern.png \
            ../../../share/icons/256x256/mimetypes/application-x-valentina-i-measurements.png \
            ../../../share/icons/256x256/mimetypes/application-x-valentina-s-measurements.png \
            ../../../share/icons/256x256/mimetypes/application-x-valentina-layout.png

        hicolor_512_apps.path = $$DATADIR/icons/hicolor/512x512/apps/
        hicolor_512_apps.files += \
            ../../../share/icons/512x512/apps/$${TARGET}.png \
            ../../../share/icons/512x512/apps/tape.png \
            ../../../share/icons/512x512/apps/puzzle.png

        hicolor_512_mimetypes.path = $$DATADIR/icons/hicolor/512x512/mimetypes/
        hicolor_512_mimetypes.files += \
            ../../../share/icons/512x512/mimetypes/application-x-valentina-pattern.png \
            ../../../share/icons/512x512/mimetypes/application-x-valentina-i-measurements.png \
            ../../../share/icons/512x512/mimetypes/application-x-valentina-s-measurements.png \
            ../../../share/icons/512x512/mimetypes/application-x-valentina-layout.png

        # Path to translation files after installation
        translations.path = $$PKGDATADIR/translations/
        translations.files = $$INSTALL_TRANSLATIONS

        # Path to templates after installation
        templates.path = $$PKGDATADIR/tables/templates/
        templates.files = $$INSTALL_STANDARD_TEMPLATES

        svgfonts.path = $$PKGDATADIR/svgfonts/
        svgfonts.files = $$INSTALL_SVG_FONTS

        INSTALLS += \
            target \
            tape \
            puzzle \
            desktop \
            metainfo \
            hicolor_48_apps \
            hicolor_48_mimetypes \
            hicolor_64_apps \
            hicolor_64_mimetypes \
            hicolor_128_apps \
            hicolor_128_mimetypes \
            hicolor_256_apps \
            hicolor_256_mimetypes \
            hicolor_512_apps \
            hicolor_512_mimetypes \
            translations \
            templates \
            svgfonts
    }
    macx{
        # Some macx stuff
        QMAKE_MAC_SDK = macosx

        # QMAKE_MACOSX_DEPLOYMENT_TARGET defined in common.pri

        CONFIG(release, debug|release){
            QMAKE_RPATHDIR += @executable_path/../Frameworks

            # Path to resources in app bundle
            #RESOURCES_DIR = "Contents/Resources" defined in translation.pri
            FRAMEWORKS_DIR = "Contents/Frameworks"
            MACOS_DIR = "Contents/MacOS"
            # On macx we will use app bundle. Bundle doesn't need bin directory inside.
            # See issue #166: Creating OSX Homebrew (Mac OS X package manager) formula.
            target.path = $$MACOS_DIR

            #languages added inside translations.pri

            # Symlinks also good names for copying. Make will take origin file and copy them with using symlink name.
            # For bundle this names more then enough. We don't need care much about libraries versions.
            libraries.path = $$FRAMEWORKS_DIR
            libraries.files += $${OUT_PWD}/../../libs/qmuparser/$${DESTDIR}/libqmuparser.2.dylib
            libraries.files += $${OUT_PWD}/../../libs/vpropertyexplorer/$${DESTDIR}/libvpropertyexplorer.1.dylib

            tape.path = $$MACOS_DIR
            tape.files += $${OUT_PWD}/../tape/$${DESTDIR}/tape.app/$$MACOS_DIR/tape

            puzzle.path = $$MACOS_DIR
            puzzle.files += $${OUT_PWD}/../puzzle/$${DESTDIR}/puzzle.app/$$MACOS_DIR/puzzle

            # Utility pdftops need for saving a layout image to PS and EPS formates.
            xpdf.path = $$MACOS_DIR
            xpdf.files += $${PWD}/../../../dist/macx/bin64/pdftops

            # logo on macx.
            ICON = ../../../dist/Valentina.icns

            QMAKE_INFO_PLIST = $$PWD/../../../dist/macx/valentina/Info.plist

            # Copy to bundle templates files
            templates.path = $$RESOURCES_DIR/tables/templates/
            templates.files = $$INSTALL_STANDARD_TEMPLATES

            svgfonts.path = $$RESOURCES_DIR/svgfonts/
            svgfonts.files = $$INSTALL_SVG_FONTS

            icns_resources.path = $$RESOURCES_DIR/
            icns_resources.files += $$PWD/../../../dist/macx/i-measurements.icns
            icns_resources.files += $$PWD/../../../dist/macx/s-measurements.icns
            icns_resources.files += $$PWD/../../../dist/macx/pattern.icns
            icns_resources.files += $$PWD/../../../dist/macx/layout.icns

            # Copy to bundle multisize measurements files
            # We cannot add none exist files to bundle through QMAKE_BUNDLE_DATA. That's why we must do this manually.
            QMAKE_POST_LINK += $$VCOPY $$quote($${OUT_PWD}/../tape/$${DESTDIR}/tape.app/$$RESOURCES_DIR/diagrams.rcc) $$quote($$shell_path($${OUT_PWD}/$$DESTDIR/$${TARGET}.app/$$RESOURCES_DIR/)) $$escape_expand(\\n\\t)

            QMAKE_BUNDLE_DATA += \
                templates \
                svgfonts \
                libraries \
                tape \
                puzzle \
                xpdf \
                icns_resources
        }
    }
}

# "make install" command for Windows.
# Depend on inno setup script and create installer in folder "package"
win32:*g++* {
    package.path = $${OUT_PWD}/../../../package/valentina
    package.files += \
        $${OUT_PWD}/$${DESTDIR}/valentina.exe \
        $${OUT_PWD}/../tape/$${DESTDIR}/tape.exe \
        $${OUT_PWD}/../puzzle/$${DESTDIR}/puzzle.exe \
        $${OUT_PWD}/../tape/$${DESTDIR}/diagrams.rcc \
        $$PWD/../../../dist/win/valentina.ico \
        $$PWD/../../../dist/win/i-measurements.ico \
        $$PWD/../../../dist/win/s-measurements.ico \
        $$PWD/../../../dist/win/pattern.ico \
        $$PWD/../../../dist/win/layout.ico \
        $$PWD/../../../dist/win/pdftops.exe \
        $$PWD/../../../dist/win/EUDC.TTE \
        $$PWD/../../../AUTHORS.txt \
        $$PWD/../../../LICENSE_GPL.txt \
        $$PWD/../../../README.txt \
        $$PWD/../../../ChangeLog.txt \
        $$PWD/../../../share/qtlogging.ini \
        $$PWD/../../libs/qmuparser/LICENSE_BSD.txt \
        $${OUT_PWD}/../../libs/qmuparser/$${DESTDIR}/qmuparser2.dll \
        $${OUT_PWD}/../../libs/vpropertyexplorer/$${DESTDIR}/vpropertyexplorer.dll \
        $$[QT_INSTALL_BINS]/Qt5Core.dll \
        $$[QT_INSTALL_BINS]/Qt5Concurrent.dll \
        $$[QT_INSTALL_BINS]/Qt5Gui.dll \
        $$[QT_INSTALL_BINS]/Qt5Network.dll \
        $$[QT_INSTALL_BINS]/Qt5PrintSupport.dll \
        $$[QT_INSTALL_BINS]/Qt5Svg.dll \
        $$[QT_INSTALL_BINS]/Qt5Widgets.dll \
        $$[QT_INSTALL_BINS]/Qt5Xml.dll \
        $$[QT_INSTALL_BINS]/Qt5XmlPatterns.dll \
        $$[QT_INSTALL_BINS]/libgcc_s_*-1.dll \ # There are several different exception handler for MinGW available: sjlj, dwarf, seh
        $$[QT_INSTALL_BINS]/libstdc++-6.dll \
        $$[QT_INSTALL_BINS]/libwinpthread-1.dll \
        $$[QT_INSTALL_BINS]/libEGL.dll \
        $$[QT_INSTALL_BINS]/libGLESv2.dll \
        $$[QT_INSTALL_BINS]/d3dcompiler_*.dll \
        $$[QT_INSTALL_BINS]/opengl32sw.dll

    # For support Windows 7+
    greaterThan(QT_MAJOR_VERSION, 4):greaterThan(QT_MINOR_VERSION, 6) {
        package.files += $$[QT_INSTALL_BINS]/Qt5WinExtras.dll
    }

    greaterThan(QT_MAJOR_VERSION, 4):greaterThan(QT_MINOR_VERSION, 10) {
        versionAtLeast(QT_VERSION, 5.12.4){
            # Minimal supported OpenSSL version since Qt 5.12.4 is 1.1.1.
            contains(QMAKE_HOST.arch, x86_64) {
                package.files += \
                    $$PWD/../../../dist/win/openssl/win64/libcrypto-1_1-x64.dll \
                    $$PWD/../../../dist/win/openssl/win64/libssl-1_1-x64.dll
            } else {
                package.files += \
                    $$PWD/../../../dist/win/openssl/win32/libcrypto-1_1.dll \
                    $$PWD/../../../dist/win/openssl/win32/libssl-1_1.dll
            }
        } else {
            package.files += \
                $$PWD/../../../dist/win/msvcr120.dll \

            contains(QMAKE_HOST.arch, x86_64) {
                package.files += \
                    $$PWD/../../../dist/win/openssl/win64/libeay32.dll \
                    $$PWD/../../../dist/win/openssl/win64/ssleay32.dll
            } else {
                package.files += \
                    $$PWD/../../../dist/win/openssl/win32/libeay32.dll \
                    $$PWD/../../../dist/win/openssl/win32/ssleay32.dll
            }
        }
    } else {
        package.files += \
            $$PWD/../../../dist/win/msvcr120.dll \

        contains(QMAKE_HOST.arch, x86_64) {
            package.files += \
                $$PWD/../../../dist/win/openssl/win64/libeay32.dll \
                $$PWD/../../../dist/win/openssl/win64/ssleay32.dll
        } else {
            package.files += \
                $$PWD/../../../dist/win/openssl/win32/libeay32.dll \
                $$PWD/../../../dist/win/openssl/win32/ssleay32.dll
        }
    }

    package.CONFIG = no_check_exist
    INSTALLS += package

    package_templates.path = $${OUT_PWD}/../../../package/valentina/tables/templates
    package_templates.files += $$INSTALL_STANDARD_TEMPLATES
    INSTALLS += package_templates

    package_svgfonts.path = $${OUT_PWD}/../../../package/valentina/svgfonts
    package_svgfonts.files += $$INSTALL_SVG_FONTS
    INSTALLS += package_svgfonts

    package_translations.path = $${OUT_PWD}/../../../package/valentina/translations
    package_translations.files += \
        $$INSTALL_TRANSLATIONS \ # Valentina
        $$[QT_INSTALL_TRANSLATIONS]/qt_ar.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_pl.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_pt.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_sk.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_sl.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_sv.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_uk.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_zh_CN.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_zh_TW.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_ca.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_cs.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_da.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_de.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_es.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_en.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_fa.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_fi.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_fr.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_gl.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_he.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_hu.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_it.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_ja.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_ko.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_lt.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qtxmlpatterns_*.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qtbase_*.qm
    INSTALLS += package_translations

    package_bearer.path = $${OUT_PWD}/../../../package/valentina/bearer
    package_bearer.files += \
        $$[QT_INSTALL_PLUGINS]/bearer/qgenericbearer.dll \
        $$[QT_INSTALL_PLUGINS]/bearer/qnativewifibearer.dll
    INSTALLS += package_bearer

    package_iconengines.path = $${OUT_PWD}/../../../package/valentina/iconengines
    package_iconengines.files += $$[QT_INSTALL_PLUGINS]/iconengines/qsvgicon.dll
    INSTALLS += package_iconengines

    package_imageformats.path = $${OUT_PWD}/../../../package/valentina/imageformats
    package_imageformats.files += \
        $$[QT_INSTALL_PLUGINS]/imageformats/qdds.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qgif.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qicns.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qico.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qjp2.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qjpeg.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qmng.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qsvg.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qtga.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qtiff.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qwbmp.dll \
        $$[QT_INSTALL_PLUGINS]/imageformats/qwebp.dll
    INSTALLS += package_imageformats

    package_platforms.path = $${OUT_PWD}/../../../package/valentina/platforms
    package_platforms.files += $$[QT_INSTALL_PLUGINS]/platforms/qwindows.dll
    INSTALLS += package_platforms

    package_printsupport.path = $${OUT_PWD}/../../../package/valentina/printsupport
    package_printsupport.files += $$[QT_INSTALL_PLUGINS]/printsupport/windowsprintersupport.dll
    INSTALLS += package_printsupport

    # Since 5.10, platform styles such as QWindowsVistaStyle, QMacStyle, etc., are no longer embedded in the QtWidgets
    # library.
    greaterThan(QT_MAJOR_VERSION, 4):greaterThan(QT_MINOR_VERSION, 9) {
        package_styles.path = $${OUT_PWD}/../../../package/valentina/styles
        package_styles.files += $$[QT_INSTALL_PLUGINS]/styles/qwindowsvistastyle.dll
        INSTALLS += package_styles
    }

    noWindowsInstaller{ # For enable run qmake with CONFIG+=noWindowsInstaller
        #do nothing
    } else {
        SCP_FOUND = false
        exists("C:/Program Files (x86)/Inno Setup 5/iscc.exe") {
                    INNO_ISCC = "C:/Program Files (x86)/Inno Setup 5/iscc.exe"
                    SCP_FOUND = true
            } else {
                exists("C:/Program Files/Inno Setup 5/iscc.exe") {
                    INNO_ISCC = "C:/Program Files/Inno Setup 5/iscc.exe"
                    SCP_FOUND = true
               }
        }

        if($$SCP_FOUND) {
            package_inno.path = $${OUT_PWD}/../../../package
            package_inno.files += \
                $$PWD/../../../dist/win/inno/LICENSE_VALENTINA \
                $$PWD/../../../dist/win/inno/valentina.iss
            INSTALLS += package_inno

            # Do the packaging
            # First, mangle all of INSTALLS values. We depend on them.
            unset(MANGLED_INSTALLS)
            for(x, INSTALLS):MANGLED_INSTALLS += install_$${x}
            build_package.path = $${OUT_PWD}/../../../package
            build_package.commands = $$INNO_ISCC \"$${OUT_PWD}/../../../package/valentina.iss\"
            build_package.depends = $${MANGLED_INSTALLS}
            INSTALLS += build_package
        } else {
            message("Inno Setup was not found!")
        }
    }
}

win32 {
    for(DIR, INSTALL_PDFTOPS) {
        #add these absolute paths to a variable which
        #ends up as 'mkcommands = path1 path2 path3 ...'
        pdftops_path += $${PWD}/$$DIR
    }
    copyToDestdir($$pdftops_path, $$shell_path($${OUT_PWD}/$$DESTDIR))

    for(DIR, INSTALL_OPENSSL) {
        #add these absolute paths to a variable which
        #ends up as 'mkcommands = path1 path2 path3 ...'
        openssl_path += $${PWD}/$$DIR
    }
    copyToDestdir($$openssl_path, $$shell_path($${OUT_PWD}/$$DESTDIR))
}

noRunPath{ # For enable run qmake with CONFIG+=noRunPath
    # do nothing
} else {
    unix:!macx{
        # suppress the default RPATH
        # helps to run the program without Qt Creator
        # see problem with path to libqmuparser and libpropertybrowser
        QMAKE_LFLAGS_RPATH =
        QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\' -Wl,-rpath,$${OUT_PWD}/../../libs/qmuparser/$${DESTDIR} -Wl,-rpath,$${OUT_PWD}/../../libs/vpropertyexplorer/$${DESTDIR}"
    }
}

# When the GNU linker sees a library, it discards all symbols that it doesn't need.
# Dependent library go first.

#VTools static library (depend on VWidgets, VMisc, VPatternDB)
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vtools/$${DESTDIR}/ -lvtools

INCLUDEPATH += $$PWD/../../libs/vtools
INCLUDEPATH += $$OUT_PWD/../../libs/vtools/$${UI_DIR} # For UI files
DEPENDPATH += $$PWD/../../libs/vtools

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vtools/$${DESTDIR}/vtools.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vtools/$${DESTDIR}/libvtools.a

# VLayout static library  (depend on IFC, VGeometry, VWidgets, VFormat)
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vlayout/$${DESTDIR}/ -lvlayout

INCLUDEPATH += $$PWD/../../libs/vlayout
DEPENDPATH += $$PWD/../../libs/vlayout

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vlayout/$${DESTDIR}/vlayout.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vlayout/$${DESTDIR}/libvlayout.a

#VWidgets static library
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vwidgets/$${DESTDIR}/ -lvwidgets

INCLUDEPATH += $$PWD/../../libs/vwidgets
DEPENDPATH += $$PWD/../../libs/vwidgets

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vwidgets/$${DESTDIR}/vwidgets.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vwidgets/$${DESTDIR}/libvwidgets.a

# VFormat static library (depend on VPatternDB, IFC)
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vformat/$${DESTDIR}/ -lvformat

INCLUDEPATH += $$PWD/../../libs/vformat
DEPENDPATH += $$PWD/../../libs/vformat

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vformat/$${DESTDIR}/vformat.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vformat/$${DESTDIR}/libvformat.a

#VPatternDB static library (depend on vgeometry, vmisc)
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vpatterndb/$${DESTDIR} -lvpatterndb

INCLUDEPATH += $$PWD/../../libs/vpatterndb
DEPENDPATH += $$PWD/../../libs/vpatterndb

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vpatterndb/$${DESTDIR}/vpatterndb.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vpatterndb/$${DESTDIR}/libvpatterndb.a

# VGeometry static library (depend on ifc)
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vgeometry/$${DESTDIR}/ -lvgeometry

INCLUDEPATH += $$PWD/../../libs/vgeometry
DEPENDPATH += $$PWD/../../libs/vgeometry

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vgeometry/$${DESTDIR}/vgeometry.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vgeometry/$${DESTDIR}/libvgeometry.a

# Fervor static library (depend on VMisc, IFC)
unix|win32: LIBS += -L$$OUT_PWD/../../libs/fervor/$${DESTDIR}/ -lfervor

INCLUDEPATH += $$PWD/../../libs/fervor
DEPENDPATH += $$PWD/../../libs/fervor

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/fervor/$${DESTDIR}/fervor.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/fervor/$${DESTDIR}/libfervor.a

# IFC static library (depend on QMuParser, VMisc)
unix|win32: LIBS += -L$$OUT_PWD/../../libs/ifc/$${DESTDIR}/ -lifc

INCLUDEPATH += $$PWD/../../libs/ifc
DEPENDPATH += $$PWD/../../libs/ifc

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/ifc/$${DESTDIR}/ifc.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/ifc/$${DESTDIR}/libifc.a

#VMisc static library
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vmisc/$${DESTDIR}/ -lvmisc

INCLUDEPATH += $$PWD/../../libs/vmisc
DEPENDPATH += $$PWD/../../libs/vmisc

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vmisc/$${DESTDIR}/vmisc.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vmisc/$${DESTDIR}/libvmisc.a

# VObj static library
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vobj/$${DESTDIR}/ -lvobj

INCLUDEPATH += $$PWD/../../libs/vobj
DEPENDPATH += $$PWD/../../libs/vobj

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vobj/$${DESTDIR}/vobj.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vobj/$${DESTDIR}/libvobj.a

# VDxf static library
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vdxf/$${DESTDIR}/ -lvdxf

INCLUDEPATH += $$PWD/../../libs/vdxf
DEPENDPATH += $$PWD/../../libs/vdxf

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vdxf/$${DESTDIR}/vdxf.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vdxf/$${DESTDIR}/libvdxf.a

# VHPGL static library
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vhpgl/$${DESTDIR}/ -lvhpgl

INCLUDEPATH += $$PWD/../../libs/vhpgl
DEPENDPATH += $$PWD/../../libs/vhpgl

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vhpgl/$${DESTDIR}/vhpgl.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vhpgl/$${DESTDIR}/libvhpgl.a

# VGAnalytics static library
unix|win32: LIBS += -L$$OUT_PWD/../../libs/vganalytics/$${DESTDIR}/ -lvganalytics

INCLUDEPATH += $$PWD/../../libs/vganalytics
DEPENDPATH += $$PWD/../../libs/vganalytics

win32:!win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vganalytics/$${DESTDIR}/vganalytics.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/vganalytics/$${DESTDIR}/libvganalytics.a

# QMuParser library
win32:CONFIG(release, debug|release): LIBS += -L$${OUT_PWD}/../../libs/qmuparser/$${DESTDIR} -lqmuparser2
else:win32:CONFIG(debug, debug|release): LIBS += -L$${OUT_PWD}/../../libs/qmuparser/$${DESTDIR} -lqmuparser2
else:unix: LIBS += -L$${OUT_PWD}/../../libs/qmuparser/$${DESTDIR} -lqmuparser

INCLUDEPATH += $${PWD}/../../libs/qmuparser
DEPENDPATH += $${PWD}/../../libs/qmuparser

# VPropertyExplorer library
win32:CONFIG(release, debug|release): LIBS += -L$${OUT_PWD}/../../libs/vpropertyexplorer/$${DESTDIR} -lvpropertyexplorer
else:win32:CONFIG(debug, debug|release): LIBS += -L$${OUT_PWD}/../../libs/vpropertyexplorer/$${DESTDIR} -lvpropertyexplorer
else:unix: LIBS += -L$${OUT_PWD}/../../libs/vpropertyexplorer/$${DESTDIR} -lvpropertyexplorer

INCLUDEPATH += $${PWD}/../../libs/vpropertyexplorer
DEPENDPATH += $${PWD}/../../libs/vpropertyexplorer

contains(DEFINES, APPIMAGE) {
    unix:!macx: LIBS += -licudata -licui18n -licuuc
}

CONFIG(release, debug|release){
    noStripDebugSymbols {
        # do nothing
    } else {
        !macx:!*msvc*{
            noDebugSymbols{ # For enable run qmake with CONFIG+=noDebugSymbols
                win32{
                    # Strip after you link all libaries.
                    QMAKE_POST_LINK += objcopy --strip-debug bin/${TARGET}
                }

                unix{
                    # Strip after you link all libaries.
                    QMAKE_POST_LINK += objcopy --strip-debug ${TARGET}
                }
            } else {
                win32{
                    # Strip debug symbols.
                    QMAKE_POST_LINK += objcopy --only-keep-debug bin/${TARGET} bin/${TARGET}.dbg &&
                    QMAKE_POST_LINK += objcopy --strip-debug bin/${TARGET} &&
                    QMAKE_POST_LINK += objcopy --add-gnu-debuglink="bin/${TARGET}.dbg" bin/${TARGET}

                    QMAKE_DISTCLEAN += bin/${TARGET}.dbg
                }

                unix{
                    # Strip debug symbols.
                    QMAKE_POST_LINK += objcopy --only-keep-debug ${TARGET} ${TARGET}.dbg &&
                    QMAKE_POST_LINK += objcopy --strip-debug ${TARGET} &&
                    QMAKE_POST_LINK += objcopy --add-gnu-debuglink="${TARGET}.dbg" ${TARGET}

                    QMAKE_DISTCLEAN += ${TARGET}.dbg
                }
            }
        }
    }
}

CONFIG(release, debug|release){
    macx{
       # run macdeployqt to include all qt libraries in packet
       QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/macdeployqt $${OUT_PWD}/$${DESTDIR}/$${TARGET}.app
    }
}
