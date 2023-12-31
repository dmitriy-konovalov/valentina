/************************************************************************
 **
 **  @file   dialoggroup.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   4 4, 2016
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

#include "dialoggroup.h"

#include <QCompleter>
#include <QLineEdit>

#include "ui_dialoggroup.h"

//---------------------------------------------------------------------------------------------------------------------
DialogGroup::DialogGroup(const VContainer *data, VAbstractPattern *doc, quint32 toolId, QWidget *parent)
  : DialogTool(data, doc, toolId, parent),
    ui(new Ui::DialogGroup),
    group(),
    flagName(false)
{
    ui->setupUi(this);
    InitOkCancel(ui);

    connect(ui->lineEditName, &QLineEdit::textChanged, this, &DialogGroup::NameChanged);
}

//---------------------------------------------------------------------------------------------------------------------
DialogGroup::~DialogGroup()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogGroup::SetName(const QString &name)
{
    ui->lineEditName->setText(name);
}

//---------------------------------------------------------------------------------------------------------------------
auto DialogGroup::GetName() const -> QString
{
    return ui->lineEditName->text();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogGroup::SetTags(const QStringList &tags)
{
    ui->lineEditTags->setText(tags.join(", "));
}

//---------------------------------------------------------------------------------------------------------------------
auto DialogGroup::GetTags() const -> QStringList
{
    return ui->lineEditTags->text().split(",");
}

//---------------------------------------------------------------------------------------------------------------------
void DialogGroup::SetGroupCategories(const QStringList &categories)
{
    ui->lineEditTags->SetCompletion(categories);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogGroup::ShowDialog(bool click)
{
    if (not click)
    {
        if (group.isEmpty())
        {
            return;
        }

        SetName(tr("New group"));

        setModal(true);
        emit ToolTip(QString());
        show();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogGroup::SelectedObject(bool selected, quint32 object, quint32 tool)
{
    if (selected)
    {
        group.insert(object, tool);
    }
    else
    {
        group.remove(object);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogGroup::NameChanged()
{
    ui->lineEditName->text().isEmpty() ? flagName = false : flagName = true;
    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
auto DialogGroup::GetGroup() const -> QMap<quint32, quint32>
{
    return group;
}
