/************************************************************************
 **
 **  @file
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   22 11, 2016
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

#ifndef VISTOOLPIECEPATH_H
#define VISTOOLPIECEPATH_H

#include <QtCore/QObject>
#include <QtGlobal>

#include "vispath.h"
#include "../vpatterndb/vpiecepath.h"

class VSimplePoint;

class VisToolPiecePath : public VisPath
{
    Q_OBJECT // NOLINT
public:
    VisToolPiecePath(const VContainer *data, QGraphicsItem *parent = nullptr);
    virtual ~VisToolPiecePath() Q_DECL_EQ_DEFAULT;

    virtual void RefreshGeometry() override;
    void         SetPath(const VPiecePath &path);
    void         SetCuttingPath(const QVector<QPointF> &cuttingPath);
    virtual int  type() const override {return Type;}
    enum { Type = UserType + static_cast<int>(Vis::ToolPiecePath)};
protected:
    virtual void mousePressEvent( QGraphicsSceneMouseEvent * event ) override;

private:
    Q_DISABLE_COPY_MOVE(VisToolPiecePath) // NOLINT
    QVector<VSimplePoint *> m_points;

    VScaledLine *m_line;

    VPiecePath m_path;

    QVector<QPointF> m_cuttingPath;

    VSimplePoint *GetPoint(quint32 i, const QColor &color);

    void HideAllItems();
};

#endif // VISTOOLPIECEPATH_H
