/************************************************************************
 **
 **  @file   vobjectproperty.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   28 8, 2014
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

#ifndef VOBJECTPROPERTY_H
#define VOBJECTPROPERTY_H

#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QStyleOptionViewItem>
#include <QVariant>
#include <QtGlobal>

#include "../vproperty.h"
#include "../vpropertyexplorer_global.h"

class QComboBox;

namespace VPE
{

class VPROPERTYEXPLORERSHARED_EXPORT VObjectProperty : public VProperty
{
    Q_OBJECT // NOLINT

public:
    //! Constructor
    explicit VObjectProperty(const QString &name);

    //! Destructor
    virtual ~VObjectProperty() override {}

    //! Get the data how it should be displayed
    virtual auto data(int column = DPC_Name, int role = Qt::DisplayRole) const -> QVariant override;

    //! Returns an editor widget, or NULL if it doesn't supply one
    //! \param parent The widget to which the editor will be added as a child
    //! \options Render options
    //! \delegate A pointer to the QAbstractItemDelegate requesting the editor. This can be used to connect signals and
    //! slots.
    virtual auto createEditor(QWidget *parent, const QStyleOptionViewItem &options,
                              const QAbstractItemDelegate *delegate) -> QWidget * override;

    //! Sets the property's data to the editor (returns false, if the standard delegate should do that)
    virtual auto setEditorData(QWidget *editor) -> bool override;

    //! Gets the data from the widget
    virtual auto getEditorData(const QWidget *editor) const -> QVariant override;

    //! Sets the objects list
    void setObjectsList(const QMap<QString, quint32> &objects);

    //! Get the settings. This function has to be implemented in a subclass in order to have an effect
    virtual auto getObjects() const -> QMap<QString, quint32>;

    //! Sets the value of the property
    virtual void setValue(const QVariant &value) override;

    //! Returns a string containing the type of the property
    virtual auto type() const -> QString override;

    //! Clones this property
    //! \param include_children Indicates whether to also clone the children
    //! \param container If a property is being passed here, no new VProperty is being created but instead it is tried
    //! to fill all the data into container. This can also be used when subclassing this function.
    //! \return Returns the newly created property (or container, if it was not NULL)
    Q_REQUIRED_RESULT virtual auto clone(bool include_children = true, VProperty *container = nullptr) const
        -> VProperty * override;

public slots:
    void currentIndexChanged(int index);

protected:
    //! The list of possible objects
    QMap<QString, quint32> objects;

    void FillList(QComboBox *box, const QMap<QString, quint32> &list) const;
    // No use of d-pointer in this case, because it is unlikely this will change. If it does, we can still add other
    // members by reimplementing the VPropertyPrivate class without touching this header file.

private:
    Q_DISABLE_COPY_MOVE(VObjectProperty) // NOLINT
};

} // namespace VPE

#endif // VOBJECTPROPERTY_H
