win32{
    # Because "copy" doesn't support files that containe plus sign (+) in name we will use xcopy instead.
    unset(QMAKE_COPY)
    QMAKE_COPY = xcopy /y

    unset(QMAKE_COPY_FILE)
    QMAKE_COPY_FILE = xcopy /y

    unset(QMAKE_INSTALL_FILE)
    QMAKE_INSTALL_FILE = xcopy /y

    unset(QMAKE_INSTALL_PROGRAM)
    QMAKE_INSTALL_PROGRAM = xcopy /y
}

unix{
    *g++* {
        GCC_VERSION = $$system("g++ -dumpfullversion -dumpversion")
        GCC_VERSIONS = $$split(GCC_VERSION, ".")
        COMPILER_MAJOR_VERSION = $$member(GCC_VERSIONS, 0)
        message("g++ version $${COMPILER_MAJOR_VERSION}.x found")

        greaterThan(COMPILER_MAJOR_VERSION, 3):CONFIG += g++4
        greaterThan(COMPILER_MAJOR_VERSION, 4):CONFIG += g++5
        greaterThan(COMPILER_MAJOR_VERSION, 5):CONFIG += g++6
        greaterThan(COMPILER_MAJOR_VERSION, 6):CONFIG += g++7
        greaterThan(COMPILER_MAJOR_VERSION, 7):CONFIG += g++8
        greaterThan(COMPILER_MAJOR_VERSION, 8):CONFIG += g++9
    }

    macx{
        VCOPY = $$QMAKE_COPY
    } else {
        VCOPY = $$QMAKE_COPY -u
    }

    # Default prefix. Use for creation install path.
    DEFAULT_PREFIX = /usr

    # Prefix for binary file.
    isEmpty(PREFIX){
        PREFIX = $$DEFAULT_PREFIX
    }

    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share
    PKGDATADIR = $$DATADIR/valentina
    DEFINES += BINDIR=\\\"$$BINDIR\\\" DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"
}

win32{
    VCOPY = $$QMAKE_COPY /D
}

macx{
    # Check which minimal OSX version supports current Qt version
    # See page https://doc.qt.io/qt-5/supported-platforms.html
    # For qt 5.14 https://doc.qt.io/archives/qt-5.14/supported-platforms.html
    # For Qt 5.13 https://doc.qt.io/archives/qt-5.13/supported-platforms.html
    # For Qt 5.12 https://doc.qt.io/archives/qt-5.12/supported-platforms.html
    # For older versions https://doc.qt.io/archives/qt-5.11/supported-platforms-and-configurations.html
    equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 13) {# Qt 5.14
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.13
    } else {
        equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 11) {# Qt 5.12
            QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
        } else {
            equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 10) {# Qt 5.11
                QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
            } else {
                equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 9) {# Qt 5.10
                    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
                } else {
                    equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 8) {# Qt 5.9
                        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10
                    } else {
                        equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 7) {# Qt 5.8
                            QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
                        } else {
                            equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 6) {# Qt 5.7
                                QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
                            } else {
                                equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 3) {# Qt 5.4
                                    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
                                } else {
                                    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

# See question on StackOwerflow "QSslSocket error when SSL is NOT used" (http://stackoverflow.com/a/31277055/3045403)
# Copy of answer:
# We occasionally had customers getting very similar warning messages but the software was also crashing.
# We determined it was because, although we weren't using SSL either, the program found a copy of OpenSSL on the
# customer's computer and tried interfacing with it. The version it found was too old though (from Qt 5.2 onwards v1.0.0
# or later is required).
#
# Our solution was to distribute the OpenSSL DLLs along with our application (~1.65 MB). The alternative is to compile
# Qt from scratch without OpenSSL support.
win32 {
    greaterThan(QT_MAJOR_VERSION, 4):greaterThan(QT_MINOR_VERSION, 10) {
        versionAtLeast(QT_VERSION, 5.12.4){
            # Minimal supported OpenSSL version since Qt 5.12.4 is 1.1.1.
            contains(QMAKE_HOST.arch, x86_64) {
                INSTALL_OPENSSL += \
                    ../../../dist/win/openssl/win64/libcrypto-1_1-x64.dll \
                    ../../../dist/win/openssl/win64/libssl-1_1-x64.dll
            } else {
                INSTALL_OPENSSL += \
                    ../../../dist/win/openssl/win32/libcrypto-1_1.dll \
                    ../../../dist/win/openssl/win32/libssl-1_1.dll
            }
        } else {
            INSTALL_OPENSSL += \
                ../../../dist/win/msvcr120.dll

            contains(QMAKE_HOST.arch, x86_64) {
                INSTALL_OPENSSL += \
                    ../../../dist/win/openssl/win64/libeay32.dll \
                    ../../../dist/win/openssl/win64/ssleay32.dll
            } else {
                INSTALL_OPENSSL += \
                    ../../../dist/win/openssl/win32/libeay32.dll \
                    ../../../dist/win/openssl/win32/ssleay32.dll
            }
        }
    } else {
        INSTALL_OPENSSL += \
            ../../../dist/win/msvcr120.dll

        contains(QMAKE_HOST.arch, x86_64) {
            INSTALL_OPENSSL += \
                ../../../dist/win/openssl/win64/libeay32.dll \
                ../../../dist/win/openssl/win64/ssleay32.dll
        } else {
            INSTALL_OPENSSL += \
                ../../../dist/win/openssl/win32/libeay32.dll \
                ../../../dist/win/openssl/win32/ssleay32.dll
        }
    }
}

DEFINES += QT_NO_FOREACH

CONFIG(release, debug|release){
    !noDebugSymbols:win32:!*msvc*{
        unset(QMAKE_STRIP)
        QMAKE_STRIP = echo # we do striping manualy
    }
}

defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    greaterThan(QT_MAJOR_VERSION, $$maj) {
        return(true)
    }
    return(false)
}

