/************************************************************************
 **
 **  @file   vlineangle_p.h
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

#ifndef VLINEANGLE_P_H
#define VLINEANGLE_P_H

#include <QSharedData>

#include "../vmisc/defglobal.h"
#include "../vmisc/typedef.h"

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Weffc++")
QT_WARNING_DISABLE_GCC("-Wnon-virtual-dtor")

class VLineAngleData final : public QSharedData
{
public:
    VLineAngleData() = default;
    VLineAngleData(const quint32 &p1Id, const quint32 &p2Id);
    VLineAngleData(const VLineAngleData &var) = default;
    ~VLineAngleData() = default;

    quint32 p1Id{NULL_ID}; // NOLINT(misc-non-private-member-variables-in-classes)
    quint32 p2Id{NULL_ID}; // NOLINT(misc-non-private-member-variables-in-classes)

private:
    Q_DISABLE_ASSIGN_MOVE(VLineAngleData) // NOLINT
};

QT_WARNING_POP

//---------------------------------------------------------------------------------------------------------------------
inline VLineAngleData::VLineAngleData(const quint32 &p1Id, const quint32 &p2Id)
  : p1Id(p1Id),
    p2Id(p2Id)
{
}

#endif // VLINEANGLE_P_H
