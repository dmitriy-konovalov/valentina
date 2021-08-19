/************************************************************************
 **
 **  @file   vpundomovepieceonsheet.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   19 8, 2021
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2021 Valentina project
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
#include "vpundomovepieceonsheet.h"
#include "../vmisc/def.h"
#include "../layout/vpsheet.h"
#include "../layout/vplayout.h"
#include "../layout/vppiece.h"

//---------------------------------------------------------------------------------------------------------------------
VPUndoMovePieceOnSheet::VPUndoMovePieceOnSheet(const VPSheetPtr &sheet, const VPPiecePtr &piece, QUndoCommand *parent)
    : VPUndoCommand(false, parent),
      m_sheet(sheet),
      m_piece(piece)
{
    SCASSERT(not piece.isNull())

    m_oldSheet = piece->Sheet();

    setText(tr("move piece on sheet"));
}

//---------------------------------------------------------------------------------------------------------------------
void VPUndoMovePieceOnSheet::undo()
{
    VPSheetPtr sourceSheet = m_oldSheet.toStrongRef();
    VPSheetPtr activateSheet = sourceSheet;
    if (activateSheet.isNull())
    {
        activateSheet = m_sheet.toStrongRef();
    }

    VPLayoutPtr layout;

    if (not activateSheet.isNull())
    {
        layout = activateSheet->GetLayout();
        if (not layout.isNull() && not activateSheet->TrashSheet())
        {
            layout->SetFocusedSheet(activateSheet);
        }
    }

    VPPiecePtr piece = m_piece.toStrongRef();
    if (not piece.isNull())
    {
        piece->SetSheet(sourceSheet);

        if (not layout.isNull())
        {
            emit layout->PieceSheetChanged(piece);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPUndoMovePieceOnSheet::redo()
{
    VPSheetPtr sourceSheet = m_sheet.toStrongRef();
    VPSheetPtr activateSheet = sourceSheet;
    if (activateSheet.isNull())
    {
        activateSheet = m_oldSheet.toStrongRef();
    }

    VPLayoutPtr layout;

    if (not activateSheet.isNull())
    {
        layout = activateSheet->GetLayout();
        if (not layout.isNull() && not activateSheet->TrashSheet())
        {
            layout->SetFocusedSheet(activateSheet);
        }
    }

    VPPiecePtr piece = m_piece.toStrongRef();
    if (not piece.isNull())
    {
        piece->SetSheet(sourceSheet);

        if (not layout.isNull())
        {
            emit layout->PieceSheetChanged(piece);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
auto VPUndoMovePieceOnSheet::id() const -> int
{
    return static_cast<int>(ML::UndoCommand::MoveOnSheet);
}
