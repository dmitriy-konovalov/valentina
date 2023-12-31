/************************************************************************
 **
 **  @file   vfilepropertyeditor.h
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

#ifndef VFILEPROPERTYEDITOR_H
#define VFILEPROPERTYEDITOR_H

#include <QLineEdit>
#include <QMetaObject>
#include <QMimeData>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QToolButton>
#include <QWidget>
#include <QtGlobal>

#include "../vpropertyexplorer_global.h"

namespace VPE
{

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wsuggest-final-types")

class VPROPERTYEXPLORERSHARED_EXPORT VFileEditWidget : public QWidget
{
    Q_OBJECT // NOLINT

public:
    explicit VFileEditWidget(QWidget *parent, bool is_directory = false);
    virtual ~VFileEditWidget() override;

    //! This function returns the file currently set to this editor
    auto getFile() const -> QString;

    //! Needed for proper event handling
    virtual auto eventFilter(QObject *obj, QEvent *ev) -> bool override;

    //! Returns the directory/file setting
    //! \return True, if a directory dialog is being shown, false if a file dialog
    auto isDirectory() -> bool;

signals:
    //! This signal is emitted when the user changed the curret file.
    //! Actions triggering this signal are either using the file dialog
    //! to select a new file or changing the file path in the line edit.
    void dataChangedByUser(const QString &getFile, VFileEditWidget *editor);

    //! This signal is emitted whenever dataChangedByUser() gets emmitted
    //! and is connected to the delegate's commitData() signal
    void commitData(QWidget *editor);

public slots:
    //! Sets the current file, does not check if it is valid
    //! \param value The new filepath the widget should show
    //! \param emit_signal If true, this will emit the dataChangedByUser()-signal (if file differs from the current
    //! file)
    void setFile(const QString &value, bool emit_signal = false);

    //! Sets a filter for the file field
    //! \param dialog_filter The filter used for the File Dialog
    //! \param filter_list The list of file endings. The filters are being checked using regular expressions
    void setFilter(const QString &dialog_filter = QString(), const QStringList &filter_list = QStringList());

    //! Sets whether the property stores a directory or a file
    void setDirectory(bool dir);

private slots:
    //! This slot gets activated, when the "..." button gets clicked
    void onToolButtonClicked();

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;

    //! This function checks the mime data, if it is compatible with the filters
    virtual auto checkMimeData(const QMimeData *data, QString &file) const -> bool;

    //! This checks, if a file is compatible with the filters
    virtual auto checkFileFilter(const QString &file) const -> bool;

    QString CurrentFilePath;
    QToolButton *ToolButton;
    QLineEdit *FileLineEdit;
    QString FileDialogFilter;
    QStringList FilterList;

    //! Specifies whether it is being looked for a directory (true) or a file (false, default)
    bool Directory;

private:
    Q_DISABLE_COPY_MOVE(VFileEditWidget) // NOLINT
};

QT_WARNING_POP

} // namespace VPE

#endif // VFILEPROPERTYEDITOR_H
