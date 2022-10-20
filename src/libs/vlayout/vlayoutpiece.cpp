/************************************************************************
 **
 **  @file   vlayoutdetail.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   2 1, 2015
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

#include "vlayoutpiece.h"

#include <QBrush>
#include <QFlags>
#include <QFont>
#include <QFontMetrics>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>
#include <QGraphicsPathItem>
#include <QList>
#include <QMatrix>
#include <QMessageLogger>
#include <QPainterPath>
#include <QPoint>
#include <QPolygonF>
#include <QTransform>
#include <Qt>
#include <QtDebug>
#include <QUuid>
#include <QtMath>

#include "../vpatterndb/floatItemData/vpatternlabeldata.h"
#include "../vpatterndb/floatItemData/vpiecelabeldata.h"
#include "../vpatterndb/floatItemData/vgrainlinedata.h"
#include "../vmisc/vabstractvalapplication.h"
#include "../vmisc/compatibility.h"
#include "../vmisc/literals.h"
#include "../vpatterndb/vcontainer.h"
#include "../vpatterndb/calculator.h"
#include "../vpatterndb/vpassmark.h"
#include "../vpatterndb/vpiecenode.h"
#include "../vgeometry/vpointf.h"
#include "../vgeometry/vplacelabelitem.h"
#include "vlayoutpiece_p.h"
#include "vtextmanager.h"
#include "vgraphicsfillitem.h"
#include "../vgeometry/vlayoutplacelabel.h"

namespace
{
//---------------------------------------------------------------------------------------------------------------------
auto ConvertInternalPaths(const VPiece &piece, const VContainer *pattern) -> QVector<VLayoutPiecePath>
{
    SCASSERT(pattern != nullptr)

    QVector<VLayoutPiecePath> paths;
    const QVector<quint32> pathsId = piece.GetInternalPaths();
    const QVector<QPointF> cuttingPath = piece.CuttingPathPoints(pattern);
    paths.reserve(pathsId.size());
    for (auto id : pathsId)
    {
        const VPiecePath path = pattern->GetPiecePath(id);
        if (path.GetType() == PiecePathType::InternalPath && path.IsVisible(pattern->DataVariables()))
        {
            VLayoutPiecePath convertedPath(path.PathPoints(pattern, cuttingPath));
            convertedPath.SetCutPath(path.IsCutPath());
            convertedPath.SetPenStyle(path.GetPenType());
            paths.append(convertedPath);
        }
    }
    return paths;
}

//---------------------------------------------------------------------------------------------------------------------
auto FindLabelGeometry(const VPatternLabelData &labelData, const VContainer *pattern, qreal &rotationAngle,
                       qreal &labelWidth, qreal &labelHeight, QPointF &pos) -> bool
{
    SCASSERT(pattern != nullptr)

    try
    {
        Calculator cal1;
        rotationAngle = cal1.EvalFormula(pattern->DataVariables(), labelData.GetRotation());
    }
    catch(qmu::QmuParserError &e)
    {
        Q_UNUSED(e);
        return false;
    }

    const quint32 topLeftPin = labelData.TopLeftPin();
    const quint32 bottomRightPin = labelData.BottomRightPin();

    if (topLeftPin != NULL_ID && bottomRightPin != NULL_ID)
    {
        try
        {
            const auto topLeftPinPoint = pattern->GeometricObject<VPointF>(topLeftPin);
            const auto bottomRightPinPoint = pattern->GeometricObject<VPointF>(bottomRightPin);

            const QRectF labelRect = QRectF(static_cast<QPointF>(*topLeftPinPoint),
                                            static_cast<QPointF>(*bottomRightPinPoint));
            labelWidth = qAbs(labelRect.width());
            labelHeight = qAbs(labelRect.height());

            pos = labelRect.topLeft();

            return true;
        }
        catch(const VExceptionBadId &)
        {
            // do nothing.
        }
    }

    try
    {
        Calculator cal1;
        labelWidth = cal1.EvalFormula(pattern->DataVariables(), labelData.GetLabelWidth());
        labelWidth = ToPixel(labelWidth, *pattern->GetPatternUnit());

        Calculator cal2;
        labelHeight = cal2.EvalFormula(pattern->DataVariables(), labelData.GetLabelHeight());
        labelHeight = ToPixel(labelHeight, *pattern->GetPatternUnit());
    }
    catch(qmu::QmuParserError &e)
    {
        Q_UNUSED(e);
        return false;
    }

    const quint32 centerPin = labelData.CenterPin();
    if (centerPin != NULL_ID)
    {
        try
        {
            const auto centerPinPoint = pattern->GeometricObject<VPointF>(centerPin);
            pos = static_cast<QPointF>(*centerPinPoint) - QRectF(0, 0, labelWidth, labelHeight).center();
        }
        catch(const VExceptionBadId &)
        {
            pos = labelData.GetPos();
        }
    }
    else
    {
        pos = labelData.GetPos();
    }

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VLayoutDetail::RotatePoint rotates a point around the center for given angle
 * @param ptCenter center around which the point is rotated
 * @param pt point, which is rotated around the center
 * @param dAng angle of rotation
 * @return position of point pt after rotating it around the center for dAng radians
 */
auto RotatePoint(const QPointF &ptCenter, const QPointF& pt, qreal dAng) -> QPointF
{
    QPointF ptDest;
    QPointF ptRel = pt - ptCenter;
    ptDest.setX(cos(dAng)*ptRel.x() - sin(dAng)*ptRel.y());
    ptDest.setY(sin(dAng)*ptRel.x() + cos(dAng)*ptRel.y());

    return ptDest + ptCenter;
}

//---------------------------------------------------------------------------------------------------------------------
auto PieceLabelText(const QVector<QPointF> &labelShape, const VTextManager &tm) -> QStringList
{
    QStringList text;
    if (labelShape.count() > 2)
    {
        int sourceCount = tm.GetSourceLinesCount();
        text.reserve(sourceCount);
        for (int i = 0; i < sourceCount; ++i)
        {
            text.append(tm.GetSourceLine(i).m_qsText);
        }
    }
    return text;
}

//---------------------------------------------------------------------------------------------------------------------
auto ConvertPlaceLabels(const VPiece &piece, const VContainer *pattern) -> QVector<VLayoutPlaceLabel>
{
    QVector<VLayoutPlaceLabel> labels;
    const auto placeLabels = piece.GetPlaceLabels();
    labels.reserve(placeLabels.size());
    for(const auto &placeLabel : placeLabels)
    {
        const auto label = pattern->GeometricObject<VPlaceLabelItem>(placeLabel);
        if (label->IsVisible())
        {
            QT_WARNING_PUSH
            QT_WARNING_DISABLE_GCC("-Wnoexcept")
            // noexcept-expression evaluates to 'false' because of a call to 'constexpr QPointF::QPointF()'

            labels.append(VLayoutPlaceLabel(*label));

            QT_WARNING_POP
        }
    }
    return labels;
}

