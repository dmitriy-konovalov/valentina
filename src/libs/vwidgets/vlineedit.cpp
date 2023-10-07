/************************************************************************
 **
 **  @file
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   12 8, 2017
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2017 Valentina project
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

#include "vlineedit.h"

#include <QApplication>
#include <QCompleter>
#include <QStringListModel>
#include <QTimer>
#include <QtDebug>
#include <chrono>

#include "../vmisc/defglobal.h"

#if (defined(Q_CC_GNU) && Q_CC_GNU < 409) && !defined(Q_CC_CLANG)
// DO NOT WORK WITH GCC 4.8
#else
#if __cplusplus >= 201402L
using namespace std::chrono_literals;
#else
#include "../vmisc/bpstd/chrono.hpp"
using namespace bpstd::literals::chrono_literals;
#endif // __cplusplus >= 201402L
#endif //(defined(Q_CC_GNU) && Q_CC_GNU < 409) && !defined(Q_CC_CLANG)

#if QT_VERSION < QT_VERSION_CHECK(6, 4, 0)
#include "../vmisc/compatibility.h"
#endif

using namespace Qt::Literals::StringLiterals;

namespace
{
class MultiSelectCompleter : public QCompleter
{
public:
    explicit MultiSelectCompleter(QObject *parent = nullptr);
    virtual ~MultiSelectCompleter() = default;

    virtual auto pathFromIndex(const QModelIndex &index) const -> QString override;
    virtual auto splitPath(const QString &path) const -> QStringList override;

private:
    Q_DISABLE_COPY_MOVE(MultiSelectCompleter) // NOLINT
};
} // namespace

// MultiSelectCompleter
//---------------------------------------------------------------------------------------------------------------------
MultiSelectCompleter::MultiSelectCompleter(QObject *parent)
  : QCompleter(parent)
{
}

//---------------------------------------------------------------------------------------------------------------------
auto MultiSelectCompleter::pathFromIndex(const QModelIndex &index) const -> QString
{
    QString path = QCompleter::pathFromIndex(index);

    QString text = static_cast<QLineEdit *>(widget())->text();

    vsizetype pos = text.lastIndexOf(',');
    if (pos >= 0)
    {
        path = text.left(pos) + ", " + path;
    }

    return path;
}

//---------------------------------------------------------------------------------------------------------------------
auto MultiSelectCompleter::splitPath(const QString &path) const -> QStringList
{
    vsizetype pos = path.lastIndexOf(',') + 1;

    while (pos < path.length() && path.at(pos) == ' '_L1)
    {
        pos++;
    }

    return QStringList(path.mid(pos));
}

// VLineEdit
//---------------------------------------------------------------------------------------------------------------------
VLineEdit::VLineEdit(QWidget *parent)
  : QLineEdit(parent),
    m_selectOnMousePress(false)
{
}

//---------------------------------------------------------------------------------------------------------------------
VLineEdit::VLineEdit(const QString &contents, QWidget *parent)
  : QLineEdit(contents, parent),
    m_selectOnMousePress(false)
{
}

//---------------------------------------------------------------------------------------------------------------------
void VLineEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    selectAll();
    m_selectOnMousePress = true;
}

//---------------------------------------------------------------------------------------------------------------------
void VLineEdit::focusOutEvent(QFocusEvent *e)
{
    const int start = selectionStart();
    const int selectionLength = static_cast<int>(selectedText().length());
    const bool wasTextSelected = hasSelectedText();

    QLineEdit::focusOutEvent(e);

    if (wasTextSelected)
    {
        setSelection(start, selectionLength);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VLineEdit::mousePressEvent(QMouseEvent *e)
{
    QLineEdit::mousePressEvent(e);
    if (m_selectOnMousePress)
    {
        selectAll();
        m_selectOnMousePress = false;
    }
}

// VCompleterLineEdit
//---------------------------------------------------------------------------------------------------------------------
VCompleterLineEdit::VCompleterLineEdit(QWidget *parent)
  : VLineEdit(parent),
    m_model(new QStringListModel(this))
{
    setCompleter(new MultiSelectCompleter(this));
    completer()->setModel(m_model);
    completer()->setCompletionMode(QCompleter::PopupCompletion);
    completer()->setCaseSensitivity(Qt::CaseInsensitive);
    connect(this, &VCompleterLineEdit::textEdited, this, &VCompleterLineEdit::ShowCompletion);
}

//---------------------------------------------------------------------------------------------------------------------
void VCompleterLineEdit::SetCompletion(const QStringList &list)
{
    m_model->setStringList(list);
}

//---------------------------------------------------------------------------------------------------------------------
void VCompleterLineEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    // force completion when line edit is focued in
    completer()->complete();
}

//---------------------------------------------------------------------------------------------------------------------
void VCompleterLineEdit::ShowCompletion()
{
    //    PrepareCompletion();

    // force to show all items when text is empty
    completer()->setCompletionMode(text().isEmpty() ? QCompleter::UnfilteredPopupCompletion
                                                    : QCompleter::PopupCompletion);
    if (text().isEmpty())
    {
        // completion list will be hidden now; we will show it again after a delay
        QTimer::singleShot(V_MSECONDS(100), this, &VCompleterLineEdit::CompletionPopup);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VCompleterLineEdit::CompletionPopup()
{
    // apparently, complete() works only in event handler
    QApplication::postEvent(this, new QEvent(QEvent::User));
}

//---------------------------------------------------------------------------------------------------------------------
void VCompleterLineEdit::customEvent(QEvent *e)
{
    QLineEdit::customEvent(e);
    // force completion after text is deleted
    completer()->complete();
}
