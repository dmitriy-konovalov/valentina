/************************************************************************
 **
 **  @file   dialogsetupmultisize.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   26 9, 2020
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
#include "dialogsetupmultisize.h"
#include "ui_dialogsetupmultisize.h"

#include <QPushButton>
#include <QShowEvent>

#include "../mapplication.h"
#include "../vmisc/backport/qoverload.h"

//---------------------------------------------------------------------------------------------------------------------
DialogSetupMultisize::DialogSetupMultisize(Unit unit, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetupMultisize),
    m_xDimension(QSharedPointer<VXMeasurementDimension>::create(unit)),
    m_yDimension(QSharedPointer<VYMeasurementDimension>::create(unit)),
    m_wDimension(QSharedPointer<VWMeasurementDimension>::create(unit)),
    m_zDimension(QSharedPointer<VZMeasurementDimension>::create(unit))
{
    ui->setupUi(this);

    ui->labelError->clear();
    ui->checkBoxYDimensionCircumference->setChecked(m_yDimension->IsCircumference());

    InitXDimension();
    InitYDimension();
    InitWDimension();
    InitZDimension();

    // height
    connect(ui->doubleSpinBoxXDimensionMinValue, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
            this, [this](qreal value)
            {
                DimensionMinValueChanged(value, ui->doubleSpinBoxXDimensionMaxValue, ui->comboBoxXDimensionStep,
                                         ui->comboBoxXDimensionBase, m_xDimension);
            });
    connect(ui->doubleSpinBoxXDimensionMaxValue, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
            this, [this](qreal value)
            {
                DimensionMaxValueChanged(value, ui->doubleSpinBoxXDimensionMinValue, ui->comboBoxXDimensionStep,
                                         ui->comboBoxXDimensionBase, m_xDimension);
            });
    connect(ui->comboBoxXDimensionStep, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index)
            {
                DimensionStepChanged(index, ui->comboBoxXDimensionStep, ui->comboBoxXDimensionBase, m_xDimension);
            });
    connect(ui->comboBoxXDimensionBase, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index)
            {
                DimensionBaseChanged(index, ui->comboBoxXDimensionBase, m_xDimension);
            });

    // size
    connect(ui->doubleSpinBoxYDimensionMinValue, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
            this, [this](qreal value)
            {
                DimensionMinValueChanged(value, ui->doubleSpinBoxYDimensionMaxValue, ui->comboBoxYDimensionStep,
                                         ui->comboBoxYDimensionBase, m_yDimension);
            });
    connect(ui->doubleSpinBoxYDimensionMaxValue, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
            this, [this](qreal value)
            {
                DimensionMaxValueChanged(value, ui->doubleSpinBoxYDimensionMinValue, ui->comboBoxYDimensionStep,
                                         ui->comboBoxYDimensionBase, m_yDimension);
            });
    connect(ui->comboBoxYDimensionStep, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index)
            {
                DimensionStepChanged(index, ui->comboBoxYDimensionStep, ui->comboBoxYDimensionBase, m_yDimension);
            });
    connect(ui->comboBoxYDimensionBase, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index)
            {
                DimensionBaseChanged(index, ui->comboBoxYDimensionBase, m_yDimension);
            });

    // hip
    connect(ui->doubleSpinBoxWDimensionMinValue, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
            this, [this](qreal value)
            {
                DimensionMinValueChanged(value, ui->doubleSpinBoxWDimensionMaxValue, ui->comboBoxWDimensionStep,
                                         ui->comboBoxWDimensionBase, m_wDimension);
            });
    connect(ui->doubleSpinBoxWDimensionMaxValue, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
            this, [this](qreal value)
            {
                DimensionMaxValueChanged(value, ui->doubleSpinBoxWDimensionMinValue, ui->comboBoxWDimensionStep,
                                         ui->comboBoxWDimensionBase, m_wDimension);
            });
    connect(ui->comboBoxWDimensionStep, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index)
            {
                DimensionStepChanged(index, ui->comboBoxWDimensionStep, ui->comboBoxWDimensionBase, m_wDimension);
            });
    connect(ui->comboBoxWDimensionBase, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index)
            {
                DimensionBaseChanged(index, ui->comboBoxWDimensionBase, m_wDimension);
            });

    // waist
    connect(ui->doubleSpinBoxZDimensionMinValue, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
            this, [this](qreal value)
            {
                DimensionMinValueChanged(value, ui->doubleSpinBoxZDimensionMaxValue, ui->comboBoxZDimensionStep,
                                         ui->comboBoxZDimensionBase, m_zDimension);
            });
    connect(ui->doubleSpinBoxZDimensionMaxValue, QOverload<qreal>::of(&QDoubleSpinBox::valueChanged),
            this, [this](qreal value)
            {
                DimensionMaxValueChanged(value, ui->doubleSpinBoxZDimensionMinValue, ui->comboBoxZDimensionStep,
                                         ui->comboBoxZDimensionBase, m_zDimension);
            });
    connect(ui->comboBoxZDimensionStep, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index)
            {
                DimensionStepChanged(index, ui->comboBoxZDimensionStep, ui->comboBoxZDimensionBase, m_zDimension);
            });
    connect(ui->comboBoxZDimensionBase, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index)
            {
                DimensionBaseChanged(index, ui->comboBoxZDimensionBase, m_zDimension);
            });


    connect(ui->groupBoxXDimension, &QGroupBox::clicked, this, [this](){CheckState();});
    connect(ui->groupBoxYDimension, &QGroupBox::clicked, this, [this](){CheckState();});
    connect(ui->groupBoxWDimension, &QGroupBox::clicked, this, [this](){CheckState();});
    connect(ui->groupBoxZDimension, &QGroupBox::clicked, this, [this](){CheckState();});

    connect(ui->checkBoxFullCircumference, &QCheckBox::stateChanged,
            this, &DialogSetupMultisize::ShowFullCircumference);
    connect(ui->checkBoxYDimensionCircumference, &QCheckBox::stateChanged,
            this, &DialogSetupMultisize::YDimensionCircumferenceChanged);

    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
DialogSetupMultisize::~DialogSetupMultisize()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
auto DialogSetupMultisize::Dimensions() const -> QVector<MeasurementDimension_p>
{
    QVector<MeasurementDimension_p> dimensions;

    if (ui->groupBoxXDimension->isChecked())
    {
        dimensions.append(m_xDimension);
    }

    if (ui->groupBoxYDimension->isChecked())
    {
        dimensions.append(m_yDimension);
    }

    if (ui->groupBoxWDimension->isChecked())
    {
        dimensions.append(m_wDimension);
    }

    if (ui->groupBoxZDimension->isChecked())
    {
        dimensions.append(m_zDimension);
    }

    return dimensions;
}

//---------------------------------------------------------------------------------------------------------------------
auto DialogSetupMultisize::FullCircumference() const -> bool
{
    return ui->checkBoxFullCircumference->isChecked();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        // retranslate designer form (single inheritance approach)
        ui->retranslateUi(this);
    }

    // remember to call base class implementation
    QDialog::changeEvent(event);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::showEvent(QShowEvent *event)
{
    QDialog::showEvent( event );
    if ( event->spontaneous() )
    {
        return;
    }

    if (m_isInitialized)
    {
        return;
    }
    // do your init stuff here

    setMaximumSize(size());
    setMinimumSize(size());

    m_isInitialized = true;//first show windows are held
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::ShowFullCircumference()
{
    auto ShowDimensionFullCircumference = [this](QDoubleSpinBox *doubleSpinBoxMinValue,
            QDoubleSpinBox *doubleSpinBoxMaxValue, QComboBox *comboBoxStep, QComboBox *comboBoxBase,
            const MeasurementDimension_p &dimension)
    {
        SCASSERT(doubleSpinBoxMinValue != nullptr)
        SCASSERT(doubleSpinBoxMaxValue != nullptr)
        SCASSERT(comboBoxStep != nullptr)
        SCASSERT(comboBoxBase != nullptr)

        InitDimension(doubleSpinBoxMinValue, doubleSpinBoxMaxValue, comboBoxStep, dimension);
        UpdateBase(comboBoxBase, dimension);

        comboBoxBase->blockSignals(true);
        comboBoxBase->setCurrentIndex(-1);
        comboBoxBase->blockSignals(false);
    };

    ShowDimensionFullCircumference(ui->doubleSpinBoxYDimensionMinValue, ui->doubleSpinBoxYDimensionMaxValue,
                                   ui->comboBoxYDimensionStep, ui->comboBoxYDimensionBase, m_yDimension);
    ShowDimensionFullCircumference(ui->doubleSpinBoxWDimensionMinValue, ui->doubleSpinBoxWDimensionMaxValue,
                                   ui->comboBoxWDimensionStep, ui->comboBoxWDimensionBase, m_wDimension);
    ShowDimensionFullCircumference(ui->doubleSpinBoxZDimensionMinValue, ui->doubleSpinBoxZDimensionMaxValue,
                                   ui->comboBoxZDimensionStep, ui->comboBoxZDimensionBase, m_zDimension);

    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::YDimensionCircumferenceChanged()
{
    bool checked = ui->checkBoxYDimensionCircumference->isChecked();
    m_yDimension->SetCircumference(checked);

    const bool c = m_yDimension->IsCircumference();
    const QString unitStr = c ? " " + UnitsToStr(m_yDimension->Units()) : QString();

    ui->doubleSpinBoxYDimensionMinValue->setSuffix(unitStr);
    ui->doubleSpinBoxYDimensionMaxValue->setSuffix(unitStr);

    InitDimension(ui->doubleSpinBoxYDimensionMinValue, ui->doubleSpinBoxYDimensionMaxValue, ui->comboBoxYDimensionStep,
                  m_yDimension);

    UpdateBase(ui->comboBoxYDimensionBase, m_yDimension);

    ui->comboBoxYDimensionBase->blockSignals(true);
    ui->comboBoxYDimensionBase->setCurrentIndex(-1);
    ui->comboBoxYDimensionBase->blockSignals(false);

    bool ok = false;
    const qreal base = ui->comboBoxYDimensionBase->currentData().toDouble(&ok);
    m_yDimension->SetBaseValue(ok ? base : -1);

    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::CheckState()
{
    ui->labelError->clear();

    bool xDimensionValid = true;
    bool yDimensionValid = true;
    bool wDimensionValid = true;
    bool zDimensionValid = true;

    int dimensions = 0;

    auto CheckDimension = [this](QGroupBox *group, bool &dimensionValid, int &dimensions,
                                 const MeasurementDimension_p &dimension)
    {
        SCASSERT(group != nullptr)

        if (group->isChecked())
        {
            dimensionValid = dimension->IsValid();
            ++dimensions;

            if (ui->labelError->text().isEmpty() && not dimensionValid)
            {
                ui->labelError->setText(tr("Please, provide correct data for dimensions"));
            }
        }
    };

    CheckDimension(ui->groupBoxXDimension, xDimensionValid, dimensions, m_xDimension);
    CheckDimension(ui->groupBoxYDimension, yDimensionValid, dimensions, m_yDimension);
    CheckDimension(ui->groupBoxWDimension, wDimensionValid, dimensions, m_wDimension);
    CheckDimension(ui->groupBoxZDimension, zDimensionValid, dimensions, m_zDimension);

    if (ui->labelError->text().isEmpty() && dimensions == 0)
    {
        ui->labelError->setText(tr("Please, select at least one dimension"));
    }
    else if (ui->labelError->text().isEmpty() && dimensions > 3)
    {
        ui->labelError->setText(tr("No more than 3 dimensions allowed"));
    }

    const bool enough = dimensions > 0 && dimensions <= 3;

    QPushButton *bOk = ui->buttonBox->button(QDialogButtonBox::Ok);
    SCASSERT(bOk != nullptr)
    bOk->setEnabled(enough && xDimensionValid && yDimensionValid && wDimensionValid && zDimensionValid);

    if (ui->labelError->text().isEmpty())
    {
        ui->labelError->setText(tr("Ready"));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::InitDimensionMinMax(QDoubleSpinBox *doubleSpinBoxMinValue,
                                               QDoubleSpinBox *doubleSpinBoxMaxValue,
                                               const MeasurementDimension_p &dimension)
{
    SCASSERT(doubleSpinBoxMinValue != nullptr)
    SCASSERT(doubleSpinBoxMaxValue != nullptr)

    dimension->SetMinValue(dimension->RangeMin());
    dimension->SetMaxValue(dimension->RangeMax());

    const bool fc = ui->checkBoxFullCircumference->isChecked();
    const bool c = dimension->IsCircumference();

    doubleSpinBoxMinValue->blockSignals(true);
    const QString unitStr = " " + UnitsToStr(dimension->Units());
    if (c || dimension->Type() == MeasurementDimension::X)
    {
        doubleSpinBoxMinValue->setSuffix(unitStr);
    }

    doubleSpinBoxMinValue->setDecimals(dimension->Units() == Unit::Mm ? 0 : 1);
    doubleSpinBoxMinValue->setMinimum(c && fc ? dimension->RangeMin()*2 : dimension->RangeMin());
    doubleSpinBoxMinValue->setMaximum(c && fc ? dimension->MaxValue()*2 : dimension->MaxValue());
    doubleSpinBoxMinValue->setValue(c && fc ? dimension->MinValue()*2 : dimension->MinValue());
    doubleSpinBoxMinValue->blockSignals(false);

    doubleSpinBoxMaxValue->blockSignals(true);
    if (c || dimension->Type() == MeasurementDimension::X)
    {
        doubleSpinBoxMaxValue->setSuffix(unitStr);
    }

    doubleSpinBoxMaxValue->setDecimals(dimension->Units() == Unit::Mm ? 0 : 1);
    doubleSpinBoxMaxValue->setMinimum(c && fc ? dimension->MinValue()*2 : dimension->MinValue());
    doubleSpinBoxMaxValue->setMaximum(c && fc ? dimension->RangeMax()*2 : dimension->RangeMax());
    doubleSpinBoxMaxValue->setValue(c && fc ? dimension->RangeMax()*2 : dimension->RangeMax());
    doubleSpinBoxMaxValue->setValue(c && fc ? dimension->MaxValue()*2 : dimension->MaxValue());
    doubleSpinBoxMaxValue->blockSignals(false);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::InitDimensionStep(QComboBox *comboBoxStep,
                                             const MeasurementDimension_p &dimension)
{
    SCASSERT(comboBoxStep != nullptr)

    const bool fc = ui->checkBoxFullCircumference->isChecked();
    const bool c = dimension->IsCircumference();
    const QString unitStr = " " + UnitsToStr(dimension->Units());

    dimension->SetStep(-1);

    comboBoxStep->blockSignals(true);
    const QVector<qreal> steps = dimension->ValidSteps();
    comboBoxStep->clear();
    for(auto step : steps)
    {
        comboBoxStep->addItem(QStringLiteral("%1%2").arg(c && fc ? step*2 : step)
                                  .arg(c || dimension->Type() == MeasurementDimension::X ? unitStr : QString()), step);
    }

    comboBoxStep->setCurrentIndex(-1); // force a user to select
    comboBoxStep->blockSignals(false);

    bool ok = false;
    const qreal step = comboBoxStep->currentData().toDouble(&ok);
    dimension->SetStep(ok ? step : -1);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::InitDimension(QDoubleSpinBox *doubleSpinBoxMinValue, QDoubleSpinBox *doubleSpinBoxMaxValue,
                                         QComboBox *comboBoxStep, const MeasurementDimension_p &dimension)
{
    InitDimensionMinMax(doubleSpinBoxMinValue, doubleSpinBoxMaxValue, dimension);
    InitDimensionStep(comboBoxStep, dimension);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::InitXDimension()
{
    InitDimension(ui->doubleSpinBoxXDimensionMinValue, ui->doubleSpinBoxXDimensionMaxValue, ui->comboBoxXDimensionStep,
                  m_xDimension);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::InitYDimension()
{
    InitDimension(ui->doubleSpinBoxYDimensionMinValue, ui->doubleSpinBoxYDimensionMaxValue, ui->comboBoxYDimensionStep,
                  m_yDimension);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::InitWDimension()
{
    InitDimension(ui->doubleSpinBoxWDimensionMinValue, ui->doubleSpinBoxWDimensionMaxValue, ui->comboBoxWDimensionStep,
                  m_wDimension);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::InitZDimension()
{
    InitDimension(ui->doubleSpinBoxZDimensionMinValue, ui->doubleSpinBoxZDimensionMaxValue, ui->comboBoxZDimensionStep,
                  m_zDimension);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::DimensionMinValueChanged(qreal value, QDoubleSpinBox *doubleSpinBoxMaxValue,
                                                    QComboBox *comboBoxStep, QComboBox *comboBoxBase,
                                                    const MeasurementDimension_p &dimension)
{
    SCASSERT(doubleSpinBoxMaxValue != nullptr)
    SCASSERT(comboBoxStep != nullptr)
    SCASSERT(comboBoxBase != nullptr)

    const bool fc = ui->checkBoxFullCircumference->isChecked();
    const bool c = dimension->IsCircumference();

    dimension->SetMinValue(c && fc ? value / 2 : value);

    doubleSpinBoxMaxValue->blockSignals(true);
    doubleSpinBoxMaxValue->setMinimum(value);
    doubleSpinBoxMaxValue->blockSignals(false);

    dimension->SetMaxValue(c && fc ? doubleSpinBoxMaxValue->value() / 2 : doubleSpinBoxMaxValue->value());

    UpdateSteps(comboBoxStep, dimension);
    UpdateBase(comboBoxBase, dimension);

    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::DimensionMaxValueChanged(qreal value, QDoubleSpinBox *doubleSpinBoxMinValue,
                                                    QComboBox *comboBoxStep, QComboBox *comboBoxBase,
                                                    const MeasurementDimension_p &dimension)
{
    SCASSERT(doubleSpinBoxMinValue != nullptr)
    SCASSERT(comboBoxStep != nullptr)
    SCASSERT(comboBoxBase != nullptr)

    const bool fc = ui->checkBoxFullCircumference->isChecked();
    const bool c = dimension->IsCircumference();

    dimension->SetMaxValue(c && fc ? value / 2 : value);

    doubleSpinBoxMinValue->blockSignals(true);
    doubleSpinBoxMinValue->setMaximum(value);
    doubleSpinBoxMinValue->blockSignals(false);

    dimension->SetMinValue(c && fc ? doubleSpinBoxMinValue->value() / 2 : doubleSpinBoxMinValue->value());

    UpdateSteps(comboBoxStep, dimension);
    UpdateBase(comboBoxBase, dimension);

    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::DimensionStepChanged(int index, QComboBox *comboBoxStep, QComboBox *comboBoxBase,
                                                const MeasurementDimension_p &dimension)
{
    SCASSERT(comboBoxStep != nullptr)
    SCASSERT(comboBoxBase != nullptr)

    bool ok = false;
    const qreal step = comboBoxStep->itemData(index).toDouble(&ok);
    dimension->SetStep(ok ? step : -1);

    UpdateBase(comboBoxBase, dimension);

    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::DimensionBaseChanged(int index, QComboBox *comboBoxBase,
                                                const MeasurementDimension_p &dimension)
{
    SCASSERT(comboBoxBase != nullptr)

    bool ok = false;
    const qreal base = comboBoxBase->itemData(index).toDouble(&ok);
    dimension->SetBaseValue(ok ? base : -1);

    CheckState();
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::UpdateSteps(QComboBox *comboBoxStep,
                                       const MeasurementDimension_p &dimension)
{
    SCASSERT(comboBoxStep != nullptr)

    qreal oldStep = -1;
    if (comboBoxStep->currentIndex() != -1)
    {
        oldStep = comboBoxStep->currentData().toDouble();
    }

    comboBoxStep->blockSignals(true);

    const QString unitStr = " " + UnitsToStr(dimension->Units());
    const QVector<qreal> steps = dimension->ValidSteps();
    comboBoxStep->clear();

    const bool fc = ui->checkBoxFullCircumference->isChecked();
    const bool c = dimension->IsCircumference();

    for(auto step : steps)
    {
        comboBoxStep->addItem(QStringLiteral("%1%2").arg(c && fc ? step * 2 : step)
                                  .arg(c ? unitStr : QString()), step);
    }

    comboBoxStep->setCurrentIndex(comboBoxStep->findData(oldStep));
    comboBoxStep->blockSignals(false);

    bool ok = false;
    const qreal step = comboBoxStep->currentData().toDouble(&ok);
    dimension->SetStep(ok ? step : -1);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogSetupMultisize::UpdateBase(QComboBox *comboBoxBase,
                                      const MeasurementDimension_p &dimension)
{
    SCASSERT(comboBoxBase != nullptr)

    qreal oldBase = -1;
    if (comboBoxBase->currentIndex() != -1)
    {
        oldBase = comboBoxBase->currentData().toDouble();
    }

    comboBoxBase->blockSignals(true);

    const QString unitStr = " " + UnitsToStr(dimension->Units());
    const QVector<qreal> bases = dimension->ValidBases();
    comboBoxBase->clear();
    const bool fc = ui->checkBoxFullCircumference->isChecked();
    const bool c = dimension->IsCircumference();

    for(auto base : bases)
    {
        comboBoxBase->addItem(QStringLiteral("%1%2").arg(c && fc ? base * 2 : base)
                                  .arg(c || dimension->Type() == MeasurementDimension::X ? unitStr : QString()), base);
    }

    comboBoxBase->setCurrentIndex(comboBoxBase->findData(oldBase));
    comboBoxBase->blockSignals(false);

    bool ok = false;
    const qreal base = comboBoxBase->currentData().toDouble(&ok);
    dimension->SetBaseValue(ok ? base : -1);
}