# Copies the given files to the destination directory
defineTest(copyToDestdir) {
    files = $$1
    DDIR = $$2
    mkpath($$DDIR)

    for(FILE, files) {
        unix{
            !exists($$DDIR/$$basename(FILE)) {
                QMAKE_POST_LINK += ln -s -f $$quote($$FILE) $$quote($$DDIR/$$basename(FILE)) & $$escape_expand(\\n\\t)
            }
        } else {
            !exists($$DDIR/$$basename(FILE)) {
                # Replace slashes in paths with backslashes for Windows
                win32{
                    FILE ~= s,/,\\,g
                    DDIR ~= s,/,\\,g
                }
                QMAKE_POST_LINK += $$VCOPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
            }

            QMAKE_CLEAN += $$DDIR/$$basename(FILE)
        }
    }

    export(QMAKE_POST_LINK)
    export(QMAKE_CLEAN)
}

# Alwayse copies the given files to the destination directory
defineTest(forceCopyToDestdir) {
    files = $$1
    DDIR = $$2
    mkpath($$DDIR)

    for(FILE, files) {
        unix{
            QMAKE_POST_LINK += ln -s -f $$quote($$FILE) $$quote($$DDIR/$$basename(FILE)) & $$escape_expand(\\n\\t)
        } else {
            # Replace slashes in paths with backslashes for Windows
            win32{
                FILE ~= s,/,\\,g
                DDIR ~= s,/,\\,g
            }
            QMAKE_POST_LINK += $$VCOPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
            QMAKE_CLEAN += $$DDIR/$$basename(FILE)
        }
    }

    export(QMAKE_POST_LINK)
    export(QMAKE_CLEAN)
}

# We use precompiled headers for more fast compilation source code.
defineReplace(set_PCH){
    no_ccache{
        no_pch{ # To disable recompiled headers (PCH) run qmake with CONFIG+=no_pch
            return(true)
        } else {
            CONFIG += precompile_header # Turn on creation precompiled headers (PCH).
            export(CONFIG) # export value to global variable.

            PRECOMPILED_HEADER = stable.h # Header file with all all static headers: libraries, static local headers.
            export(PRECOMPILED_HEADER) # export value to global variable

            *msvc* {
                PRECOMPILED_SOURCE = stable.cpp # MSVC need also cpp file.
                export(PRECOMPILED_SOURCE) # export value to global variable.
            }
        }
    }
    return(true)
}

