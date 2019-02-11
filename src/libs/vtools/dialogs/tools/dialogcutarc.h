/************************************************************************
 **
 **  @file   dialogcutarc.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   7 1, 2014
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Valentina project
 **  <https://bitbucket.org/dismine/valentina> All Rights Reserved.
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

#ifndef DIALOGCUTARC_H
#define DIALOGCUTARC_H

#include <qcompilerdetection.h>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "../vmisc/def.h"
#include "dialogtool.h"

namespace Ui
{
    class DialogCutArc;
}

/**
 * @brief The DialogCutArc class dialog for ToolCutArc.
 */
class DialogCutArc : public DialogTool
{
    Q_OBJECT
public:

    DialogCutArc(const VContainer *data, quint32 toolId, QWidget *parent = nullptr);
    virtual ~DialogCutArc() override;

    QString           GetPointName() const;
    void              SetPointName(const QString &value);

    QString           GetFormula() const;
    void              SetFormula(const QString &value);

    quint32           getArcId() const;
    void              setArcId(quint32 value);
public slots:
    virtual void      ChosenObject(quint32 id, const SceneObject &type) override;
    /**
     * @brief DeployFormulaTextEdit grow or shrink formula input
     */
    void              DeployFormulaTextEdit();

    void              FXLength();
    void              EvalFormula();
protected:
    virtual void      ShowVisualization() override;
    /**
     * @brief SaveData Put dialog data in local variables
     */
    virtual void      SaveData() override;
    virtual void      closeEvent(QCloseEvent *event) override;
    virtual bool      IsValid() const final;
private:
    Q_DISABLE_COPY(DialogCutArc)
    /** @brief ui keeps information about user interface */
    Ui::DialogCutArc  *ui;

    /** @brief formula string with formula */
    QString formula;
    QString pointName;

    /** @brief formulaBaseHeight base height defined by dialogui */
    int               formulaBaseHeight;

    QTimer *timerFormula;

    bool flagFormula;
    bool flagName;
};

//---------------------------------------------------------------------------------------------------------------------
inline bool DialogCutArc::IsValid() const
{
    return flagFormula && flagName;
}

#endif // DIALOGCUTARC_H
