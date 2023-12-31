/************************************************************************
 **
 **  @file
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   12 9, 2016
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2016 Valentina project
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

#ifndef VISTOOLFLIPPINGBYLINE_H
#define VISTOOLFLIPPINGBYLINE_H

#include <QtGlobal>

#include "visoperation.h"

class VisToolFlippingByLine : public VisOperation
{
    Q_OBJECT // NOLINT
public:
    explicit VisToolFlippingByLine(const VContainer *data, QGraphicsItem *parent = nullptr);
    ~VisToolFlippingByLine() override = default;

    void RefreshGeometry() override;

    void SetFirstLinePointId(quint32 value);
    void SetSecondLinePointId(quint32 value);

    auto type() const -> int override {return Type;}
    enum { Type = UserType + static_cast<int>(Vis::ToolFlippingByLine)};
private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(VisToolFlippingByLine) // NOLINT
    quint32         m_lineP1Id{NULL_ID};
    quint32         m_lineP2Id{NULL_ID};
    VScaledEllipse *m_point1{nullptr};
    VScaledEllipse *m_point2{nullptr};
};

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolFlippingByLine::SetFirstLinePointId(quint32 value)
{
    m_lineP1Id = value;
}

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolFlippingByLine::SetSecondLinePointId(quint32 value)
{
    m_lineP2Id = value;
}

#endif // VISTOOLFLIPPINGBYLINE_H
