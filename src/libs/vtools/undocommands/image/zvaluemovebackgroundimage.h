/************************************************************************
 **
 **  @file   zvaluemovebackgroundimage.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   27 1, 2022
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2022 Valentina project
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
#ifndef ZVALUEMOVEBACKGROUNDIMAGE_H
#define ZVALUEMOVEBACKGROUNDIMAGE_H

#include "../vundocommand.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
#include "../vmisc/defglobal.h"
#endif // QT_VERSION < QT_VERSION_CHECK(5, 13, 0)

enum class ZValueMoveType
{
    Top,
    Up,
    Down,
    Bottom
};

class ZValueMoveBackgroundImage : public VUndoCommand
{
    Q_OBJECT // NOLINT
public:
    ZValueMoveBackgroundImage(QUuid id, ZValueMoveType move, VAbstractPattern *doc, QUndoCommand *parent = nullptr);
    ~ZValueMoveBackgroundImage() override =default;
    void undo() override;
    void redo() override;
private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(ZValueMoveBackgroundImage) // NOLINT
    QUuid m_id;
    ZValueMoveType m_move;
    QHash<QUuid, qreal> m_oldValues{};
};

#endif // ZVALUEMOVEBACKGROUNDIMAGE_H
