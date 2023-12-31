/************************************************************************
 **
 **  @file
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   26 11, 2016
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

#ifndef SAVEPIECEPATHOPTIONS_H
#define SAVEPIECEPATHOPTIONS_H

#include <QtGlobal>

#include "../vpatterndb/vpiecepath.h"
#include "vundocommand.h"

class SavePiecePathOptions : public VUndoCommand
{
    Q_OBJECT // NOLINT
public:
    SavePiecePathOptions(quint32 pieceId, const VPiecePath &oldPath, const VPiecePath &newPath, VAbstractPattern *doc,
                         VContainer *data, quint32 id, QUndoCommand *parent = nullptr);
    virtual ~SavePiecePathOptions() = default;

    virtual void undo() override;
    virtual void redo() override;
    virtual auto mergeWith(const QUndoCommand *command) -> bool override;
    virtual auto id() const -> int override;
    auto PathId() const -> quint32;
    auto NewPath() const -> VPiecePath;

private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(SavePiecePathOptions) // NOLINT

    const VPiecePath m_oldPath;
    VPiecePath       m_newPath;

    VContainer *m_data;
    quint32 m_pieceId;
};

//---------------------------------------------------------------------------------------------------------------------
inline auto SavePiecePathOptions::id() const -> int
{
    return static_cast<int>(UndoCommand::SavePiecePathOptions);
}

//---------------------------------------------------------------------------------------------------------------------
inline auto SavePiecePathOptions::PathId() const -> quint32
{
    return nodeId;
}

//---------------------------------------------------------------------------------------------------------------------
inline auto SavePiecePathOptions::NewPath() const -> VPiecePath
{
    return m_newPath;
}

#endif // SAVEPIECEPATHOPTIONS_H
