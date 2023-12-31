/************************************************************************
 **
 **  @file   vpointf_p.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   20 8, 2014
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Valentina project
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

#ifndef VPOINTF_P_H
#define VPOINTF_P_H

#include "../vmisc/defglobal.h"
#include <QPointF>
#include <QSharedData>

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Weffc++")
QT_WARNING_DISABLE_GCC("-Wnon-virtual-dtor")

class VPointFData final : public QSharedData
{
public:
    VPointFData() = default;
    VPointFData(const VPointFData &point) = default;
    explicit VPointFData(const QPointF &point);
    VPointFData(qreal x, qreal y, qreal mx, qreal my);
    VPointFData(const QPointF &point, qreal mx, qreal my);
    ~VPointFData() = default;

    /** @brief _mx offset name respect to x */
    qreal _mx{0}; // NOLINT(misc-non-private-member-variables-in-classes)

    /** @brief _my offset name respect to y */
    qreal _my{0}; // NOLINT(misc-non-private-member-variables-in-classes)

    /** @brief _x x coordinate */
    qreal _x{0}; // NOLINT(misc-non-private-member-variables-in-classes)

    /** @brief _y y coordinate */
    qreal _y{0}; // NOLINT(misc-non-private-member-variables-in-classes)

    /** @brief m_hideLabel show or not label for this point */
    bool m_showLabel{true}; // NOLINT(misc-non-private-member-variables-in-classes)

private:
    Q_DISABLE_ASSIGN_MOVE(VPointFData) // NOLINT
};

//---------------------------------------------------------------------------------------------------------------------
inline VPointFData::VPointFData(const QPointF &point)
  : _x(point.x()),
    _y(point.y())
{
}

//---------------------------------------------------------------------------------------------------------------------
inline VPointFData::VPointFData(qreal x, qreal y, qreal mx, qreal my)
  : _mx(mx),
    _my(my),
    _x(x),
    _y(y)
{
}

//---------------------------------------------------------------------------------------------------------------------
inline VPointFData::VPointFData(const QPointF &point, qreal mx, qreal my)
  : _mx(mx),
    _my(my),
    _x(point.x()),
    _y(point.y())
{
}

QT_WARNING_POP

#endif // VPOINTF_P_H
