/************************************************************************
 **
 **  @file   vpropertyset.h
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

#ifndef VPROPERTYSET_H
#define VPROPERTYSET_H


#include <QMap>
#include <QString>
#include <QtGlobal>
#include <QtCore/qcontainerfwd.h>

#include "vproperty.h"
#include "vpropertyexplorer_global.h"

namespace VPE
{

// Forward declaration
class VPropertySetPrivate;

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wsuggest-final-types")
QT_WARNING_DISABLE_GCC("-Wsuggest-final-methods")

// todo: better description
//! \brief  VPropertySet is a simple class for managing a set of properties.
//!         If you don't need all the Model-functionality, chose this class
//!         over VPropertyModel.
//!

//!
class VPROPERTYEXPLORERSHARED_EXPORT VPropertySet
{
public:
    //! Default constructor, creating an empty property set
    VPropertySet();

    //! Destructor
    virtual ~VPropertySet();

    //! Adds the property to the model and attaches it to the parentid. Note that if the property has a parent which is
    //! not part of this set, it will be removed from that parent.
    //! \param property The property to add
    //! \param id The property ID. If id is empty, the property will not be accessable by it's id but still be added.
    //! If the property was filed under another ID before, that will no longer be valid.
    //! \param parentid The property's ID to which to add the property as child. Pass empty string to add it to the
    //! root properties.
    virtual auto addProperty(VProperty *property, const QString &id, const QString &parentid) -> bool;

    //! Adds the property to the model and attaches it to the parent property.
    //! \param property The property to add
    //! \param id The property ID. If id is empty, the property will not be accessable by it's id but still be added.
    //! If the property was filed under another ID before, that will no longer be valid.
    //! \param parent_property The property to which to add the property as child. Pass NULL to add it to the root
    //! properties.
    virtual auto addProperty(VProperty *property, const QString &id, VProperty *parent_property = nullptr) -> bool;

    //! Checks whether a property belongs to this set and returns the result
    //! \param property The property to check for
    //! \return True, if the property is part of this set, false otherwise
    virtual auto hasProperty(VProperty *property) const -> bool;

    //! Gets a property by it's ID
    virtual auto getProperty(const QString &id) const -> VProperty *;

    //! Removes a property from the set and returns it
    virtual auto takeProperty(const QString &id) -> VProperty *;

    //! Removes a property from the set and deletes it
    virtual void removeProperty(const QString& id);

    //! Removes a property from the set and deletes it optionally
    virtual void removeProperty(VProperty* prop, bool delete_property = true);

    //! Returns the number of properties with in ID that are directly accessable by getProperty()
    virtual auto count() const -> vpesizetype;

    //! Clears the set and (optionally) deletes all properties
    //! \param delete_properties Set this to false, if you don't want the properties to get deleted.
    virtual void clear(bool delete_properties = true);

    //! Returns the ID of the property within the set
    //! The concept of property IDs is, that the object that manages the properties
    //! and not the properties themselves handle the IDs.
    //! \param prop The property of which to get the ID.
    //! \param look_for_parent_id If this is TRUE and the property has no ID, all the parent properties are checked.
    //! \return Returns the ID under which the property is stored within the set
    virtual auto getPropertyID(const VProperty *prop, bool look_for_parent_id = true) const -> QString;

    //! Returns a const reference to the map of properties
    auto getPropertiesMap() const -> const QMap<QString, VProperty *> &;

    //! Returns a const reference to the list of root properties
    auto getRootProperties() const -> const QList<VProperty *> &;

    //! Returns the root property in a certain row
    //! \param row The root row in which to look for the root property
    auto getRootProperty(int row) const -> VProperty *;

    //! Returns the number of independent properties
    auto getRootPropertyCount() const -> vpesizetype;

    //! Clones the property set
    Q_REQUIRED_RESULT auto clone() const -> VPropertySet *;

protected:
    //! Checks whether a property belongs to this set and returns the result
    //! \param property The property to check for
    //! \param parent The parent property from which to start checking all the children
    //! \return True, if the property is part of this set, false otherwise
    virtual auto hasProperty(VProperty *property, VProperty *parent) const -> bool;

    //! Clones a property into another property set
    void cloneProperty(VProperty* property_to_clone, VProperty* parent_property, VPropertySet* output_set) const;

    //! Recursivly removes a property's child properties from the set, but not from the parent
    virtual void removePropertyFromSet(VProperty* prop);

    //! The data
    VPropertySetPrivate* d_ptr;
private:
    Q_DISABLE_COPY_MOVE(VPropertySet) // NOLINT
};

QT_WARNING_POP

}  // namespace VPE

#endif // VPROPERTYMODEL_H