defineReplace(enable_ccache){
    *clang*:clazy {
        QMAKE_CXX = clazy
        export(QMAKE_CXX) # export value to global variable.
    } else {
        no_ccache{ # For enable run qmake with CONFIG+=no_ccache
            $$set_PCH()
        } else {
            # ccache support only Unix systems.
            unix:{
                equals(QT_MAJOR_VERSION, 5):greaterThan(QT_MINOR_VERSION, 8):greaterThan(QT_PATCH_VERSION, 1) {
                    # since Qt 5.9.2 we have built-in support for ccache
                    CONFIG += ccache
                    export(CONFIG) # export value to global variable.
                } else {
                    ccache_prefix = ccache

                    for(tool, $$list(QMAKE_CC QMAKE_CXX QMAKE_LINK QMAKE_LINK_SHLIB QMAKE_LINK_C)): \
                        $$tool = $$ccache_prefix $$eval($$tool)

                    export(QMAKE_CC)
                    export(QMAKE_CXX)
                    export(QMAKE_LINK)
                    export(QMAKE_LINK_SHLIB)
                    export(QMAKE_LINK_C)
                }
            } else {
                $$set_PCH()
            }
        }
    }
    return(true)
}

defineReplace(FindBuildRevision){
    CONFIG(debug, debug|release){
        # Debug mode
        return(\\\"unknown\\\")
    }else{
        # Release mode
        DVCS_HESH=$$system("git rev-parse --short HEAD")
        isEmpty(DVCS_HESH){
            DVCS_HESH = \\\"unknown\\\" # if we can't find build revision left unknown.
        } else {
            DVCS_HESH=\\\"Git:$${DVCS_HESH}\\\"
        }
        return($${DVCS_HESH})
    }
}

defineReplace(FindLatestTagDistance){
    CONFIG(debug, debug|release){
        # Debug mode
        return(0)
    }else{
        # Release mode

        # tag is formatted as TAG-N-gSHA:
        # 1. latest stable version is TAG, or vX.Y.Z
        # 2. number of commits since latest stable version is N
        # 3. latest commit is gSHA
        tag_all = $$system(git describe --tags)
        tag_split = $$split(tag_all, "-") #split at the dashes
        GIT_DISTANCE = $$member(tag_split,1) #get 2nd element of results

        isEmpty(GIT_DISTANCE){
            GIT_DISTANCE = 0 # if we can't find local revision left 0.
        }

        message("Latest tag distance:" $${GIT_DISTANCE})

        return($${GIT_DISTANCE})
    }
}

# In debug mode on Unix system we use all usefull for us compilers keys for checking errors.
# Also trying make all possible for speed up build time.
unix {
LIBS_USED_FOR_QT = \
    QtCore \
    QtSvg \
    QtXml \
    QtPrintSupport \
    QtXmlPatterns \
    QtWidgets \
    QtGui \
    QtNetwork \
    QtTest \
    QtConcurrent

# Key -isystem disable checking errors in system headers. Marking ignore for warnings in Qt headers.
!macx{ 
ISYSTEM += -isystem "$$[QT_INSTALL_HEADERS]"

for(somelib, $$list($$LIBS_USED_FOR_QT)) {
    ISYSTEM += -isystem "$$[QT_INSTALL_HEADERS]/$${somelib}"
}
} else {
for(somelib, $$list($$LIBS_USED_FOR_QT)) {
    ISYSTEM += -isystem "$$[QT_INSTALL_LIBS]/$${somelib}.framework/Versions/5/Headers"
    ISYSTEM += -isystem "$$[QT_INSTALL_LIBS]/$${somelib}.framework/Headers"
}
}

# Usefull GCC warnings keys.
GCC_DEBUG_CXXFLAGS += \
    -O0 \ # Turn off oprimization.
    $$ISYSTEM \ # Ignore warnings Qt headers.
    # Last gdb doesn't show debug symbols with Qt Creator (issue with Python 3 and debug scripts that use Python 2.7).
    # Solution to use older version gdb, that's why we use old standard of debug information.
    -gdwarf-3 \
    -Wall \
    -Wextra \
    -pedantic \
    -Weffc++ \
    -Woverloaded-virtual \
    -Wctor-dtor-privacy \
    -Wnon-virtual-dtor \
    -Wold-style-cast \
    -Wconversion \
    -Winit-self \
#    -Wstack-protector \
    -Wunreachable-code \
    -Wcast-align \
    -Wcast-qual \
    -Wdisabled-optimization \
    -Wfloat-equal \
    -Wformat=2 \
    -Wimport \
    -Wmissing-include-dirs \
    -Wpacked \
    -Wredundant-decls \
#    -Winline \
    -Winvalid-pch \
#    -Wunsafe-loop-optimizations \
#    -Wlong-long \ We have been using C++11
    -Wmissing-format-attribute \
    -Wswitch-default \
    -Wuninitialized \
    -Wvariadic-macros \
    -Wlogical-op \
    -Wnoexcept \
    -Wmissing-noreturn \
    -Wpointer-arith \
    -Wstrict-null-sentinel \
    -Wstrict-overflow=5 \
    -Wno-error=strict-overflow \
    -Wundef \
    -Wno-unused \
    -Wno-pragmas \
    -ftrapv

# Since GCC 5
g++5:GCC_DEBUG_CXXFLAGS += \
    -Wswitch-bool \
    -Wlogical-not-parentheses \
    -Wsizeof-array-argument \
#    -Wsuggest-final-types \
#    -Wsuggest-final-methods \
    -Wbool-compare

# Since GCC 6
g++6:GCC_DEBUG_CXXFLAGS += \
    -Wshift-negative-value \
    -Wshift-overflow \
    -Wshift-overflow=2 \
    -Wtautological-compare \
#    -Wnull-dereference \
    -Wduplicated-cond \
    -Wmisleading-indentation

# Since GCC 7
g++7:GCC_DEBUG_CXXFLAGS += \
    -Wduplicated-branches \
    -Wrestrict \
    -Walloc-zero \
    -Wnonnull \
    -Wno-stringop-overflow # cannot suppress warning in Qt headers

# Since GCC 8
g++8:GCC_DEBUG_CXXFLAGS += \
     -Wmultistatement-macros \
     -Warray-bounds

# Usefull Clang warnings keys.
CLANG_DEBUG_CXXFLAGS += \
    -O0 \ # Turn off oprimization.
    $$ISYSTEM \ # Ignore warnings in Qt headers.
    # Last gdb doesn't show debug symbols with Qt Creator (issue with Python 3 and debug scripts that use Python 2.7).
    # Solution to use older version gdb, that's why we use old standard of debug information.
    -gdwarf-3 \
    -fparse-all-comments \
    -Wabi \
    -Wabstract-final-class \
    -Wabstract-vbase-init \
    -Waddress \
    -Waddress-of-temporary \
    -Waggregate-return \
    -Wall \
    -Wambiguous-macro \
    -Wambiguous-member-template \
    -Wanalyzer-incompatible-plugin \
    -Wanonymous-pack-parens \
    -Warc \
    -Warc-bridge-casts-disallowed-in-nonarc \
    -Warc-maybe-repeated-use-of-weak \
    -Warc-non-pod-memaccess \
    -Warc-performSelector-leaks \
    -Warc-repeated-use-of-weak \
    -Warc-retain-cycles \
    -Warc-unsafe-retained-assign \
    -Warray-bounds \
    -Warray-bounds-pointer-arithmetic \
    -Wasm \
    -Wasm-operand-widths \
    -Wassign-enum \
    -Watomic-properties \
    -Watomic-property-with-user-defined-accessor \
    -Wattributes \
    -Wauto-import \
    -Wauto-storage-class \
    -Wauto-var-id \
    -Wavailability \
    -Wbackslash-newline-escape \
#    -Wbad-array-new-length \
    -Wbad-function-cast \
    -Wbind-to-temporary-copy \
        -Wno-c++98-compat-bind-to-temporary-copy \
    -Wbitfield-constant-conversion \
    -Wbitwise-op-parentheses \
    -Wbool-conversion \
    -Wbool-conversions \
    -Wbuiltin-macro-redefined \
    -Wbuiltin-requires-header \
    -Wc++-compat \
    -Wc++0x-compat \
    -Wc++0x-extensions \
    -Wc++0x-narrowing \
    -Wc++11-compat \
    -Wc++11-compat-pedantic \
    -Wc++11-compat-reserved-user-defined-literal \
    -Wc++11-extensions \
    -Wc++11-extra-semi \
    -Wc++11-long-long \
    -Wc++11-narrowing \
    -Wc11-extensions \
    -Wcast-align \
    -Wcast-of-sel-type \
    -Wcast-qual \
    -WCFString-literal \
    -Wchar-align \
    -Wchar-subscripts \
    -Wcomment \
    -Wcomments \
    -Wcompare-distinct-pointer-types \
    -Wcomplex-component-init \
    -Wconditional-type-mismatch \
    -Wconditional-uninitialized \
    -Wconfig-macros \
    -Wconstant-conversion \
    -Wconstant-logical-operand \
    -Wconstexpr-not-const \
    -Wconversion \
    -Wconversion-null \
    -Wno-covered-switch-default \
    -Wctor-dtor-privacy \
    -Wcustom-atomic-properties \
    -Wdangling-else \
    -Wdangling-field \
    -Wdangling-initializer-list \
    -Wdeclaration-after-statement \
    -Wdelegating-ctor-cycles \
    -Wdelete-incomplete \
    -Wdelete-non-virtual-dtor \
    -Wdeprecated \
    -Wdeprecated-declarations \
    -Wdeprecated-implementations \
    -Wdeprecated-objc-isa-usage \
    -Wdeprecated-objc-pointer-introspection \
    -Wdeprecated-objc-pointer-introspection-performSelector \
    -Wdeprecated-register \
    -Wdeprecated-writable-strings \
    -Wdirect-ivar-access \
#   -Wdisabled-macro-expansion \ Disabled
    -Wdisabled-optimization \
    -Wdiscard-qual \
    -Wdistributed-object-modifiers \
    -Wdiv-by-zero \
    -Wdivision-by-zero \
    -Wdocumentation \
    -Wdocumentation-deprecated-sync \
    -Wdocumentation-html \
    -Wdollar-in-identifier-extension \
    -Wduplicate-decl-specifier \
    -Wduplicate-enum \
    -Wduplicate-method-arg \
    -Wduplicate-method-match \
    -Wdynamic-class-memaccess \
    -Weffc++ \
    -Wembedded-directive \
    -Wempty-body \
    -Wempty-translation-unit \
    -Wendif-labels \
    -Wenum-compare \
    -Wenum-conversion \
    -Wexplicit-ownership-type \
    -Wextern-initializer \
    -Wextra \
    -Wextra-semi \
    -Wextra-tokens \
    -Wflexible-array-extensions \
    -Wfloat-equal \
    -Wformat \
    -Wformat-extra-args \
    -Wformat-invalid-specifier \
    -Wformat-non-iso \
    -Wformat-nonliteral \
    -Wformat-security \
    -Wformat-y2k \
    -Wformat-zero-length \
    -Wformat=2 \
    -Wfour-char-constants \
    -Wgcc-compat \
    -Wgnu \
    -Wgnu-array-member-paren-init \
    -Wgnu-designator \
    -Wgnu-static-float-init \
    -Wheader-guard \
    -Wheader-hygiene \
    -Widiomatic-parentheses \
    -Wignored-attributes \
    -Wignored-qualifiers \
    -Wimplicit \
    -Wimplicit-atomic-properties \
    -Wimplicit-conversion-floating-point-to-bool \
    -Wimplicit-exception-spec-mismatch \
    -Wimplicit-fallthrough \
    -Wimplicit-fallthrough-per-function \
    -Wimplicit-function-declaration \
    -Wimplicit-int \
    -Wimplicit-retain-self \
    -Wimport \
    -Wimport-preprocessor-directive-pedantic \
    -Wincompatible-library-redeclaration \
    -Wincompatible-pointer-types \
    -Wincompatible-pointer-types-discards-qualifiers \
    -Wincomplete-implementation \
    -Wincomplete-module \
    -Wincomplete-umbrella \
    -Winit-self \
    -Winitializer-overrides \
#    -Winline \
    -Wint-conversion \
    -Wint-conversions \
    -Wint-to-pointer-cast \
    -Wint-to-void-pointer-cast \
    -Winteger-overflow \
    -Winvalid-constexpr \
    -Winvalid-iboutlet \
    -Winvalid-noreturn \
    -Winvalid-offsetof \
    -Winvalid-pch \
    -Winvalid-pp-token \
    -Winvalid-source-encoding \
    -Winvalid-token-paste \
    -Wknr-promoted-parameter \
    -Wlanguage-extension-token \
    -Wlarge-by-value-copy \
    -Wliteral-conversion \
    -Wliteral-range \
    -Wlocal-type-template-args \
        -Wno-c++98-compat-local-type-template-args \
    -Wlogical-not-parentheses \
    -Wlogical-op-parentheses \
#    -Wlong-long \ We have been using C++11
    -Wloop-analysis \
    -Wmain \
    -Wmain-return-type \
    -Wmalformed-warning-check \
    -Wmethod-signatures \
    -Wmicrosoft \
    -Wmicrosoft-exists \
    -Wmismatched-parameter-types \
    -Wmismatched-return-types \
    -Wmismatched-tags \
    -Wmissing-braces \
    -Wmissing-declarations \
    -Wmissing-field-initializers \
    -Wmissing-format-attribute \
    -Wmissing-include-dirs \
    -Wmissing-method-return-type \
    -Wmissing-noreturn \
    -Wmissing-prototypes \
    -Wmissing-selector-name \
    -Wmissing-sysroot \
    -Wmissing-variable-declarations \
    -Wmodule-conflict \
    -Wmost \
    -Wmultichar \
    -Wnarrowing \
    -Wnested-externs \
    -Wnewline-eof \
    -Wnon-gcc \
    -Wnon-literal-null-conversion \
    -Wnon-pod-varargs \
    -Wnon-virtual-dtor \
    -Wnonnull \
    -Wnonportable-cfstrings \
    -Wno-c++98-compat \
    -WNSObject-attribute \
    -Wnull-arithmetic \
    -Wnull-character \
    -Wnull-conversion \
    -Wnull-dereference \
    -Wodr \
    -Wold-style-cast \
    -Wold-style-definition \
    -Wout-of-line-declaration \
    -Wover-aligned \
    -Woverflow \
    -Woverlength-strings \
    -Woverloaded-shift-op-parentheses \
    -Woverloaded-virtual \
    -Woverriding-method-mismatch \
    -Wpacked \
    -Wparentheses \
    -Wparentheses-equality \
    -Wpedantic \
    -Wpointer-arith \
    -Wpointer-sign \
    -Wpointer-to-int-cast \
    -Wpointer-type-mismatch \
    -Wpredefined-identifier-outside-function \
    -Wprivate-extern \
    -Wprotocol \
    -Wprotocol-property-synthesis-ambiguity \
    -Wreadonly-iboutlet-property \
    -Wreceiver-expr \
    -Wreceiver-forward-class \
    -Wredundant-decls \
    -Wreinterpret-base-class \
    -Wreorder \
    -Wrequires-super-attribute \
    -Wreserved-user-defined-literal \
    -Wreturn-stack-address \
    -Wreturn-type \
    -Wreturn-type-c-linkage \
    -Wsection \
    -Wselector \
    -Wselector-type-mismatch \
    -Wself-assign \
    -Wself-assign-field \
    -Wsemicolon-before-method-body \
    -Wsentinel \
    -Wsequence-point \
    -Wserialized-diagnostics \
    -Wshadow-ivar \
    -Wshift-count-negative \
    -Wshift-count-overflow \
    -Wshift-op-parentheses \
    -Wshift-overflow \
    -Wshift-sign-overflow \
    -Wshorten-64-to-32 \
    -Wsign-compare \
    -Wsign-conversion \
    -Wsign-promo \
    -Wsizeof-array-argument \
    -Wsizeof-array-decay \
    -Wsizeof-pointer-memaccess \
    -Wsometimes-uninitialized \
    -Wsource-uses-openmp \
#    -Wstack-protector \
    -Wstatic-float-init \
    -Wstatic-in-inline \
    -Wstatic-local-in-inline \
    -Wstatic-self-init \
    -Wstrict-aliasing \
    -Wstrict-aliasing=0 \
    -Wstrict-aliasing=1 \
    -Wstrict-aliasing=2 \
    -Wstrict-overflow \
    -Wstrict-overflow=0 \
    -Wstrict-overflow=1 \
    -Wstrict-overflow=2 \
    -Wstrict-overflow=3 \
    -Wstrict-overflow=4 \
    -Wstrict-overflow=5 \
    -Wstrict-prototypes \
    -Wstrict-selector-match \
    -Wstring-compare \
    -Wstring-conversion \
    -Wstring-plus-int \
    -Wstrlcpy-strlcat-size \
    -Wstrncat-size \
    -Wsuper-class-method-mismatch \
    -Wswitch \
    -Wswitch-default \
    -Wsynth \
    -Wtautological-compare \
    -Wtautological-constant-out-of-range-compare \
    -Wtentative-definition-incomplete-type \
    -Wthread-safety \
    -Wthread-safety-analysis \
    -Wthread-safety-attributes \
    -Wthread-safety-beta \
    -Wthread-safety-precise \
    -Wtrigraphs \
    -Wtype-limits \
    -Wtype-safety \
    -Wtypedef-redefinition \
    -Wtypename-missing \
    -Wundeclared-selector \
    -Wundef \
    -Wundefined-inline \
    -Wundefined-internal \
    -Wundefined-reinterpret-cast \
    -Wunicode \
    -Wunicode-whitespace \
    -Wuninitialized \
    -Wunnamed-type-template-args \
    -Wunneeded-internal-declaration \
    -Wunneeded-member-function \
    -Wunsequenced \
    -Wunsupported-visibility \
    -Wunused \
    -Wunused-argument \
    -Wunused-command-line-argument \
    -Wunused-comparison \
    -Wunused-exception-parameter \
    -Wunused-function \
    -Wunused-label \
    -Wunused-member-function \
    -Wunused-parameter \
    -Wunused-private-field \
    -Wunused-result \
    -Wunused-value \
    -Wunused-variable \
    -Wunused-volatile-lvalue \
    -Wuser-defined-literals \
    -Wvarargs \
    -Wvariadic-macros \
    -Wvector-conversion \
    -Wvector-conversions \
    -Wvexing-parse \
    -Wvisibility \
    -Wvla \
    -Wvla-extension \
    -Wvolatile-register-var \
    -Wweak-template-vtables \
#    -Wweak-vtables \
    -Wwrite-strings \
    -Wzero-length-array \
    -Qunused-arguments \
    -fcolor-diagnostics \
    -Wno-gnu-zero-variadic-macro-arguments \
    -fms-extensions # Need for pragma message

macx{
    # Cannot suppress warnings from Qt headers
    CLANG_DEBUG_CXXFLAGS += \
        -Wno-sign-conversion \
        -Wno-duplicate-enum \
        -Wno-shorten-64-to-32 \
        -Wno-deprecated
}

CONFIG(release, debug|release){
    CLANG_DEBUG_CXXFLAGS += -Wno-unknown-warning-option
}

ICC_DEBUG_CXXFLAGS += \
    $$ISYSTEM \ # Ignore warnings Qt headers.
    -Wcomment \
    -Weffc++ \
    -Wextra-tokens \
    -Wformat \
#    -Winline \
    -Wmain \
    -Wmissing-declarations \
    -Wmissing-prototypes \
    -Wnon-virtual-dtor \
    -Wp64 \
    -Wpointer-arith \
    -Wremarks \
    -Wreturn-type \
    -Wsign-compare \
    -Wstrict-aliasing \
    -Wstrict-prototypes \
    -Wtrigraphs \
    -Wuninitialized \
    -Wno-unknown-pragmas \
    -Wunused-variable \
    -Wno-pch-messages \
    -wd1418,2012,2015,2017,2022,2013 #disable warnings. Syntax example -wd1572,873,2259,2261
} else {
# Don't use additional GCC and Clang keys on Windows system.
# Can't find way mark ignore Qt header on Windows.
GCC_DEBUG_CXXFLAGS += \
    -O0 \
    -Wall \
    -Wno-pragmas \
    -Wno-error=strict-overflow \
    -Wextra \
    -fno-omit-frame-pointer # Need for exchndl.dll

win32:equals(QT_MAJOR_VERSION, 5):equals(QT_MINOR_VERSION, 6) {
    GCC_DEBUG_CXXFLAGS += \
        -Wno-array-bounds # annoying warning
}

CLANG_DEBUG_CXXFLAGS += \
    -O0 \
    -Wall \
    -Wextra \
    -pedantic \
    -Wno-gnu-zero-variadic-macro-arguments \
    -fno-omit-frame-pointer \ # Need for exchndl.dll
    -fms-extensions # Need for pragma message

MSVC_DEBUG_CXXFLAGS += \
    -Wall \
    -wd4061 \ # enum value is not *explicitly* handled in switch
    -wd4099 \ # first seen using 'struct' now seen using 'class'
    -wd4127 \ # conditional expression is constant
    -wd4217 \ # member template isn't copy constructor
    -wd4250 \ # inherits (implements) some member via dominance
    -wd4251 \ # needs to have dll-interface to be used by clients
    -wd4275 \ # exported class derived from non-exported class
    -wd4347 \ # "behavior change", function called instead of template
    -wd4355 \ # "'this': used in member initializer list
    -wd4505 \ # unreferenced function has been removed
    -wd4510 \ # default constructor could not be generated
    -wd4511 \ # copy constructor could not be generated
    -wd4512 \ # assignment operator could not be generated
    -wd4513 \ # destructor could not be generated
    -wd4514 \ # 'function' : unreferenced inline function has been removed
    -wd4610 \ # can never be instantiated user defined constructor required
    -wd4623 \ # default constructor could not be generated
    -wd4624 \ # destructor could not be generated
    -wd4625 \ # copy constructor could not be generated
    -wd4626 \ # assignment operator could not be generated
    -wd4640 \ # a local static object is not thread-safe
    -wd4661 \ # a member of the template class is not defined.
    -wd4670 \ # a base class of an exception class is inaccessible for catch
    -wd4672 \ # a base class of an exception class is ambiguous for catch
    -wd4673 \ # a base class of an exception class is inaccessible for catch
    -wd4675 \ # resolved overload was found by argument-dependent lookup
    -wd4702 \ # unreachable code, e.g. in <list> header.
    -wd4710 \ # call was not inlined
    -wd4711 \ # call was inlined
    -wd4820 \ # some padding was added
    -wd4917 \ # a GUID can only be associated with a class, interface or namespace
    -wd4351 \ # elements of array 'array' will be default initialized
    # The following are real warnings but are generated by almost all MS headers, including
    # standard library headers, so it's impractical to leave them on.
    -wd4619 \ # there is no warning number 'XXXX'
    -wd4668 \ # XXX is not defined as a preprocessor macro
    -wd5045 \ # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
    # Because Microsoft doesn't provide a way to suppress warnings in headers we will suppress
    # all warnings we meet in headers globally
    -wd4548 \
    -wd4350 \
    -wd4242 \
    -wd4265 \
    -wd4599 \
    -wd4371 \
    -wd4718 \
    -wd4946 \
    -wd4868 \ # compiler may not enforce left-to-right evaluation order in braced initializer list
    -wd4866 \ # compiler may not enforce left-to-right evaluation order for call to 'C++17 operator'
    -wd4571 \ # catch(...) semantics changed since Visual C++ 7.1; structured exceptions (SEH) are no longer caught
    -wd5026 \ # move constructor was implicitly defined as deleted
    -wd5027 \ # move assignment operator was implicitly defined as deleted
    -wd5039 \ # pointer or reference to potentially throwing function passed to extern C function under -EHc.
    -wd4774 \ # format string expected in argument 3 is not a string literal
    -wd5204 \ # class has virtual functions, but its trivial destructor is not virtual; instances of objects derived
              # from this class may not be destructed correctly
    -wd5219 \ # implicit conversion from '<type>' to '<type>', possible loss of data
    -wd5243   # 'type-name': using incomplete class 'class-name' can cause potential one definition rule violation due
              # to ABI limitation
}
