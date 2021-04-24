/************************************************************************
 **
 **  @file   projectversion.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   8 7, 2015
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

#ifndef PROJECTVERSION_H
#define PROJECTVERSION_H

class QString;

extern const int MAJOR_VERSION;
extern const int MINOR_VERSION;
extern const int DEBUG_VERSION;

extern const QString APP_VERSION_STR;

/*
   APP_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define APP_VERSION V_VERSION_CHECK(MAJOR_VERSION, MINOR_VERSION, DEBUG_VERSION)
/*
   can be used like #if (APP_VERSION >= V_VERSION_CHECK(0, 7, 0))
*/
#define V_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

// Change version number in projectversion.cpp too.
// Synchronize valentina.nsi

#define VER_FILEVERSION             0,7,46
#define VER_FILEVERSION_STR         "0.7.46\0"

#define V_PRERELEASE // Mark prerelease builds

#define VER_PRODUCTVERSION          VER_FILEVERSION
#define VER_PRODUCTVERSION_STR      VER_FILEVERSION_STR

#define VER_COMPANYNAME_STR         "ValentinaTeam"
//#define VER_FILEDESCRIPTION_STR   "Patternmaking program." // Defined in program
//#define VER_INTERNALNAME_STR      "Valentina" // Defined in program
#define VER_LEGALCOPYRIGHT_STR      "Copyright © 2014-2021 Valentina Team"
#define VER_LEGALTRADEMARKS1_STR    "All Rights Reserved"
#define VER_LEGALTRADEMARKS2_STR    VER_LEGALTRADEMARKS1_STR
//#define VER_ORIGINALFILENAME_STR  "valentina.exe" // Defined in program
//#define VER_PRODUCTNAME_STR       "Valentina" // Defined in program

#define VER_COMPANYDOMAIN_STR       "valentinaproject.bitbucket.io"

QString compilerString();
QString buildCompatibilityString();

#endif // PROJECTVERSION_H
