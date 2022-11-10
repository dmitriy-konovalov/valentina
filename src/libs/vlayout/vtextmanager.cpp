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

#include <QDate>
#include <QFileInfo>
#include <QFontMetrics>
#include <QLatin1String>
#include <QRegularExpression>
#include <QApplication>
#include <QDebug>
#include <QFlags> // QFlags<Qt::Alignment>
#include <QtMath>

#include "../ifc/xml/vabstractpattern.h"
#include "../vpatterndb/floatItemData/vpiecelabeldata.h"
#include "../vmisc/vabstractvalapplication.h"
#include "../vpatterndb/vcontainer.h"
#include "../vpatterndb/calculator.h"
#include "../vpatterndb/variables/vmeasurement.h"
#include "vtextmanager.h"

const quint32 TextLine::streamHeader = 0xA3881E49; // CRC-32Q string "TextLine"
const quint16 TextLine::classVersion = 1;

// Friend functions
//---------------------------------------------------------------------------------------------------------------------
auto operator<<(QDataStream &dataStream, const TextLine &data) -> QDataStream&
{
    dataStream << TextLine::streamHeader << TextLine::classVersion;

    // Added in classVersion = 1
    dataStream << data.m_qsText;
    dataStream << data.m_iFontSize;
    dataStream << data.m_bold;
    dataStream << data.m_italic;
    dataStream << data.m_eAlign;

    // Added in classVersion = 2

    return dataStream;
}

//---------------------------------------------------------------------------------------------------------------------
auto operator>>(QDataStream &dataStream, TextLine &data) -> QDataStream&
{
    quint32 actualStreamHeader = 0;
    dataStream >> actualStreamHeader;

    if (actualStreamHeader != TextLine::streamHeader)
    {
        QString message = QCoreApplication::tr("TextLine prefix mismatch error: actualStreamHeader = 0x%1 and "
                                               "streamHeader = 0x%2")
                .arg(actualStreamHeader, 8, 0x10, QChar('0'))
                .arg(TextLine::streamHeader, 8, 0x10, QChar('0'));
        throw VException(message);
    }

    quint16 actualClassVersion = 0;
    dataStream >> actualClassVersion;

    if (actualClassVersion > TextLine::classVersion)
    {
        QString message = QCoreApplication::tr("TextLine compatibility error: actualClassVersion = %1 and "
                                               "classVersion = %2")
                .arg(actualClassVersion).arg(TextLine::classVersion);
        throw VException(message);
    }

    dataStream >> data.m_qsText;
    dataStream >> data.m_iFontSize;
    dataStream >> data.m_bold;
    dataStream >> data.m_italic;
    dataStream >> data.m_eAlign;

//    if (actualClassVersion >= 2)
//    {

//    }

    return dataStream;
}

QVector<TextLine> VTextManager::m_patternLabelLines = QVector<TextLine>();
const quint32 VTextManager::streamHeader = 0x47E6A9EE; // CRC-32Q string "VTextManager"
const quint16 VTextManager::classVersion = 1;

// Friend functions
//---------------------------------------------------------------------------------------------------------------------
auto operator<<(QDataStream &dataStream, const VTextManager &data) -> QDataStream&
{
    dataStream << VTextManager::streamHeader << VTextManager::classVersion;

    // Added in classVersion = 1
    dataStream << data.m_font;
    dataStream << data.m_liLines;

    // Added in classVersion = 2

    return dataStream;
}

//---------------------------------------------------------------------------------------------------------------------
auto operator>>(QDataStream &dataStream, VTextManager &data) -> QDataStream&
{
    quint32 actualStreamHeader = 0;
    dataStream >> actualStreamHeader;

    if (actualStreamHeader != VTextManager::streamHeader)
    {
        QString message = QCoreApplication::tr("VTextManager prefix mismatch error: actualStreamHeader = 0x%1 and "
                                               "streamHeader = 0x%2")
                .arg(actualStreamHeader, 8, 0x10, QChar('0'))
                .arg(VTextManager::streamHeader, 8, 0x10, QChar('0'));
        throw VException(message);
    }

    quint16 actualClassVersion = 0;
    dataStream >> actualClassVersion;

    if (actualClassVersion > VTextManager::classVersion)
    {
        QString message = QCoreApplication::tr("VTextManager compatibility error: actualClassVersion = %1 and "
                                               "classVersion = %2")
                .arg(actualClassVersion).arg(VTextManager::classVersion);
        throw VException(message);
    }

    dataStream >> data.m_font;
    dataStream >> data.m_liLines;

//    if (actualClassVersion >= 2)
//    {

//    }

    return dataStream;
}

