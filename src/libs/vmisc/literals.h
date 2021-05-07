/************************************************************************
 **
 **  @file   literals.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   8 4, 2018
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2018 Valentina project
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
#ifndef LITERALS_H
#define LITERALS_H

#include <QString>

// From documantation: If you use QStringLiteral you should avoid declaring the same literal in multiple places: This
// furthermore blows up the binary sizes.

extern const QString LONG_OPTION_NO_HDPI_SCALING;

extern const QString editDeleteIcon;
extern const QString preferencesOtherIcon;
extern const QString degreeSymbol;
extern const QString trueStr;
extern const QString falseStr;

#endif // LITERALS_H
