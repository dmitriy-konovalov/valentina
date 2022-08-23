/************************************************************************
 **
 **  @file   vistoolpointofcontact.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   14 8, 2014
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

#include "vistoolpointofcontact.h"

#include <QGraphicsEllipseItem>
#include <QLineF>
#include <QPointF>
#include <QSharedPointer>
#include <Qt>
#include <new>

#include "../../tools/drawTools/toolpoint/toolsinglepoint/vtoolpointofcontact.h"
#include "../vgeometry/vpointf.h"
#include "../vpatterndb/vcontainer.h"
#include "../visualization.h"
#include "visline.h"
#include "../vwidgets/global.h"

//---------------------------------------------------------------------------------------------------------------------
VisToolPointOfContact::VisToolPointOfContact(const VContainer *data, QGraphicsItem *parent)
    :VisLine(data, parent)
{
    m_arcPoint = InitPoint(Color(VColor::SupportColor), this);
    m_lineP1 = InitPoint(Color(VColor::SupportColor), this);
    m_lineP2 = InitPoint(Color(VColor::SupportColor), this);
    m_circle = InitItem<QGraphicsEllipseItem>(Color(VColor::SupportColor), this);

    m_point = InitPoint(Color(VColor::MainColor), this);
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolPointOfContact::RefreshGeometry()
{
    if (m_lineP1Id > NULL_ID)
    {
        const QSharedPointer<VPointF> first = GetData()->GeometricObject<VPointF>(m_lineP1Id);
        DrawPoint(m_lineP1, static_cast<QPointF>(*first), Color(VColor::SupportColor));

        if (m_lineP2Id <= NULL_ID)
        {
            DrawLine(this, QLineF(static_cast<QPointF>(*first), ScenePos()), Color(VColor::SupportColor));
        }
        else
        {
            const QSharedPointer<VPointF> second = GetData()->GeometricObject<VPointF>(m_lineP2Id);
            DrawPoint(m_lineP2, static_cast<QPointF>(*second), Color(VColor::SupportColor));
            DrawLine(this, QLineF(static_cast<QPointF>(*first), static_cast<QPointF>(*second)),
                     Color(VColor::SupportColor));

            if (m_radiusId > NULL_ID)
            {
                const QSharedPointer<VPointF> third = GetData()->GeometricObject<VPointF>(m_radiusId);
                DrawPoint(m_arcPoint, static_cast<QPointF>(*third), Color(VColor::SupportColor));

                if (not qFuzzyIsNull(m_radius))
                {
                    QPointF fPoint;
                    VToolPointOfContact::FindPoint(m_radius, static_cast<QPointF>(*third), static_cast<QPointF>(*first),
                                                   static_cast<QPointF>(*second), &fPoint);
                    DrawPoint(m_point, fPoint, Color(VColor::MainColor));

                    m_circle->setRect(PointRect(m_radius));
                    DrawPoint(m_circle, static_cast<QPointF>(*third), Color(VColor::SupportColor), Qt::DashLine);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolPointOfContact::VisualMode(quint32 id)
{
    m_lineP1Id = id;
    StartVisualMode();
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolPointOfContact::SetRadius(const QString &expression)
{
    m_radius = FindLengthFromUser(expression, GetData()->DataVariables());
}
