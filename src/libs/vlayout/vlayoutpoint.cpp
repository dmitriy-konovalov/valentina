/************************************************************************
 **
 **  @file   vlayoutpoint.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   17 10, 2022
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2022 Valentina project
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

#include "vlayoutpoint.h"

#include <QJsonObject>

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPoint::toJson() const -> QJsonObject
{
    QJsonObject pointObject;
    pointObject[QLatin1String("type")] = "VLayoutPoint";
    pointObject[QLatin1String("x")] = x();
    pointObject[QLatin1String("y")] = y();

    if (m_turnPoint)
    {
        pointObject[QLatin1String("turnPoint")] = m_turnPoint;
    }

    if (m_curvePoint)
    {
        pointObject[QLatin1String("curvePoint")] = m_curvePoint;
    }

    return pointObject;
}
