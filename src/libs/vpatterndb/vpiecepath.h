/************************************************************************
 **
 **  @file
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   22 11, 2016
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

#ifndef VPIECEPATH_H
#define VPIECEPATH_H

#include <QtGlobal>
#include <QSharedDataPointer>

#include "../vmisc/def.h"
#include "../vgeometry/vabstractcurve.h"

class VPiecePathData;
class VSAPoint;
class VContainer;
class QPainterPath;
class VPointF;
class VPieceNode;
class VInternalVariable;
class VLayoutPoint;

class VPiecePath
{
public:
    VPiecePath();
    explicit VPiecePath(PiecePathType type);
    VPiecePath(const VPiecePath &path);

    ~VPiecePath();

    VPiecePath &operator=(const VPiecePath &path);
#ifdef Q_COMPILER_RVALUE_REFS
    VPiecePath(VPiecePath &&path) Q_DECL_NOTHROW;
    VPiecePath &operator=(VPiecePath &&path) Q_DECL_NOTHROW;
#endif

    void   Append(const VPieceNode &node);
    void   Clear();
    vsizetype CountNodes() const;

    VPieceNode & operator[](vsizetype indx);
    const VPieceNode & at (vsizetype indx ) const;

    QVector<VPieceNode> GetNodes() const;
    void                SetNodes(const QVector<VPieceNode> &nodes);

    PiecePathType GetType() const;
    void          SetType(PiecePathType type);

    QString GetName() const;
    void    SetName(const QString &name);

    Qt::PenStyle GetPenType() const;
    void         SetPenType(const Qt::PenStyle &type);

    bool IsCutPath() const;
    void SetCutPath(bool cut);

    QString GetVisibilityTrigger() const;
    void    SetVisibilityTrigger(const QString &formula);

    void SetFirstToCuttingContour(bool value);
    bool IsFirstToCuttingContour() const;

    void SetLastToCuttingContour(bool value);
    bool IsLastToCuttingContour() const;

    QVector<VLayoutPoint>     PathPoints(const VContainer *data,
                                         const QVector<QPointF> &cuttingPath = QVector<QPointF>()) const;
    QVector<VPointF>          PathNodePoints(const VContainer *data, bool showExcluded = true) const;
    QVector<QVector<QPointF> > PathCurvePoints(const VContainer *data) const;
    QVector<VSAPoint>         SeamAllowancePoints(const VContainer *data, qreal width, bool reverse) const;

    QPainterPath          PainterPath(const VContainer *data, const QVector<QPointF> &cuttingPath) const;
    QVector<QPainterPath> CurvesPainterPath(const VContainer *data) const;

    QList<quint32> Dependencies() const;
    QVector<quint32> MissingNodes(const VPiecePath &path) const;

    QString NodeName(int nodeIndex, const VContainer *data) const;

    int  indexOfNode(quint32 id) const;
    void NodeOnEdge(quint32 index, VPieceNode &p1, VPieceNode &p2) const;
    bool Contains(quint32 id) const;
    bool OnEdge(quint32 p1, quint32 p2) const;
    vsizetype Edge(quint32 p1, quint32 p2) const;

    QVector<VPieceNode> ListNodePoint() const;

    VPiecePath RemoveEdge(quint32 index) const;

    VSAPoint StartSegment(const VContainer *data, int i) const;
    VSAPoint EndSegment(const VContainer *data, int i) const;

    QPointF NodePreviousPoint(const VContainer *data, int i) const;
    QPointF NodeNextPoint(const VContainer *data, int i) const;

    bool IsVisible(const QHash<QString, QSharedPointer<VInternalVariable> > *vars) const;

    static int indexOfNode(const QVector<VPieceNode> &nodes, quint32 id);

    static vsizetype FindInLoopNotExcludedUp(vsizetype start, const QVector<VPieceNode> &nodes);
    static vsizetype FindInLoopNotExcludedDown(vsizetype start, const QVector<VPieceNode> &nodes);

    static VSAPoint StartSegment(const VContainer *data, const QVector<VPieceNode> &nodes, vsizetype i);
    static VSAPoint EndSegment(const VContainer *data, const QVector<VPieceNode> &nodes, vsizetype i);

    static VSAPoint PreparePointEkv(const VPieceNode &node, const VContainer *data);

    static QVector<VSAPoint> CurveSeamAllowanceSegment(const VContainer *data, const QVector<VPieceNode> &nodes,
                                                       const QSharedPointer<VAbstractCurve> &curve,
                                                       vsizetype i, bool reverse, qreal width,
                                                       const QString &piece = QString());

    static QString NodeName(const QVector<VPieceNode> &nodes, vsizetype nodeIndex, const VContainer *data);

    static QVector<VLayoutPoint> NodesToPoints(const VContainer *data, const QVector<VPieceNode> &nodes,
                                               const QString &piece = QString());

private:
    QSharedDataPointer<VPiecePathData> d;
};

Q_DECLARE_TYPEINFO(VPiecePath, Q_MOVABLE_TYPE); // NOLINT
Q_DECLARE_METATYPE(VPiecePath)

#endif // VPIECEPATH_H
