/************************************************************************
 **
 **  @file   vplayout.cpp
 **  @author Ronan Le Tiec
 **  @date   13 4, 2020
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2020 Valentina project
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

#include "vplayout.h"
#include "vppiece.h"
#include "vpsheet.h"

#include <QLoggingCategory>
#include <QUndoStack>

Q_LOGGING_CATEGORY(pLayout, "p.layout")

//---------------------------------------------------------------------------------------------------------------------
VPLayout::VPLayout(QUndoStack *undoStack) :
    m_undoStack(undoStack)
{
    SCASSERT(m_undoStack != nullptr)
}

//---------------------------------------------------------------------------------------------------------------------
VPLayoutPtr VPLayout::CreateLayout(QUndoStack *undoStack)
{
    SCASSERT(undoStack != nullptr)
    undoStack->clear();
    VPLayoutPtr layout(new VPLayout(undoStack));
    layout->AddTrashSheet(VPSheetPtr(new VPSheet(layout)));
    return layout;
}

//---------------------------------------------------------------------------------------------------------------------
void VPLayout::AddPiece(const VPLayoutPtr &layout, const VPPiecePtr &piece)
{
    piece->SetLayout(layout);
    layout->AddPiece(piece);
}

//---------------------------------------------------------------------------------------------------------------------
void VPLayout::AddPiece(const VPPiecePtr &piece)
{
    if ((piece != nullptr) && not m_pieces.contains(piece->GetUniqueID()))
    {
        m_pieces.insert(piece->GetUniqueID(), piece);
    }
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::GetPieces() const -> QList<VPPiecePtr>
{
    return m_pieces.values();
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::GetUnplacedPieces() const -> QList<VPPiecePtr>
{
    return PiecesForSheet(VPSheetPtr());
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::GetTrashedPieces() const -> QList<VPPiecePtr>
{
    if (m_trashSheet.isNull())
    {
        return {};
    }
    return PiecesForSheet(m_trashSheet->Uuid());
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::AddSheet(const VPSheetPtr &sheet) -> VPSheetPtr
{
    if (not sheet.isNull() && GetSheet(sheet->Uuid()).isNull())
    {
        m_sheets.append(sheet);
    }
    return sheet;
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::GetSheets() -> QList<VPSheetPtr>
{
    return m_sheets;
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::GetSheet(const QUuid &uuid) -> VPSheetPtr
{
    auto sheet = std::find_if(m_sheets.begin(), m_sheets.end(),
                              [uuid](const VPSheetPtr &sheet) { return sheet->Uuid() == uuid; });

    if (sheet != m_sheets.end())
    {
        return *sheet;
    }

    return {};
}

//---------------------------------------------------------------------------------------------------------------------
void VPLayout::SetFocusedSheet(const VPSheetPtr &focusedSheet)
{
    if (m_sheets.isEmpty())
    {
        m_focusedSheet = {};
    }
    else
    {
        m_focusedSheet = focusedSheet.isNull() ? m_sheets.first() : focusedSheet;
    }

    emit ActiveSheetChanged(m_focusedSheet);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::GetFocusedSheet() -> VPSheetPtr
{
    return m_focusedSheet;
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::GetTrashSheet() -> VPSheetPtr
{
    return m_trashSheet;
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::LayoutSettings() -> VPLayoutSettings &
{
    return m_layoutSettings;
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::PiecesForSheet(const VPSheetPtr &sheet) const -> QList<VPPiecePtr>
{
    QList<VPPiecePtr> list;
    list.reserve(m_pieces.size());

    for (const auto& piece : m_pieces)
    {
        if (not piece.isNull() && piece->Sheet() == sheet)
        {
            list.append(piece);
        }
    }

    return list;
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::PiecesForSheet(const QUuid &uuid) const -> QList<VPPiecePtr>
{
    QList<VPPiecePtr> list;
    list.reserve(m_pieces.size());

    for (const auto& piece : m_pieces)
    {
        if (not piece.isNull())
        {
            VPSheetPtr sheet = piece->Sheet();
            if (not sheet.isNull() && sheet->Uuid() == uuid)
            {
                list.append(piece);
            }
        }
    }

    return list;
}

//---------------------------------------------------------------------------------------------------------------------
auto VPLayout::UndoStack() const -> QUndoStack *
{
    return m_undoStack;
}

//---------------------------------------------------------------------------------------------------------------------
void VPLayout::SetUndoStack(QUndoStack *newUndoStack)
{
    m_undoStack = newUndoStack;
}

//---------------------------------------------------------------------------------------------------------------------
void VPLayout::Clear()
{
    if (m_undoStack != nullptr)
    {
        m_undoStack->clear();
    }

    m_pieces.clear();
    m_trashSheet->Clear();
    m_sheets.clear();
    m_focusedSheet.clear();
    m_layoutSettings = VPLayoutSettings();
}

//---------------------------------------------------------------------------------------------------------------------
void VPLayout::AddTrashSheet(const VPSheetPtr &sheet)
{
    m_trashSheet = sheet;
    m_trashSheet->SetTrashSheet(true);
}
