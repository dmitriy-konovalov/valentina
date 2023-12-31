/************************************************************************
 **
 **  @file   vispath.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   15 8, 2014
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

#ifndef VISPATH_H
#define VISPATH_H

#include <QGraphicsItem>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QtGlobal>

#include "../visualization.h"
#include "../vmisc/def.h"
#include "../vwidgets/vcurvepathitem.h"

class VSimplePoint;

class VisPath : public Visualization, public VCurvePathItem
{
    Q_OBJECT // NOLINT

public:
    explicit VisPath(const VContainer *data, QGraphicsItem *parent = nullptr);
    ~VisPath() override = default;

    void SetApproximationScale(qreal approximationScale);

    auto type() const -> int override { return Type; }
    enum
    {
        Type = UserType + static_cast<int>(Vis::Path)
    };

protected:
    void InitPen() override;
    void AddOnScene() override;

    auto GetPoint(QVector<VSimplePoint *> &points, quint32 i, VColorRole role) -> VSimplePoint *;

    auto ApproximationScale() const -> qreal;

private:
    Q_DISABLE_COPY_MOVE(VisPath) // NOLINT

    qreal m_approximationScale{0};
};

//---------------------------------------------------------------------------------------------------------------------
inline auto VisPath::ApproximationScale() const -> qreal
{
    return m_approximationScale;
}

#endif // VISPATH_H
