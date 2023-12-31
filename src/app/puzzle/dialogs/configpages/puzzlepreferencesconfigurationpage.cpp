/************************************************************************
 **
 **  @file   puzzlepreferencesconfigurationpage.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   21 5, 2021
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

#include "puzzlepreferencesconfigurationpage.h"
#include "../../vpapplication.h"
#include "../vganalytics/vganalytics.h"
#include "../vmisc/theme/vtheme.h"
#include "ui_puzzlepreferencesconfigurationpage.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
#include "../vmisc/backport/qoverload.h"
#endif // QT_VERSION < QT_VERSION_CHECK(5, 7, 0)

#include <QStyleHints>

//---------------------------------------------------------------------------------------------------------------------
PuzzlePreferencesConfigurationPage::PuzzlePreferencesConfigurationPage(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::PuzzlePreferencesConfigurationPage)
{
    ui->setupUi(this);

    InitLanguages(ui->langCombo);
    connect(ui->langCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [this]() { m_langChanged = true; });

    VPSettings *settings = VPApplication::VApp()->PuzzleSettings();

    // Theme
    SetThemeModeComboBox();
    int index = ui->comboBoxThemeMode->findData(static_cast<int>(settings->GetThemeMode()));
    if (index != -1)
    {
        ui->comboBoxThemeMode->setCurrentIndex(index);
    }

    // Native dialogs
    ui->checkBoxDontUseNativeDialog->setChecked(settings->IsDontUseNativeDialog());

    //----------------------- Toolbar
    ui->toolBarStyleCheck->setChecked(settings->GetToolBarStyle());

    // Undo
    ui->undoCount->setValue(settings->GetUndoCount());

    // Graphical output
    ui->graphOutputCheck->setChecked(settings->GetGraphicalOutput());
    ui->checkBoxOpenGLRender->setChecked(settings->IsOpenGLRender());

    // Font
    ui->checkBoxSingleStrokeOutlineFont->setChecked(settings->GetSingleStrokeOutlineFont());
    ui->checkBoxSingleLineFonts->setChecked(settings->GetSingleLineFonts());

    //----------------------- Update
    ui->checkBoxAutomaticallyCheckUpdates->setChecked(settings->IsAutomaticallyCheckUpdates());

    // Tab Scrolling
    ui->spinBoxDuration->setMinimum(VCommonSettings::scrollingDurationMin);
    ui->spinBoxDuration->setMaximum(VCommonSettings::scrollingDurationMax);
    ui->spinBoxDuration->setValue(settings->GetScrollingDuration());

    ui->spinBoxUpdateInterval->setMinimum(VCommonSettings::scrollingUpdateIntervalMin);
    ui->spinBoxUpdateInterval->setMaximum(VCommonSettings::scrollingUpdateIntervalMax);
    ui->spinBoxUpdateInterval->setValue(settings->GetScrollingUpdateInterval());

    ui->doubleSpinBoxSensor->setMinimum(VCommonSettings::sensorMouseScaleMin);
    ui->doubleSpinBoxSensor->setMaximum(VCommonSettings::sensorMouseScaleMax);
    ui->doubleSpinBoxSensor->setValue(settings->GetSensorMouseScale());

    ui->doubleSpinBoxWheel->setMinimum(VCommonSettings::wheelMouseScaleMin);
    ui->doubleSpinBoxWheel->setMaximum(VCommonSettings::wheelMouseScaleMax);
    ui->doubleSpinBoxWheel->setValue(settings->GetWheelMouseScale());

    ui->doubleSpinBoxAcceleration->setMinimum(VCommonSettings::scrollingAccelerationMin);
    ui->doubleSpinBoxAcceleration->setMaximum(VCommonSettings::scrollingAccelerationMax);
    ui->doubleSpinBoxAcceleration->setValue(settings->GetScrollingAcceleration());

    // Tab Privacy
    ui->checkBoxSendUsageStatistics->setChecked(settings->IsCollectStatistic());
}

//---------------------------------------------------------------------------------------------------------------------
PuzzlePreferencesConfigurationPage::~PuzzlePreferencesConfigurationPage()
{
    delete ui;
}

//---------------------------------------------------------------------------------------------------------------------
auto PuzzlePreferencesConfigurationPage::Apply() -> QStringList
{
    QStringList preferences;
    VPSettings *settings = VPApplication::VApp()->PuzzleSettings();

    settings->SetToolBarStyle(ui->toolBarStyleCheck->isChecked());

    auto themeMode = static_cast<VThemeMode>(ui->comboBoxThemeMode->currentData().toInt());
    if (settings->GetThemeMode() != themeMode)
    {
        QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        if (themeMode == VThemeMode::System && VTheme::NativeDarkThemeAvailable())
        {
            if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark)
            {
                settings->SetThemeMode(VThemeMode::Light);
            }
            else
            {
                settings->SetThemeMode(VThemeMode::Dark);
            }

            VTheme::Instance()->ResetThemeSettings();
            QCoreApplication::processEvents();
        }
#endif

        settings->SetThemeMode(themeMode);
        VTheme::Instance()->ResetThemeSettings();
        QGuiApplication::restoreOverrideCursor();
    }

    if (settings->IsDontUseNativeDialog() != ui->checkBoxDontUseNativeDialog->isChecked())
    {
        settings->SetDontUseNativeDialog(ui->checkBoxDontUseNativeDialog->isChecked());
    }

    if (m_langChanged)
    {
        const auto locale = qvariant_cast<QString>(ui->langCombo->currentData());
        settings->SetLocale(locale);
        VGAnalytics::Instance()->SetGUILanguage(settings->GetLocale());
        m_langChanged = false;

        VAbstractApplication::VApp()->LoadTranslation(locale);
        QCoreApplication::processEvents(); // force to call changeEvent
    }

    /* Maximum number of commands in undo stack may only be set when the undo stack is empty, since setting it on a
     * non-empty stack might delete the command at the current index. Calling setUndoLimit() on a non-empty stack
     * prints a warning and does nothing.*/
    if (settings->GetUndoCount() != ui->undoCount->value())
    {
        preferences.append(tr("undo limit"));
        settings->SetUndoCount(ui->undoCount->value());
    }

    // Scene antialiasing
    if (settings->GetGraphicalOutput() != ui->graphOutputCheck->isChecked())
    {
        preferences.append(tr("antialiasing"));
        settings->SetGraphicalOutput(ui->graphOutputCheck->isChecked());
    }

    if (settings->IsOpenGLRender() != ui->checkBoxOpenGLRender->isChecked())
    {
        preferences.append(tr("scene render"));
        settings->SetOpenGLRender(ui->checkBoxOpenGLRender->isChecked());
    }

    if (settings->IsAutomaticallyCheckUpdates() != ui->checkBoxAutomaticallyCheckUpdates->isChecked())
    {
        settings->SetAutomaticallyCheckUpdates(ui->checkBoxAutomaticallyCheckUpdates->isChecked());
    }

    if (settings->GetSingleStrokeOutlineFont() != ui->checkBoxSingleStrokeOutlineFont->isChecked())
    {
        settings->SetSingleStrokeOutlineFont(ui->checkBoxSingleStrokeOutlineFont->isChecked());
    }
    settings->SetSingleLineFonts(ui->checkBoxSingleLineFonts->isChecked());

    // Tab Scrolling
    settings->SetScrollingDuration(ui->spinBoxDuration->value());
    settings->SetScrollingUpdateInterval(ui->spinBoxUpdateInterval->value());
    settings->SetSensorMouseScale(ui->doubleSpinBoxSensor->value());
    settings->SetWheelMouseScale(ui->doubleSpinBoxWheel->value());
    settings->SetScrollingAcceleration(ui->doubleSpinBoxAcceleration->value());

    // Tab Privacy
    settings->SetCollectStatistic(ui->checkBoxSendUsageStatistics->isChecked());
    VGAnalytics::Instance()->Enable(ui->checkBoxSendUsageStatistics->isChecked());

    return preferences;
}

//---------------------------------------------------------------------------------------------------------------------
void PuzzlePreferencesConfigurationPage::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        // retranslate designer form (single inheritance approach)
        ui->retranslateUi(this);
    }
    // remember to call base class implementation
    QWidget::changeEvent(event);
}

//---------------------------------------------------------------------------------------------------------------------
void PuzzlePreferencesConfigurationPage::SetThemeModeComboBox()
{
    ui->comboBoxThemeMode->clear();
    ui->comboBoxThemeMode->addItem(tr("System", "theme"), static_cast<int>(VThemeMode::System));
    ui->comboBoxThemeMode->addItem(tr("Dark", "theme"), static_cast<int>(VThemeMode::Dark));
    ui->comboBoxThemeMode->addItem(tr("Light", "theme"), static_cast<int>(VThemeMode::Light));
}