//---------------------------------------------------------------------------------------------------------------------
auto PrepareSAPassmark(const VPiece &piece, const VContainer *pattern, const VPassmark &passmark,
                       PassmarkSide side, bool &ok) -> VLayoutPassmark
{
    QT_WARNING_PUSH
    QT_WARNING_DISABLE_GCC("-Wnoexcept")
    // noexcept-expression evaluates to 'false' because of a call to 'constexpr QPointF::QPointF()'

    VLayoutPassmark layoutPassmark;

    QT_WARNING_POP

    VPiecePassmarkData pData = passmark.Data();
    const QVector<VPieceNode> path = piece.GetUnitedPath(pattern);
    const int nodeIndex = VPiecePath::indexOfNode(path, pData.id);
    if (nodeIndex == -1)
    {
        const QString errorMsg =
            QObject::tr("Passmark '%1' is not part of piece '%2'.")
                .arg(pData.nodeName, piece.GetName());
        VAbstractApplication::VApp()->IsPedantic() ? throw VException(errorMsg) :
            qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
        ok = false;
        return {};
    }

    QVector<QLineF> baseLines = passmark.SAPassmarkBaseLine(piece, pattern, static_cast<PassmarkSide>(side));
    if (baseLines.isEmpty())
    {
        const QString errorMsg =
            QObject::tr("Cannot prepare passmark '%1' for piece '%2'. Passmark base line is empty.")
                .arg(pData.nodeName, piece.GetName());
        VAbstractApplication::VApp()->IsPedantic() ? throw VException(errorMsg) :
            qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
        ok = false;
        return {};
    }

    if (side == PassmarkSide::All || side == PassmarkSide::Right)
    {
        layoutPassmark.baseLine = ConstFirst(baseLines);
    }
    else if (side == PassmarkSide::Right)
    {
        layoutPassmark.baseLine = ConstLast(baseLines);
    }

    const QVector<QLineF> lines = passmark.SAPassmark(piece, pattern, side);
    if (lines.isEmpty())
    {
        const QString errorMsg =
            QObject::tr("Cannot prepare passmark '%1' for piece '%2'. Passmark is empty.")
                .arg(pData.nodeName, piece.GetName());
        VAbstractApplication::VApp()->IsPedantic() ? throw VException(errorMsg) :
            qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
        ok = false;
        return {};
    }

    layoutPassmark.lines = lines;
    layoutPassmark.type = pData.passmarkLineType;
    layoutPassmark.isBuiltIn = false;

    ok = true;
    return layoutPassmark;
}

//---------------------------------------------------------------------------------------------------------------------
auto PreapreBuiltInSAPassmark(const VPiece &piece, const VContainer *pattern, const VPassmark &passmark,
                              bool &ok) -> VLayoutPassmark
{
    QT_WARNING_PUSH
    QT_WARNING_DISABLE_GCC("-Wnoexcept")
    // noexcept-expression evaluates to 'false' because of a call to 'constexpr QPointF::QPointF()'

    VLayoutPassmark layoutPassmark;

    QT_WARNING_POP

    VPiecePassmarkData pData = passmark.Data();
    const QVector<VPieceNode> path = piece.GetUnitedPath(pattern);
    const int nodeIndex = VPiecePath::indexOfNode(path, pData.id);
    if (nodeIndex == -1)
    {
        const QString errorMsg =
            QObject::tr("Passmark '%1' is not part of piece '%2'.")
                .arg(pData.nodeName, piece.GetName());
        VAbstractApplication::VApp()->IsPedantic() ? throw VException(errorMsg) :
            qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
        ok = false;
        return {};
    }

    const QVector<QLineF> lines = passmark.BuiltInSAPassmark(piece, pattern);
    if (lines.isEmpty())
    {
        const QString errorMsg =
            QObject::tr("Cannot prepare builtin passmark '%1' for piece '%2'. Passmark is empty.")
                .arg(pData.nodeName, piece.GetName());
        VAbstractApplication::VApp()->IsPedantic() ? throw VException(errorMsg) :
            qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
        ok = false;
        return {};
    }

    layoutPassmark.lines = lines;

    const QVector<QLineF> baseLines = passmark.BuiltInSAPassmarkBaseLine(piece);
    if (baseLines.isEmpty())
    {
        const QString errorMsg =
            QObject::tr("Cannot prepare builtin  passmark '%1' for piece '%2'. Passmark base line is "
                        "empty.")
                .arg(pData.nodeName, piece.GetName());
        VAbstractApplication::VApp()->IsPedantic() ? throw VException(errorMsg) :
            qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
        ok = false;
        return {};
    }

    layoutPassmark.baseLine = ConstFirst (baseLines);
    layoutPassmark.type = pData.passmarkLineType;
    layoutPassmark.isBuiltIn = true;

    ok = true;
    return layoutPassmark;
}

