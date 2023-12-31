/************************************************************************
 **
 **  @file   vistoolnormal.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   12 8, 2014
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

#ifndef VISTOOLNORMAL_H
#define VISTOOLNORMAL_H


#include <QGraphicsItem>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "../vmisc/def.h"
#include "visline.h"

class VisToolNormal : public VisLine
{
    Q_OBJECT // NOLINT
public:
    explicit VisToolNormal(const VContainer *data, QGraphicsItem *parent = nullptr);
    ~VisToolNormal() override = default;

    void RefreshGeometry() override;
    void VisualMode(quint32 id) override;

    void SetPoint1Id(quint32 value);
    void SetPoint2Id(quint32 value);
    void SetLength(const QString &expression);
    auto GetAngle() const -> qreal;
    void SetAngle(const qreal &value);

    auto type() const -> int override {return Type;}
    enum {Type = UserType + static_cast<int>(Vis::ToolNormal)};

private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(VisToolNormal) // NOLINT
    quint32         m_object1Id{NULL_ID};
    quint32         m_object2Id{NULL_ID};
    VScaledEllipse *m_point{nullptr};
    VScaledEllipse *m_lineP1{nullptr};
    VScaledEllipse *m_lineP2{nullptr};
    VScaledLine    *m_line{nullptr};
    qreal           m_length{0};
    qreal           m_angle{0};
};

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolNormal::SetPoint1Id(quint32 value)
{
    m_object1Id = value;
}

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolNormal::SetPoint2Id(quint32 value)
{
    m_object2Id = value;
}

#endif // VISTOOLNORMAL_H
