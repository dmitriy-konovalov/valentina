/************************************************************************
 **
 **  @file   dialogpin.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   31 1, 2017
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2017 Valentina project
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

#ifndef DIALOGPIN_H
#define DIALOGPIN_H

#include "../dialogtool.h"

namespace Ui
{
    class DialogPin;
}

class DialogPin : public DialogTool
{
    Q_OBJECT
public:
    explicit DialogPin(const VContainer *data, quint32 toolId, QWidget *parent = nullptr);
    virtual ~DialogPin();

    void EnbleShowMode(bool disable);

    quint32 GetPieceId() const;
    void    SetPieceId(quint32 id);

    quint32 GetPointId() const;
    void    SetPointId(quint32 id);

    virtual void SetPiecesList(const QVector<quint32> &list) override;

public slots:
    virtual void ChosenObject(quint32 id, const SceneObject &type) override;

protected:
    virtual void ShowVisualization() override;
    virtual bool IsValid() const final;

private:
    Q_DISABLE_COPY(DialogPin)
    Ui::DialogPin *ui;
    bool m_showMode;
    bool m_flagPoint;
    bool m_flagError;

    void CheckPieces();
    void CheckPoint();
};

//---------------------------------------------------------------------------------------------------------------------
inline bool DialogPin::IsValid() const
{
    return m_flagPoint && m_flagError;
}

#endif // DIALOGPIN_H
