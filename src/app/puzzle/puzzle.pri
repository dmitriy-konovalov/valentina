# ADD TO EACH PATH $$PWD VARIABLE!!!!!!
# This need for corect working file translations.pro

SOURCES += \
    $$PWD/dialogs/configpages/puzzlepreferencesconfigurationpage.cpp \
    $$PWD/dialogs/configpages/puzzlepreferencespathpage.cpp \
    $$PWD/dialogs/configpages/puzzlepreferenceslayoutpage.cpp \
    $$PWD/dialogs/dialogpuzzlepreferences.cpp \
    $$PWD/dialogs/vpdialogabout.cpp \
    $$PWD/layout/layoutdef.cpp \
    $$PWD/main.cpp \
    $$PWD/undocommands/vpundoaddsheet.cpp \
    $$PWD/undocommands/vpundocommand.cpp \
    $$PWD/undocommands/vpundomovepieceonsheet.cpp \
    $$PWD/undocommands/vpundooriginmove.cpp \
    $$PWD/undocommands/vpundopiecemove.cpp \
    $$PWD/undocommands/vpundopiecerotate.cpp \
    $$PWD/undocommands/vpundopiecezvaluemove.cpp \
    $$PWD/undocommands/vpundoremovesheet.cpp \
    $$PWD/vpapplication.cpp \
    $$PWD/carousel/vpcarrousel.cpp \
    $$PWD/carousel/vpcarrouselpiece.cpp \
    $$PWD/carousel/vpcarrouselpiecelist.cpp \
    $$PWD/vpcommandline.cpp \
    $$PWD/vpcommands.cpp \
    $$PWD/scene/vpgraphicspiece.cpp \
    $$PWD/scene/vpgraphicspiececontrols.cpp \
    $$PWD/scene/vpgraphicssheet.cpp \
    $$PWD/scene/vpgraphicstilegrid.cpp \
    $$PWD/layout/vplayout.cpp \
    $$PWD/layout/vplayoutsettings.cpp \
    $$PWD/scene/vpmaingraphicsview.cpp \
    $$PWD/vpmainwindow.cpp \
    $$PWD/carousel/vpmimedatapiece.cpp \
    $$PWD/layout/vppiece.cpp \
    $$PWD/vpsettings.cpp \
    $$PWD/layout/vpsheet.cpp \
    $$PWD/vptilefactory.cpp \
    $$PWD/xml/vplayoutfilereader.cpp \
    $$PWD/xml/vplayoutfilewriter.cpp \
    $$PWD/xml/vplayoutliterals.cpp \
    $$PWD/dialogs/dialogsavemanuallayout.cpp

*msvc*:SOURCES += $$PWD/stable.cpp

HEADERS += \
    $$PWD/dialogs/configpages/puzzlepreferencesconfigurationpage.h \
    $$PWD/dialogs/configpages/puzzlepreferencespathpage.h \
    $$PWD/dialogs/configpages/puzzlepreferenceslayoutpage.h \
    $$PWD/dialogs/dialogpuzzlepreferences.h \
    $$PWD/dialogs/vpdialogabout.h \
    $$PWD/layout/layoutdef.h \
    $$PWD/scene/scenedef.h \
    $$PWD/stable.h \
    $$PWD/undocommands/vpundoaddsheet.h \
    $$PWD/undocommands/vpundocommand.h \
    $$PWD/undocommands/vpundomovepieceonsheet.h \
    $$PWD/undocommands/vpundooriginmove.h \
    $$PWD/undocommands/vpundopiecemove.h \
    $$PWD/undocommands/vpundopiecerotate.h \
    $$PWD/undocommands/vpundopiecezvaluemove.h \
    $$PWD/undocommands/vpundoremovesheet.h \
    $$PWD/vpapplication.h \
    $$PWD/carousel/vpcarrousel.h \
    $$PWD/carousel/vpcarrouselpiece.h \
    $$PWD/carousel/vpcarrouselpiecelist.h \
    $$PWD/vpcommandline.h \
    $$PWD/vpcommands.h \
    $$PWD/scene/vpgraphicspiece.h \
    $$PWD/scene/vpgraphicspiececontrols.h \
    $$PWD/scene/vpgraphicssheet.h \
    $$PWD/scene/vpgraphicstilegrid.h \
    $$PWD/layout/vplayout.h \
    $$PWD/layout/vplayoutsettings.h \
    $$PWD/scene/vpmaingraphicsview.h \
    $$PWD/vpmainwindow.h \
    $$PWD/carousel/vpmimedatapiece.h \
    $$PWD/layout/vppiece.h \
    $$PWD/vpsettings.h \
    $$PWD/layout/vpsheet.h \
    $$PWD/vptilefactory.h \
    $$PWD/xml/vplayoutfilereader.h \
    $$PWD/xml/vplayoutfilewriter.h \
    $$PWD/xml/vplayoutliterals.h \
    $$PWD/dialogs/dialogsavemanuallayout.h

FORMS += \
    $$PWD/dialogs/configpages/puzzlepreferencesconfigurationpage.ui \
    $$PWD/dialogs/configpages/puzzlepreferencespathpage.ui \
    $$PWD/dialogs/configpages/puzzlepreferenceslayoutpage.ui \
    $$PWD/dialogs/dialogpuzzlepreferences.ui \
    $$PWD/dialogs/vpdialogabout.ui \
    $$PWD/carousel/vpcarrousel.ui \
    $$PWD/vpmainwindow.ui \
    $$PWD/dialogs/dialogsavemanuallayout.ui
