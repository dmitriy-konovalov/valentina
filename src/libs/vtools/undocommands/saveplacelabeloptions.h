/************************************************************************
 **
 **  @file   saveplacelabeloptions.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   16 10, 2017
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2017 Valentina project
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
#ifndef SAVEPLACELABELOPTIONS_H
#define SAVEPLACELABELOPTIONS_H

#include "vundocommand.h"
#include "../vgeometry/vplacelabelitem.h"

class SavePlaceLabelOptions : public VUndoCommand
{
    Q_OBJECT // NOLINT
public:
    SavePlaceLabelOptions(quint32 pieceId, const VPlaceLabelItem &oldLabel, const VPlaceLabelItem &newLabel,
                          VAbstractPattern *doc, VContainer *data, quint32 id, QUndoCommand *parent = nullptr);
    virtual ~SavePlaceLabelOptions()=default;

    virtual void undo() override;
    virtual void redo() override;
    virtual auto mergeWith(const QUndoCommand *command) -> bool override;
    virtual auto id() const -> int override;

    auto LabelId() const -> quint32;
    auto NewLabel() const -> VPlaceLabelItem;

private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(SavePlaceLabelOptions) // NOLINT

    const VPlaceLabelItem m_oldLabel;
    VPlaceLabelItem       m_newLabel;

    VContainer *m_data;
    quint32 m_pieceId;
};

//---------------------------------------------------------------------------------------------------------------------
inline auto SavePlaceLabelOptions::id() const -> int
{
    return static_cast<int>(UndoCommand::SavePlaceLabelOptions);
}

//---------------------------------------------------------------------------------------------------------------------
inline auto SavePlaceLabelOptions::LabelId() const -> quint32
{
    return nodeId;
}

//---------------------------------------------------------------------------------------------------------------------
inline auto SavePlaceLabelOptions::NewLabel() const -> VPlaceLabelItem
{
    return m_newLabel;
}

#endif // SAVEPLACELABELOPTIONS_H