//---------------------------------------------------------------------------------------------------------------------
auto ConvertPassmarks(const VPiece &piece, const VContainer *pattern) -> QVector<VLayoutPassmark>
{
    const QVector<VPassmark> passmarks = piece.Passmarks(pattern);
    QVector<VLayoutPassmark> layoutPassmarks;
    layoutPassmarks.reserve(passmarks.size());

    for(const auto &passmark : passmarks)
    {
        if (passmark.IsNull())
        {
            continue;
        }

        auto AddPassmark = [passmark, piece, pattern, &layoutPassmarks](PassmarkSide side)
        {
            bool ok = false;
            VLayoutPassmark layoutPassmark = PrepareSAPassmark(piece, pattern, passmark, side, ok);
            if (ok)
            {
                layoutPassmarks.append(layoutPassmark);
            }
        };

        auto AddBuiltInPassmark = [passmark, piece, pattern, &layoutPassmarks]()
        {
            bool ok = false;
            VLayoutPassmark layoutPassmark = PreapreBuiltInSAPassmark(piece, pattern, passmark, ok);
            if (ok)
            {
                layoutPassmarks.append(layoutPassmark);
            }
        };

        if (piece.IsSeamAllowanceBuiltIn())
        {
            AddBuiltInPassmark();
            continue;
        }

        VPiecePassmarkData pData = passmark.Data();

        switch(pData.passmarkAngleType)
        {
            case PassmarkAngleType::Straightforward:
            case PassmarkAngleType::Bisector:
                AddPassmark(PassmarkSide::All);
                break;
            case PassmarkAngleType::Intersection:
            case PassmarkAngleType::Intersection2:
                AddPassmark(PassmarkSide::Left);
                AddPassmark(PassmarkSide::Right);
                break;
            case PassmarkAngleType::IntersectionOnlyLeft:
            case PassmarkAngleType::Intersection2OnlyLeft:
                AddPassmark(PassmarkSide::Left);
                break;
            case PassmarkAngleType::IntersectionOnlyRight:
            case PassmarkAngleType::Intersection2OnlyRight:
                AddPassmark(PassmarkSide::Right);
                break;
            default:
                break;
        }

        if (VAbstractApplication::VApp()->Settings()->IsDoublePassmark()
                && (VAbstractApplication::VApp()->Settings()->IsPieceShowMainPath() ||
                    not piece.IsHideMainPath())
                && pData.isMainPathNode
                && pData.passmarkAngleType != PassmarkAngleType::Intersection
                && pData.passmarkAngleType != PassmarkAngleType::IntersectionOnlyLeft
                && pData.passmarkAngleType != PassmarkAngleType::IntersectionOnlyRight
                && pData.passmarkAngleType != PassmarkAngleType::Intersection2
                && pData.passmarkAngleType != PassmarkAngleType::Intersection2OnlyLeft
                && pData.passmarkAngleType != PassmarkAngleType::Intersection2OnlyRight
                && pData.isShowSecondPassmark)
        {
            AddBuiltInPassmark();
        }
    }

    return layoutPassmarks;
}

//---------------------------------------------------------------------------------------------------------------------
auto PrepareGradationPlaceholders(const VContainer *data) -> QMap<QString, QString>
{
    SCASSERT(data != nullptr)

    QMap<QString, QString> placeholders;

    QString heightValue = QString::number(VAbstractValApplication::VApp()->GetDimensionHeight());
    placeholders.insert(pl_height, heightValue);
    placeholders.insert(pl_dimensionX, heightValue);

    QString sizeValue = QString::number(VAbstractValApplication::VApp()->GetDimensionSize());
    placeholders.insert(pl_size, sizeValue);
    placeholders.insert(pl_dimensionY, sizeValue);

    QString hipValue = QString::number(VAbstractValApplication::VApp()->GetDimensionHip());
    placeholders.insert(pl_hip, hipValue);
    placeholders.insert(pl_dimensionZ, hipValue);

    QString waistValue = QString::number(VAbstractValApplication::VApp()->GetDimensionWaist());
    placeholders.insert(pl_waist, waistValue);
    placeholders.insert(pl_dimensionW, waistValue);

    {
        QString label = VAbstractValApplication::VApp()->GetDimensionHeightLabel();
        placeholders.insert(pl_heightLabel, not label.isEmpty() ? label : heightValue);
        placeholders.insert(pl_dimensionXLabel, not label.isEmpty() ? label : heightValue);

        label = VAbstractValApplication::VApp()->GetDimensionSizeLabel();
        placeholders.insert(pl_sizeLabel, not label.isEmpty() ? label : sizeValue);
        placeholders.insert(pl_dimensionYLabel, not label.isEmpty() ? label : heightValue);

        label = VAbstractValApplication::VApp()->GetDimensionHipLabel();
        placeholders.insert(pl_hipLabel, not label.isEmpty() ? label : hipValue);
        placeholders.insert(pl_dimensionZLabel, not label.isEmpty() ? label : heightValue);

        label = VAbstractValApplication::VApp()->GetDimensionWaistLabel();
        placeholders.insert(pl_waistLabel, not label.isEmpty() ? label : waistValue);
        placeholders.insert(pl_dimensionWLabel, not label.isEmpty() ? label : heightValue);
    }

    {
        const QMap<QString, QSharedPointer<VMeasurement> > measurements = data->DataMeasurements();
        auto i = measurements.constBegin();
        while (i != measurements.constEnd())
        {
            placeholders.insert(pl_measurement + i.key(), QString::number(*i.value()->GetValue()));
            ++i;
        }
    }

    return placeholders;
}

//---------------------------------------------------------------------------------------------------------------------
auto ReplacePlaceholders(const QMap<QString, QString> &placeholders, QString line) -> QString
{
    QChar per('%');

    auto TestDimension = [per, placeholders, line](const QString &placeholder, const QString &errorMsg)
    {
        if (line.contains(per+placeholder+per) && placeholders.value(placeholder) == QChar('0'))
        {
            VAbstractApplication::VApp()->IsPedantic()
                    ? throw VException(errorMsg)
                    : qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
        }
    };

    TestDimension(pl_height, QObject::tr("No data for the height dimension."));
    TestDimension(pl_size, QObject::tr("No data for the size dimension."));
    TestDimension(pl_hip, QObject::tr("No data for the hip dimension."));
    TestDimension(pl_waist, QObject::tr("No data for the waist dimension."));

    TestDimension(pl_dimensionX, QObject::tr("No data for the X dimension."));
    TestDimension(pl_dimensionY, QObject::tr("No data for the Y dimension."));
    TestDimension(pl_dimensionZ, QObject::tr("No data for the Z dimension."));
    TestDimension(pl_dimensionW, QObject::tr("No data for the W dimension."));

    auto i = placeholders.constBegin();
    while (i != placeholders.constEnd())
    {
        line.replace(per+i.key()+per, i.value());
        ++i;
    }
    return line;
}

//---------------------------------------------------------------------------------------------------------------------
auto PrepareGradationId(const QString &label, const VContainer *pattern) -> QString
{
    const QMap<QString, QString> placeholders = PrepareGradationPlaceholders(pattern);
    return ReplacePlaceholders(placeholders, label);
}
}  // namespace

// Friend functions
//---------------------------------------------------------------------------------------------------------------------
auto operator<<(QDataStream &dataStream, const VLayoutPoint &p) -> QDataStream &
{
    dataStream << static_cast<QPointF>(p); // NOLINT(cppcoreguidelines-slicing)
    dataStream << p.TurnPoint();
    dataStream << p.CurvePoint();
    return dataStream;
}

//---------------------------------------------------------------------------------------------------------------------
auto operator>>(QDataStream &dataStream, VLayoutPoint &p) -> QDataStream &
{
    QPointF tmp;
    bool turnPointFlag = false;
    bool curvePointFlag = false;
    dataStream >> tmp;
    dataStream >> turnPointFlag;
    dataStream >> curvePointFlag;

    p = VLayoutPoint(tmp);
    p.SetTurnPoint(turnPointFlag);
    p.SetCurvePoint(curvePointFlag);

    return dataStream;
}

