/************************************************************************
 **
 **  @file   vpiecearea_p.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   9 11, 2022
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
#ifndef VPIECEAREA_P_H
#define VPIECEAREA_P_H

#include <QSharedData>

#include "../vmisc/defglobal.h"
#include "../vmisc/typedef.h"

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Weffc++")
QT_WARNING_DISABLE_GCC("-Wnon-virtual-dtor")

class VPieceAreaData final : public QSharedData
{
public:
    VPieceAreaData() = default;
    explicit VPieceAreaData(quint32 pieceId);
    VPieceAreaData(const VPieceAreaData &var) = default;
    ~VPieceAreaData() = default;

    quint32 m_pieceId{NULL_ID}; // NOLINT(misc-non-private-member-variables-in-classes)

private:
    Q_DISABLE_ASSIGN_MOVE(VPieceAreaData) // NOLINT
};

QT_WARNING_POP

//---------------------------------------------------------------------------------------------------------------------
inline VPieceAreaData::VPieceAreaData(quint32 pieceId)
  : m_pieceId(pieceId)
{
}

#endif // VPIECEAREA_P_H
