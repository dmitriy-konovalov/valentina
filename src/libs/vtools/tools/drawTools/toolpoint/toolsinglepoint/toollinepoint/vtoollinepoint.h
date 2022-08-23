/************************************************************************
 **
 **  @file   vtoollinepoint.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
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

#ifndef VTOOLLINEPOINT_H
#define VTOOLLINEPOINT_H

#include <qcompilerdetection.h>
#include <QGraphicsItem>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "../vpatterndb/vformula.h"
#include "../vtoolsinglepoint.h"
#include "../vmisc/def.h"

template <class T> class QSharedPointer;

struct VToolLinePointInitData : VToolSinglePointInitData
{
    VToolLinePointInitData()
        : VToolSinglePointInitData()
    {}

    QString typeLine{TypeLineLine};
    QString lineColor{ColorBlack};
};

/**
 * @brief The VToolLinePoint class parent for all tools what create point with line.
 */
class VToolLinePoint : public VToolSinglePoint
{
    Q_OBJECT // NOLINT
public:
    VToolLinePoint(VAbstractPattern *doc, VContainer *data, const quint32 &id, const QString &typeLine, const
                   QString &lineColor, const QString &formula, const quint32 &basePointId, const qreal &angle,
                   const QString &notes, QGraphicsItem * parent = nullptr);
    virtual ~VToolLinePoint() override;
    virtual int       type() const override {return Type;}
    enum { Type = UserType + static_cast<int>(Tool::LinePoint)};

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr) override;

    VFormula GetFormulaLength() const;
    void     SetFormulaLength(const VFormula &value);

    QString BasePointName() const;

    qreal   GetAngle() const;
    void    SetAngle(const qreal &value);

    QString GetLineColor() const;
    void    SetLineColor(const QString &value);

public slots:
    virtual void      Disable(bool disable, const QString &namePP) override;
    virtual void      FullUpdateFromFile() override;
protected:
    /** @brief formula string with length formula. */
    QString           formulaLength;

    /** @brief angle line angle. */
    qreal             angle;

    /** @brief basePointId id base line point. */
    quint32           basePointId;

    /** @brief mainLine line item. */
    VScaledLine      *mainLine;

    /** @brief lineColor color of a line. */
    QString           lineColor;

    virtual void      RefreshGeometry();
    virtual void      RemoveReferens() override;
    virtual void      SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj) override;
    virtual void      hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void      hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual QString   MakeToolTip() const override;
private:
    Q_DISABLE_COPY_MOVE(VToolLinePoint) // NOLINT
};

#endif // VTOOLLINEPOINT_H
