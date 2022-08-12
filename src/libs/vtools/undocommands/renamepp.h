/************************************************************************
 **
 **  @file   renamepp.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   17 7, 2014
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

#ifndef RENAMEPP_H
#define RENAMEPP_H

#include <qcompilerdetection.h>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "vundocommand.h"

class RenamePP :public VUndoCommand
{
    Q_OBJECT // NOLINT
public:
    RenamePP(VAbstractPattern *doc, const QString &newPPname, QComboBox *combo, QUndoCommand *parent = nullptr);
    virtual ~RenamePP() override;

    virtual void undo() override;
    virtual void redo() override;
    virtual bool mergeWith(const QUndoCommand *command) override;
    virtual int  id() const override;
    QString      getNewPPname() const;
    QString      getOldPPname() const;
private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(RenamePP) // NOLINT
    QComboBox *combo;
    QString   newPPname;
    QString   oldPPname;
    void      ChangeName(const QString &oldName, const QString &newName);
};

//---------------------------------------------------------------------------------------------------------------------
inline QString RenamePP::getNewPPname() const
{
    return newPPname;
}

//---------------------------------------------------------------------------------------------------------------------
inline QString RenamePP::getOldPPname() const
{
    return oldPPname;
}

#endif // RENAMEPP_H
