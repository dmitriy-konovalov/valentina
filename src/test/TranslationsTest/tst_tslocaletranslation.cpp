/************************************************************************
 **
 **  @file   tst_tslocaletranslation.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   17 2, 2018
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2018 Valentina project
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
#include "tst_tslocaletranslation.h"
#include "qnamespace.h"

#include <QtTest>

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(Qt::LayoutDirection)
#endif

//---------------------------------------------------------------------------------------------------------------------
TST_TSLocaleTranslation::TST_TSLocaleTranslation(const QString &locale, QObject *parent)
  : TST_AbstractTranslation(parent),
    m_locale(locale)
{
}

//---------------------------------------------------------------------------------------------------------------------
void TST_TSLocaleTranslation::CheckPlaceMarkerExist_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("translation");
    QTest::addColumn<Qt::LayoutDirection>("direction");

    const QString filename = QStringLiteral("valentina_%1.ts").arg(m_locale);
    QLocale locale(m_locale);

    const QDomNodeList messages = LoadTSFile(filename);
    if (messages.isEmpty())
    {
        QFAIL("Can't begin test.");
    }

    for (qint32 i = 0, num = messages.size(); i < num; ++i)
    {
        const QDomElement message = messages.at(i).toElement();
        if (!message.isNull())
        {
            const QString source = message.firstChildElement(TagSource).text();
            if (source.isEmpty())
            {
                continue;
            }

            const QDomElement translationTag = message.firstChildElement(TagTranslation);
            if (translationTag.hasAttribute(AttrType))
            {
                const QString attrVal = translationTag.attribute(AttrType);
                if (attrVal == AttrValVanished || attrVal == AttrValUnfinished || attrVal == AttrValObsolete)
                {
                    continue;
                }
            }
            const QString translation = translationTag.text();
            if (translation.isEmpty())
            {
                continue;
            }

            const QString caseName =
                QStringLiteral("File '%1'. Check place holder source message '%2'").arg(filename, source);
            QTest::newRow(qUtf8Printable(caseName)) << source << translation << locale.textDirection();
        }
        else
        {
            const QString caseName = QStringLiteral("File '%2'. Message %1 is null.").arg(i).arg(filename);
            QFAIL(qUtf8Printable(caseName));
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void TST_TSLocaleTranslation::CheckPlaceMarkerExist()
{
    QFETCH(QString, source);
    QFETCH(QString, translation);
    QFETCH(Qt::LayoutDirection, direction);

    int sourceMarkCount = 0;
    int translationMarkCount = 0;
    const QChar per('%');

    for (int i = 1; i <= 99; ++i)
    {
        const QString number = QString::number(i);
        const QString sourceMarker = per + number;
        const QString translationMarker = direction == Qt::RightToLeft ? number + per : sourceMarker;
        const bool sourceMarkerFlag = source.indexOf(sourceMarker) != -1;
        if (sourceMarkerFlag)
        {
            ++sourceMarkCount;
            if (sourceMarkCount != i)
            {
                const QString message = QString("In source string '%1' was missed place marker ").arg(source) +
                                        QLatin1String("'%") + QString().setNum(sourceMarkCount) + QLatin1String("'.");
                QFAIL(qUtf8Printable(message));
            }
        }

        const bool translationMarkerFlag = translation.indexOf(translationMarker) != -1;
        if (translationMarkerFlag)
        {
            ++translationMarkCount;
            if (translationMarkCount != i)
            {
                const QString message =
                    QString("In translation string '%1' was missed place marker ").arg(translation) +
                    QLatin1String("'%") + QString().setNum(translationMarkCount) + QLatin1String("'.");
                QFAIL(qUtf8Printable(message));
            }
        }

        if (sourceMarkerFlag != translationMarkerFlag)
        {
            const QString message =
                QString("Compare to source string in the translation string '%1' was missed place marker ")
                    .arg(translation) + QLatin1String("'%") + QString().setNum(sourceMarkCount) +
                    QLatin1String("'.");
            QFAIL(qUtf8Printable(message));
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void TST_TSLocaleTranslation::TestPunctuation_data()
{
    QTest::addColumn<QString>("locale");
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("translation");

    const QString filename = QString("valentina_%1.ts").arg(m_locale);

    const QDomNodeList messages = LoadTSFile(filename);
    if (messages.isEmpty())
    {
        QFAIL("Can't begin test.");
    }

    for (qint32 i = 0, num = messages.size(); i < num; ++i)
    {
        const QDomElement message = messages.at(i).toElement();
        if (message.isNull() == false)
        {
            const QString source = message.firstChildElement(TagSource).text();
            if (source.isEmpty())
            {
                continue;
            }

            const QDomElement translationTag = message.firstChildElement(TagTranslation);
            if (translationTag.hasAttribute(AttrType))
            {
                const QString attrVal = translationTag.attribute(AttrType);
                if (attrVal == AttrValVanished || attrVal == AttrValUnfinished || attrVal == AttrValObsolete)
                {
                    continue;
                }
            }
            const QString translation = translationTag.text();
            if (translation.isEmpty())
            {
                continue;
            }

            const QString caseName = QStringLiteral("File '%1'.").arg(filename);
            QTest::newRow(qUtf8Printable(caseName)) << m_locale << source << translation;
        }
        else
        {
            const QString caseName = QStringLiteral("File '%2'. Message %1 is null.").arg(i).arg(filename);
            QFAIL(qUtf8Printable(caseName));
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void TST_TSLocaleTranslation::TestPunctuation()
{
    QFETCH(QString, locale);
    QFETCH(QString, source);
    QFETCH(QString, translation);

    static const QStringList punctuation{
        QChar('.'),   QChar(':'), QChar(QChar::Space), QChar('\n'), QChar('!'), QChar('?'), QChar(';'),
        QChar(0x2026) // …
    };

    if (QLocale(locale).textDirection() != Qt::LeftToRight)
    {
        QSKIP("Not supported text direction");
    }

    bool testFail = false;
    const QChar cSource = source.at(source.length() - 1);
    QChar cPunctuation = cSource;
    const QChar cTranslation = translation.at(translation.length() - 1);
    if (punctuation.contains(cSource))
    {
        if (not translation.endsWith(cSource))
        {
            testFail = true;

            if (locale == QLatin1String("el_GR")
                // Greek question mark
                // https://en.wikipedia.org/wiki/Question_mark#Greek_question_mark
                && (cSource == QLatin1Char('?') && cTranslation == QLatin1Char(';')))
            {
                testFail = false;
            }
            else if (locale == QLatin1String("zh_CN")
                     // Beside usage similar to that of English, the colon has other functions. Several
                     // compatibility forms for Chinese and Japanese typography are encoded in Unicode.
                     // https://en.wikipedia.org/wiki/Colon_(punctuation)#Usage_in_other_languages
                     && (cSource == QLatin1Char(':') && cTranslation == QString("：")))
            {
                testFail = false;
            }
        }
    }
    else
    {
        if (punctuation.contains(cTranslation))
        {
            cPunctuation = cTranslation;
            testFail = true;
        }
    }
    if (testFail)
    {
        const QString message = QString("Translation string does not end with the same punctuation character '%1' or "
                                        "vice versa. ")
                                    .arg(cPunctuation) +
                                QString("Original name:'%1'").arg(source) +
                                QString(", translated name:'%1'").arg(translation);
        QFAIL(qUtf8Printable(message));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void TST_TSLocaleTranslation::TestHTMLTags_data()
{
    QTest::addColumn<QString>("source");
    QTest::addColumn<QString>("translation");

    const QString filename = QString("valentina_%1.ts").arg(m_locale);

    const QDomNodeList messages = LoadTSFile(filename);
    if (messages.isEmpty())
    {
        QFAIL("Can't begin test.");
    }

    for (qint32 i = 0, num = messages.size(); i < num; ++i)
    {
        const QDomElement message = messages.at(i).toElement();
        if (message.isNull() == false)
        {
            const QString source = message.firstChildElement(TagSource).text();
            if (source.isEmpty())
            {
                continue;
            }

            const QDomElement translationTag = message.firstChildElement(TagTranslation);
            if (translationTag.hasAttribute(AttrType))
            {
                const QString attrVal = translationTag.attribute(AttrType);
                if (attrVal == AttrValVanished || attrVal == AttrValUnfinished || attrVal == AttrValObsolete)
                {
                    continue;
                }
            }
            const QString translation = translationTag.text();
            if (translation.isEmpty())
            {
                continue;
            }

            const QString caseName = QStringLiteral("File '%1'.").arg(filename);
            QTest::newRow(qUtf8Printable(caseName)) << source << translation;
        }
        else
        {
            const QString caseName = QStringLiteral("File '%2'. Message %1 is null.").arg(i).arg(filename);
            QFAIL(qUtf8Printable(caseName));
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void TST_TSLocaleTranslation::TestHTMLTags()
{
    QFETCH(QString, source);
    QFETCH(QString, translation);

    static const QStringList tags = QStringList()
                                    << QLatin1String("p") << QLatin1String("html") << QLatin1String("body");
    static const QString pattern("{1}.*>");
    for (const auto &tag : tags)
    {
        const QRegularExpression openRegex(QLatin1String("<") + tag + pattern,
                                           QRegularExpression::DotMatchesEverythingOption);
        if (source.contains(openRegex))
        {
            const auto countOpenTag = source.count(openRegex);
            const QRegularExpression closeRegex(QLatin1String("</") + tag + pattern,
                                                QRegularExpression::DotMatchesEverythingOption);
            const auto countCloseTag = translation.count(closeRegex);
            if (not translation.contains(closeRegex) || countCloseTag != countOpenTag)
            {
                const QString message = QString("Tag mismatch. Tag: '<%1>'. ").arg(tag) +
                                        QString("Original name:'%1'").arg(source) +
                                        QString(", translated name:'%1'").arg(translation);
                QFAIL(qUtf8Printable(message));
            }
        }
    }
}
