/************************************************************************
 **
 **  @file
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   9 11, 2016
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

#ifndef VABSTRACTPIECE_P_H
#define VABSTRACTPIECE_P_H

#include <QSharedData>
#include <QString>
#include <QCoreApplication>
#include <QUuid>

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
#include "../vmisc/diagnostic.h"
#endif // QT_VERSION < QT_VERSION_CHECK(5, 5, 0)

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#   include "../vmisc/vdatastreamenum.h"
#endif

#include "../vmisc/defglobal.h"
#include "../ifc/exception/vexception.h"

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Weffc++")
QT_WARNING_DISABLE_GCC("-Wnon-virtual-dtor")

class VAbstractPieceData : public QSharedData
{
    Q_DECLARE_TR_FUNCTIONS(VAbstractPieceData) // NOLINT
public:
    VAbstractPieceData() = default;

    VAbstractPieceData(const VAbstractPieceData &piece) = default;

    ~VAbstractPieceData() = default;

    friend auto operator<<(QDataStream& dataStream, const VAbstractPieceData& piece) -> QDataStream&;
    friend auto operator>>(QDataStream& dataStream, VAbstractPieceData& piece) -> QDataStream&;

    QString m_name{tr("Detail")}; // NOLINT (misc-non-private-member-variables-in-classes)
    /** @brief forbidFlipping forbid piece be mirrored in a layout. */
    bool    m_forbidFlipping{false}; // NOLINT (misc-non-private-member-variables-in-classes)
    bool    m_forceFlipping{false}; // NOLINT (misc-non-private-member-variables-in-classes)
    bool    m_seamAllowance{false}; // NOLINT (misc-non-private-member-variables-in-classes)
    bool    m_seamAllowanceBuiltIn{false}; // NOLINT (misc-non-private-member-variables-in-classes)
    bool    m_hideMainPath{false}; // NOLINT (misc-non-private-member-variables-in-classes)
    qreal   m_width{0}; // NOLINT (misc-non-private-member-variables-in-classes)
    qreal   m_mx{0}; // NOLINT (misc-non-private-member-variables-in-classes)
    qreal   m_my{0}; // NOLINT (misc-non-private-member-variables-in-classes)
    uint    m_priority{0}; // NOLINT (misc-non-private-member-variables-in-classes)
    QUuid   m_uuid{QUuid::createUuid()}; // NOLINT (misc-non-private-member-variables-in-classes)
    bool    m_onDrawing{false}; // NOLINT (misc-non-private-member-variables-in-classes)

private:
    Q_DISABLE_ASSIGN_MOVE(VAbstractPieceData) // NOLINT

    static constexpr quint32 streamHeader = 0x05CDD73A; // CRC-32Q string "VAbstractPieceData"
    static constexpr quint16 classVersion = 4;
};

QT_WARNING_POP

// See https://stackoverflow.com/a/46719572/3045403
#if __cplusplus < 201703L // C++17
constexpr quint32 VAbstractPieceData::streamHeader;  // NOLINT(readability-redundant-declaration)
constexpr quint16 VAbstractPieceData::classVersion;  // NOLINT(readability-redundant-declaration)
#endif

// Friend functions
//---------------------------------------------------------------------------------------------------------------------
inline auto operator<<(QDataStream &dataStream, const VAbstractPieceData &piece) -> QDataStream &
{
    dataStream << VAbstractPieceData::streamHeader << VAbstractPieceData::classVersion;

    // Added in classVersion = 1
    dataStream << piece.m_name;
    dataStream << piece.m_forbidFlipping;
    dataStream << piece.m_forceFlipping;
    dataStream << piece.m_seamAllowance;
    dataStream << piece.m_seamAllowanceBuiltIn;
    dataStream << piece.m_hideMainPath;
    dataStream << piece.m_width;
    dataStream << piece.m_mx;
    dataStream << piece.m_my;

    // Added in classVersion = 2
    dataStream << piece.m_priority;

    // Added in classVersion = 3
    dataStream << piece.m_uuid;

    // Added in classVersion = 4
    dataStream << piece.m_onDrawing;

    return dataStream;
}

//---------------------------------------------------------------------------------------------------------------------
inline auto operator>>(QDataStream &dataStream, VAbstractPieceData &piece) -> QDataStream &
{
    quint32 actualStreamHeader = 0;
    dataStream >> actualStreamHeader;

    if (actualStreamHeader != VAbstractPieceData::streamHeader)
    {
        QString message = QCoreApplication::tr("VAbstractPieceData prefix mismatch error: actualStreamHeader = 0x%1 "
                                               "and streamHeader = 0x%2")
                .arg(actualStreamHeader, 8, 0x10, QChar('0'))
                .arg(VAbstractPieceData::streamHeader, 8, 0x10, QChar('0'));
        throw VException(message);
    }

    quint16 actualClassVersion = 0;
    dataStream >> actualClassVersion;

    if (actualClassVersion > VAbstractPieceData::classVersion)
    {
        QString message = QCoreApplication::tr("VAbstractPieceData compatibility error: actualClassVersion = %1 and "
                                               "classVersion = %2")
                .arg(actualClassVersion).arg(VAbstractPieceData::classVersion);
        throw VException(message);
    }

    dataStream >> piece.m_name;
    dataStream >> piece.m_forbidFlipping;
    dataStream >> piece.m_forceFlipping;
    dataStream >> piece.m_seamAllowance;
    dataStream >> piece.m_seamAllowanceBuiltIn;
    dataStream >> piece.m_hideMainPath;
    dataStream >> piece.m_width;
    dataStream >> piece.m_mx;
    dataStream >> piece.m_my;

    if (actualClassVersion >= 2)
    {
        dataStream >> piece.m_priority;
    }

    if (actualClassVersion >= 3)
    {
        dataStream >> piece.m_uuid;
    }

    if (actualClassVersion >= 4)
    {
        dataStream >> piece.m_onDrawing;
    }

    return dataStream;
}

#endif // VABSTRACTPIECE_P_H

