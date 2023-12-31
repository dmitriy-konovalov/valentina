/************************************************************************
 **
 **  @file   vpundopiecemove.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   16 8, 2021
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
#ifndef VPUNDOPIECEMOVE_H
#define VPUNDOPIECEMOVE_H

#include "vpundocommand.h"

#include <QTransform>

#include "../layout/layoutdef.h"

class VPUndoPieceMove : public VPUndoCommand
{
    Q_OBJECT // NOLINT
public:
    explicit VPUndoPieceMove(const VPPiecePtr &piece, qreal dx, qreal dy, bool allowMerge = false,
                             QUndoCommand *parent = nullptr);
    ~VPUndoPieceMove() override =default;

    void undo() override;
    void redo() override;
    // cppcheck-suppress unusedFunction
    auto mergeWith(const QUndoCommand *command) -> bool override;
    auto id() const -> int override;

    auto Piece() const -> VPPiecePtr;
    auto Dx() const -> qreal;
    auto Dy() const -> qreal;

private:
    Q_DISABLE_COPY_MOVE(VPUndoPieceMove) // NOLINT

    VPPieceWeakPtr m_piece;
    QTransform     m_oldTransform{};
    qreal          m_dx;
    qreal          m_dy;
};

//---------------------------------------------------------------------------------------------------------------------
inline auto VPUndoPieceMove::Piece() const -> VPPiecePtr
{
    return m_piece;
}

//---------------------------------------------------------------------------------------------------------------------
inline auto VPUndoPieceMove::Dx() const -> qreal
{
    return m_dx;
}

//---------------------------------------------------------------------------------------------------------------------
inline auto VPUndoPieceMove::Dy() const -> qreal
{
    return m_dy;
}

// Move pieces
class VPUndoPiecesMove : public VPUndoCommand
{
    Q_OBJECT // NOLINT
public:
    explicit VPUndoPiecesMove(const QList<VPPiecePtr> &pieces, qreal dx, qreal dy, bool allowMerge = false,
                              QUndoCommand *parent = nullptr);
    ~VPUndoPiecesMove() override =default;

    void undo() override;
    void redo() override;
    // cppcheck-suppress unusedFunction
    auto mergeWith(const QUndoCommand *command) -> bool override;
    auto id() const -> int override ;

    auto PieceIds() const -> QSet<QString>;
    auto Dx() const -> qreal;
    auto Dy() const -> qreal;

private:
    Q_DISABLE_COPY_MOVE(VPUndoPiecesMove) // NOLINT

    QVector<VPPieceWeakPtr>   m_pieces{};
    QMap<QString, QTransform> m_oldTransforms{};
    qreal                     m_dx;
    qreal                     m_dy;

    auto Layout() const -> VPLayoutPtr;
    auto Sheet() const -> VPSheetPtr;
};

//---------------------------------------------------------------------------------------------------------------------
inline auto VPUndoPiecesMove::Dx() const -> qreal
{
    return m_dx;
}

//---------------------------------------------------------------------------------------------------------------------
inline auto VPUndoPiecesMove::Dy() const -> qreal
{
    return m_dy;
}

#endif // VPUNDOPIECEMOVE_H
