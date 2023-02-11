/***************************************************************************************************
 **
 **  Copyright (C) 2016 Roman Telezhynskyi <dismine(at)gmail.com>
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

#include "qmudef.h"

#include <QLocale>
#include <QSet>

enum State
{
    Init     = 0,
    Sign     = 1,
    Thousand = 2,
    Mantissa = 3,
    Dot	     = 4,
    Abscissa = 5,
    ExpMark	 = 6,
    ExpSign	 = 7,
    Exponent = 8,
    Done	 = 9
};

enum InputToken
{
    InputSign       = 1,
    InputThousand   = 2,
    InputDigit      = 3,
    InputDot        = 4,
    InputExp        = 5
};

static const QChar QmuEOF = QChar(static_cast<ushort>(0xffff)); //guaranteed not to be a character.

//---------------------------------------------------------------------------------------------------------------------
static QChar GetChar(const QString &formula, int &index)
{
    if (index >= formula.size())
    {
        return QmuEOF;
    }

    return formula.at(index++);
}

//---------------------------------------------------------------------------------------------------------------------
static QChar EatWhiteSpace(const QString &formula, int &index)
{
    QChar c;
    do
    {
        c = GetChar(formula, index);
    }
    while ( c != QmuEOF && c.isSpace() );

    return c;
}

//---------------------------------------------------------------------------------------------------------------------
static int CheckChar(QChar &c, const QLocale &locale, const QChar &decimal, const QChar &thousand)
{
    INIT_LOCALE_VARIABLES(locale);
    Q_UNUSED(decimalPoint)
    Q_UNUSED(groupSeparator)

    if (c == positiveSign)
    {
        c = '+';
        return InputToken::InputSign;
    }
    else if (c == negativeSign)
    {
        c = '-';
        return InputToken::InputSign;
    }
    else if (c == sign0)
    {
        c = '0';
        return InputToken::InputDigit;
    }
    else if (c == sign1)
    {
        c = '1';
        return InputToken::InputDigit;
    }
    else if (c == sign2)
    {
        c = '2';
        return InputToken::InputDigit;
    }
    else if (c == sign3)
    {
        c = '3';
        return InputToken::InputDigit;
    }
    else if (c == sign4)
    {
        c = '4';
        return InputToken::InputDigit;
    }
    else if (c == sign5)
    {
        c = '5';
        return InputToken::InputDigit;
    }
    else if (c == sign6)
    {
        c = '6';
        return InputToken::InputDigit;
    }
    else if (c == sign7)
    {
        c = '7';
        return InputToken::InputDigit;
    }
    else if (c == sign8)
    {
        c = '8';
        return InputToken::InputDigit;
    }
    else if (c == sign9)
    {
        c = '9';
        return InputToken::InputDigit;
    }
    else if (c == decimal)
    {
        return InputToken::InputDot;
    }
    else if (c == thousand)
    {
        return InputToken::InputThousand;
    }
    else if (c == expLower)
    {
        c = 'e';
        return InputToken::InputExp;
    }
    else if (c == expUpper)
    {
        c = 'E';
        return InputToken::InputExp;
    }
    else
    {
        return 0;
    }

    return 0;
}

//---------------------------------------------------------------------------------------------------------------------
qmusizetype ReadVal(const QString &formula, qreal &val, const QLocale &locale, const QChar &decimal,
                    const QChar &thousand)
{
    // Must not be equal
    if (decimal == thousand || formula.isEmpty())
    {
        val = 0;
        return -1;
    }

    INIT_LOCALE_VARIABLES(locale);
    Q_UNUSED(decimalPoint)
    Q_UNUSED(groupSeparator)

    QSet<QChar> reserved
    {
                positiveSign,
                negativeSign,
                sign0,
                sign1,
                sign2,
                sign3,
                sign4,
                sign5,
                sign6,
                sign7,
                sign8,
                sign9,
                expUpper,
                expLower
    };

    if (reserved.contains(decimal) || reserved.contains(thousand))
    {
        val = 0;
        return -1;
    }

    // row - current state, column - new state
    static uchar table[9][6] =
    {
        /*    None	     InputSign     InputThousand      InputDigit     InputDot      InputExp */
        { 0,	       State::Sign,    0,               State::Mantissa, State::Dot,  0,	         }, // Init
        { 0,	       0,	           0,               State::Mantissa, State::Dot,  0,	         }, // Sign
        { 0,	       0,	           0,               State::Mantissa, 0,	          0,	         }, // Thousand
        { State::Done, State::Done,    State::Thousand, State::Mantissa, State::Dot,  State::ExpMark,}, // Mantissa
        { 0,	       0,	           0,               State::Abscissa, 0,	          0,	         }, // Dot
        { State::Done, State::Done,    0,               State::Abscissa, 0,           State::ExpMark,}, // Abscissa
        { 0,	       State::ExpSign, 0,               State::Exponent, 0,	          0,	         }, // ExpMark
        { 0,	       0,	           0,               State::Exponent, 0,	          0,	         }, // ExpSign
        { State::Done, 0,              0,               State::Exponent, 0,           State::Done	 }  // Exponent
    };

    int state = State::Init;	// parse state
    QString buf;

    int index = 0; // start position
    QChar c = EatWhiteSpace(formula, index);

    while ( true )
    {
        const int input = CheckChar(c, locale, decimal, thousand);// input token

        state = table[state][input];

        if (state == 0)
        {
            val = 0;
            return -1;
        }
        else if (state == Done)
        {
            // Convert to C locale
            QLocale cLocale(QLocale::C);
            const QChar cDecimal = LocaleDecimalPoint(cLocale);
            const QChar cThousand = LocaleGroupSeparator(cLocale);
            if (locale != cLocale && (cDecimal != decimal || cThousand != thousand))
            {
                if (decimal == cThousand)
                {// Handle reverse to C locale case: thousand '.', decimal ','
                    const QChar tmpThousand = QLatin1Char('@');
                    buf.replace(thousand, tmpThousand);
                    buf.replace(decimal, cDecimal);
                    buf.replace(tmpThousand, cThousand);
                }
                else
                {
                    buf.replace(thousand, cThousand);
                    buf.replace(decimal, cDecimal);
                }
            }

            bool ok = false;
            const double d = cLocale.toDouble(buf, &ok);
            if (ok)
            {
                val = d;
                return buf.size();
            }

            val = 0;
            return -1;
        }

        buf.append(c);
        c = GetChar(formula, index);
    }

    return -1;
}

