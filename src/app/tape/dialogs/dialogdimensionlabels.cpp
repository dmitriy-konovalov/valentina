/************************************************************************
 **
 **  @file   dialogdimensionlabels.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   8 10, 2020
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
#include "dialogdimensionlabels.h"
#include "ui_dialogdimensionlabels.h"
#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
#include "../vmisc/backport/qoverload.h"
#endif // QT_VERSION < QT_VERSION_CHECK(5, 7, 0)

//---------------------------------------------------------------------------------------------------------------------
DialogDimensionLabels::DialogDimensionLabels(const QMap<MeasurementDimension, MeasurementDimension_p> &dimensions,
                                             bool fullCircumference, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::DialogDimensionLabels),
      m_dimensions(dimensions),
      m_fullCircumference(fullCircumference)
{
    ui->setupUi(this);

    InitLabels();

    InitDimensions();
    InitTable();

    connect(ui->comboBoxDimensionLabels, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &DialogDimensionLabels::DimensionChanged);
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &DialogDimensionLabels::LabelChanged);
}

//---------------------------------------------------------------------------------------------------------------------
DialogDimensionLabels::~DialogDimensionLabels()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
void DialogDimensionLabels::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        // retranslate designer form (single inheritance approach)
        ui->retranslateUi(this);

        MeasurementDimension type =
            static_cast<MeasurementDimension>(ui->comboBoxDimensionLabels->currentData().toInt());

        InitDimensions();

        int index = ui->comboBoxDimensionLabels->findData(static_cast<int>(type));
        if (index != -1)
        {
            ui->comboBoxDimensionLabels->blockSignals(true);
            ui->comboBoxDimensionLabels->setCurrentIndex(index);
            ui->comboBoxDimensionLabels->blockSignals(false);
        }

        const int row = ui->tableWidget->currentRow();
        InitTable();
        ui->tableWidget->blockSignals(true);
        ui->tableWidget->selectRow(row);
        ui->tableWidget->blockSignals(false);
    }

    // remember to call base class implementation
    QDialog::changeEvent(event);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogDimensionLabels::DimensionChanged()
{
    InitTable();
    ui->tableWidget->selectRow(0);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogDimensionLabels::LabelChanged(QTableWidgetItem *item)
{
    if (item != nullptr)
    {
        MeasurementDimension type =
            static_cast<MeasurementDimension>(ui->comboBoxDimensionLabels->currentData().toInt());
        qreal value = item->data(Qt::UserRole).toDouble();

        DimesionLabels labels = m_labels.value(type);
        labels.insert(value, item->text());

        m_labels.insert(type, labels);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogDimensionLabels::InitLabels()
{
    m_labels.clear();

    const QList<MeasurementDimension_p> dimensions = m_dimensions.values();

    for(const auto &dimension : dimensions)
    {
        m_labels.insert(dimension->Type(), dimension->Labels());
    }
}

//---------------------------------------------------------------------------------------------------------------------
void DialogDimensionLabels::InitDimensions()
{
    ui->comboBoxDimensionLabels->blockSignals(true);
    ui->comboBoxDimensionLabels->clear();

    for(auto &dimension : m_dimensions)
    {
        ui->comboBoxDimensionLabels->addItem(dimension->Name(), static_cast<int>(dimension->Type()));
    }

    ui->comboBoxDimensionLabels->blockSignals(false);
}

//---------------------------------------------------------------------------------------------------------------------
void DialogDimensionLabels::InitTable()
{
    ui->tableWidget->blockSignals(true);
    ui->tableWidget->clearContents();

    const MeasurementDimension type =
        static_cast<MeasurementDimension>(ui->comboBoxDimensionLabels->currentData().toInt());

    MeasurementDimension_p dimension;

    if (m_dimensions.contains(type))
    {
        dimension = m_dimensions.value(type);
    }

    if (dimension.isNull())
    {
        return;
    }

    const QVector<qreal> bases = dimension->ValidBases();

    ui->tableWidget->setRowCount(static_cast<int>(bases.size()));

    const DimesionLabels labels = m_labels.value(type);

    for(int row = 0; row < bases.size(); ++row)
    {
        const qreal base = bases.at(row);

        {
            auto *itemValue = new QTableWidgetItem(DimensionValue(dimension, base));
            itemValue->setData(Qt::UserRole, base);
            itemValue->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);

            // set the item non-editable (view only), and non-selectable
            Qt::ItemFlags flags = itemValue->flags();
            flags &= ~(Qt::ItemIsEditable); // reset/clear the flag
            itemValue->setFlags(flags);

            ui->tableWidget->setItem(row, 0, itemValue);
        }

        {
            auto *itemLabel = new QTableWidgetItem(VFuzzyValue(labels, base));
            itemLabel->setData(Qt::UserRole, base);
            itemLabel->setTextAlignment(Qt::AlignHCenter | Qt::AlignCenter);

            ui->tableWidget->setItem(row, 1, itemLabel);
        }
    }

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->blockSignals(false);
}

//---------------------------------------------------------------------------------------------------------------------
auto DialogDimensionLabels::DimensionValue(const MeasurementDimension_p &dimension, qreal value) const -> QString
{
    if (dimension->Type() == MeasurementDimension::X)
    {
        return QString::number(value);
    }

    if (dimension->Type() == MeasurementDimension::Y)
    {
        if (dimension->IsBodyMeasurement())
        {
            return QString::number(m_fullCircumference ? value*2 : value);
        }

        return QString::number(value);

    }

    if (dimension->Type() == MeasurementDimension::W || dimension->Type() == MeasurementDimension::Z)
    {
        return QString::number(m_fullCircumference ? value*2 : value);
    }

    return QString::number(value);
}
