/************************************************************************
 **
 **  @file   vpatternpiecedata.cpp
 **  @author Bojan Kverh
 **  @date   July 19, 2016
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

#ifndef VTEXTMANAGER_H
#define VTEXTMANAGER_H

#include <QCoreApplication>
#include <QDate>
#include <QFont>
#include <QList>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QtGlobal>

#include "../vmisc/def.h"
#include "../vmisc/defglobal.h"

class VPieceLabelData;
class VAbstractPattern;
class VContainer;
class VSvgFont;

/**
 * @brief The TextLine struct holds the information about one text line
 */
struct TextLine
{
    QString m_qsText{};
    int m_iFontSize{0}; // 0 means default
    bool m_bold{false};
    bool m_italic{false};
    Qt::Alignment m_eAlign{Qt::AlignCenter};

    friend auto operator<<(QDataStream &dataStream, const TextLine &data) -> QDataStream &;
    friend auto operator>>(QDataStream &dataStream, TextLine &data) -> QDataStream &;

private:
    static const quint32 streamHeader;
    static const quint16 classVersion;
};

/**
 * @brief The VTextManager class this class is used to determine whether a collection of
 * text lines can fit into specified bounding box and with what font size
 */
class VTextManager
{
    Q_DECLARE_TR_FUNCTIONS(VTextManager) // NOLINT

public:
    DEF_CONSTRUCTOR(VTextManager)
    virtual ~VTextManager() = default;

    VTextManager(const VTextManager &text) = default;
    auto operator=(const VTextManager &text) -> VTextManager & = default;

#ifdef Q_COMPILER_RVALUE_REFS
    VTextManager(VTextManager &&text) = default;
    auto operator=(VTextManager &&text) -> VTextManager & = default;
#endif

    virtual auto GetSpacing() const -> int;

    void SetFont(const QFont &font);
    auto GetFont() const -> const QFont &;

    void SetSVGFontFamily(const QString &fontFamily);
    auto GetSVGFontFamily() const -> QString;

    void SetSVGFontPointSize(int pointSize);
    auto GetSVGFontPointSize() const -> int;

    void SetFontSize(int iFS);

    auto GetAllSourceLines() const -> QVector<TextLine>;
    void SetAllSourceLines(const QVector<TextLine> &lines);
    auto GetSourceLinesCount() const -> vsizetype;
    auto GetSourceLine(vsizetype i) const -> const TextLine &;

    auto GetLabelSourceLines(int width, const QFont &font) const -> QVector<TextLine>;
    auto GetLabelSourceLines(int width, const VSvgFont &font, qreal penWidth) const -> QVector<TextLine>;

    auto MaxLineWidthOutlineFont(int width) const -> int;
    auto MaxLineWidthSVGFont(int width, qreal penWidth) const -> int;

    void Update(const QString &qsName, const VPieceLabelData &data, const VContainer *pattern);
    void Update(VAbstractPattern *pDoc, const VContainer *pattern);

    friend auto operator<<(QDataStream &dataStream, const VTextManager &data) -> QDataStream &;
    friend auto operator>>(QDataStream &dataStream, VTextManager &data) -> QDataStream &;

private:
    QFont m_font{};
    QString m_svgFontFamily{};
    int m_svgFontPointSize{-1};
    QVector<TextLine> m_liLines{};

    static const quint32 streamHeader;
    static const quint16 classVersion;

    auto BreakTextIntoLines(const QString &text, const QFont &font, int maxWidth) const -> QStringList;
    auto BreakTextIntoLines(const QString &text, const VSvgFont &font, int maxWidth, qreal penWidth) const
        -> QStringList;
};

#endif // VTEXTMANAGER_H
