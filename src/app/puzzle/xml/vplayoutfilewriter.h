/************************************************************************
 **
 **  @file   vplayoutfilewriter.h
 **  @author Ronan Le Tiec
 **  @date   18 4, 2020
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
 ** *************************************************************************/

#ifndef VPLAYOUTFILEWRITER_H
#define VPLAYOUTFILEWRITER_H

#include <QLocale>
#include <QXmlStreamWriter>
#include <QCoreApplication>
#include <functional>

#include "../vmisc/literals.h"
#include "../layout/layoutdef.h"

class VPLayout;
class VPSheet;
class VPPieceList;
class VPPiece;
class QFile;
class QMarginsF;
class VTextManager;

class VPLayoutFileWriter : public QXmlStreamWriter
{
    Q_DECLARE_TR_FUNCTIONS(VPLayoutFileWriter)
public:
    VPLayoutFileWriter()= default;
    ~VPLayoutFileWriter()= default;

    void WriteFile(const VPLayoutPtr &layout, QIODevice *file);

private:
    void WriteLayout(const VPLayoutPtr &layout);
    void WriteLayoutProperties(const VPLayoutPtr &layout);
    void WriteSheets(const VPLayoutPtr &layout);
    void WriteSheet(const VPSheetPtr &sheet);
    void WriteTiles(const VPLayoutPtr &layout);
    void WritePieceList(const QList<VPPiecePtr> &list, const QString &tagName);
    void WritePiece(const VPPiecePtr &piece);
    void WriteLabel(const QVector<QPointF> &labelShape, const VTextManager &tm, const QString &tagName);
    void WriteLabelLines(const VTextManager &tm);

    void WriteMargins(const QMarginsF &margins);
    void WriteSize(QSizeF size);

    template <typename T>
    void SetAttribute(const QString &name, const T &value);

    template <size_t N>
    void SetAttribute(const QString &name, const char (&value)[N]);

    template <typename T>
    void SetAttributeOrRemoveIf(const QString &name, const T &value,
                                const std::function<bool(const T&)> &removeCondition);
};

//---------------------------------------------------------------------------------------------------------------------
template<typename T>
void VPLayoutFileWriter::SetAttribute(const QString &name, const T &value)
{
    // See specification for xs:decimal
    const QLocale locale = QLocale::c();
    writeAttribute(name, locale.toString(value).remove(locale.groupSeparator()));
}

//---------------------------------------------------------------------------------------------------------------------
template <>
inline void VPLayoutFileWriter::SetAttribute<QString>(const QString &name, const QString &value)
{
    writeAttribute(name, value);
}

//---------------------------------------------------------------------------------------------------------------------
template <>
inline void VPLayoutFileWriter::SetAttribute<QChar>(const QString &name, const QChar &value)
{
    writeAttribute(name, value);
}

//---------------------------------------------------------------------------------------------------------------------
template <>
inline void VPLayoutFileWriter::SetAttribute<bool>(const QString &name, const bool &value)
{
    writeAttribute(name, value ? trueStr : falseStr);
}

//---------------------------------------------------------------------------------------------------------------------
template <size_t N>
inline void VPLayoutFileWriter::SetAttribute(const QString &name, const char (&value)[N])
{
    writeAttribute(name, QString(value));
}

//---------------------------------------------------------------------------------------------------------------------
template <typename T>
inline void VPLayoutFileWriter::SetAttributeOrRemoveIf(const QString &name, const T &value,
                                                       const std::function<bool(const T&)> &removeCondition)
{
    if (not removeCondition(value))
    {
        SetAttribute(name, value);
    }
}

#endif // VPLAYOUTFILEWRITER_H
