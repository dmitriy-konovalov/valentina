/************************************************************************
 **
 **  @file   vlinecolorproperty.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   7 2, 2015
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

#ifndef VLINECOLORPROPERTY_H
#define VLINECOLORPROPERTY_H

#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QStyleOptionViewItem>
#include <QVariant>
#include <QVector>
#include <QtGlobal>

#include "../vproperty.h"
#include "../vpropertyexplorer_global.h"

namespace VPE
{

class VPROPERTYEXPLORERSHARED_EXPORT VLineColorProperty : public VProperty
{
    Q_OBJECT // NOLINT

public:
    //! Constructor
    explicit VLineColorProperty(const QString &name);

    //! Destructor
    ~VLineColorProperty() override {}

    //! Get the data how it should be displayed
    virtual auto data(int column = DPC_Name, int role = Qt::DisplayRole) const -> QVariant override;

    //! Returns an editor widget, or NULL if it doesn't supply one
    //! \param parent The widget to which the editor will be added as a child
    //! \options Render options
    //! \delegate A pointer to the QAbstractItemDelegate requesting the editor. This can be used to connect signals and
    //! slots.
    virtual auto createEditor(QWidget *parent, const QStyleOptionViewItem &options,
                              const QAbstractItemDelegate *delegate) -> QWidget * override;

    //! Gets the data from the widget
    virtual auto getEditorData(const QWidget *editor) const -> QVariant override;

    //! Sets the colors
    virtual void setColors(const QMap<QString, QString> &colors);

    //! Get the settings. This function has to be implemented in a subclass in order to have an effect
    virtual auto getColors() const -> QMap<QString, QString>;

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

    static auto IndexOfColor(const QMap<QString, QString> &colors, const QString &color) -> vpesizetype;

public slots:
    void currentIndexChanged(int index);

protected:
    //! The list of possible options to choose from
    QMap<QString, QString> colors;
    QVector<QString> indexList;
    // No use of d-pointer in this case, because it is unlikely this will change. If it does, we can still add other
    // members by reimplementing the VPropertyPrivate class without touching this header file.

private:
    Q_DISABLE_COPY_MOVE(VLineColorProperty) // NOLINT
};

} // namespace VPE

#endif // VLINECOLORPROPERTY_H
