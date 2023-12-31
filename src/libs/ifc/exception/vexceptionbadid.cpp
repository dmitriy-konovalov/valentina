/************************************************************************
 **
 **  @file   vexceptionbadid.cpp
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

#include "vexceptionbadid.h"

#include "vexception.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
#include "../vmisc/compatibility.h"
#endif

using namespace Qt::Literals::StringLiterals;

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VExceptionBadId exception bad id
 * @param error string with error
 * @param id id
 */
VExceptionBadId::VExceptionBadId(const QString &error, const quint32 &id) V_NOEXCEPT_EXPR(true)
  : VException(error),
    id(id),
    key(QString())
{
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VExceptionBadId exception bad id
 * @param error string with error
 * @param key string key
 */
VExceptionBadId::VExceptionBadId(const QString &error, const QString &key) V_NOEXCEPT_EXPR(true)
  : VException(error),
    id(NULL_ID),
    key(key)
{
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VExceptionBadId copy constructor
 * @param e exception
 */
VExceptionBadId::VExceptionBadId(const VExceptionBadId &e) V_NOEXCEPT_EXPR(true)
  : VException(e),
    id(e.BadId()),
    key(e.BadKey())
{
}

//---------------------------------------------------------------------------------------------------------------------
auto VExceptionBadId::operator=(const VExceptionBadId &e) V_NOEXCEPT_EXPR(true) -> VExceptionBadId &
{
    if (&e == this)
    {
        return *this;
    }
    VException::operator=(e);
    this->id = e.BadId();
    this->key = e.BadKey();
    return *this;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ErrorMessage return main error message
 * @return main error message
 */
auto VExceptionBadId::ErrorMessage() const -> QString
{
    QString error;
    if (key.isEmpty())
    {
        error = u"ExceptionBadId: %1, id = %2"_s.arg(this->error).arg(id);
    }
    else
    {
        error = u"ExceptionBadId: %1, id = %2"_s.arg(this->error, key);
    }
    return error;
}