//---------------------------------------------------------------------------------------------------------------------
QString NameRegExp()
{
    static QString regex;

    if (regex.isEmpty())
    {
        const QList<QLocale> allLocales =
                QLocale::matchingLocales(QLocale::AnyLanguage, QLocale::AnyScript, QLocale::AnyCountry);

        QString positiveSigns;
        QString negativeSigns;
        QString decimalPoints;
        QString groupSeparators;

        for(const auto &locale : allLocales)
        {
            if (not positiveSigns.contains(LocalePositiveSign(locale)))
            {
                positiveSigns.append(LocalePositiveSign(locale));
            }

            if (not negativeSigns.contains(LocaleNegativeSign(locale)))
            {
                negativeSigns.append(LocaleNegativeSign(locale));
            }

            if (not decimalPoints.contains(LocaleDecimalPoint(locale)))
            {
                decimalPoints.append(LocaleDecimalPoint(locale));
            }

            if (not groupSeparators.contains(LocaleGroupSeparator(locale)))
            {
                groupSeparators.append(LocaleGroupSeparator(locale));
            }
        }

        negativeSigns.replace('-', QLatin1String("\\-"));
        groupSeparators.remove('\'');

        //Same regexp in pattern.xsd shema file. Don't forget to synchronize.
        // \p{Nd} - \p{Decimal_Digit_Number}
        // \p{Zs} - \p{Space_Separator}
        // Here we use permanent start of string and end of string anchors \A and \z to match whole pattern as one
        // string. In some cases, a user may pass multiline or line that ends with a new line. To cover case with a new
        // line at the end of string use /z anchor.
        regex = QString("\\A([^\\p{Nd}\\p{Zs}*\\/&|!<>^\\n\\()%1%2%3%4=?:;'\"]){1,1}"
                        "([^\\p{Zs}*\\/&|!<>^\\n\\()%1%2%3%4=?:;\"]){0,}\\z")
                .arg(negativeSigns, positiveSigns, decimalPoints, groupSeparators);
    }

    return regex;
}

