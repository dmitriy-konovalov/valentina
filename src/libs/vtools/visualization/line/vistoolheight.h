/************************************************************************
 **
 **  @file   vistoolheight.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   13 8, 2014
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

#ifndef VISTOOLHEIGHT_H
#define VISTOOLHEIGHT_H


#include <QGraphicsItem>
#include <QLineF>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "../vmisc/def.h"
#include "visline.h"

class VisToolHeight final : public VisLine
{
    Q_OBJECT // NOLINT
public:
    explicit VisToolHeight(const VContainer *data, QGraphicsItem *parent = nullptr);
    ~VisToolHeight() override = default;

    void RefreshGeometry() override;
    void VisualMode(quint32 id) override;

    void SetBasePointId(quint32 value);
    void SetLineP1Id(quint32 value);
    void SetLineP2Id(quint32 value);

    auto type() const -> int override {return Type;}
    enum {Type = UserType + static_cast<int>(Vis::ToolHeight)};
private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(VisToolHeight) // NOLINT
    quint32         m_basePointId{NULL_ID};
    quint32         m_lineP1Id{NULL_ID};//first point of line
    quint32         m_lineP2Id{NULL_ID};//second point of line
    VScaledEllipse *m_point{nullptr};
    VScaledEllipse *m_basePoint{nullptr};
    VScaledEllipse *m_lineP1{nullptr};
    VScaledEllipse *m_lineP2{nullptr};
    VScaledLine    *m_line{nullptr};
    VScaledLine    *m_lineIntersection{nullptr};

    void ShowIntersection(const QLineF &height_line, const QLineF &base_line);
};

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolHeight::SetBasePointId(quint32 value)
{
    m_basePointId = value;
}

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolHeight::SetLineP1Id(quint32 value)
{
    m_lineP1Id = value;
}

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolHeight::SetLineP2Id(quint32 value)
{
    m_lineP2Id = value;
}

#endif // VISTOOLHEIGHT_H
