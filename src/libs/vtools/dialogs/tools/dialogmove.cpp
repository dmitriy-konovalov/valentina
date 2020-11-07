/************************************************************************
 **
 **  @file   dialogmove.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   30 9, 2016
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

#include "dialogmove.h"

#include <QColor>
#include <QComboBox>
#include <QCompleter>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QLineF>
#include <QPlainTextEdit>
#include <QPointF>
#include <QPointer>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSharedPointer>
#include <QStringList>
#include <QTimer>
#include <QToolButton>
#include <Qt>
#include <new>

#include "../../visualization/visualization.h"
#include "../../visualization/line/operation/vistoolmove.h"
#include "../ifc/xml/vabstractpattern.h"
#include "../ifc/xml/vdomdocument.h"
#include "../qmuparser/qmudef.h"
#include "../support/dialogeditwrongformula.h"
#include "../vgeometry/vpointf.h"
#include "../vmisc/vabstractapplication.h"
#include "../vmisc/vcommonsettings.h"
#include "../vmisc/compatibility.h"
#include "../vpatterndb/vcontainer.h"
#include "../vpatterndb/vtranslatevars.h"
#include "../vwidgets/vabstractmainwindow.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../vwidgets/vmaingraphicsview.h"
#include "ui_dialogmove.h"

//---------------------------------------------------------------------------------------------------------------------
DialogMove::DialogMove(const VContainer *data, quint32 toolId, QWidget *parent)
    : DialogTool(data, toolId, parent),
      ui(new Ui::DialogMove),
      timerAngle(new QTimer(this)),
      timerRotationAngle(new QTimer(this)),
      timerLength(new QTimer(this)),
      formulaAngle(),
      formulaRotationAngle(),
      formulaLength(),
      formulaBaseHeightAngle(0),
      formulaBaseHeightRotationAngle(0),
      formulaBaseHeightLength(0),
      stage1(true),
      stage2(false),
      m_suffix(),
      optionalRotationOrigin(false),
      flagAngle(false),
      flagRotationAngle(false),
      flagLength(false),
      flagName(true),
      flagGroupName(true)
{
    ui->setupUi(this);

    this->formulaBaseHeightAngle = ui->plainTextEditAngle->height();
    ui->plainTextEditAngle->installEventFilter(this);

    this->formulaBaseHeightRotationAngle = ui->plainTextEditRotationAngle->height();
    ui->plainTextEditRotationAngle->installEventFilter(this);

    this->formulaBaseHeightLength = ui->plainTextEditLength->height();
    ui->plainTextEditLength->installEventFilter(this);

    ui->lineEditSuffix->setText(qApp->getCurrentDocument()->GenerateSuffix());

    timerAngle->setSingleShot(true);
    connect(timerAngle, &QTimer::timeout, this, &DialogMove::EvalAngle);

    timerRotationAngle->setSingleShot(true);
    connect(timerRotationAngle, &QTimer::timeout, this, &DialogMove::EvalRotationAngle);

    timerLength->setSingleShot(true);
    connect(timerLength, &QTimer::timeout, this, &DialogMove::EvalLength);

    InitOkCancelApply(ui);

    FillComboBoxPoints(ui->comboBoxRotationOriginPoint);

    ui->comboBoxRotationOriginPoint->blockSignals(true);
    ui->comboBoxRotationOriginPoint->addItem(tr("Center point"), NULL_ID);
    ui->comboBoxRotationOriginPoint->blockSignals(false);

    connect(ui->lineEditSuffix, &QLineEdit::textChanged, this, &DialogMove::SuffixChanged);
    connect(ui->lineEditVisibilityGroup, &QLineEdit::textChanged, this, &DialogMove::GroupNameChanged);
    connect(ui->toolButtonExprAngle, &QPushButton::clicked, this, &DialogMove::FXAngle);
    connect(ui->toolButtonExprRotationAngle, &QPushButton::clicked, this, &DialogMove::FXRotationAngle);
    connect(ui->toolButtonExprLength, &QPushButton::clicked, this, &DialogMove::FXLength);
    connect(ui->plainTextEditAngle, &QPlainTextEdit::textChanged, this, [this]()
    {
        timerAngle->start(formulaTimerTimeout);
    });

    connect(ui->plainTextEditRotationAngle, &QPlainTextEdit::textChanged, this, [this]()
    {
        timerRotationAngle->start(formulaTimerTimeout);
    });

    connect(ui->plainTextEditLength, &QPlainTextEdit::textChanged, this, [this]()
    {
        timerLength->start(formulaTimerTimeout);
    });

    connect(ui->pushButtonGrowAngle, &QPushButton::clicked, this, &DialogMove::DeployAngleTextEdit);
    connect(ui->pushButtonGrowRotationAngle, &QPushButton::clicked, this, &DialogMove::DeployRotationAngleTextEdit);
    connect(ui->pushButtonGrowLength, &QPushButton::clicked, this, &DialogMove::DeployLengthTextEdit);

    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &DialogMove::ShowSourceDetails);
    connect(ui->lineEditAlias, &QLineEdit::textEdited, this, &DialogMove::AliasChanged);

    vis = new VisToolMove(data);

    SetRotationOrigPointId(NULL_ID);

    ui->tabWidget->setCurrentIndex(0);
    SetTabStopDistance(ui->plainTextEditToolNotes);
}

//---------------------------------------------------------------------------------------------------------------------
DialogMove::~DialogMove()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogMove::GetAngle() const
{
    return qApp->TrVars()->TryFormulaFromUser(formulaAngle, qApp->Settings()->GetOsSeparator());
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetAngle(const QString &value)
{
    formulaAngle = qApp->TrVars()->FormulaToUser(value, qApp->Settings()->GetOsSeparator());
    // increase height if needed.
    if (formulaAngle.length() > 80)
    {
        this->DeployAngleTextEdit();
    }
    ui->plainTextEditAngle->setPlainText(formulaAngle);

    VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
    SCASSERT(operation != nullptr)
    operation->SetAngle(formulaAngle);

    MoveCursorToEnd(ui->plainTextEditAngle);
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogMove::GetLength() const
{
    return qApp->TrVars()->TryFormulaFromUser(formulaLength, qApp->Settings()->GetOsSeparator());
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetLength(const QString &value)
{
    formulaLength = qApp->TrVars()->FormulaToUser(value, qApp->Settings()->GetOsSeparator());
    // increase height if needed.
    if (formulaLength.length() > 80)
    {
        this->DeployLengthTextEdit();
    }
    ui->plainTextEditLength->setPlainText(formulaLength);

    VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
    SCASSERT(operation != nullptr)
    operation->SetLength(formulaLength);

    MoveCursorToEnd(ui->plainTextEditLength);
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogMove::GetRotationAngle() const
{
    return qApp->TrVars()->TryFormulaFromUser(formulaRotationAngle, qApp->Settings()->GetOsSeparator());
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetRotationAngle(const QString &value)
{
    formulaRotationAngle = qApp->TrVars()->FormulaToUser(value, qApp->Settings()->GetOsSeparator());
    // increase height if needed.
    if (formulaRotationAngle.length() > 80)
    {
        this->DeployAngleTextEdit();
    }
    ui->plainTextEditRotationAngle->setPlainText(formulaRotationAngle);

    VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
    SCASSERT(operation != nullptr)
    operation->SetRotationAngle(formulaRotationAngle);

    MoveCursorToEnd(ui->plainTextEditRotationAngle);
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogMove::GetSuffix() const
{
    return m_suffix;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetSuffix(const QString &value)
{
    m_suffix = value;
    ui->lineEditSuffix->setText(value);
}

//---------------------------------------------------------------------------------------------------------------------
quint32 DialogMove::GetRotationOrigPointId() const
{
    return getCurrentObjectId(ui->comboBoxRotationOriginPoint);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetRotationOrigPointId(const quint32 &value)
{
    ChangeCurrentData(ui->comboBoxRotationOriginPoint, value);
    VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
    SCASSERT(operation != nullptr)
    operation->SetRotationOriginPointId(value);
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogMove::GetVisibilityGroupName() const
{
    return ui->lineEditVisibilityGroup->text();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetVisibilityGroupName(const QString &name)
{
    ui->lineEditVisibilityGroup->setText(name.isEmpty() ? tr("Rotation") : name);
}

//---------------------------------------------------------------------------------------------------------------------
bool DialogMove::HasLinkedVisibilityGroup() const
{
    return ui->groupBoxVisibilityGroup->isChecked();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetHasLinkedVisibilityGroup(bool linked)
{
    ui->groupBoxVisibilityGroup->setChecked(linked);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetVisibilityGroupTags(const QStringList &tags)
{
    ui->lineEditGroupTags->setText(tags.join(", "));
}

//---------------------------------------------------------------------------------------------------------------------
QStringList DialogMove::GetVisibilityGroupTags() const
{
    return ui->lineEditGroupTags->text().split(',');
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetGroupCategories(const QStringList &categories)
{
    m_groupTags = categories;
    ui->lineEditGroupTags->SetCompletion(m_groupTags);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::ShowDialog(bool click)
{
    if (stage1 && not click)
    {
        if (sourceObjects.isEmpty())
        {
            return;
        }

        stage1 = false;
        prepare = true;

        VMainGraphicsScene *scene = qobject_cast<VMainGraphicsScene *>(qApp->getCurrentScene());
        SCASSERT(scene != nullptr)
        scene->clearSelection();

        VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
        SCASSERT(operation != nullptr)
        operation->SetObjects(SourceToObjects(sourceObjects));
        operation->VisualMode();

        VAbstractMainWindow *window = qobject_cast<VAbstractMainWindow *>(qApp->getMainWindow());
        SCASSERT(window != nullptr)
        connect(operation, &VisToolMove::ToolTip, window, &VAbstractMainWindow::ShowToolTip);

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
    }
    else if (not stage2 && not stage1 && prepare && click)
    {
        VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
        SCASSERT(operation != nullptr)

        if (operation->LengthValue() > 0)
        {
            formulaAngle = qApp->TrVars()->FormulaToUser(operation->Angle(), qApp->Settings()->GetOsSeparator());
            formulaLength = qApp->TrVars()->FormulaToUser(operation->Length(), qApp->Settings()->GetOsSeparator());
            operation->SetAngle(formulaAngle);
            operation->SetLength(formulaLength);
            operation->RefreshGeometry();
            emit ToolTip(operation->CurrentToolTip());
            stage2 = true;
        }
    }
    else if (not stage1 && stage2 && prepare && click)
    {
        VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
        SCASSERT(operation != nullptr)

        if (QGuiApplication::keyboardModifiers() == Qt::ControlModifier)
        {
            if (not optionalRotationOrigin)
            {
                operation->SetRotationOriginPointId(NULL_ID);
                SetObject(NULL_ID, ui->comboBoxRotationOriginPoint, QString());
                operation->RefreshGeometry();
            }
            optionalRotationOrigin = false; // Handled, next click on empty filed will disable selection
        }
        else
        {
            SetAngle(qApp->TrVars()->FormulaFromUser(formulaAngle, qApp->Settings()->GetOsSeparator()));
            SetLength(qApp->TrVars()->FormulaFromUser(formulaLength, qApp->Settings()->GetOsSeparator()));
            SetRotationAngle(operation->RotationAngle());
            setModal(true);
            emit ToolTip(QString());
            timerAngle->start();
            timerRotationAngle->start();
            timerLength->start();
            show();
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::ChosenObject(quint32 id, const SceneObject &type)
{
    if (not stage1 && stage2 && prepare)// After first choose we ignore all objects
    {
        if (type == SceneObject::Point && QGuiApplication::keyboardModifiers() == Qt::ControlModifier)
        {
            if (SetObject(id, ui->comboBoxRotationOriginPoint, QString()))
            {
                VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
                SCASSERT(operation != nullptr)

                operation->SetRotationOriginPointId(id);
                operation->RefreshGeometry();
                optionalRotationOrigin = true;
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SelectedObject(bool selected, quint32 object, quint32 tool)
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
void DialogMove::DeployAngleTextEdit()
{
    DeployFormula(this, ui->plainTextEditAngle, ui->pushButtonGrowAngle, formulaBaseHeightAngle);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::DeployRotationAngleTextEdit()
{
    DeployFormula(this, ui->plainTextEditRotationAngle, ui->pushButtonGrowRotationAngle, formulaBaseHeightRotationAngle);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::DeployLengthTextEdit()
{
    DeployFormula(this, ui->plainTextEditLength, ui->pushButtonGrowLength, formulaBaseHeightLength);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::FXAngle()
{
    DialogEditWrongFormula *dialog = new DialogEditWrongFormula(data, toolId, this);
    dialog->setWindowTitle(tr("Edit angle"));
    dialog->SetFormula(GetAngle());
    dialog->setPostfix(degreeSymbol);
    if (dialog->exec() == QDialog::Accepted)
    {
        SetAngle(dialog->GetFormula());
    }
    delete dialog;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::FXRotationAngle()
{
    DialogEditWrongFormula *dialog = new DialogEditWrongFormula(data, toolId, this);
    dialog->setWindowTitle(tr("Edit rotation angle"));
    dialog->SetFormula(GetRotationAngle());
    dialog->setPostfix(degreeSymbol);
    if (dialog->exec() == QDialog::Accepted)
    {
        SetRotationAngle(dialog->GetFormula());
    }
    delete dialog;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::FXLength()
{
    DialogEditWrongFormula *dialog = new DialogEditWrongFormula(data, toolId, this);
    dialog->setWindowTitle(tr("Edit length"));
    dialog->SetFormula(GetLength());
    dialog->setPostfix(UnitsToStr(qApp->patternUnits(), true));
    if (dialog->exec() == QDialog::Accepted)
    {
        SetLength(dialog->GetFormula());
    }
    delete dialog;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SuffixChanged()
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
void DialogMove::GroupNameChanged()
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
void DialogMove::ShowSourceDetails(int row)
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

    SetAliasValid(sourceItem.id, SourceAliasValid(sourceItem, obj, data,
                                                  OriginAlias(sourceItem.id, sourceObjects, obj)));
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::AliasChanged(const QString &text)
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
void DialogMove::ShowVisualization()
{
    AddVisualization<VisToolMove>();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SaveData()
{
    m_suffix = ui->lineEditSuffix->text();
    formulaAngle = ui->plainTextEditAngle->toPlainText();
    formulaRotationAngle = ui->plainTextEditRotationAngle->toPlainText();
    formulaLength = ui->plainTextEditLength->toPlainText();

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

    VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
    SCASSERT(operation != nullptr)

    operation->SetObjects(SourceToObjects(sourceObjects));
    operation->SetAngle(formulaAngle);
    operation->SetLength(formulaLength);
    operation->SetRotationAngle(formulaRotationAngle);
    operation->SetRotationOriginPointId(GetRotationOrigPointId());
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
void DialogMove::closeEvent(QCloseEvent *event)
{
    ui->plainTextEditAngle->blockSignals(true);
    ui->plainTextEditRotationAngle->blockSignals(true);
    ui->plainTextEditLength->blockSignals(true);
    DialogTool::closeEvent(event);
}

//---------------------------------------------------------------------------------------------------------------------
QVector<SourceItem> DialogMove::GetSourceObjects() const
{
    return sourceObjects;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetSourceObjects(const QVector<SourceItem> &value)
{
    sourceObjects = value;
    FillSourceList();

    VisToolMove *operation = qobject_cast<VisToolMove *>(vis);
    SCASSERT(operation != nullptr)
    operation->SetObjects(SourceToObjects(sourceObjects));
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::EvalAngle()
{
    FormulaData formulaData;
    formulaData.formula = ui->plainTextEditAngle->toPlainText();
    formulaData.variables = data->DataVariables();
    formulaData.labelEditFormula = ui->labelEditAngle;
    formulaData.labelResult = ui->labelResultAngle;
    formulaData.postfix = degreeSymbol;
    formulaData.checkZero = false;

    Eval(formulaData, flagAngle);

    if (not flagAngle)
    {
        ui->labelStatus->setText(tr("Invalid angle formula"));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::EvalRotationAngle()
{
    FormulaData formulaData;
    formulaData.formula = ui->plainTextEditRotationAngle->toPlainText();
    formulaData.variables = data->DataVariables();
    formulaData.labelEditFormula = ui->labelEditRotationAngle;
    formulaData.labelResult = ui->labelResultRotationAngle;
    formulaData.postfix = degreeSymbol;
    formulaData.checkZero = false;

    Eval(formulaData, flagRotationAngle);

    if (not flagRotationAngle)
    {
        ui->labelStatus->setText(tr("Invalid rotation angle formula"));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::EvalLength()
{
    FormulaData formulaData;
    formulaData.formula = ui->plainTextEditLength->toPlainText();
    formulaData.variables = data->DataVariables();
    formulaData.labelEditFormula = ui->labelEditLength;
    formulaData.labelResult = ui->labelResultLength;
    formulaData.postfix = UnitsToStr(qApp->patternUnits(), true);

    Eval(formulaData, flagLength);

    if (not flagLength)
    {
        ui->labelStatus->setText(tr("Invalid length formula"));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::FillSourceList()
{
    ui->listWidget->blockSignals(true);

    ui->listWidget->clear();

    int row = -1;

    for (auto &sourceItem : sourceObjects)
    {
        const QSharedPointer<VGObject> obj = data->GetGObject(sourceItem.id);
        bool valid = SourceAliasValid(sourceItem, obj, data, OriginAlias(sourceItem.id, sourceObjects, obj));

        auto *item = new QListWidgetItem(valid ? obj->ObjectName() : obj->ObjectName() + '*');
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
void DialogMove::ValidateSourceAliases()
{
    for (int i=0; i<ui->listWidget->count(); ++i)
    {
        if (const QListWidgetItem *item = ui->listWidget->item(i))
        {
            auto sourceItem = qvariant_cast<SourceItem>(item->data(Qt::UserRole));

            const QSharedPointer<VGObject> obj = data->GetGObject(sourceItem.id);

            if (not SourceAliasValid(sourceItem, obj, data, OriginAlias(sourceItem.id, sourceObjects, obj)))
            {
                flagAlias = false;
                ui->labelStatus->setText(obj->getType() == GOType::Point ? tr("Invalid label") : tr("Invalid alias"));
                SetAliasValid(sourceItem.id, false);
                CheckState();
                return;
            }
            else
            {
                SetAliasValid(sourceItem.id, true);
            }
        }
    }

    flagAlias = true;
    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetAliasValid(quint32 id, bool valid)
{
    if (ui->listWidget->currentRow() != -1)
    {
        auto *item = ui->listWidget->item(ui->listWidget->currentRow());
        const auto sourceItem = qvariant_cast<SourceItem>(item->data(Qt::UserRole));

        if (id == sourceItem.id)
        {
            const QSharedPointer<VGObject> obj = data->GetGObject(sourceItem.id);
            item->setText(valid ? obj->ObjectName() : obj->ObjectName() + '*');

            ChangeColor(ui->labelAlias, valid ? OkColor(this) : errorColor);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogMove::SetNotes(const QString &notes)
{
    ui->plainTextEditToolNotes->setPlainText(notes);
}

//---------------------------------------------------------------------------------------------------------------------
QString DialogMove::GetNotes() const
{
    return ui->plainTextEditToolNotes->toPlainText();
}

//---------------------------------------------------------------------------------------------------------------------
bool DialogMove::IsValid() const
{
    bool ready = flagAngle && flagRotationAngle && flagLength && flagName && flagGroupName && flagAlias;

    if (ready)
    {
        ui->labelStatus->setText(tr("Ready"));
    }

    return ready;
}
