/************************************************************************
 **
 **  @file   vtoolpointofintersectioncurves.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   22 1, 2016
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

#ifndef VTOOLPOINTOFINTERSECTIONCURVES_H
#define VTOOLPOINTOFINTERSECTIONCURVES_H


#include <QDomElement>
#include <QGraphicsItem>
#include <QMetaObject>
#include <QObject>
#include <QPointF>
#include <QString>
#include <QVector>
#include <QtGlobal>

#include "../ifc/xml/vabstractpattern.h"
#include "../vmisc/def.h"
#include "vtoolsinglepoint.h"

template <class T> class QSharedPointer;

struct VToolPointOfIntersectionCurvesInitData : VToolSinglePointInitData
{
    quint32 firstCurveId{NULL_ID};
    quint32 secondCurveId{NULL_ID};
    VCrossCurvesPoint vCrossPoint{VCrossCurvesPoint::HighestPoint};
    HCrossCurvesPoint hCrossPoint{HCrossCurvesPoint::LeftmostPoint};
    QPair<QString, QString> curve1Segments{};
    QPair<QString, QString> curve2Segments{};
    QString curve1AliasSuffix1{};
    QString curve1AliasSuffix2{};
    QString curve2AliasSuffix1{};
    QString curve2AliasSuffix2{};
};

class VToolPointOfIntersectionCurves : public VToolSinglePoint
{
    Q_OBJECT // NOLINT
public:
    virtual void SetDialog() override;
    static auto Create(const QPointer<DialogTool> &dialog, VMainGraphicsScene *scene, VAbstractPattern *doc,
                       VContainer *data) -> VToolPointOfIntersectionCurves *;
    static auto Create(VToolPointOfIntersectionCurvesInitData initData) -> VToolPointOfIntersectionCurves *;
    static auto FindPoint(const QVector<QPointF> &curve1Points, const QVector<QPointF> &curve2Points,
                          VCrossCurvesPoint vCrossPoint, HCrossCurvesPoint hCrossPoint, QPointF *intersectionPoint)
        -> bool;
    static const QString ToolType;
    virtual auto type() const -> int override { return Type; }
    enum { Type = UserType + static_cast<int>(Tool::PointOfIntersectionCurves) };

    auto FirstCurveName() const -> QString;
    auto SecondCurveName() const -> QString;

    auto GetVCrossPoint() const -> VCrossCurvesPoint;
    void              SetVCrossPoint(VCrossCurvesPoint value);

    auto GetHCrossPoint() const -> HCrossCurvesPoint;
    void              SetHCrossPoint(HCrossCurvesPoint value);

    virtual void ShowVisualization(bool show) override;
protected slots:
    virtual void ShowContextMenu(QGraphicsSceneContextMenuEvent *event, quint32 id=NULL_ID) override;
protected:
    virtual void RemoveReferens() override;
    virtual void SaveDialog(QDomElement &domElement, QList<quint32> &oldDependencies,
                            QList<quint32> &newDependencies) override;
    virtual void SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj) override;
    virtual void ReadToolAttributes(const QDomElement &domElement) override;
    virtual void SetVisualization() override;
    virtual auto MakeToolTip() const -> QString override;

    void SetCurve1Segments(const QPair<QString, QString> &segments);
    void SetCurve2Segments(const QPair<QString, QString> &segments);
private:
    Q_DISABLE_COPY_MOVE(VToolPointOfIntersectionCurves) // NOLINT

    quint32 firstCurveId;
    quint32 secondCurveId;

    VCrossCurvesPoint vCrossPoint;
    HCrossCurvesPoint hCrossPoint;

    QPair<QString, QString> m_curve1Segments{};
    QPair<QString, QString> m_curve2Segments{};

    QString m_curve1AliasSuffix1{};
    QString m_curve1AliasSuffix2{};
    QString m_curve2AliasSuffix1{};
    QString m_curve2AliasSuffix2{};

    explicit VToolPointOfIntersectionCurves(const VToolPointOfIntersectionCurvesInitData &initData,
                                            QGraphicsItem *parent = nullptr);
};

#endif // VTOOLPOINTOFINTERSECTIONCURVES_H
