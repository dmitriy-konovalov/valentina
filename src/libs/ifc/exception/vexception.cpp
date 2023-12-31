/************************************************************************
 **
 **  @file   vexception.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
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

#include "vexception.h"

#include <QByteArray>
#include <QtGlobal>

#include "../ifcdef.h"

QT_WARNING_PUSH
QT_WARNING_DISABLE_CLANG("-Wmissing-prototypes")
QT_WARNING_DISABLE_INTEL(1418)

//Q_LOGGING_CATEGORY(vExcep, "v.excep") // NOLINT //Commented because don't use now

QT_WARNING_POP

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VException constructor exception
 * @param error string with error
 */
VException::VException(const QString &error) V_NOEXCEPT_EXPR (true)
    : error(error)
{
    Q_ASSERT_X(not error.isEmpty(), Q_FUNC_INFO, "Error message is empty");
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VException copy constructor
 * @param e exception
 */
VException::VException(const VException &e) V_NOEXCEPT_EXPR (true)
    :error(e.WhatUtf8()), moreInfo(e.MoreInformation())
{}

//---------------------------------------------------------------------------------------------------------------------
auto VException::operator=(const VException &e) V_NOEXCEPT_EXPR(true) -> VException &
{
    if ( &e == this )
    {
        return *this;
    }
    this->error = e.WhatUtf8();
    this->moreInfo = e.MoreInformation();
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ErrorMessage return main error message
 * @return error message
 */
auto VException::ErrorMessage() const -> QString
{
    return tr("Exception: %1").arg(error);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief AddMoreInformation add more information for error
 * @param info information
 */
void VException::AddMoreInformation(const QString &info)
{
    if (info.isEmpty())
    {
        return;
    }

    moreInfo = QStringLiteral("%1\n%2").arg(moreInfo, info);
}

//---------------------------------------------------------------------------------------------------------------------
auto VException::MoreInfo(const QString &detInfo) const -> QString
{
    if (not moreInfo.isEmpty())
    {
        return QStringLiteral("%1\n%2").arg(moreInfo, detInfo);
    }

    return detInfo;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief DetailedInformation return detailed information about error
 * @return detailed information
 */
auto VException::DetailedInformation() const -> QString
{
    return moreInfo;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief clone clone exception
 * @return new exception
 */
// cppcheck-suppress unusedFunction
auto VException::clone() const -> VException *
{
    return new VException(*this);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief raise method raise for exception
 */
// cppcheck-suppress unusedFunction
Q_NORETURN void VException::raise() const
{
    throw *this;
}

//-----------------------------------------VExceptionToolWasDeleted----------------------------------------------------
VExceptionToolWasDeleted::VExceptionToolWasDeleted(const QString &error) V_NOEXCEPT_EXPR (true)
    :VException(error)
{
}

//---------------------------------------------------------------------------------------------------------------------
VExceptionToolWasDeleted::VExceptionToolWasDeleted(const VExceptionToolWasDeleted &e) V_NOEXCEPT_EXPR (true)
    :VException(e)
{
}

//---------------------------------------------------------------------------------------------------------------------
auto VExceptionToolWasDeleted::operator=(const VExceptionToolWasDeleted &e) V_NOEXCEPT_EXPR(true)
    -> VExceptionToolWasDeleted &
{
    if ( &e == this )
    {
        return *this;
    }
    VException::operator=(e);
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief raise method raise for exception
 */
// cppcheck-suppress unusedFunction
Q_NORETURN void VExceptionToolWasDeleted::raise() const
{
    throw *this;
}

//---------------------------------------------------------------------------------------------------------------------
auto VExceptionToolWasDeleted::clone() const -> VExceptionToolWasDeleted *
{
    return new VExceptionToolWasDeleted(*this);
}
