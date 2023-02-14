/************************************************************************
 **
 **  @file   vpropertydef.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   10 2, 2023
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2023 Valentina project
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

#include "vpropertydef.h"

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocalePositiveSign(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.positiveSign();
    if (sign.size() == 1)
    {
        return sign.front();
    }

    return QLocale::c().positiveSign().front();
#else
    return locale.positiveSign();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleNegativeSign(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.negativeSign();
    if (sign.size() == 1)
    {
        return sign.front();
    }

    return QLocale::c().negativeSign().front();
#else
    return locale.negativeSign();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign0(const QLocale &locale)
{
    const QString sign = locale.toString(0);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'0'};
#else
    return QChar('0');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign1(const QLocale &locale)
{
    const QString sign = locale.toString(1);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'1'};
#else
    return QChar('1');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign2(const QLocale &locale)
{
    const QString sign = locale.toString(2);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'2'};
#else
    return QChar('2');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign3(const QLocale &locale)
{
    const QString sign = locale.toString(3);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'3'};
#else
    return QChar('3');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign4(const QLocale &locale)
{
    const QString sign = locale.toString(4);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'4'};
#else
    return QChar('4');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign5(const QLocale &locale)
{
    const QString sign = locale.toString(5);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'5'};
#else
    return QChar('5');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign6(const QLocale &locale)
{
    const QString sign = locale.toString(6);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'6'};
#else
    return QChar('6');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign7(const QLocale &locale)
{
    const QString sign = locale.toString(7);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'7'};
#else
    return QChar('7');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign8(const QLocale &locale)
{
    const QString sign = locale.toString(8);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'8'};
#else
    return QChar('8');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleSign9(const QLocale &locale)
{
    const QString sign = locale.toString(9);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'9'};
#else
    return QChar('9');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleExpUpper(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.exponential();
    if (sign.size() == 1)
    {
        return sign.front().toUpper();
    }

    return QLocale::c().exponential().front().toUpper();
#else
    return locale.exponential().toUpper();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleExpLower(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.exponential();
    if (sign.size() == 1)
    {
        return sign.front().toLower();
    }

    return QLocale::c().exponential().front().toLower();
#else
    return locale.exponential().toLower();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleDecimalPoint(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.decimalPoint();
    if (sign.size() == 1)
    {
        return sign.front();
    }

    return QLocale::c().decimalPoint().front();
#else
    return locale.decimalPoint();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar VPELocaleGroupSeparator(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.groupSeparator();
    if (sign.size() == 1)
    {
        return sign.front();
    }

    return QLocale::c().groupSeparator().front();
#else
    return locale.groupSeparator();
#endif
}
