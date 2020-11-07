/************************************************************************
 **
 **  @file   dialogrotation.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   10 4, 2016
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

#ifndef DIALOGROTATION_H
#define DIALOGROTATION_H

#include <qcompilerdetection.h>
#include <QList>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QVector>
#include <QtGlobal>

#include "../vmisc/def.h"
#include "dialogtool.h"
#include "../../tools/toolsdef.h"

namespace Ui
{
    class DialogRotation;
}

class DialogRotation : public DialogTool
{
    Q_OBJECT
public:
    explicit DialogRotation(const VContainer *data, quint32 toolId, QWidget *parent = nullptr);
    virtual ~DialogRotation();

    quint32 GetOrigPointId() const;
    void    SetOrigPointId(quint32 value);

    QString GetAngle() const;
    void    SetAngle(const QString &value);

    QString GetSuffix() const;
    void    SetSuffix(const QString &value);

    QString GetVisibilityGroupName() const;
    void    SetVisibilityGroupName(const QString &name);

    bool HasLinkedVisibilityGroup() const;
    void SetHasLinkedVisibilityGroup(bool linked);

    void        SetVisibilityGroupTags(const QStringList &tags);
    QStringList GetVisibilityGroupTags() const;

    void    SetNotes(const QString &notes);
    QString GetNotes() const;

    virtual void SetGroupCategories(const QStringList &categories) override;

    virtual void ShowDialog(bool click) override;

    QVector<SourceItem> GetSourceObjects() const;
    void                SetSourceObjects(const QVector<SourceItem> &value);

public slots:
    virtual void ChosenObject(quint32 id, const SceneObject &type) override;
    virtual void SelectedObject(bool selected, quint32 object, quint32 tool) override;

private slots:
    /** @brief DeployAngleTextEdit grow or shrink formula input */
    void DeployAngleTextEdit();
    void FXAngle();
    void SuffixChanged();
    void GroupNameChanged();
    void EvalAngle();
    void ShowSourceDetails(int row);
    void AliasChanged(const QString &text);

protected:
    virtual void ShowVisualization() override;

    /** @brief SaveData Put dialog data in local variables */
    virtual void SaveData() override;
    virtual void closeEvent(QCloseEvent *event) override;
    virtual bool IsValid() const final;

private slots:
    void PointChanged();

private:
    Q_DISABLE_COPY(DialogRotation)
    Ui::DialogRotation *ui;

    /** @brief timerAngle timer of check formula of angle */
    QTimer  *timerAngle;

    /** @brief angle formula of angle */
    QString formulaAngle;

    /** @brief formulaBaseHeightAngle base height defined by dialogui */
    int     formulaBaseHeightAngle;

    QVector<SourceItem> sourceObjects{};

    bool stage1;

    QString m_suffix;

    bool m_firstRelease;

    /** @brief flagAngle true if value of angle is correct */
    bool flagAngle;
    bool flagName;
    bool flagGroupName;
    bool flagError;
    bool flagAlias{true};

    QStringList m_groupTags{};

    void FillSourceList();

    void ValidateSourceAliases();
    void SetAliasValid(quint32 id, bool valid);
};

#endif // DIALOGROTATION_H