namespace
{

//---------------------------------------------------------------------------------------------------------------------
auto PreparePlaceholders(const VAbstractPattern *doc, const VContainer *data) -> QMap<QString, QString>
{
    SCASSERT(doc != nullptr)
    SCASSERT(data != nullptr)

    QMap<QString, QString> placeholders;

    // Pattern tags
    QLocale locale(VAbstractApplication::VApp()->Settings()->GetLocale());

    const QString date = locale.toString(QDate::currentDate(), doc->GetLabelDateFormat());
    placeholders.insert(pl_date, date);

    const QString time = locale.toString(QTime::currentTime(), doc->GetLabelTimeFormat());
    placeholders.insert(pl_time, time);

    placeholders.insert(pl_patternName, doc->GetPatternName());
    placeholders.insert(pl_patternNumber, doc->GetPatternNumber());
    placeholders.insert(pl_author, doc->GetCompanyName());

    placeholders.insert(pl_mUnits, UnitsToStr(VAbstractValApplication::VApp()->MeasurementsUnits(), true));
    placeholders.insert(pl_pUnits, UnitsToStr(VAbstractValApplication::VApp()->patternUnits(), true));
    placeholders.insert(pl_mSizeUnits, UnitsToStr(VAbstractValApplication::VApp()->DimensionSizeUnits(), true));

    if (VAbstractValApplication::VApp()->GetMeasurementsType() == MeasurementsType::Individual)
    {
        placeholders.insert(pl_customer, VAbstractValApplication::VApp()->GetCustomerName());

        const QString birthDate = locale.toString(VAbstractValApplication::VApp()->GetCustomerBirthDate(),
                                                  doc->GetLabelDateFormat());
        placeholders.insert(pl_birthDate, birthDate);

        placeholders.insert(pl_email, VAbstractValApplication::VApp()->CustomerEmail());
    }
    else
    {
        placeholders.insert(pl_customer, doc->GetCustomerName());

        const QString birthDate = locale.toString(doc->GetCustomerBirthDate(), doc->GetLabelDateFormat());
        placeholders.insert(pl_birthDate, birthDate);

        placeholders.insert(pl_email, doc->GetCustomerEmail());
    }

    placeholders.insert(pl_pExt, QStringLiteral("val"));
    placeholders.insert(pl_pFileName, QFileInfo(VAbstractValApplication::VApp()->GetPatternPath()).baseName());
    placeholders.insert(pl_mFileName, QFileInfo(doc->MPath()).baseName());

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

        label = VAbstractValApplication::VApp()->GetDimensionSizeLabel();
        placeholders.insert(pl_sizeLabel, not label.isEmpty() ? label : sizeValue);

        label = VAbstractValApplication::VApp()->GetDimensionHipLabel();
        placeholders.insert(pl_hipLabel, not label.isEmpty() ? label : hipValue);

        label = VAbstractValApplication::VApp()->GetDimensionWaistLabel();
        placeholders.insert(pl_waistLabel, not label.isEmpty() ? label : waistValue);
    }

    placeholders.insert(pl_mExt, VAbstractValApplication::VApp()->GetMeasurementsType() == MeasurementsType::Multisize
                        ? QStringLiteral("vst") : QStringLiteral("vit"));

