/************************************************************************
 **
 **  @file
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   23 2, 2017
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2017 Valentina project
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

#ifndef FLOATITEMDEF_H
#define FLOATITEMDEF_H

#include <QtGlobal>

// denotes the type of arrow for the grainline
enum class GrainlineArrowDirection : qint8
{
    twoWaysUpDown = 0,
    oneWayUp = 1,
    oneWayDown = 2,
    fourWays = 3,
    twoWaysUpLeft = 4,
    twoWaysUpRight = 5,
    twoWaysDownLeft = 6,
    twoWaysDownRight = 7,
    threeWaysUpDownLeft = 8,
    threeWaysUpDownRight = 9,
    threeWaysUpLeftRight = 10,
    threeWaysDownLeftRight = 11
};

#endif // FLOATITEMDEF_H

