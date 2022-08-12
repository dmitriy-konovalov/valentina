/************************************************************************
 **
 **  @file   vpsettings.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   16 2, 2020
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2020 Valentina project
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
#include "vpsettings.h"

#include <QMarginsF>

namespace
{
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingDockWidgetPropertiesActive, (QLatin1String("dockWidget/properties"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingDockPropertiesContentsActive, (QLatin1String("dockWidget/contents"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutUnit, (QLatin1String("layout/unit"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutSheetPaperWidth, (QLatin1String("layout/sheetPaperWidth"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutSheetPaperHeight, (QLatin1String("layout/sheetPaperHeight"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutTilePaperWidth, (QLatin1String("layout/tilePaperWidth"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutTilePaperHeight, (QLatin1String("layout/tilePaperHeight"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutSheetMargins, (QLatin1String("layout/sheetMargins"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutTileMargins, (QLatin1String("layout/tileMargins"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutSheetIgnoreMargins, (QLatin1String("layout/sheetIgnoreMargins"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutTileIgnoreMargins, (QLatin1String("layout/tileIgnoreMargins"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutTileShowTiles, (QLatin1String("layout/tileShowTiles"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutTileShowWatermark, (QLatin1String("layout/tileShowWatermark"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutWarningPiecesSuperposition, // NOLINT
                          (QLatin1String("layout/warningPiecesSuperposition")))
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutStickyEdges, (QLatin1String("layout/stickyEdges"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutWarningPiecesOutOfBound, // NOLINT
                          (QLatin1String("layout/warningPiecesOutOfBound")))
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutFollowGrainline, (QLatin1String("layout/followGrainline"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutPieceGap, (QLatin1String("layout/pieceGap"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutExportFormat, (QLatin1String("layout/exportFormat"))) // NOLINT
Q_GLOBAL_STATIC_WITH_ARGS(const QString, settingLayoutLineWidth, (QLatin1String("layout/lineWidth"))) // NOLINT

int cachedLineWidth = -1;
}  // namespace

Q_DECLARE_METATYPE(QMarginsF)

//---------------------------------------------------------------------------------------------------------------------
VPSettings::VPSettings(Format format, Scope scope, const QString &organization, const QString &application,
                                 QObject *parent)
    : VCommonSettings(format, scope, organization, application, parent)
{
    qRegisterMetaTypeStreamOperators<QMarginsF>("QMarginsF");
}

//---------------------------------------------------------------------------------------------------------------------
VPSettings::VPSettings(const QString &fileName, QSettings::Format format, QObject *parent)
    : VCommonSettings(fileName, format, parent)
{
    qRegisterMetaTypeStreamOperators<QMarginsF>("QMarginsF");
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::IsDockWidgetPropertiesActive() const -> bool
{
    return value(*settingDockWidgetPropertiesActive, GetDefDockWidgetPropertiesActive()).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetDefDockWidgetPropertiesActive() -> bool
{
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetDockWidgetPropertiesActive(bool value)
{
    setValue(*settingDockWidgetPropertiesActive, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::IsDockWidgetPropertiesContentsActive() const -> bool
{
    return value(*settingDockWidgetPropertiesActive, GetDefDockWidgetPropertiesActive()).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetDefDockWidgetPropertiesContentsActive() -> bool
{
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetDockWidgetPropertiesContentsActive(bool value)
{
    setValue(*settingDockPropertiesContentsActive, value);
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutUnit(Unit unit)
{
    setValue(*settingLayoutUnit, UnitsToStr(unit));
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::LayoutUnit() const -> Unit
{
    return StrToUnits(value(*settingLayoutUnit, QString()).toString());
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutSheetPaperWidth(qreal width)
{
    setValue(*settingLayoutSheetPaperWidth, width);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutSheetPaperWidth() const -> qreal
{
    return ValueOrDef<qreal>(*this, *settingLayoutSheetPaperWidth, UnitConvertor(841/*A0*/, Unit::Mm, Unit::Px));
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutSheetPaperHeight(qreal height)
{
    setValue(*settingLayoutSheetPaperHeight, height);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutSheetPaperHeight() const -> qreal
{
    return ValueOrDef<qreal>(*this, *settingLayoutSheetPaperHeight, UnitConvertor(1189/*A0*/, Unit::Mm, Unit::Px));
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutTilePaperWidth(qreal width)
{
    setValue(*settingLayoutTilePaperWidth, width);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutTilePaperWidth() const -> qreal
{
    return ValueOrDef<qreal>(*this, *settingLayoutTilePaperWidth, UnitConvertor(210/*A4*/, Unit::Mm, Unit::Px));
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutTilePaperHeight(qreal height)
{
    setValue(*settingLayoutTilePaperHeight, height);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutTilePaperHeight() const -> qreal
{
    return ValueOrDef<qreal>(*this, *settingLayoutTilePaperHeight, UnitConvertor(297/*A4*/, Unit::Mm, Unit::Px));
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutSheetMargins(const QMarginsF &margins)
{
    setValue(*settingLayoutSheetMargins, QVariant::fromValue(margins));
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutSheetMargins() const -> QMarginsF
{
    return ValueOrDef<QMarginsF>(*this, *settingLayoutSheetMargins, QMarginsF());
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutTileMargins(const QMarginsF &margins)
{
    setValue(*settingLayoutTileMargins, QVariant::fromValue(margins));
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutTileMargins() const -> QMarginsF
{
    return ValueOrDef<QMarginsF>(*this, *settingLayoutTileMargins, QMarginsF());
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutSheetIgnoreMargins(bool value)
{
    setValue(*settingLayoutSheetIgnoreMargins, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutSheetIgnoreMargins() const -> bool
{
    return value(*settingLayoutSheetIgnoreMargins, false).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutTileIgnoreMargins(bool value)
{
    setValue(*settingLayoutTileIgnoreMargins, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutTileIgnoreMargins() const -> bool
{
    return value(*settingLayoutTileIgnoreMargins, false).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutTileShowTiles(bool value)
{
    setValue(*settingLayoutTileShowTiles, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutTileShowTiles() const -> bool
{
    return value(*settingLayoutTileShowTiles, true).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutTileShowWatermark(bool value)
{
    setValue(*settingLayoutTileShowWatermark, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutTileShowWatermark() const -> bool
{
    return value(*settingLayoutTileShowWatermark, false).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutWarningPiecesSuperposition(bool value)
{
    setValue(*settingLayoutWarningPiecesSuperposition, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutWarningPiecesSuperposition() const -> bool
{
    return value(*settingLayoutWarningPiecesSuperposition, true).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutStickyEdges(bool value)
{
    setValue(*settingLayoutStickyEdges, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutStickyEdges() const -> bool
{
    return value(*settingLayoutStickyEdges, true).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutWarningPiecesOutOfBound(bool value)
{
    setValue(*settingLayoutWarningPiecesOutOfBound, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutWarningPiecesOutOfBound() const -> bool
{
    return value(*settingLayoutWarningPiecesOutOfBound, true).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutFollowGrainline(bool value)
{
    setValue(*settingLayoutFollowGrainline, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutFollowGrainline() const -> bool
{
    return value(*settingLayoutFollowGrainline, false).toBool();
}

//---------------------------------------------------------------------------------------------------------------------
qreal VPSettings::GetMaxLayoutPieceGap()
{
    return UnitConvertor(50, Unit::Cm, Unit::Px);
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutPieceGap(qreal value)
{
    setValue(*settingLayoutPieceGap, value);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutPieceGap() const -> qreal
{
    return ValueOrDef<qreal>(*this, *settingLayoutPieceGap, UnitConvertor(5, Unit::Mm, Unit::Px));
}

//---------------------------------------------------------------------------------------------------------------------
qint8 VPSettings::GetLayoutExportFormat() const
{
    return qvariant_cast<qint8>(value(*settingLayoutExportFormat, 0));
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutExportFormat(qint8 format)
{
    setValue(*settingLayoutExportFormat, format);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPSettings::GetLayoutLineWidth() const -> int
{
    if (cachedLineWidth == -1)
    {
        cachedLineWidth = qvariant_cast<int>(value(*settingLayoutLineWidth, 3));
    }

    return cachedLineWidth;
}

//---------------------------------------------------------------------------------------------------------------------
void VPSettings::SetLayoutLineWidth(int width)
{
    cachedLineWidth = qBound(1, width, 10);
    setValue(*settingLayoutLineWidth, cachedLineWidth);
}
