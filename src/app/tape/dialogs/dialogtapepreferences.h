/************************************************************************
 **
 **  @file   dialogtapepreferences.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   12 4, 2017
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2017 Valentina project
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

#ifndef DIALOGTAPEPREFERENCES_H
#define DIALOGTAPEPREFERENCES_H

#include <QDialog>

#include "../vmisc/defglobal.h"

namespace Ui
{
    class DialogTapePreferences;
}

class TapePreferencesConfigurationPage;
class TapePreferencesPathPage;
class QListWidgetItem;

class DialogTapePreferences : public QDialog
{
    Q_OBJECT // NOLINT

public:
    explicit DialogTapePreferences(QWidget *parent = nullptr);
    ~DialogTapePreferences() override;
signals:
    void UpdateProperties();
protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void changeEvent(QEvent* event) override;
private slots:
    void Apply();
    void Ok();
    void PageChanged(QListWidgetItem *current, QListWidgetItem *previous);
private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(DialogTapePreferences) // NOLINT
    Ui::DialogTapePreferences *ui;
    bool m_isInitialized{false};
    TapePreferencesConfigurationPage *m_configurationPage;
    TapePreferencesPathPage          *m_pathPage;
};

#endif // DIALOGTAPEPREFERENCES_H
