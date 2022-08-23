/************************************************************************
 **
 **  @file   vistoolcubicbezierpath.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   18 3, 2016
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

#include "vistoolcubicbezierpath.h"

#include <QGraphicsLineItem>
#include <QGraphicsPathItem>
#include <QLineF>
#include <QPainterPath>
#include <QPointF>
#include <Qt>
#include <QtAlgorithms>

#include "../vgeometry/vabstractcurve.h"
#include "../vgeometry/vpointf.h"
#include "../vgeometry/vspline.h"
#include "../visualization.h"
#include "vispath.h"
#include "../vwidgets/scalesceneitems.h"
#include "../vmisc/vmodifierkey.h"
#include "../vmisc/compatibility.h"

//---------------------------------------------------------------------------------------------------------------------
VisToolCubicBezierPath::VisToolCubicBezierPath(const VContainer *data, QGraphicsItem *parent)
    : VisPath(data, parent)
{
    helpLine1 = InitItem<VScaledLine>(Color(VColor::MainColor), this);
    helpLine2 = InitItem<VScaledLine>(Color(VColor::MainColor), this);

    newCurveSegment = InitItem<VCurvePathItem>(Color(VColor::MainColor), this);
}

//---------------------------------------------------------------------------------------------------------------------
VisToolCubicBezierPath::~VisToolCubicBezierPath()
{
    qDeleteAll(mainPoints);
    qDeleteAll(lines);
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolCubicBezierPath::RefreshGeometry()
{
    const QVector<VPointF> pathPoints = path.GetCubicPath();
    const int size = pathPoints.size();
    if (size > 0)
    {
        const int countSubSpl = VCubicBezierPath::CountSubSpl(size);

        for (int i = 0; i < size; ++i)
        {
            VScaledEllipse *point = this->GetPoint(mainPoints, static_cast<unsigned>(i), 1/*zValue*/);
            DrawPoint(point, static_cast<QPointF>(pathPoints.at(i)), Color(VColor::SupportColor));
        }

        if (GetMode() == Mode::Creation)
        {
            if (countSubSpl < 1)
            {
                Creating(pathPoints, size-1);
            }
            else
            {
                const qint32 last = VCubicBezierPath::SubSplOffset(countSubSpl) + 3;
                Creating(pathPoints, size-1-last);
            }
        }

        if (countSubSpl >= 1)
        {
            DrawPath(this, path.GetPath(), path.DirectionArrows(), Color(VColor::MainColor), LineStyle(), Qt::RoundCap);

            for (qint32 i = 1; i<=countSubSpl; ++i)
            {
                const int preLastPoint = (countSubSpl - 1) * 2;
                const int lastPoint = preLastPoint + 1;

                const VSpline spl = path.GetSpline(i);

                VScaledLine *ctrlLine1 = this->GetLine(static_cast<unsigned>(preLastPoint));
                DrawLine(ctrlLine1, QLineF(static_cast<QPointF>(spl.GetP1()), static_cast<QPointF>(spl.GetP2())),
                         Color(VColor::MainColor), Qt::DashLine);

                VScaledEllipse *p2 = this->GetPoint(ctrlPoints, static_cast<unsigned>(preLastPoint));
                DrawPoint(p2, static_cast<QPointF>(spl.GetP2()), Qt::green);

                VScaledLine *ctrlLine2 = this->GetLine(static_cast<unsigned>(lastPoint));
                DrawLine(ctrlLine2, QLineF(static_cast<QPointF>(spl.GetP4()), static_cast<QPointF>(spl.GetP3())),
                         Color(VColor::MainColor), Qt::DashLine);

                VScaledEllipse *p3 = this->GetPoint(ctrlPoints, static_cast<unsigned>(lastPoint));
                DrawPoint(p3, static_cast<QPointF>(spl.GetP3()), Qt::green);
            }
        }

        RefreshToolTip();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolCubicBezierPath::VisualMode(quint32 id)
{
    Q_UNUSED(id)
    StartVisualMode();
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolCubicBezierPath::SetPath(const VCubicBezierPath &value)
{
    path = value;

    RefreshToolTip();
}

//---------------------------------------------------------------------------------------------------------------------
auto VisToolCubicBezierPath::GetPoint(QVector<VScaledEllipse *> &points, quint32 i, qreal z) -> VScaledEllipse *
{
    if (not points.isEmpty() && static_cast<quint32>(points.size() - 1) >= i)
    {
        return points.at(static_cast<int>(i));
    }

    auto *point = InitPoint(Color(VColor::SupportColor), this, z);
    points.append(point);
    return point;
}

//---------------------------------------------------------------------------------------------------------------------
auto VisToolCubicBezierPath::GetLine(quint32 i) -> VScaledLine *
{
    if (static_cast<quint32>(lines.size() - 1) >= i && not lines.isEmpty())
    {
        return lines.at(static_cast<int>(i));
    }

    auto *line = InitItem<VScaledLine>(Color(VColor::MainColor), this);
    lines.append(line);
    return line;
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolCubicBezierPath::Creating(const QVector<VPointF> &pathPoints, int pointsLeft)
{
    const int size = pathPoints.size();
    if (pathPoints.isEmpty() || size+1 < pointsLeft)
    {
        return;
    }

    int subSplPoints = 0;
    const int subSplCount = VCubicBezierPath::CountSubSpl(size);
    if (subSplCount >= 1)
    {
        subSplPoints = VCubicBezierPath::SubSplPointsCount(subSplCount)-1;
    }

    switch(pointsLeft)
    {
        case 0:
        {
            const VPointF &p1 = ConstLast(pathPoints);
            if (pathPoints.size() >= 4)
            {
                QLineF p1p2(static_cast<QPointF>(p1), ScenePos());
                QLineF prP3p1(static_cast<QPointF>(pathPoints.at(size-2)), static_cast<QPointF>(p1));
                p1p2.setAngle(prP3p1.angle());

                const QPointF p2 = p1p2.p2();

                VSpline spline(p1, p2, ScenePos(), VPointF(ScenePos()));
                DrawPath(newCurveSegment, spline.GetPath(), Color(VColor::MainColor), Qt::SolidLine, Qt::RoundCap);

                DrawLine(helpLine1, p1p2, Color(VColor::MainColor), Qt::DashLine);

                const int preLastPoint = subSplCount * 2;
                VScaledEllipse *p2Ctrl = this->GetPoint(ctrlPoints, static_cast<unsigned>(preLastPoint));
                DrawPoint(p2Ctrl, p2, Qt::green);
            }
            else
            {
                DrawLine(helpLine1, QLineF(static_cast<QPointF>(p1), ScenePos()), Color(VColor::MainColor),
                         Qt::DashLine);
            }
            break;
        }
        case 1:
        {
            const VPointF& p1 = pathPoints.at(subSplPoints + pointsLeft-1);
            auto p2 = static_cast<QPointF>(pathPoints.at(subSplPoints + pointsLeft));

            if (subSplCount >= 1)
            {
                QLineF p1p2(static_cast<QPointF>(p1), p2);
                QLineF prP3p1(static_cast<QPointF>(pathPoints.at(subSplPoints + pointsLeft-2)),
                              static_cast<QPointF>(p1));
                p1p2.setAngle(prP3p1.angle());
                p2 = p1p2.p2();
            }

            DrawLine(helpLine1, QLineF(static_cast<QPointF>(p1), p2), Color(VColor::MainColor), Qt::DashLine);

            VSpline spline(p1, p2, ScenePos(), VPointF(ScenePos()));
            DrawPath(newCurveSegment, spline.GetPath(), Color(VColor::MainColor), Qt::SolidLine, Qt::RoundCap);

            const int preLastPoint = subSplCount * 2;
            VScaledEllipse *p2Ctrl = this->GetPoint(ctrlPoints, static_cast<unsigned>(preLastPoint));
            DrawPoint(p2Ctrl, p2, Qt::green);
            break;
        }
        case 2:
        {
            const VPointF& p1 = pathPoints.at(subSplPoints + pointsLeft-2);
            auto p2 = static_cast<QPointF>(pathPoints.at(subSplPoints + pointsLeft-1));
            const QPointF p3 = static_cast<QPointF>(pathPoints.at(subSplPoints + pointsLeft));

            if (subSplCount >= 1)
            {
                QLineF p1p2(static_cast<QPointF>(p1), p2);
                QLineF prP3p1(static_cast<QPointF>(pathPoints.at(subSplPoints + pointsLeft-3)),
                              static_cast<QPointF>(p1));
                p1p2.setAngle(prP3p1.angle());
                p2 = p1p2.p2();
            }

            DrawLine(helpLine1, QLineF(static_cast<QPointF>(p1), p2), Color(VColor::MainColor), Qt::DashLine);
            DrawLine(helpLine2, QLineF(p3, ScenePos()), Color(VColor::MainColor), Qt::DashLine);

            VSpline spline(p1, p2, p3, VPointF(ScenePos()));
            DrawPath(newCurveSegment, spline.GetPath(), Color(VColor::MainColor), Qt::SolidLine, Qt::RoundCap);

            const int preLastPoint = subSplCount * 2;
            VScaledEllipse *p2Ctrl = this->GetPoint(ctrlPoints, static_cast<unsigned>(preLastPoint));
            DrawPoint(p2Ctrl, p2, Qt::green);
            break;
        }
        default:
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VisToolCubicBezierPath::RefreshToolTip()
{
    const int size = path.CountPoints();
    if (size > 0)
    {
        const int countSubSpl = VCubicBezierPath::CountSubSpl(size);

        if (size < 7)
        {
            SetToolTip(tr("<b>Curved path</b>: select seven or more points"));
        }
        else if (size - VCubicBezierPath::SubSplPointsCount(countSubSpl) == 0)
        {
            SetToolTip(tr("<b>Curved path</b>: select seven or more points, "
                          "<b>%1</b> - finish creation").arg(VModifierKey::EnterKey()));
        }
        else
        {
            SetToolTip(tr("<b>Curved path</b>: select more points for complete segment"));
        }

        if (GetMode() == Mode::Show)
        {
            SetToolTip(QString());
        }
        emit ToolTip(CurrentToolTip());
    }
}
