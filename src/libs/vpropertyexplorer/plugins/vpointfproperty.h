/************************************************************************
 **
 **  @file   vpointfproperty.h
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

#ifndef VPOINTFPROPERTY_H
#define VPOINTFPROPERTY_H

#include <QMetaObject>
#include <QObject>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QtGlobal>

#include "../vproperty.h"
#include "../vpropertyexplorer_global.h"

namespace VPE
{

class VPROPERTYEXPLORERSHARED_EXPORT VPointFProperty : public VProperty
{
    Q_OBJECT // NOLINT

public:
    explicit VPointFProperty(const QString &name);

    virtual ~VPointFProperty() override {}

    //! Get the data how it should be displayed
    virtual auto data(int column = DPC_Name, int role = Qt::DisplayRole) const -> QVariant override;

    //! Returns item flags
    auto flags(int column = DPC_Name) const -> Qt::ItemFlags override;

    //! Returns the QPointF
    virtual auto getPointF() const -> QPointF;

    //! Sets the QPointF
    virtual void setPointF(const QPointF &point);

    //! Sets the QPointF
    virtual void setPointF(qreal x, qreal y);

    //! Returns a string containing the type of the property
    virtual auto type() const -> QString override;

    //! Clones this property
    //! \param include_children Indicates whether to also clone the children
    //! \param container If a property is being passed here, no new VProperty is being created but instead it is tried
    //! to fill all the data into container. This can also be used when subclassing this function.
    //! \return Returns the newly created property (or container, if it was not NULL)
    Q_REQUIRED_RESULT virtual auto clone(bool include_children = true, VProperty *container = nullptr) const
        -> VProperty * override;

    //! Sets the value of the property
    virtual void setValue(const QVariant &value) override;

    //! Returns the value of the property as a QVariant
    virtual auto getValue() const -> QVariant override;

private:
    Q_DISABLE_COPY_MOVE(VPointFProperty) // NOLINT
};

} // namespace VPE

#endif // VPOINTFPROPERTY_H
