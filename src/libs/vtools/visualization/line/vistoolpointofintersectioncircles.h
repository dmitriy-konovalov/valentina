/************************************************************************
 **
 **  @file   vistoolpointofintersectioncircles.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   29 5, 2015
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

#ifndef VISTOOLPOINTOFINTERSECTIONCIRCLES_H
#define VISTOOLPOINTOFINTERSECTIONCIRCLES_H


#include <QGraphicsItem>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "../ifc/xml/vabstractpattern.h"
#include "../vmisc/def.h"
#include "visline.h"

class VisToolPointOfIntersectionCircles : public VisLine
{
    Q_OBJECT // NOLINT
public:
    explicit VisToolPointOfIntersectionCircles(const VContainer *data, QGraphicsItem *parent = nullptr);
    ~VisToolPointOfIntersectionCircles() override = default;

    void RefreshGeometry() override;
    void VisualMode(quint32 id) override;

    void SetCircle1Id(quint32 value);
    void SetCircle2Id(quint32 value);
    void SetC1Radius(const QString &value);
    void SetC2Radius(const QString &value);
    void SetCrossPoint(CrossCirclesPoint value);

    auto type() const -> int override {return Type;}
    enum {Type = UserType + static_cast<int>(Vis::ToolPointOfIntersectionCircles)};
private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(VisToolPointOfIntersectionCircles) // NOLINT
    quint32            m_circle1Id{NULL_ID};
    quint32            m_circle2Id{NULL_ID};
    qreal              m_c1Radius{0};
    qreal              m_c2Radius{0};
    CrossCirclesPoint  m_crossPoint{CrossCirclesPoint::FirstPoint};
    VScaledEllipse    *m_point{nullptr};
    VScaledEllipse    *m_c1Center{nullptr};
    VScaledEllipse    *m_c2Center{nullptr};
    VScaledEllipse    *m_c1Path{nullptr};
    VScaledEllipse    *m_c2Path{nullptr};
};

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolPointOfIntersectionCircles::SetCircle1Id(quint32 value)
{
    m_circle1Id = value;
}

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolPointOfIntersectionCircles::SetCircle2Id(quint32 value)
{
    m_circle2Id = value;
}

//---------------------------------------------------------------------------------------------------------------------
inline void VisToolPointOfIntersectionCircles::SetCrossPoint(CrossCirclesPoint value)
{
    m_crossPoint = value;
}

#endif // VISTOOLPOINTOFINTERSECTIONCIRCLES_H
