/************************************************************************
 **
 **  @file   vcontrolpointspline.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
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

#ifndef VCONTROLPOINTSPLINE_H
#define VCONTROLPOINTSPLINE_H

#include <QGraphicsItem>
#include <QMetaObject>
#include <QObject>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QtGlobal>

#include "../vgeometry/vgeometrydef.h"
#include "../vmisc/def.h"
#include "vscenepoint.h"

class VScaledLine;

/**
 * @brief The VControlPointSpline class control spline point.
 */
class VControlPointSpline : public QObject, public VScenePoint
{
    Q_OBJECT // NOLINT

public:
    VControlPointSpline(const vsizetype &indexSpline, SplinePointPosition position, QGraphicsItem *parent = nullptr);
    VControlPointSpline(const vsizetype &indexSpline, SplinePointPosition position, const QPointF &controlPoint,
                        bool freeAngle, bool freeLength, QGraphicsItem *parent = nullptr);
    ~VControlPointSpline() override = default;

    auto type() const -> int override { return Type; }
    enum
    {
        Type = UserType + static_cast<int>(Vis::ControlPointSpline)
    };

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
signals:
    /**
     * @brief ControlPointChangePosition emit when control point change position.
     * @param indexSpline index spline in list.
     * @param position position point in spline.
     * @param pos new posotion.
     */
    void ControlPointChangePosition(const vsizetype &indexSpline, SplinePointPosition position, const QPointF &pos);
    /**
     * @brief ShowContextMenu emit when need show tool's context menu.
     * @param event context menu event.
     */
    void ShowContextMenu(QGraphicsSceneContextMenuEvent *event);
    void Released();
    void Selected(bool selected);
public slots:
    void RefreshCtrlPoint(const vsizetype &indexSpline, SplinePointPosition pos, const QPointF &controlPoint,
                          const QPointF &splinePoint, bool freeAngle = true, bool freeLength = true);
    void setEnabledPoint(bool enable);

protected:
    /** @brief controlLine pointer to line control point. */
    VScaledLine *controlLine;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    auto itemChange(GraphicsItemChange change, const QVariant &value) -> QVariant override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    Q_DISABLE_COPY_MOVE(VControlPointSpline) // NOLINT
    /** @brief indexSpline index spline in list.. */
    vsizetype indexSpline;

    /** @brief position position point in spline. */
    SplinePointPosition position;

    bool freeAngle;
    bool freeLength;

    void Init();
    void SetCtrlLine(const QPointF &controlPoint, const QPointF &splinePoint);
};

#endif // VCONTROLPOINTSPLINE_H