//---------------------------------------------------------------------------------------------------------------------
auto operator<<(QDataStream &dataStream, const VLayoutPiece &piece) -> QDataStream &
{
    dataStream << static_cast<VAbstractPiece>(piece); // NOLINT(cppcoreguidelines-slicing)
    dataStream << *piece.d;
    return dataStream;
}

//---------------------------------------------------------------------------------------------------------------------
auto operator>>(QDataStream &dataStream, VLayoutPiece &piece) -> QDataStream &
{
    dataStream >> static_cast<VAbstractPiece &>(piece);
    dataStream >> *piece.d;
    return dataStream;
}

//---------------------------------------------------------------------------------------------------------------------
VLayoutPiece::VLayoutPiece()
    :d(new VLayoutPieceData)
{}

//---------------------------------------------------------------------------------------------------------------------
VLayoutPiece::VLayoutPiece(const VLayoutPiece &detail) // NOLINT(modernize-use-equals-default)
    :VAbstractPiece(detail), d(detail.d)
{}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::operator=(const VLayoutPiece &detail) -> VLayoutPiece &
{
    if ( &detail == this )
    {
        return *this;
    }
    VAbstractPiece::operator=(detail);
    d = detail.d;
    return *this;
}

#ifdef Q_COMPILER_RVALUE_REFS
//---------------------------------------------------------------------------------------------------------------------
VLayoutPiece::VLayoutPiece(VLayoutPiece &&detail) Q_DECL_NOTHROW
    :VAbstractPiece(std::move(detail)), d(std::move(detail.d))
{}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::operator=(VLayoutPiece &&detail) Q_DECL_NOTHROW -> VLayoutPiece &
{
    VAbstractPiece::operator=(detail);
    std::swap(d, detail.d);
    return *this;
}
#endif

