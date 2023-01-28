/************************************************************************
 **
 **  @file   customevents.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   23 6, 2014
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

#ifndef CUSTOMEVENTS_H
#define CUSTOMEVENTS_H

#include <qcompilerdetection.h>
#include <QEvent>
#include <QString>

#include "typedef.h"

enum CustomEventType {
    UndoEventType = 1,
    LiteParseEventType = 2,
    FitBestCurrentEventType = 3,
    WarningMessageEventType = 4,
};

//---------------------------------------------------------------------------------------------------------------------
QT_WARNING_PUSH
#if !defined(Q_OS_MACOS) && defined(Q_CC_CLANG)
QT_WARNING_DISABLE_CLANG("-Wenum-enum-conversion")
#endif

// Define undo event identifier
const QEvent::Type UNDO_EVENT = static_cast<QEvent::Type>(QEvent::User + CustomEventType::UndoEventType);

QT_WARNING_POP

class UndoEvent : public QEvent
{
public:
    UndoEvent()
        : QEvent(UNDO_EVENT)
    {}

    virtual ~UndoEvent() =default;
};

//---------------------------------------------------------------------------------------------------------------------
QT_WARNING_PUSH
#if !defined(Q_OS_MACOS) && defined(Q_CC_CLANG)
QT_WARNING_DISABLE_CLANG("-Wenum-enum-conversion")
#endif

const QEvent::Type LITE_PARSE_EVENT = static_cast<QEvent::Type>(QEvent::User + CustomEventType::LiteParseEventType);

QT_WARNING_POP

class LiteParseEvent : public QEvent
{
public:
    LiteParseEvent()
        : QEvent(LITE_PARSE_EVENT)
    {}

    virtual ~LiteParseEvent() =default;
};

//---------------------------------------------------------------------------------------------------------------------
QT_WARNING_PUSH
#if !defined(Q_OS_MACOS) && defined(Q_CC_CLANG)
QT_WARNING_DISABLE_CLANG("-Wenum-enum-conversion")
#endif

const QEvent::Type FIT_BEST_CURRENT_EVENT = static_cast<QEvent::Type>(QEvent::User +
                                                                      CustomEventType::FitBestCurrentEventType);

QT_WARNING_POP

class FitBestCurrentEvent : public QEvent
{
public:
    FitBestCurrentEvent()
        : QEvent(FIT_BEST_CURRENT_EVENT)
    {}

    virtual ~FitBestCurrentEvent() =default;
};

//---------------------------------------------------------------------------------------------------------------------
QT_WARNING_PUSH
#if !defined(Q_OS_MACOS) && defined(Q_CC_CLANG)
QT_WARNING_DISABLE_CLANG("-Wenum-enum-conversion")
#endif

const QEvent::Type WARNING_MESSAGE_EVENT = static_cast<QEvent::Type>(QEvent::User +
                                                                      CustomEventType::WarningMessageEventType);

QT_WARNING_POP

class WarningMessageEvent : public QEvent
{
public:
    WarningMessageEvent(const QString &message, QtMsgType severity)
        : QEvent(WARNING_MESSAGE_EVENT),
          m_message(message),
          m_severity(severity)
    {}

    virtual ~WarningMessageEvent() =default;

    QString Message() const;

    QtMsgType Severity() const;

private:
    QString m_message;
    QtMsgType m_severity;
};

inline QString WarningMessageEvent::Message() const
{
    return m_message;
}

inline QtMsgType WarningMessageEvent::Severity() const
{
    return m_severity;
}

#endif // CUSTOMEVENTS_H