    const QMap<int, QString> materials = doc->GetPatternMaterials();
    for (int i = 0; i < userMaterialPlaceholdersQuantity; ++i)
    {
        const QString number = QString::number(i + 1);

        QString value;
        if (materials.contains(i + 1))
        {
            value = materials.value(i + 1);
        }

        placeholders.insert(pl_userMaterial + number, value);
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

    {
        const QVector<VFinalMeasurement> measurements = doc->GetFinalMeasurements();
        const VContainer completeData = doc->GetCompleteData();

        for (int i=0; i < measurements.size(); ++i)
        {
            const VFinalMeasurement &m = measurements.at(i);

            try
            {
                QScopedPointer<Calculator> cal(new Calculator());
                const qreal result = cal->EvalFormula(completeData.DataVariables(), m.formula);

                placeholders.insert(pl_finalMeasurement + m.name, QString::number(result));
            }
            catch (qmu::QmuParserError &e)
            {
                const QString errorMsg = QObject::tr("Failed to prepare final measurement placeholder. Parser error at "
                                                     "line %1: %2.").arg(i+1).arg(e.GetMsg());
                VAbstractApplication::VApp()->IsPedantic() ? throw VException(errorMsg) :
                                              qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
            }
        }
    }

    // Piece tags
    placeholders.insert(pl_pLetter, QString());
    placeholders.insert(pl_pAnnotation, QString());
    placeholders.insert(pl_pOrientation, QString());
    placeholders.insert(pl_pRotation, QString());
    placeholders.insert(pl_pTilt, QString());
    placeholders.insert(pl_pFoldPosition, QString());
    placeholders.insert(pl_pName, QString());
    placeholders.insert(pl_pQuantity, QString());
    placeholders.insert(pl_wOnFold, QString());
    placeholders.insert(pl_mFabric, QObject::tr("Fabric"));
    placeholders.insert(pl_mLining, QObject::tr("Lining"));
    placeholders.insert(pl_mInterfacing, QObject::tr("Interfacing"));
    placeholders.insert(pl_mInterlining, QObject::tr("Interlining"));
    placeholders.insert(pl_wCut, QObject::tr("Cut"));

    return placeholders;
}

//---------------------------------------------------------------------------------------------------------------------
void InitPiecePlaceholders(QMap<QString, QString> &placeholders, const QString &name, const VPieceLabelData& data)
{
    placeholders[pl_pLetter] = data.GetLetter();
    placeholders[pl_pAnnotation] = data.GetAnnotation();
    placeholders[pl_pOrientation] = data.GetOrientation();
    placeholders[pl_pRotation] = data.GetRotationWay();
    placeholders[pl_pTilt] = data.GetTilt();
    placeholders[pl_pFoldPosition] = data.GetFoldPosition();
    placeholders[pl_pName] = name;
    placeholders[pl_pQuantity] = QString::number(data.GetQuantity());

    if (data.IsOnFold())
    {
        placeholders[pl_wOnFold] = QObject::tr("on fold");
    }
}

//---------------------------------------------------------------------------------------------------------------------
auto ReplacePlaceholders(const QMap<QString, QString> &placeholders, QString line) -> QString
{
    QChar per('%');

    auto TestDimension = [per, placeholders, line](const QString &placeholder, const QString &errorMsg)
    {
        if (line.contains(per+placeholder+per) && placeholders.value(placeholder) == QChar('0'))
        {
            VAbstractApplication::VApp()->IsPedantic() ? throw VException(errorMsg) :
                                              qWarning() << VAbstractValApplication::warningMessageSignature + errorMsg;
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
auto PrepareLines(const QVector<VLabelTemplateLine> &lines) -> QVector<TextLine>
{
    QVector<TextLine> textLines;

    for (auto &line : lines)
    {
        if (not line.line.isEmpty())
        {
            TextLine tl;
            tl.m_qsText = line.line;
            tl.m_eAlign = static_cast<Qt::Alignment>(line.alignment);
            tl.m_iFontSize = line.fontSizeIncrement;
            tl.m_bold = line.bold;
            tl.m_italic = line.italic;

            textLines << tl;
        }
    }

    return textLines;
}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VTextManager::VTextManager constructor
 */
VTextManager::VTextManager()
     : m_font(), m_liLines()
{}

//---------------------------------------------------------------------------------------------------------------------
VTextManager::VTextManager(const VTextManager &text)
    : m_font(text.GetFont()), m_liLines(text.GetAllSourceLines())
{}

//---------------------------------------------------------------------------------------------------------------------
VTextManager &VTextManager::operator=(const VTextManager &text)
{
    if ( &text == this )
    {
        return *this;
    }
    m_font = text.GetFont();
    m_liLines = text.GetAllSourceLines();
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetSpacing returns the vertical spacing between the lines
 * @return spacing
 */
auto VTextManager::GetSpacing() const -> int
{
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetFont set the text base font
 * @param font text base font
 */
void VTextManager::SetFont(const QFont& font)
{
    m_font = font;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetFont returns the text base font
 * @return text base font
 */
auto VTextManager::GetFont() const -> const QFont&
{
    return m_font;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetFontSize sets the font size
 * @param iFS font size in pixels
 */
void VTextManager::SetFontSize(int iFS)
{
    iFS < MIN_FONT_SIZE ? m_font.setPixelSize(MIN_FONT_SIZE) : m_font.setPixelSize(iFS);
}

//---------------------------------------------------------------------------------------------------------------------
auto VTextManager::GetAllSourceLines() const -> QVector<TextLine>
{
    return m_liLines;
}

//---------------------------------------------------------------------------------------------------------------------
void VTextManager::SetAllSourceLines(const QVector<TextLine> &lines)
{
    m_liLines = lines;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VTextManager::GetSourceLinesCount returns the number of input text lines
 * @return number of text lines that were added to the list by calling AddLine
 */
auto VTextManager::GetSourceLinesCount() const -> int
{
    return m_liLines.count();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VTextManager::GetSourceLine returns the reference to i-th text line
 * @param i index of the requested line
 * @return reference to the requested TextLine object
 */
auto VTextManager::GetSourceLine(int i) const -> const TextLine&
{
    Q_ASSERT(i >= 0);
    Q_ASSERT(i < m_liLines.count());
    return m_liLines.at(i);
}

//---------------------------------------------------------------------------------------------------------------------
auto VTextManager::MaxLineWidth(int width) const -> int
{
    int maxWidth = 0;
    for (int i = 0; i < m_liLines.count(); ++i)
    {
        const TextLine& tl = m_liLines.at(i);

        QFont fnt = m_font;
        fnt.setPixelSize(fnt.pixelSize() + tl.m_iFontSize);
        fnt.setBold(tl.m_bold);
        fnt.setItalic(tl.m_italic);

        QFontMetrics fm(fnt);

        QString qsText = tl.m_qsText;

        if (TextWidth(fm, qsText) > width)
        {
            qsText = fm.elidedText(qsText, Qt::ElideMiddle, width);
        }

        maxWidth = qMax(TextWidth(fm, qsText), maxWidth);
    }

    return maxWidth;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VTextManager::FitFontSize sets the font size just big enough, so that the text fits into rectangle of
 * size (fW, fH)
 * @param fW rectangle width
 * @param fH rectangle height
 */
void VTextManager::FitFontSize(qreal fW, qreal fH)
{
    int iFS = 0;
    if (GetSourceLinesCount() > 0)
    {//division by zero
        iFS = 3*qFloor(fH/GetSourceLinesCount())/4;
    }

    if (iFS < MIN_FONT_SIZE)
    {
        iFS = MIN_FONT_SIZE;
    }

    // get ratio between char width and height

    int iMaxLen = 0;
    TextLine maxLine;
    QFont fnt;
    for (int i = 0; i < GetSourceLinesCount(); ++i)
    {
        const TextLine& tl = GetSourceLine(i);
        fnt = m_font;
        fnt.setPixelSize(iFS + tl.m_iFontSize);
        fnt.setBold(tl.m_bold);
        fnt.setItalic(tl.m_italic);
        QFontMetrics fm(fnt);
        const int iTW = TextWidth(fm, tl.m_qsText);
        if (iTW > iMaxLen)
        {
            iMaxLen = iTW;
            maxLine = tl;
        }
    }
    if (iMaxLen > fW)
    {
        QFont fnt = m_font;
        fnt.setBold(maxLine.m_bold);
        fnt.setItalic(maxLine.m_italic);

        int lineLength = 0;
        do
        {
            --iFS;
            fnt.setPixelSize(iFS + maxLine.m_iFontSize);
            QFontMetrics fm(fnt);
            lineLength = TextWidth(fm, maxLine.m_qsText);
        }
        while (lineLength > fW && iFS > MIN_FONT_SIZE);
    }
    SetFontSize(iFS);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VTextManager::Update updates the text lines with detail data
 * @param qsName detail name
 * @param data reference to the detail data
 */
void VTextManager::Update(const QString& qsName, const VPieceLabelData& data, const VContainer *pattern)
{
    m_liLines.clear();

    QMap<QString, QString> placeholders =
            PreparePlaceholders(VAbstractValApplication::VApp()->getCurrentDocument(), pattern);
    InitPiecePlaceholders(placeholders, qsName, data);

    QVector<VLabelTemplateLine> lines = data.GetLabelTemplate();

    for (int i=0; i<lines.size(); ++i)
    {
        lines[i].line = ReplacePlaceholders(placeholders, lines.at(i).line);
    }

    m_liLines = PrepareLines(lines);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VTextManager::Update updates the text lines with pattern info
 * @param pDoc pointer to the abstract pattern object
 */
void VTextManager::Update(VAbstractPattern *pDoc, const VContainer *pattern)
{
    m_liLines.clear();

    if (m_patternLabelLines.isEmpty() || pDoc->GetPatternWasChanged())
    {
        QVector<VLabelTemplateLine> lines = pDoc->GetPatternLabelTemplate();
        if (lines.isEmpty() && m_patternLabelLines.isEmpty())
        {
            return; // Nothing to parse
        }

        const QMap<QString, QString> placeholders = PreparePlaceholders(pDoc, pattern);

        for (int i=0; i<lines.size(); ++i)
        {
            lines[i].line = ReplacePlaceholders(placeholders, lines.at(i).line);
        }

        pDoc->SetPatternWasChanged(false);
        m_patternLabelLines = PrepareLines(lines);
    }

    m_liLines = m_patternLabelLines;
}
