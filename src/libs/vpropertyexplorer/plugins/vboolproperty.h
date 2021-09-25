/************************************************************************
 **
 **  @file   vboolproperty.h
 **  @author hedgeware <internal(at)hedgeware.net>
 **  @date
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

#ifndef VBOOLPROPERTY_H
#define VBOOLPROPERTY_H

#include <qcompilerdetection.h>
#include <stddef.h>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QVariant>
#include <Qt>
#include <QtGlobal>

#include "../vproperty.h"
#include "../vpropertyexplorer_global.h"

namespace VPE
{

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wsuggest-final-types")

//! The VBoolProperty can take two states: True or False.
class VPROPERTYEXPLORERSHARED_EXPORT VBoolProperty : public VProperty
{
    Q_OBJECT
public:
    //! Default constructor
    explicit VBoolProperty(const QString& name);

    //! Destructor
    virtual ~VBoolProperty()  override {}

    //! Get the data how it should be displayed
    virtual QVariant data (int column = DPC_Name, int role = Qt::DisplayRole) const override;

    //! This is used by the model to set the data
    //! \param data The data to set
    //! \param role The role. Default is Qt::EditRole
    //! \return Returns true, if the data was changed, false if not.
    virtual bool setData (const QVariant& data, int role = Qt::EditRole) override;

    //! Returns item flags
    virtual Qt::ItemFlags flags(int column = DPC_Name) const override;

    //! Returns a string containing the type of the property
    virtual QString type() const override;

    //! Clones this property
    //! \param include_children Indicates whether to also clone the children
    //! \param container If a property is being passed here, no new VProperty is being created but instead it is tried
    //! to fill all the data into container. This can also be used when subclassing this function.
    //! \return Returns the newly created property (or container, if it was not NULL)
    virtual VProperty* clone(bool include_children = true, VProperty* container = NULL) const override;

protected:
    //! The (translatable) text displayed when the property is set to true (default: "True")
    static QVariant TrueText;

    //! The (translatable) text displayed when the property is set to false (default: "False")
    static QVariant FalseText;

private:
    Q_DISABLE_COPY(VBoolProperty)
};

QT_WARNING_POP

}

#endif // VBOOLPROPERTY_H
