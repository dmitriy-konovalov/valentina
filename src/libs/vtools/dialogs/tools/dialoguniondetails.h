/************************************************************************
 **
 **  @file   dialoguniondetails.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   23 12, 2013
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

#ifndef DIALOGUNIONDETAILS_H
#define DIALOGUNIONDETAILS_H

#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "../vmisc/def.h"
#include "dialogtool.h"

namespace Ui
{
class DialogUnionDetails;
}

/**
 * @brief The DialogUnionDetails class dialog for ToolUnionDetails.
 */
class DialogUnionDetails : public DialogTool
{
    Q_OBJECT // NOLINT

public:
    DialogUnionDetails(const VContainer *data, VAbstractPattern *doc, quint32 toolId, QWidget *parent = nullptr);
    ~DialogUnionDetails() override;

    auto getD1() const -> quint32;
    auto getD2() const -> quint32;
    auto getIndexD1() const -> vsizetype;
    auto getIndexD2() const -> vsizetype;

    auto RetainPieces() const -> bool;

public slots:
    void ChosenObject(quint32 id, const SceneObject &type) override;

protected:
    auto IsValid() const -> bool final { return true; }

private:
    Q_DISABLE_COPY_MOVE(DialogUnionDetails) // NOLINT

    /** @brief ui keeps information about user interface */
    Ui::DialogUnionDetails *ui;

    /** @brief indexD1 index edge first detail */
    vsizetype indexD1;

    /** @brief indexD2 index edge second detail */
    vsizetype indexD2;

    /** @brief d1 id first detail */
    quint32 d1;

    /** @brief d2 id second detail */
    quint32 d2;

    /** @brief numberD number of detail, what we already have */
    qint32 numberD;

    /** @brief numberP number of points, what we already have */
    qint32 numberP;

    /** @brief p1 id first point of detail */
    quint32 p1;

    /** @brief p2 id second point of detail */
    quint32 p2;

    auto CheckObject(const quint32 &id, const quint32 &idDetail) const -> bool;
    auto CheckDetail(const quint32 &idDetail) const -> bool;

    void ChoosedDetail(const quint32 &id, const SceneObject &type, quint32 &idDetail, vsizetype &index);
};

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief getD1 return id first detail
 * @return id
 */
inline auto DialogUnionDetails::getD1() const -> quint32
{
    return d1;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief getD2 return id second detail
 * @return id
 */
inline auto DialogUnionDetails::getD2() const -> quint32
{
    return d2;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief getIndexD1 return index edge first detail
 * @return index
 */
inline auto DialogUnionDetails::getIndexD1() const -> vsizetype
{
    return indexD1;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief getIndexD2 return index edge second detail
 * @return index
 */
inline auto DialogUnionDetails::getIndexD2() const -> vsizetype
{
    return indexD2;
}

#endif // DIALOGUNIONDETAILS_H
