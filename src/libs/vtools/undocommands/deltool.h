/************************************************************************
 **
 **  @file   deltool.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   13 6, 2014
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

#ifndef DELTOOL_H
#define DELTOOL_H

#include <QDomNode>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "vundocommand.h"

class DelTool : public VUndoCommand
{
    Q_OBJECT // NOLINT

public:
    DelTool(VAbstractPattern *doc, quint32 id, QUndoCommand *parent = nullptr);
    ~DelTool() override = default;
    void undo() override;
    void redo() override;

private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(DelTool) // NOLINT
    QDomNode parentNode{};
    quint32 siblingId{NULL_ID};
    const QString nameActivDraw;
    QMap<quint32, VGroupData> m_groupsBefore{};
    QMap<quint32, VGroupData> m_groupsAfter{};

    void UpdateGroups(const QMap<quint32, VGroupData> &groups) const;
};

#endif // DELTOOL_H
