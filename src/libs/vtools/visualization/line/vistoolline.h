/************************************************************************
 **
 **  @file   vgraphicslineitem.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   15 7, 2014
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Valentina project
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

#ifndef VGRAPHICSLINEITEM_H
#define VGRAPHICSLINEITEM_H

#include <qcompilerdetection.h>
#include <QColor>
#include <QGraphicsItem>
#include <QLineF>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <Qt>
#include <QtGlobal>

#include "../vmisc/def.h"
#include "visline.h"

class VisToolLine : public VisLine
{
    Q_OBJECT // NOLINT
public:
    explicit VisToolLine(const VContainer *data, QGraphicsItem *parent = nullptr);
    virtual ~VisToolLine() = default;

    virtual void RefreshGeometry() override;
    void         setPoint2Id(const quint32 &value);
    virtual int  type() const override {return Type;}
    enum { Type = UserType + static_cast<int>(Vis::ToolLine)};
protected:
    virtual void DrawLine(VScaledLine *lineItem, const QLineF &line, const QColor &color,
                          Qt::PenStyle style = Qt::SolidLine) override;
private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(VisToolLine) // NOLINT
    quint32      point2Id;
};

#endif // VGRAPHICSLINEITEM_H