//---------------------------------------------------------------------------------------------------------------------
VLayoutPiece::~VLayoutPiece() //NOLINT(modernize-use-equals-default)
{}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::Create(const VPiece &piece, vidtype id, const VContainer *pattern) -> VLayoutPiece
{
    QFuture<QVector<VLayoutPoint> > futureSeamAllowance = QtConcurrent::run(piece, &VPiece::SeamAllowancePoints,
                                                                           pattern);
    QFuture<bool> futureSeamAllowanceValid = QtConcurrent::run(piece, &VPiece::IsSeamAllowanceValid, pattern);
    QFuture<QVector<VLayoutPoint> > futureMainPath = QtConcurrent::run(piece, &VPiece::MainPathPoints, pattern);
    QFuture<QVector<VLayoutPiecePath> > futureInternalPaths = QtConcurrent::run(ConvertInternalPaths, piece, pattern);
    QFuture<QVector<VLayoutPassmark> > futurePassmarks = QtConcurrent::run(ConvertPassmarks, piece, pattern);
    QFuture<QVector<VLayoutPlaceLabel> > futurePlaceLabels = QtConcurrent::run(ConvertPlaceLabels, piece, pattern);

    VLayoutPiece det;

    det.SetMx(piece.GetMx());
    det.SetMy(piece.GetMy());

    det.SetName(piece.GetName());
    det.SetUUID(piece.GetUUID());
    det.SetGradationId(PrepareGradationId(piece.GetGradationLabel(), pattern));

    det.SetSAWidth(VAbstractValApplication::VApp()->toPixel(piece.GetSAWidth()));
    det.SetForbidFlipping(piece.IsForbidFlipping());
    det.SetForceFlipping(piece.IsForceFlipping());
    det.SetId(id);

    if (not futureSeamAllowanceValid.result())
    {
        const QString errorMsg = QObject::tr("Piece '%1'. Seam allowance is not valid.")
                .arg(piece.GetName());
        VAbstractApplication::VApp()->IsPedantic() ? throw VException(errorMsg) :
                                              qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
    }

    det.SetCountourPoints(futureMainPath.result(),
                          VAbstractApplication::VApp()->Settings()->IsPieceShowMainPath() ? false
                                                                                          : piece.IsHideMainPath());
    det.SetSeamAllowancePoints(futureSeamAllowance.result(), piece.IsSeamAllowance(), piece.IsSeamAllowanceBuiltIn());
    det.SetInternalPaths(futureInternalPaths.result());
    det.SetPassmarks(futurePassmarks.result());
    det.SetPlaceLabels(futurePlaceLabels.result());
    det.SetPriority(piece.GetPriority());

    // Very important to set main path first!
    if (det.MappedContourPath().isEmpty())
    {
        throw VException (tr("Piece %1 doesn't have shape.").arg(piece.GetName()));
    }

    const VPieceLabelData& data = piece.GetPatternPieceData();
    det.SetQuantity(data.GetQuantity());
    if (data.IsVisible())
    {
        det.SetPieceText(piece.GetName(), data, VAbstractApplication::VApp()->Settings()->GetLabelFont(), pattern);
    }

    const VPatternLabelData& geom = piece.GetPatternInfo();
    if (geom.IsVisible())
    {
        VAbstractPattern* pDoc = VAbstractValApplication::VApp()->getCurrentDocument();
        det.SetPatternInfo(pDoc, geom, VAbstractApplication::VApp()->Settings()->GetLabelFont(), pattern);
    }

    const VGrainlineData& grainlineGeom = piece.GetGrainlineGeometry();
    if (grainlineGeom.IsVisible())
    {
        det.SetGrainline(grainlineGeom, pattern);
    }

    return det;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetUniqueID() const -> QString
{
    QString id = VAbstractPiece::GetUniqueID();

    if (not d->m_gradationId.isEmpty())
    {
        id = id + '_' + d->m_gradationId;
    }

    return id;
}

//---------------------------------------------------------------------------------------------------------------------
template <class T>
auto VLayoutPiece::Map(QVector<T> points) const -> QVector<T>
{
    std::transform(points.begin(), points.end(), points.begin(),
                   [this](const T &point) { return d->m_matrix.map(point); });
    if (d->m_mirror)
    {
        std::reverse(points.begin(), points.end());
    }
    return points;
}

//---------------------------------------------------------------------------------------------------------------------
template <> // NOLINTNEXTLINE(readability-inconsistent-declaration-parameter-name)
auto VLayoutPiece::Map<VLayoutPassmark>(QVector<VLayoutPassmark> passmarks) const -> QVector<VLayoutPassmark>
{
    for (auto & passmark : passmarks)
    {
        passmark.lines = Map(passmark.lines);
        passmark.baseLine = d->m_matrix.map(passmark.baseLine);
    }

    return passmarks;
}

//---------------------------------------------------------------------------------------------------------------------
template <>
auto VLayoutPiece::Map<VLayoutPoint>(QVector<VLayoutPoint> points) const -> QVector<VLayoutPoint>
{
    std::transform(points.begin(), points.end(), points.begin(), [this](VLayoutPoint point)
    {
        auto p = static_cast<QPointF>(point); // NOLINT(cppcoreguidelines-slicing)
        p = d->m_matrix.map(p);
        point.rx() = p.x();
        point.ry() = p.y();
        return point;
    });
    if (d->m_mirror)
    {
        std::reverse(points.begin(), points.end());
    }
    return points;
}

//---------------------------------------------------------------------------------------------------------------------
// cppcheck-suppress unusedFunction
auto VLayoutPiece::GetMappedContourPoints() const -> QVector<VLayoutPoint>
{
    return Map(d->m_contour);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetContourPoints() const -> QVector<VLayoutPoint>
{
    return d->m_contour;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetCountourPoints(const QVector<VLayoutPoint> &points, bool hideMainPath)
{
    d->m_contour = RemoveDublicates(points, false);
    SetHideMainPath(hideMainPath);
}

//---------------------------------------------------------------------------------------------------------------------
// cppcheck-suppress unusedFunction
auto VLayoutPiece::GetMappedSeamAllowancePoints() const -> QVector<VLayoutPoint>
{
    return Map(d->m_seamAllowance);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetSeamAllowancePoints() const -> QVector<VLayoutPoint>
{
    return d->m_seamAllowance;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetSeamAllowancePoints(const QVector<VLayoutPoint> &points, bool seamAllowance,
                                          bool seamAllowanceBuiltIn)
{
    if (seamAllowance)
    {
        SetSeamAllowance(seamAllowance);
        SetSeamAllowanceBuiltIn(seamAllowanceBuiltIn);
        d->m_seamAllowance = points;
        if (not d->m_seamAllowance.isEmpty())
        {
            d->m_seamAllowance = RemoveDublicates(d->m_seamAllowance, false);
        }
        else if (not IsSeamAllowanceBuiltIn())
        {
            qWarning()<<"Seam allowance is empty.";
            SetSeamAllowance(false);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetMappedLayoutAllowancePoints() const -> QVector<QPointF>
{
    return Map(d->m_layoutAllowance);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetLayoutAllowancePoints() const -> QVector<QPointF>
{
    return d->m_layoutAllowance;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPieceTextPosition() const -> QPointF
{
    if (d->m_detailLabel.count() > 2)
    {
        return d->m_matrix.map(ConstFirst(d->m_detailLabel));
    }

    return {};
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPieceText() const -> QStringList
{
    return PieceLabelText(d->m_detailLabel, d->m_tmDetail);
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetPieceText(const QString& qsName, const VPieceLabelData& data, const QFont &font,
                                const VContainer *pattern)
{
    QPointF ptPos;
    qreal labelWidth = 0;
    qreal labelHeight = 0;
    qreal labelAngle = 0;
    if (not FindLabelGeometry(data, pattern, labelAngle, labelWidth, labelHeight, ptPos))
    {
        return;
    }

    QVector<QPointF> v;
    v << ptPos
      << QPointF(ptPos.x() + labelWidth, ptPos.y())
      << QPointF(ptPos.x() + labelWidth, ptPos.y() + labelHeight)
      << QPointF(ptPos.x(), ptPos.y() + labelHeight);

    const qreal dAng = qDegreesToRadians(-labelAngle);
    const QPointF ptCenter(ptPos.x() + labelWidth/2, ptPos.y() + labelHeight/2);

    for (int i = 0; i < v.count(); ++i)
    {
        v[i] = RotatePoint(ptCenter, v.at(i), dAng);
    }

    QScopedPointer<QGraphicsItem> item(GetMainPathItem());
    d->m_detailLabel = CorrectPosition(item->boundingRect(), v);

    // generate text
    d->m_tmDetail.SetFont(font);
    d->m_tmDetail.SetFontSize(data.GetFontSize());
    d->m_tmDetail.Update(qsName, data, pattern);
    // this will generate the lines of text
    d->m_tmDetail.SetFontSize(data.GetFontSize());
    d->m_tmDetail.FitFontSize(labelWidth, labelHeight);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPieceLabelRect() const -> QVector<QPointF>
{
    return d->m_detailLabel;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetPieceLabelRect(const QVector<QPointF> &rect)
{
    d->m_detailLabel = rect;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPieceLabelData() const -> VTextManager
{
    return d->m_tmDetail;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetPieceLabelData(const VTextManager &data)
{
    d->m_tmDetail = data;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPatternTextPosition() const -> QPointF
{
    if (d->m_patternInfo.count() > 2)
    {
        return d->m_matrix.map(ConstFirst(d->m_patternInfo));
    }

    return {};
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPatternText() const -> QStringList
{
    return PieceLabelText(d->m_patternInfo, d->m_tmPattern);
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetPatternInfo(VAbstractPattern* pDoc, const VPatternLabelData& geom, const QFont &font,
                                  const VContainer *pattern)
{
    QPointF ptPos;
    qreal labelWidth = 0;
    qreal labelHeight = 0;
    qreal labelAngle = 0;
    if (not FindLabelGeometry(geom, pattern, labelAngle, labelWidth, labelHeight, ptPos))
    {
        return;
    }

    QVector<QPointF> v;
    v << ptPos
      << QPointF(ptPos.x() + labelWidth, ptPos.y())
      << QPointF(ptPos.x() + labelWidth, ptPos.y() + labelHeight)
      << QPointF(ptPos.x(), ptPos.y() + labelHeight);

    const qreal dAng = qDegreesToRadians(-labelAngle);
    const QPointF ptCenter(ptPos.x() + labelWidth/2, ptPos.y() + labelHeight/2);
    for (int i = 0; i < v.count(); ++i)
    {
        v[i] = RotatePoint(ptCenter, v.at(i), dAng);
    }
    QScopedPointer<QGraphicsItem> item(GetMainPathItem());
    d->m_patternInfo = CorrectPosition(item->boundingRect(), v);

    // Generate text
    d->m_tmPattern.SetFont(font);
    d->m_tmPattern.SetFontSize(geom.GetFontSize());

    d->m_tmPattern.Update(pDoc, pattern);

    // generate lines of text
    d->m_tmPattern.SetFontSize(geom.GetFontSize());
    d->m_tmPattern.FitFontSize(labelWidth, labelHeight);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPatternLabelRect() const -> QVector<QPointF>
{
    return d->m_patternInfo;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetPatternLabelRect(const QVector<QPointF> &rect)
{
    d->m_patternInfo = rect;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPatternLabelData() const -> VTextManager
{
    return d->m_tmPattern;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetPatternLabelData(const VTextManager &data)
{
    d->m_tmPattern = data;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetGrainline(const VGrainlineData& geom, const VContainer* pattern)
{
    qreal dAng = 0;

    QScopedPointer<QGraphicsItem> item(GetMainPathItem());
    const QVector<QPointF> v = GrainlinePoints(geom, pattern, item->boundingRect(), dAng);

    if (v.isEmpty())
    {
        return;
    }

    d->m_grainlineEnabled = true;
    d->m_grainlineArrowType = geom.GetArrowType();
    d->m_grainlineAngle = qRadiansToDegrees(dAng);
    d->m_grainlinePoints = v;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetMappedGrainline() const -> QVector<QPointF>
{
    return Map(d->m_grainlinePoints);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetGrainline() const -> QVector<QPointF>
{
    return d->m_grainlinePoints;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::IsGrainlineEnabled() const -> bool
{
    return d->m_grainlineEnabled;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetGrainlineEnabled(bool enabled)
{
    d->m_grainlineEnabled = enabled;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetGrainlineAngle(qreal angle)
{
    d->m_grainlineAngle = angle;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetGrainlineArrowType(GrainlineArrowDirection type)
{
    d->m_grainlineArrowType = type;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetGrainlinePoints(const QVector<QPointF> &points)
{
    d->m_grainlinePoints = points;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GrainlineAngle() const -> qreal
{
    return d->m_grainlineAngle;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GrainlineArrowType() const -> GrainlineArrowDirection
{
    return d->m_grainlineArrowType;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetMatrix() const -> QTransform
{
    return d->m_matrix;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetMatrix(const QTransform &matrix)
{
    d->m_matrix = matrix;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetLayoutWidth() const -> qreal
{
    return d->m_layoutWidth;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetLayoutWidth(qreal value)
{
    d->m_layoutWidth = value;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetQuantity() const -> quint16
{
    return d->m_quantity;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetQuantity(quint16 value)
{
    d->m_quantity = qMax(static_cast<quint16>(1), value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetId() const -> vidtype
{
    return d->m_id;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetId(vidtype id)
{
    d->m_id = id;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::Translate(qreal dx, qreal dy)
{
    Translate(QPointF(dx, dy));
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::Translate(const QPointF &p)
{
    QTransform m;
    m.translate(p.x(), p.y());
    d->m_matrix *= m;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::Scale(qreal sx, qreal sy)
{
    d->m_xScale *= sx;
    d->m_yScale *= sy;

    QTransform m;
    m.scale(sx, sy);
    d->m_matrix *= m;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::Rotate(const QPointF &originPoint, qreal degrees)
{
    QTransform m;
    m.translate(originPoint.x(), originPoint.y());
    m.rotate(-degrees);
    m.translate(-originPoint.x(), -originPoint.y());
    d->m_matrix *= m;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::Mirror(const QLineF &edge)
{
    if (edge.isNull())
    {
        return;
    }

    const QLineF axis = QLineF(edge.x2(), edge.y2(), edge.x2() + 100, edge.y2()); // Ox axis

    const qreal angle = edge.angleTo(axis);
    const QPointF p2 = edge.p2();
    QTransform m;
    m.translate(p2.x(), p2.y());
    m.rotate(-angle);
    m.translate(-p2.x(), -p2.y());
    d->m_matrix *= m;

    m.reset();
    m.translate(p2.x(), p2.y());
    m.scale(m.m11(), m.m22()*-1);
    m.translate(-p2.x(), -p2.y());
    d->m_matrix *= m;

    m.reset();
    m.translate(p2.x(), p2.y());
    m.rotate(-(360-angle));
    m.translate(-p2.x(), -p2.y());
    d->m_matrix *= m;

    d->m_mirror = !d->m_mirror;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::Mirror()
{
    QTransform m;
    m.scale(-1, 1);
    d->m_matrix *= m;
    d->m_mirror = !d->m_mirror;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::DetailEdgesCount() const -> int
{
    return DetailPath().count();
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::LayoutEdgesCount() const -> int
{
    const int count = d->m_layoutAllowance.count();
    return count > 2 ? count : 0;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::LayoutEdge(int i) const -> QLineF
{
    return Edge(d->m_layoutAllowance, i);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::LayoutEdgeByPoint(const QPointF &p1) const -> int
{
    return EdgeByPoint(d->m_layoutAllowance, p1);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::MappedDetailBoundingRect() const -> QRectF
{
    return BoundingRect(CastTo<QPointF>(GetMappedExternalContourPoints()));
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::DetailBoundingRect() const -> QRectF
{
    return BoundingRect(CastTo<QPointF>(GetExternalContourPoints()));
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::MappedLayoutBoundingRect() const -> QRectF
{
    return BoundingRect(GetMappedLayoutAllowancePoints());
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::Diagonal() const -> qreal
{
    const QRectF rec = MappedLayoutBoundingRect();
    return qSqrt(pow(rec.height(), 2) + pow(rec.width(), 2));
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::isNull() const -> bool
{
    if (not d->m_contour.isEmpty() && d->m_layoutWidth > 0)
    {
        return not (IsSeamAllowance() && not IsSeamAllowanceBuiltIn() && not d->m_seamAllowance.isEmpty());
    }
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::Square() const -> qint64
{
    return d->m_square;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetLayoutAllowancePoints()
{
    d->m_square = 0;

    if (d->m_layoutWidth > 0)
    {
        if (IsSeamAllowance() && not IsSeamAllowanceBuiltIn())
        {
            d->m_layoutAllowance = CastTo<QPointF>(Equidistant(CastTo<VSAPoint>(GetMappedSeamAllowancePoints()),
                                                               d->m_layoutWidth, GetName()));
            if (not d->m_layoutAllowance.isEmpty())
            {
                d->m_layoutAllowance.removeLast();

                d->m_square = qFloor(qAbs(SumTrapezoids(CastTo<QPointF>(GetSeamAllowancePoints()))/2.0));
            }
        }
        else
        {
            d->m_layoutAllowance = CastTo<QPointF>(Equidistant(CastTo<VSAPoint>(GetMappedContourPoints()),
                                                               d->m_layoutWidth, GetName()));
            if (not d->m_layoutAllowance.isEmpty())
            {
                d->m_layoutAllowance.removeLast();

                d->m_square = qFloor(qAbs(SumTrapezoids(CastTo<QPointF>(GetContourPoints()))/2.0));
            }
        }
    }
    else
    {
        d->m_layoutAllowance.clear();
    }
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetMappedExternalContourPoints() const -> QVector<VLayoutPoint>
{
    return IsSeamAllowance() && not IsSeamAllowanceBuiltIn() ? GetMappedSeamAllowancePoints() :
                                                               GetMappedContourPoints();
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetExternalContourPoints() const -> QVector<VLayoutPoint>
{
    return IsSeamAllowance() && not IsSeamAllowanceBuiltIn() ? GetSeamAllowancePoints() :
                                                               GetContourPoints();
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetMappedPassmarks() const -> QVector<VLayoutPassmark>
{
    return Map(d->m_passmarks);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPassmarks() const -> QVector<VLayoutPassmark>
{
    return d->m_passmarks;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetPassmarks(const QVector<VLayoutPassmark> &passmarks)
{
    if (IsSeamAllowance())
    {
        d->m_passmarks = passmarks;
    }
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetPlaceLabels() const -> QVector<VLayoutPlaceLabel>
{
    return d->m_placeLabels;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetPlaceLabels(const QVector<VLayoutPlaceLabel> &labels)
{
    d->m_placeLabels = labels;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::MappedInternalPathsForCut(bool cut) const -> QVector<QVector<VLayoutPoint> >
{
    QVector<QVector<VLayoutPoint> > paths;
    paths.reserve(d->m_internalPaths.size());

    for (const auto &path : d->m_internalPaths)
    {
        if (path.IsCutPath() == cut)
        {
            paths.append(Map(path.Points()));
        }
    }

    return paths;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetInternalPaths() const -> QVector<VLayoutPiecePath>
{
    return d->m_internalPaths;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetInternalPaths(const QVector<VLayoutPiecePath> &internalPaths)
{
    d->m_internalPaths = internalPaths;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::MappedContourPath() const -> QPainterPath
{
    return d->m_matrix.map(ContourPath());
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::ContourPath() const -> QPainterPath
{
    QPainterPath path;

    // contour
    if (not IsHideMainPath() || not IsSeamAllowance() || IsSeamAllowanceBuiltIn())
    {
        path = PainterPath(GetContourPoints());
    }

    // seam allowance
    if (IsSeamAllowance())
    {
        if (not IsSeamAllowanceBuiltIn())
        {
            // Draw seam allowance
            QVector<VLayoutPoint> points = GetSeamAllowancePoints();

            if (ConstLast(points).toPoint() != ConstFirst(points).toPoint())
            {
                points.append(points.at(0));// Should be always closed
            }

            QPainterPath ekv;
            ekv.moveTo(points.at(0));
            for (qint32 i = 1; i < points.count(); ++i)
            {
                ekv.lineTo(points.at(i));
            }

            path.addPath(ekv);
        }

        // Draw passmarks
        QPainterPath passmaksPath;
        const QVector<VLayoutPassmark> passmarks = GetPassmarks();
        for(const auto &passmark : passmarks)
        {
            for (const auto &line : passmark.lines)
            {
                passmaksPath.moveTo(line.p1());
                passmaksPath.lineTo(line.p2());
            }
        }

        path.addPath(passmaksPath);
        path.setFillRule(Qt::WindingFill);
    }

    return path;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::MappedLayoutAllowancePath() const -> QPainterPath
{
    return PainterPath(GetMappedLayoutAllowancePoints());
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::DrawMiniature(QPainter &painter) const
{
    painter.drawPath(ContourPath());

    for (const auto &path : d->m_internalPaths)
    {
        painter.save();

        QPen pen = painter.pen();
        pen.setStyle(path.PenStyle());
        painter.setPen(pen);

        painter.drawPath(path.GetPainterPath());

        painter.restore();
    }

    for (const auto &label : d->m_placeLabels)
    {
        painter.drawPath(LabelShapePath(label));
    }

    QVector<QPointF> gPoints = GetGrainline();
    if (not gPoints.isEmpty())
    {
        QPainterPath path;
        path.moveTo(gPoints.at(0));
        for (auto p : qAsConst(gPoints))
        {
            path.lineTo(p);
        }
        painter.drawPath(path);
    }
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetItem(bool textAsPaths) const -> QGraphicsItem *
{
    QGraphicsPathItem *item = GetMainItem();

    for (const auto &path : d->m_internalPaths)
    {
        auto* pathItem = new QGraphicsPathItem(item);
        pathItem->setPath(d->m_matrix.map(path.GetPainterPath()));

        QPen pen = pathItem->pen();
        pen.setStyle(path.PenStyle());
        pen.setWidthF(VAbstractApplication::VApp()->Settings()->WidthHairLine());
        pathItem->setPen(pen);
    }

    for (const auto &label : d->m_placeLabels)
    {
        auto* pathItem = new QGraphicsPathItem(item);
        QPen pen = pathItem->pen();
        pen.setWidthF(VAbstractApplication::VApp()->Settings()->WidthHairLine());
        pathItem->setPen(pen);
        pathItem->setPath(d->m_matrix.map(LabelShapePath(PlaceLabelShape(label))));
    }

    CreateLabelStrings(item, d->m_detailLabel, d->m_tmDetail, textAsPaths);
    CreateLabelStrings(item, d->m_patternInfo, d->m_tmPattern, textAsPaths);
    CreateGrainlineItem(item);

    return item;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::IsLayoutAllowanceValid() const -> bool
{
    QVector<VLayoutPoint> base = (IsSeamAllowance() && not IsSeamAllowanceBuiltIn()) ?
                                     d->m_seamAllowance : d->m_contour;
    return VAbstractPiece::IsAllowanceValid(CastTo<QPointF>(base), d->m_layoutAllowance);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::BiggestEdge() const -> qreal
{
    qreal edge = 0;

    if (LayoutEdgesCount() < 1)
    {
        return edge;
    }

    for (int i = 1; i < LayoutEdgesCount(); ++i)
    {
        const qreal length = LayoutEdge(i).length();
        if (length > edge)
        {
            edge = length;
        }
    }

    return edge;
}


//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::MapPlaceLabelShape(PlaceLabelImg shape) const -> PlaceLabelImg
{
    for (auto & i : shape)
    {
        i = Map(i);
    }

    return shape;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::BoundingRect(QVector<QPointF> points) -> QRectF
{
    points.append(ConstFirst(points));
    return QPolygonF(points).boundingRect();
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::CreateLabelStrings(QGraphicsItem *parent, const QVector<QPointF> &labelShape,
                                      const VTextManager &tm, bool textAsPaths) const
{
    SCASSERT(parent != nullptr)

    if (labelShape.count() <= 2)
    {
        return;
    }

    const qreal dW = QLineF(labelShape.at(0), labelShape.at(1)).length();
    const qreal dH = QLineF(labelShape.at(1), labelShape.at(2)).length();
    const qreal angle = - QLineF(labelShape.at(0), labelShape.at(1)).angle();
    qreal dY = 0;

    for (int i = 0; i < tm.GetSourceLinesCount(); ++i)
    {
        const TextLine& tl = tm.GetSourceLine(i);
        QFont fnt = tm.GetFont();
        fnt.setPixelSize(tm.GetFont().pixelSize() + tl.m_iFontSize);
        fnt.setBold(tl.m_bold);
        fnt.setItalic(tl.m_italic);

        QFontMetrics fm(fnt);

        if (textAsPaths)
        {
            dY += fm.height();
        }

        if (dY > dH)
        {
            break;
        }

        QString qsText = tl.m_qsText;
        if (TextWidth(fm, qsText) > dW)
        {
            qsText = fm.elidedText(qsText, Qt::ElideMiddle, static_cast<int>(dW));
        }

        qreal dX = 0;
        if (tl.m_eAlign == 0 || (tl.m_eAlign & Qt::AlignLeft) > 0)
        {
            dX = 0;
        }
        else if ((tl.m_eAlign & Qt::AlignHCenter) > 0)
        {
            dX = (dW - TextWidth(fm, qsText))/2;
        }
        else if ((tl.m_eAlign & Qt::AlignRight) > 0)
        {
            dX = dW - TextWidth(fm, qsText);
        }

        // set up the rotation around top-left corner matrix
        QTransform labelMatrix;
        labelMatrix.translate(labelShape.at(0).x(), labelShape.at(0).y());
        if (d->m_mirror)
        {
            labelMatrix.scale(-1, 1);
            labelMatrix.rotate(-angle);
            labelMatrix.translate(-dW, 0);
            labelMatrix.translate(dX, dY); // Each string has own position
        }
        else
        {
            labelMatrix.rotate(angle);
            labelMatrix.translate(dX, dY); // Each string has own position
        }

        labelMatrix *= d->m_matrix;

        if (textAsPaths)
        {
            QPainterPath path;
            path.addText(0, - static_cast<qreal>(fm.ascent())/6., fnt, qsText);

            auto* item = new QGraphicsPathItem(parent);
            item->setPath(path);
            item->setBrush(QBrush(Qt::black));
            item->setTransform(labelMatrix);

            dY += tm.GetSpacing();
        }
        else
        {
            auto* item = new QGraphicsSimpleTextItem(parent);
            item->setFont(fnt);
            item->setText(qsText);
            item->setTransform(labelMatrix);

            dY += (fm.height() + tm.GetSpacing());
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::CreateGrainlineItem(QGraphicsItem *parent) const
{
    SCASSERT(parent != nullptr)

    if (not d->m_grainlineEnabled || d->m_grainlinePoints.count() < 2)
    {
        return;
    }
    auto* item = new VGraphicsFillItem(parent);
    item->SetWidth(VAbstractApplication::VApp()->Settings()->WidthHairLine());

    QPainterPath path;

    QVector<QPointF> gPoints = GetMappedGrainline();
    path.moveTo(gPoints.at(0));
    for (auto p : qAsConst(gPoints))
    {
        path.lineTo(p);
    }
    item->setPath(path);
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::DetailPath() const -> QVector<VLayoutPoint>
{
    if (IsSeamAllowance() && not IsSeamAllowanceBuiltIn())
    {
        return d->m_seamAllowance;
    }

    return d->m_contour;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetMainItem() const -> QGraphicsPathItem *
{
    auto *item = new QGraphicsPathItem();
    QPen pen = item->pen();
    pen.setWidthF(VAbstractApplication::VApp()->Settings()->WidthHairLine());
    item->setPen(pen);
    item->setPath(MappedContourPath());
    return item;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetMainPathItem() const -> QGraphicsPathItem *
{
    auto *item = new QGraphicsPathItem();
    QPen pen = item->pen();
    pen.setWidthF(VAbstractApplication::VApp()->Settings()->WidthHairLine());
    item->setPen(pen);

    QPainterPath path;

    // contour
    QVector<VLayoutPoint> points = GetMappedContourPoints();

    path.moveTo(points.at(0));
    for (qint32 i = 1; i < points.count(); ++i)
    {
        path.lineTo(points.at(i));
    }
    path.lineTo(points.at(0));

    item->setPath(path);
    return item;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::IsMirror() const -> bool
{
    return d->m_mirror;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetMirror(bool value)
{
    d->m_mirror = value;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetGradationId(const QString &id)
{
    d->m_gradationId = id;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetGradationId() const -> QString
{
    return d->m_gradationId;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetXScale() const -> qreal
{
    return d->m_xScale;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetXScale(qreal xs)
{
    d->m_xScale = xs;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::GetYScale() const -> qreal
{
    return d->m_yScale;
}

//---------------------------------------------------------------------------------------------------------------------
void VLayoutPiece::SetYScale(qreal ys)
{
    d->m_yScale = ys;
}

//---------------------------------------------------------------------------------------------------------------------
auto VLayoutPiece::Edge(const QVector<QPointF> &path, int i) const -> QLineF
{
    if (i < 1)
    { // Doesn't exist such edge
        return {};
    }

    int i1, i2;
    if (i < path.count())
    {
        i1 = i-1;
        i2 = i;
    }
    else
    {
        i1 = path.count()-1;
        i2 = 0;
    }

    if (d->m_mirror)
    {
        QVector<QPointF> newPath = Map(path);
        return {newPath.at(i1), newPath.at(i2)};
    }
    return {d->m_matrix.map(path.at(i1)), d->m_matrix.map(path.at(i2))};
}

//---------------------------------------------------------------------------------------------------------------------
// NOTE: Once C++17 is made mandatory, this method can further be refactored with std::optional<int>
auto VLayoutPiece::EdgeByPoint(const QVector<QPointF> &path, const QPointF &p1) const -> int
{
    if (p1.isNull() || path.count() < 3)
    {
        return 0;
    }

    const auto points = Map(path);
    const auto *const posIter = std::find_if(points.cbegin(), points.cend(),
                                             [&p1](const QPointF &point){ return VFuzzyComparePoints(point, p1); });
    if (posIter != points.cend())
    {
        return static_cast<int>(posIter - points.cbegin() + 1);
    }
    return 0; // Did not find edge
}
