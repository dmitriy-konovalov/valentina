/************************************************************************
 **
 **  @file   dialogflippingbyaxis.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   16 9, 2016
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

#include "dialogflippingbyaxis.h"

#include <QColor>
#include <QComboBox>
#include <QCompleter>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPointF>
#include <QPointer>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSharedPointer>
#include <QStringList>
#include <QToolButton>
#include <Qt>
#include <new>

#include "../../visualization/visualization.h"
#include "../../visualization/line/operation/vistoolflippingbyaxis.h"
#include "../ifc/xml/vabstractpattern.h"
#include "../ifc/xml/vdomdocument.h"
#include "../qmuparser/qmudef.h"
#include "../vgeometry/vpointf.h"
#include "../vmisc/vabstractapplication.h"
#include "../vmisc/vcommonsettings.h"
#include "../vmisc/compatibility.h"
#include "../vpatterndb/vcontainer.h"
#include "../vwidgets/vabstractmainwindow.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../vwidgets/vmaingraphicsview.h"
#include "ui_dialogflippingbyaxis.h"

//---------------------------------------------------------------------------------------------------------------------
DialogFlippingByAxis::DialogFlippingByAxis(const VContainer *data, quint32 toolId, QWidget *parent)
    : DialogTool(data, toolId, parent),
      ui(new Ui::DialogFlippingByAxis),
      stage1(true),
      m_suffix(),
      flagName(true),
      flagGroupName(true),
      flagError(false)
{
    ui->setupUi(this);

    ui->lineEditSuffix->setText(qApp->getCurrentDocument()->GenerateSuffix());

    InitOkCancelApply(ui);

    FillComboBoxPoints(ui->comboBoxOriginPoint);
    FillComboBoxAxisType(ui->comboBoxAxisType);

    ui->comboBoxOriginPoint->setCurrentIndex(-1);

    connect(ui->lineEditSuffix, &QLineEdit::textChanged, this, &DialogFlippingByAxis::SuffixChanged);
    connect(ui->lineEditVisibilityGroup, &QLineEdit::textChanged, this, &DialogFlippingByAxis::GroupNameChanged);
    connect(ui->comboBoxOriginPoint, &QComboBox::currentTextChanged,
            this, &DialogFlippingByAxis::PointChanged);

    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &DialogFlippingByAxis::ShowSourceDetails);
    connect(ui->lineEditAlias, &QLineEdit::textEdited, this, &DialogFlippingByAxis::AliasChanged);

    vis = new VisToolFlippingByAxis(data);

    ui->tabWidget->setCurrentIndex(0);
    SetTabStopDistance(ui->plainTextEditToolNotes);
}

//---------------------------------------------------------------------------------------------------------------------
DialogFlippingByAxis::~DialogFlippingByAxis()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
quint32 DialogFlippingByAxis::GetOriginPointId() const
{
    return getCurrentObjectId(ui->comboBoxOriginPoint);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SetOriginPointId(quint32 value)
{
    ChangeCurrentData(ui->comboBoxOriginPoint, value);
    VisToolFlippingByAxis *operation = qobject_cast<VisToolFlippingByAxis *>(vis);
    SCASSERT(operation != nullptr)
    operation->SetOriginPointId(value);
}

//---------------------------------------------------------------------------------------------------------------------
AxisType DialogFlippingByAxis::GetAxisType() const
{
    return getCurrentCrossPoint<AxisType>(ui->comboBoxAxisType);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SetAxisType(AxisType type)
{
    auto index = ui->comboBoxAxisType->findData(static_cast<int>(type));
    if (index != -1)
    {
        ui->comboBoxAxisType->setCurrentIndex(index);

        auto operation = qobject_cast<VisToolFlippingByAxis *>(vis);
        SCASSERT(operation != nullptr)
        operation->SetAxisType(type);
    }
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogFlippingByAxis::GetSuffix() const
{
    return m_suffix;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SetSuffix(const QString &value)
{
    m_suffix = value;
    ui->lineEditSuffix->setText(value);
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogFlippingByAxis::GetVisibilityGroupName() const
{
    return ui->lineEditVisibilityGroup->text();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SetVisibilityGroupName(const QString &name)
{
    ui->lineEditVisibilityGroup->setText(name.isEmpty() ? tr("Rotation") : name);
}

//---------------------------------------------------------------------------------------------------------------------
bool DialogFlippingByAxis::HasLinkedVisibilityGroup() const
{
    return ui->groupBoxVisibilityGroup->isChecked();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SetHasLinkedVisibilityGroup(bool linked)
{
    ui->groupBoxVisibilityGroup->setChecked(linked);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SetVisibilityGroupTags(const QStringList &tags)
{
    ui->lineEditGroupTags->setText(tags.join(", "));
}

//---------------------------------------------------------------------------------------------------------------------
QStringList DialogFlippingByAxis::GetVisibilityGroupTags() const
{
    return ui->lineEditGroupTags->text().split(',');
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SetGroupCategories(const QStringList &categories)
{
    m_groupTags = categories;
    ui->lineEditGroupTags->SetCompletion(m_groupTags);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::ShowDialog(bool click)
{
    if (stage1 && not click)
    {
        if (sourceObjects.isEmpty())
        {
            return;
        }

        stage1 = false;

        VMainGraphicsScene *scene = qobject_cast<VMainGraphicsScene *>(qApp->getCurrentScene());
        SCASSERT(scene != nullptr)
        scene->clearSelection();

        VisToolFlippingByAxis *operation = qobject_cast<VisToolFlippingByAxis *>(vis);
        SCASSERT(operation != nullptr)
        operation->SetObjects(SourceToObjects(sourceObjects));
        operation->VisualMode();

        scene->ToggleArcSelection(false);
        scene->ToggleElArcSelection(false);
        scene->ToggleSplineSelection(false);
        scene->ToggleSplinePathSelection(false);

        scene->ToggleArcHover(false);
        scene->ToggleElArcHover(false);
        scene->ToggleSplineHover(false);
        scene->ToggleSplinePathHover(false);

        qApp->getSceneView()->AllowRubberBand(false);

        FillSourceList();

        emit ToolTip(tr("Select origin point"));
    }
    else if (not stage1 && prepare && click)
    {
        CheckState();
        setModal(true);
        emit ToolTip(QString());
        show();
    }
}

//---------------------------------------------------------------------------------------------------------------------
QVector<SourceItem> DialogFlippingByAxis::GetSourceObjects() const
{
    return sourceObjects;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SetSourceObjects(const QVector<SourceItem> &value)
{
    sourceObjects = value;
    FillSourceList();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::ChosenObject(quint32 id, const SceneObject &type)
{
    if (not stage1 && not prepare)// After first choose we ignore all objects
    {
        if (type == SceneObject::Point)
        {
            auto obj = std::find_if(sourceObjects.begin(), sourceObjects.end(),
                                    [id](const SourceItem &sItem) { return sItem.id == id; });

            if (obj != sourceObjects.end())
            {
                emit ToolTip(tr("Select origin point that is not part of the list of objects"));
                return;
            }

            if (SetObject(id, ui->comboBoxOriginPoint, QString()))
            {
                VisToolFlippingByAxis *operation = qobject_cast<VisToolFlippingByAxis *>(vis);
                SCASSERT(operation != nullptr)
                operation->SetOriginPointId(id);
                operation->RefreshGeometry();

                prepare = true;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SelectedObject(bool selected, quint32 object, quint32 tool)
{
    Q_UNUSED(tool)
    if (stage1)
    {
        auto obj = std::find_if(sourceObjects.begin(), sourceObjects.end(),
                                [object](const SourceItem &sItem) { return sItem.id == object; });
        if (selected)
        {
            if (obj == sourceObjects.cend())
            {
                SourceItem item;
                item.id = object;
                sourceObjects.append(item);
            }
        }
        else
        {
            if (obj != sourceObjects.end())
            {
                sourceObjects.erase(obj);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SuffixChanged()
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    if (edit)
    {
        const QString suffix = edit->text();
        if (suffix.isEmpty())
        {
            flagName = false;
            ChangeColor(ui->labelSuffix, errorColor);
            ui->labelStatus->setText(tr("Invalid suffix"));
            CheckState();
            return;
        }
        else
        {
            if (m_suffix != suffix)
            {
                QRegularExpression rx(NameRegExp());
                const QStringList uniqueNames = data->AllUniqueNames();
                for (auto &uniqueName : uniqueNames)
                {
                    const QString name = uniqueName + suffix;
                    if (not rx.match(name).hasMatch() || not data->IsUnique(name))
                    {
                        flagName = false;
                        ChangeColor(ui->labelSuffix, errorColor);
                        ui->labelStatus->setText(tr("Invalid suffix"));
                        CheckState();
                        return;
                    }
                }
            }
        }

        flagName = true;
        ChangeColor(ui->labelSuffix, OkColor(this));
    }
    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::GroupNameChanged()
{
    QLineEdit* edit = qobject_cast<QLineEdit*>(sender());
    if (edit)
    {
        const QString name = edit->text();
        if (name.isEmpty())
        {
            flagGroupName = false;
            ChangeColor(ui->labelGroupName, errorColor);
            ui->labelStatus->setText(tr("Invalid group name"));
            CheckState();
            return;
        }

        flagGroupName = true;
        ChangeColor(ui->labelGroupName, OkColor(this));
    }
    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::ShowSourceDetails(int row)
{
    ui->lineEditAlias->setDisabled(true);

    if (ui->listWidget->count() == 0)
    {
        return;
    }

    const auto sourceItem = qvariant_cast<SourceItem>(ui->listWidget->item(row)->data(Qt::UserRole));

    const QSharedPointer<VGObject> obj = data->GetGObject(sourceItem.id);

    ui->labelAlias->setText(obj->getType() == GOType::Point ? tr("Label:") : tr("Alias:"));

    ui->lineEditAlias->blockSignals(true);
    ui->lineEditAlias->setText(sourceItem.alias);
    ui->lineEditAlias->setEnabled(true);
    ui->lineEditAlias->blockSignals(false);

    QRegularExpression rx(NameRegExp());
    if (not rx.match(sourceItem.alias).hasMatch() || not data->IsUnique(sourceItem.alias))
    {
        flagAlias = false;
        ChangeColor(ui->labelAlias, errorColor);
        ui->labelStatus->setText(obj->getType() == GOType::Point ? tr("Invalid label") : tr("Invalid alias"));
        CheckState();
        return;
    }
    else
    {
        flagAlias = true;
        ChangeColor(ui->labelAlias, errorColor);
        CheckState();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::AliasChanged(const QString &text)
{
    if (ui->listWidget->count() == 0)
    {
        return;
    }

    if (auto *item = ui->listWidget->currentItem())
    {
        auto sourceItem = qvariant_cast<SourceItem>(item->data(Qt::UserRole));
        sourceItem.alias = text;

        item->setData(Qt::UserRole, QVariant::fromValue(sourceItem));

        ValidateSourceAliases();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::ShowVisualization()
{
    AddVisualization<VisToolFlippingByAxis>();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SaveData()
{
    m_suffix = ui->lineEditSuffix->text();

    sourceObjects.clear();
    sourceObjects.reserve(ui->listWidget->count());

    for (int i=0; i<ui->listWidget->count(); ++i)
    {
        if (const QListWidgetItem *item = ui->listWidget->item(i))
        {
            auto sourceItem = qvariant_cast<SourceItem>(item->data(Qt::UserRole));
            sourceObjects.append(sourceItem);
        }
    }

    VisToolFlippingByAxis *operation = qobject_cast<VisToolFlippingByAxis *>(vis);
    SCASSERT(operation != nullptr)

    operation->SetObjects(SourceToObjects(sourceObjects));
    operation->SetOriginPointId(GetOriginPointId());
    operation->SetAxisType(GetAxisType());
    operation->RefreshGeometry();

    QStringList groupTags = ui->lineEditGroupTags->text().split(',');
    for (auto &tag : groupTags)
    {
        tag = tag.trimmed();
        if (not m_groupTags.contains(tag))
        {
            m_groupTags.append(tag);
        }
    }

    ui->lineEditGroupTags->SetCompletion(m_groupTags);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::PointChanged()
{
    QColor color;
    quint32 id = getCurrentObjectId(ui->comboBoxOriginPoint);
    auto obj = std::find_if(sourceObjects.begin(), sourceObjects.end(),
                            [id](const SourceItem &sItem) { return sItem.id == id; });
    if (obj != sourceObjects.end())
    {
        flagError = false;
        ui->labelStatus->setText(tr("Invalid point"));
        color = errorColor;
    }
    else
    {
        flagError = true;
        color = OkColor(this);
    }
    ChangeColor(ui->labelOriginPoint, color);
    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::FillComboBoxAxisType(QComboBox *box)
{
    SCASSERT(box != nullptr)

    box->addItem(tr("Vertical axis"), QVariant(static_cast<int>(AxisType::VerticalAxis)));
    box->addItem(tr("Horizontal axis"), QVariant(static_cast<int>(AxisType::HorizontalAxis)));
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::FillSourceList()
{
    ui->listWidget->blockSignals(true);

    ui->listWidget->clear();

    int row = -1;

    for (auto &sourceItem : sourceObjects)
    {
        const QSharedPointer<VGObject> obj = data->GetGObject(sourceItem.id);

        auto *item = new QListWidgetItem(obj->ObjectName());
        item->setToolTip(obj->ObjectName());
        item->setData(Qt::UserRole, QVariant::fromValue(sourceItem));
        ui->listWidget->insertItem(++row, item);
    }

    ui->listWidget->blockSignals(false);

    if (ui->listWidget->count() > 0)
    {
        ui->listWidget->setCurrentRow(0);
    }

    ValidateSourceAliases();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::ValidateSourceAliases()
{
    QRegularExpression rx(NameRegExp());

    for (int i=0; i<ui->listWidget->count(); ++i)
    {
        if (const QListWidgetItem *item = ui->listWidget->item(i))
        {
            auto sourceItem = qvariant_cast<SourceItem>(item->data(Qt::UserRole));

            const QSharedPointer<VGObject> obj = data->GetGObject(sourceItem.id);

            QString name;

            if (obj->getType() == GOType::Point)
            {
                name = sourceItem.alias;
            }
            else
            {
                const QString oldAlias = obj->GetAliasSuffix();
                obj->SetAliasSuffix(sourceItem.alias);
                name = obj->GetAlias();
                obj->SetAliasSuffix(oldAlias);
            }

            if (not rx.match(name).hasMatch() || not data->IsUnique(name))
            {
                flagAlias = false;
                ui->labelStatus->setText(obj->getType() == GOType::Point ? tr("Invalid label") : tr("Invalid alias"));
                CheckState();
                return;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogFlippingByAxis::SetNotes(const QString &notes)
{
    ui->plainTextEditToolNotes->setPlainText(notes);
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogFlippingByAxis::GetNotes() const
{
    return ui->plainTextEditToolNotes->toPlainText();
}

//---------------------------------------------------------------------------------------------------------------------
bool DialogFlippingByAxis::IsValid() const
{
    bool ready = flagError && flagName && flagGroupName && flagAlias;

    if (ready)
    {
        ui->labelStatus->setText(tr("Ready"));
    }

    return ready;
}
