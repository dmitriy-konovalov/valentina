/************************************************************************
 **
 **  @file   tst_dxf.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   15 1, 2020
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2020 Valentina project
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
#include "tst_dxf.h"

#include <QtTest>

#include "../vmisc/compatibility.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include "../vdxf/dxfdef.h"
#include "../vdxf/libdxfrw/intern/drw_textcodec.h"
#include "../vmisc/def.h"
#endif

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include "../vmisc/vtextcodec.h"
#else
#include <QTextCodec>
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#include "../vmisc/backport/text.h"
#endif

namespace
{
//---------------------------------------------------------------------------------------------------------------------
auto AvailableCodecs() -> QStringList
{
    QList<QByteArray> codecs = VTextCodec::availableCodecs();
    QSet<QString> uniqueNames;
    for (auto &codec : codecs)
    {
        uniqueNames.insert(codec);
    }

    return ConvertToList(uniqueNames);
}
} // namespace

//---------------------------------------------------------------------------------------------------------------------
TST_DXF::TST_DXF(QObject *parent)
  : QObject(parent)
{
}

//---------------------------------------------------------------------------------------------------------------------
void TST_DXF::initTestCase()
{
    QTextStream ts(stdout);
    ts << QStringLiteral("Available codecs:\n%2.").arg(AvailableCodecs().join(", ")) << Qt::endl;
}

//---------------------------------------------------------------------------------------------------------------------
void TST_DXF::TestCodecPage_data()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTest::addColumn<QString>("locale");

    QStringList locales = SupportedLocales();

    for (auto &locale : locales)
    {
        QTest::newRow(locale.toLatin1()) << locale;
    }
#endif
}

//---------------------------------------------------------------------------------------------------------------------
void TST_DXF::TestCodecPage()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QFETCH(QString, locale);

    static QMap<QString, QString> locMap = LocaleMap();
    locMap.insert("en", "ISO8859-1");
    locMap.insert("en_US", "ISO8859-1");
    locMap.insert("en_CA", "ISO8859-1");
    locMap.insert("en_IN", "ISO8859-1");

    QString language = QLocale(locale).name();
    QVERIFY(locMap.contains(language));

    QString codePage = locMap.value(language);
    codePage = codePage.toUpper();
    QMap<QString, QStringList> codeMap = DRW_TextCodec::DXFCodePageMap();

    QString dxfCodePage;

    auto i = codeMap.constBegin();
    while (i != codeMap.constEnd())
    {
        if (i.value().contains(codePage))
        {
            dxfCodePage = i.key();
            break;
        }
        ++i;
    }

    QVERIFY(not dxfCodePage.isEmpty());

    VTextCodec *codec = DRW_TextCodec::CodecForName(dxfCodePage);

    QVERIFY2(codec != nullptr, qUtf8Printable(QStringLiteral("No codec for dxf codepage %1 found.").arg(dxfCodePage)));
#else
    QSKIP("No full support for old codecs since Qt6.");
#endif
}