//---------------------------------------------------------------------------------------------------------------------
qmusizetype FindFirstNotOf(const QString &string, const QString &chars, qmusizetype pos)
{
    qmusizetype chPos = pos;
    QString::const_iterator it = string.constBegin() + pos;
    QString::const_iterator end = string.constEnd();
    while (it != end)
    {
        if (not chars.contains(*it))
        {
            return chPos;
        }
        ++it;
        ++chPos;
    }

    return -1;
}

//---------------------------------------------------------------------------------------------------------------------
bool SupportedLocale(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return locale.positiveSign().size() == 1 &&
                locale.negativeSign().size() == 1 &&
                locale.toString(0).size() == 1 &&
                locale.toString(1).size() == 1 &&
                locale.toString(2).size() == 1 &&
                locale.toString(3).size() == 1 &&
                locale.toString(4).size() == 1 &&
                locale.toString(5).size() == 1 &&
                locale.toString(6).size() == 1 &&
                locale.toString(7).size() == 1 &&
                locale.toString(8).size() == 1 &&
                locale.toString(9).size() == 1 &&
                locale.exponential().size() == 1 &&
                locale.decimalPoint().size() == 1 &&
                locale.groupSeparator().size() == 1;
#else
    Q_UNUSED(locale)
    return true;
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocalePositiveSign(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.positiveSign();
    if (sign.size() == 1)
    {
        return sign.front();
    }

    return QLocale::c().positiveSign().front();
#else
    return locale.positiveSign();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleNegativeSign(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.negativeSign();
    if (sign.size() == 1)
    {
        return sign.front();
    }

    return QLocale::c().negativeSign().front();
#else
    return locale.negativeSign();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign0(const QLocale &locale)
{
    const QString sign = locale.toString(0);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'0'};
#else
    return QChar('0');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign1(const QLocale &locale)
{
    const QString sign = locale.toString(1);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'1'};
#else
    return QChar('1');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign2(const QLocale &locale)
{
    const QString sign = locale.toString(2);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'2'};
#else
    return QChar('2');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign3(const QLocale &locale)
{
    const QString sign = locale.toString(3);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'3'};
#else
    return QChar('3');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign4(const QLocale &locale)
{
    const QString sign = locale.toString(4);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'4'};
#else
    return QChar('4');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign5(const QLocale &locale)
{
    const QString sign = locale.toString(5);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'5'};
#else
    return QChar('5');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign6(const QLocale &locale)
{
    const QString sign = locale.toString(6);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'6'};
#else
    return QChar('6');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign7(const QLocale &locale)
{
    const QString sign = locale.toString(7);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'7'};
#else
    return QChar('7');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign8(const QLocale &locale)
{
    const QString sign = locale.toString(8);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'8'};
#else
    return QChar('8');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleSign9(const QLocale &locale)
{
    const QString sign = locale.toString(9);
    if (sign.size() == 1)
    {
        return sign.front();
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return {'9'};
#else
    return QChar('9');
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleExpUpper(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.exponential();
    if (sign.size() == 1)
    {
        return sign.front().toUpper();
    }

    return QLocale::c().exponential().front().toUpper();
#else
    return locale.exponential().toUpper();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleExpLower(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.exponential();
    if (sign.size() == 1)
    {
        return sign.front().toLower();
    }

    return QLocale::c().exponential().front().toLower();
#else
    return locale.exponential().toLower();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleDecimalPoint(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.decimalPoint();
    if (sign.size() == 1)
    {
        return sign.front();
    }

    return QLocale::c().decimalPoint().front();
#else
    return locale.decimalPoint();
#endif
}

//---------------------------------------------------------------------------------------------------------------------
QChar LocaleGroupSeparator(const QLocale &locale)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QString sign = locale.groupSeparator();
    if (sign.size() == 1)
    {
        return sign.front();
    }

    return QLocale::c().groupSeparator().front();
#else
    return locale.groupSeparator();
#endif
}
