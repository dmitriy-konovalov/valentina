/************************************************************************
 **
 **  @file   vfileproperty.h
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

#ifndef VFILEPROPERTY_H
#define VFILEPROPERTY_H

#include <QMetaObject>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QStringList>
#include <QStyleOptionViewItem>
#include <QVariant>
#include <QtGlobal>

#include "../vproperty.h"
#include "../vpropertyexplorer_global.h"

namespace VPE
{

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wsuggest-final-types")
QT_WARNING_DISABLE_GCC("-Wsuggest-final-methods")

class VPROPERTYEXPLORERSHARED_EXPORT VFileProperty : public VProperty
{
    Q_OBJECT // NOLINT

public:
    explicit VFileProperty(const QString &name);

    //! The destructor
    virtual ~VFileProperty() override;

    //! Sets the file filters. The file filters have to be like the ones passed a QFileOpenDialog.
    virtual void setFileFilters(const QString &filefilters);

    //! Returns the current file filters as a string
    virtual auto getFileFilters() const -> QString;

    //! Set file
    virtual void setFile(const QString &file);

    //! Get file
    virtual auto getFile() const -> QString;

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

    //! Sets the settings. Available settings:
    //!
    //! key: "FileFilters" - value: File filters in the same format the QFileDialog expects it
    virtual void setSetting(const QString &key, const QVariant &value) override;

    //! Get the settings. This function has to be implemented in a subclass in order to have an effect
    virtual auto getSetting(const QString &key) const -> QVariant override;

    //! Returns the list of keys of the property's settings
    virtual auto getSettingKeys() const -> QStringList override;

    //! Returns a string containing the type of the property
    virtual auto type() const -> QString override;

    //! Clones this property
    //! \param include_children Indicates whether to also clone the children
    //! \param container If a property is being passed here, no new VProperty is being created but instead it is tried
    //! to fill all the data into container. This can also be used when subclassing this function.
    //! \return Returns the newly created property (or container, if it was not NULL)
    Q_REQUIRED_RESULT virtual auto clone(bool include_children = true, VProperty *container = nullptr) const
        -> VProperty * override;

    //! Returns whether this is a file (false) or a directory (true)
    virtual auto isDirectory() const -> bool;

    //! Sets whether this is a file (false) or a directory (true)
    virtual void setDirectory(bool is_directory);

private:
    Q_DISABLE_COPY_MOVE(VFileProperty) // NOLINT
};

QT_WARNING_POP

} // namespace VPE

#endif // VFILEPROPERTY_H
