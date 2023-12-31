/************************************************************************
 **
 **  @file   movelabel.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   25 12, 2014
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

#ifndef MOVELABEL_H
#define MOVELABEL_H

#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "moveabstractlabel.h"

class MoveLabel : public MoveAbstractLabel
{
    Q_OBJECT // NOLINT

public:
    MoveLabel(VAbstractPattern *doc, const QPointF &pos, const quint32 &id, QUndoCommand *parent = nullptr);
    ~MoveLabel() = default;

    auto mergeWith(const QUndoCommand *command) -> bool override;
    auto id() const -> int override;

protected:
    void Do(const QPointF &pos) override;

private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(MoveLabel) // NOLINT
    // Need for resizing scene rect
    QGraphicsScene *m_scene;
};

#endif // MOVELABEL_H
