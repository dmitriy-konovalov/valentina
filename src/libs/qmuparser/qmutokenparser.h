/***************************************************************************************************
 **
 **  Copyright (C) 2015 Roman Telezhynskyi
 **
 **  Permission is hereby granted, free of charge, to any person obtaining a copy of this
 **  software and associated documentation files (the "Software"), to deal in the Software
 **  without restriction, including without limitation the rights to use, copy, modify,
 **  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 **  permit persons to whom the Software is furnished to do so, subject to the following conditions:
 **
 **  The above copyright notice and this permission notice shall be included in all copies or
 **  substantial portions of the Software.
 **
 **  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 **  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 **  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 **  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 **  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **
 ******************************************************************************************************/

#ifndef QMUTOKENPARSER_H
#define QMUTOKENPARSER_H

#include <qcompilerdetection.h>
#include <QString>
#include <QtGlobal>

#include "qmuformulabase.h"
#include "qmuparser_global.h"

namespace qmu
{

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wsuggest-final-types")

class QMUPARSERSHARED_EXPORT QmuTokenParser : public QmuFormulaBase
{
public:
    QmuTokenParser(const QString &formula, bool osSeparator, bool fromUser = true,
                   const QMap<QString, QString> &translatedFunctions = QMap<QString, QString>());
    virtual ~QmuTokenParser() = default;

    static bool IsSingle(const QString &formula);

protected:
    static qreal Warning(const QString &warningMsg, qreal value);

private:
    Q_DISABLE_COPY_MOVE(QmuTokenParser) // NOLINT
    QmuTokenParser();
};

QT_WARNING_POP

} // namespace qmu

#endif // QMUTOKENPARSER_H
