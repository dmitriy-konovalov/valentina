/************************************************************************
 **
 **  @file   vpundoaddsheet.h
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
#ifndef VPUNDOADDSHEET_H
#define VPUNDOADDSHEET_H

#include "vpundocommand.h"

#include "../layout/layoutdef.h"

class VPUndoAddSheet : public VPUndoCommand
{
    Q_OBJECT // NOLINT
public:
    explicit VPUndoAddSheet(const VPSheetPtr &sheet, QUndoCommand *parent = nullptr);
    ~VPUndoAddSheet() override =default;

    void undo() override;
    void redo() override;
    auto id() const -> int override;

private:
    Q_DISABLE_COPY_MOVE(VPUndoAddSheet) // NOLINT

    VPSheetWeakPtr m_sheet;
};

#endif // VPUNDOADDSHEET_H
