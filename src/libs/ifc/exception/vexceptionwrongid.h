/************************************************************************
 **
 **  @file   vexceptionwrongparameterid.h
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

#ifndef VEXCEPTIONWRONGID_H
#define VEXCEPTIONWRONGID_H


#include <QString>
#include <QtGlobal>

#include "../ifcdef.h"
#include "vexception.h"

class QDomElement;

/**
 * @brief The VExceptionWrongId class for exception wrong id
 */
class VExceptionWrongId : public VException
{
public:
    VExceptionWrongId(const QString &what, const QDomElement &domElement) V_NOEXCEPT_EXPR (true);
    VExceptionWrongId(const VExceptionWrongId &e) V_NOEXCEPT_EXPR (true);
    auto operator=(const VExceptionWrongId &e) V_NOEXCEPT_EXPR(true) -> VExceptionWrongId &;
    virtual ~VExceptionWrongId() V_NOEXCEPT_EXPR (true) = default;

    Q_NORETURN virtual void raise() const override { throw *this; }

    Q_REQUIRED_RESULT virtual auto clone() const -> VExceptionWrongId * override
    {
        return new VExceptionWrongId(*this);
    }

    virtual auto ErrorMessage() const -> QString override;
    virtual auto DetailedInformation() const -> QString override;
    auto TagText() const -> QString;
    auto TagName() const -> QString;
    auto LineNumber() const -> qint32;

protected:
    /** @brief tagText tag text */
    QString         tagText;

    /** @brief tagName tag name */
    QString         tagName;

    /** @brief lineNumber line number */
    qint32          lineNumber;
};

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief TagText return tag text
 * @return tag text
 */
inline auto VExceptionWrongId::TagText() const -> QString
{
    return tagText;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief TagName return tag name
 * @return tag name
 */
inline auto VExceptionWrongId::TagName() const -> QString
{
    return tagName;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief LineNumber return line number in file
 * @return line number
 */
inline auto VExceptionWrongId::LineNumber() const -> qint32
{
    return lineNumber;
}

#endif // VEXCEPTIONWRONGID_H
