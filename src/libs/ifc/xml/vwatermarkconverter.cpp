/************************************************************************
 **
 **  @file   vwatermarkconverter.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   26 12, 2019
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2019 Valentina project
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
#include "vwatermarkconverter.h"

#include <QtGlobal>

/*
 * Version rules:
 * 1. Version have three parts "major.minor.patch";
 * 2. major part only for stable releases;
 * 3. minor - 10 or more patch changes, or one big change;
 * 4. patch - little change.
 */

const QString VWatermarkConverter::WatermarkMinVerStr = QStringLiteral("1.0.0");
const QString VWatermarkConverter::WatermarkMaxVerStr = QStringLiteral("1.1.0");
const QString VWatermarkConverter::CurrentSchema      = QStringLiteral("://schema/watermark/v1.1.0.xsd");

//VWatermarkConverter::WatermarkMinVer; // <== DON'T FORGET TO UPDATE TOO!!!!
//VWatermarkConverter::WatermarkMaxVer; // <== DON'T FORGET TO UPDATE TOO!!!!

//---------------------------------------------------------------------------------------------------------------------
VWatermarkConverter::VWatermarkConverter(const QString &fileName)
    : VAbstractConverter(fileName)
{
    m_ver = GetFormatVersion(GetFormatVersionStr());
    ValidateInputFile(CurrentSchema);
}

//---------------------------------------------------------------------------------------------------------------------
auto VWatermarkConverter::XSDSchemas() -> QHash<unsigned int, QString>
{
    static auto schemas = QHash <unsigned, QString>
    {
        std::make_pair(FormatVersion(1, 0, 0), QStringLiteral("://schema/watermark/v1.0.0.xsd")),
        std::make_pair(FormatVersion(1, 1, 0), CurrentSchema)
    };

    return schemas;
}

//---------------------------------------------------------------------------------------------------------------------
auto VWatermarkConverter::MinVer() const -> unsigned
{
    return WatermarkMinVer;
}

//---------------------------------------------------------------------------------------------------------------------
auto VWatermarkConverter::MaxVer() const -> unsigned
{
    return WatermarkMaxVer;
}

//---------------------------------------------------------------------------------------------------------------------
auto VWatermarkConverter::MinVerStr() const -> QString
{
    return WatermarkMinVerStr;
}

//---------------------------------------------------------------------------------------------------------------------
auto VWatermarkConverter::MaxVerStr() const -> QString
{
    return WatermarkMaxVerStr;
}

//---------------------------------------------------------------------------------------------------------------------
void VWatermarkConverter::ApplyPatches()
{
    switch (m_ver)
    {
        case (FormatVersion(1, 0, 0)):
            ToV1_1_0();
            ValidateXML(XSDSchema(FormatVersion(1, 1, 0)));
            Q_FALLTHROUGH();
        case (FormatVersion(1, 1, 0)):
            break;
        default:
            InvalidVersion(m_ver);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VWatermarkConverter::DowngradeToCurrentMaxVersion()
{
    SetVersion(WatermarkMaxVerStr);
    Save();
}

//---------------------------------------------------------------------------------------------------------------------
auto VWatermarkConverter::Schemas() const -> QHash<unsigned int, QString>
{
    return XSDSchemas();
}

//---------------------------------------------------------------------------------------------------------------------
void VWatermarkConverter::ToV1_1_0()
{
    // TODO. Delete if minimal supported version is 1.1.0
    Q_STATIC_ASSERT_X(VWatermarkConverter::WatermarkMinVer < FormatVersion(1, 1, 0),
                      "Time to refactor the code.");

    SetVersion(QStringLiteral("1.1.0"));
    Save();
}
