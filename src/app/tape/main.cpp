/************************************************************************
 **
 **  @file   main.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   10 7, 2015
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2015 Valentina project
 **  <https://gitlab.com/smart-pattern/valentina> All Rights Reserved.
 **
 **  Valentina is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Valentina is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Valentina.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#include "mapplication.h"

#include <QMessageBox> // For QT_REQUIRE_VERSION
#include <QTimer>

#if defined(APPIMAGE) && defined(Q_OS_LINUX)
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
#   include "../vmisc/backport/qscopeguard.h"
#else
#   include <QScopeGuard>
#endif
#   include "../vmisc/appimage.h"
#endif // defined(APPIMAGE) && defined(Q_OS_LINUX)

auto main(int argc, char *argv[]) -> int
{
#if defined(APPIMAGE) && defined(Q_OS_LINUX)
    /* Fix path to ICU_DATA when run AppImage.*/
    char *exe_dir = IcuDataPath("/../share/icu");
    auto FreeMemory = qScopeGuard([exe_dir] {free(exe_dir);});
#endif // defined(APPIMAGE) && defined(Q_OS_LINUX)

    Q_INIT_RESOURCE(tapeicon); // NOLINT
    Q_INIT_RESOURCE(theme); // NOLINT
    Q_INIT_RESOURCE(icon); // NOLINT
    Q_INIT_RESOURCE(schema); // NOLINT
    Q_INIT_RESOURCE(flags); // NOLINT
    Q_INIT_RESOURCE(style); // NOLINT

    QT_REQUIRE_VERSION(argc, argv, "5.4.0")// clazy:exclude=qstring-arg,qstring-allocations NOLINT

#if defined(Q_OS_WIN)
    VAbstractApplication::WinAttachConsole();
#endif

#ifndef Q_OS_MAC // supports natively
    InitHighDpiScaling(argc, argv);
#endif //Q_OS_MAC

    MApplication app(argc, argv);
    app.InitOptions(); 

    QTimer::singleShot(0, &app, &MApplication::ProcessCMD);

#if defined(APPIMAGE) && defined(Q_OS_LINUX)
    if (exe_dir)
    {
        qDebug() << "Path to ICU folder:" << exe_dir;
    }
#endif // defined(APPIMAGE) && defined(Q_OS_LINUX)

    return MApplication::exec();
}
