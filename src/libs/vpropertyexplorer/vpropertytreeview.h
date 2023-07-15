/************************************************************************
 **
 **  @file   vpropertytreeview.h
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

#ifndef VPROPERTYTREEVIEW_H
#define VPROPERTYTREEVIEW_H

#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QTreeView>
#include <QtGlobal>

#include "vpropertyexplorer_global.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
#include "../vmisc/defglobal.h"
#endif

namespace VPE
{

class VPROPERTYEXPLORERSHARED_EXPORT VPropertyDelegate;
class VPROPERTYEXPLORERSHARED_EXPORT VPropertyModel;
class VPropertyTreeViewPrivate;

class VPROPERTYEXPLORERSHARED_EXPORT VPropertyTreeView : public QTreeView
{
    Q_OBJECT // NOLINT

public:
    //! Default constructor
    explicit VPropertyTreeView(QWidget *parent = nullptr);

    //! The destructor, taking a model and setting it to the tree view
    //! \param model The model to set as model for this tree view
    explicit VPropertyTreeView(VPropertyModel *model, QWidget *parent = nullptr);

    //! Destructor
    virtual ~VPropertyTreeView() override;

    //! Sets the height for each row. Set this to 0 in order to let the standard delegate decide
    void setRowHeight(int height = 0, bool add_to_standard = false);

protected:
    //! This method is called by the constructors to initialize the view
    virtual void init();

    //! protected constructor
    VPropertyTreeView(VPropertyTreeViewPrivate *d, bool init_, QWidget *parent = nullptr);

    //! The protected data
    VPropertyTreeViewPrivate *d_ptr;

private:
    Q_DISABLE_COPY_MOVE(VPropertyTreeView) // NOLINT
};

} // namespace VPE

#endif // VPROPERTYTREEVIEWEEVIEW_H
