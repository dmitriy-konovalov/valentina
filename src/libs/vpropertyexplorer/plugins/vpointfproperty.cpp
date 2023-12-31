/************************************************************************
 **
 **  @file   vpointfproperty.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   27 8, 2014
 **
 **  @brief
 **  @copyright
 **  All rights reserved. This program and the accompanying materials
 **  are made available under the terms of the GNU Lesser General Public License
 **  (LGPL) version 2.1 which accompanies this distribution, and is available at
 **  http://www.gnu.org/licenses/lgpl-2.1.html
 **
 **  This library is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 **  Lesser General Public License for more details.
 **
 *************************************************************************/

#include "vpointfproperty.h"

#include <QFlags>
#include <QList>
#include <QPointF>

#include "../vproperty_p.h"
#include "vnumberproperty.h"

VPE::VPointFProperty::VPointFProperty(const QString &name)
    : VProperty(name,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                QMetaType::QPointF)
#else
                QVariant::PointF)
#endif
{
    d_ptr->VariantValue.setValue(0);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    d_ptr->VariantValue.convert(QMetaType(QMetaType::QPointF));
#else
    d_ptr->VariantValue.convert(QVariant::PointF);
#endif

    VDoubleProperty* tmpX = new VDoubleProperty("X");
    addChild(tmpX);
    tmpX->setUpdateBehaviour(true, false);

    VDoubleProperty* tmpY = new VDoubleProperty("Y");
    addChild(tmpY);
    tmpY->setUpdateBehaviour(true, false);

    setValue(QPointF());
}

auto VPE::VPointFProperty::data(int column, int role) const -> QVariant
{
    if (column == DPC_Data && Qt::DisplayRole == role)
    {
        return getPointF();
    }
    return VProperty::data(column, role);
}

auto VPE::VPointFProperty::flags(int column) const -> Qt::ItemFlags
{
    if (column == DPC_Name || column == DPC_Data)
    {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
    return Qt::NoItemFlags;
}

auto VPE::VPointFProperty::getPointF() const -> QPointF
{
    QPointF tmpValue;

    if (d_ptr->Children.count() < 2)
    {
        return tmpValue;
    }

    tmpValue.setX(d_ptr->Children.at(0)->getValue().toDouble());
    tmpValue.setY(d_ptr->Children.at(1)->getValue().toDouble());

    return tmpValue;
}

void VPE::VPointFProperty::setPointF(const QPointF &point)
{
    setPointF(point.x(), point.y());
}

void VPE::VPointFProperty::setPointF(qreal x, qreal y)
{
    if (d_ptr->Children.count() < 2)
    {
        return;
    }

    QVariant tmpX(x);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    tmpX.convert(QMetaType(QMetaType::Double));
#else
    tmpX.convert(QVariant::Double);
#endif

    QVariant tmpY(y);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    tmpY.convert(QMetaType(QMetaType::Double));
#else
    tmpY.convert(QVariant::Double);
#endif

    d_ptr->Children.at(0)->setValue(tmpX);
    d_ptr->Children.at(1)->setValue(tmpY);
}

auto VPE::VPointFProperty::type() const -> QString
{
    return "pointF";
}

auto VPE::VPointFProperty::clone(bool include_children, VProperty *container) const -> VPE::VProperty *
{
    if (!container)
    {
        container = new VPointFProperty(getName());

        if (!include_children)
        {
            const QList<VProperty*> &tmpChildren = container->getChildren();
            for (auto *tmpChild : tmpChildren)
            {
                container->removeChild(tmpChild);
                delete tmpChild;
            }
        }
    }

    return VProperty::clone(false, container);  // Child
}

void VPE::VPointFProperty::setValue(const QVariant &value)
{
    QPointF tmpPoint = value.toPointF();
    setPointF(tmpPoint);
}

auto VPE::VPointFProperty::getValue() const -> QVariant
{
    QPointF tmpValue = getPointF();
    return QString("%1,%2").arg(QString::number(tmpValue.x()), QString::number(tmpValue.y()));
}
