/************************************************************************
 **
 **  @file   vtranslatevars.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   10 6, 2015
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2015 Valentina project
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

#include "vtranslatevars.h"

#include <QList>
#include <QLocale>
#include <QMap>
#include <QMessageLogger>
#include <QString>
#include <QtDebug>

#include "../ifc/ifcdef.h"
#include "../qmuparser/qmudef.h"
#include "../qmuparser/qmuparsererror.h"
#include "../qmuparser/qmutokenparser.h"
#include "../qmuparser/qmutranslation.h"
#include "../vmisc/compatibility.h"
#include "../vmisc/def.h"
#include "../vmisc/literals.h"
#include "../vmisc/vabstractapplication.h"
#include "../vpatterndb/vtranslatevars.h"
#include "pmsystems.h"
#include "vtranslatemeasurements.h"

using namespace Qt::Literals::StringLiterals;

//---------------------------------------------------------------------------------------------------------------------
VTranslateVars::VTranslateVars()
{
    InitPatternMakingSystems();
    InitVariables();
    InitFunctions();

    PrepareFunctionTranslations();
}

#define translate(context, source, disambiguation) qmu::QmuTranslation::translate((context), (source), (disambiguation))

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::InitPatternMakingSystems()
{
    // Note. We can't use here function and variables because lupdate tool doesn't see string in variables and doesn't
    // mark such string to translation.
    qmu::QmuTranslation name;
    qmu::QmuTranslation author;
    qmu::QmuTranslation book;

    //=================================================================================================================
    name = translate("VTranslateVars", "Bunka", "System name");
    author = translate("VTranslateVars", "Bunka Fashion College", "Author name");
    book = translate("VTranslateVars", "Fundamentals of Garment Design", "Book name");
    InitSystem(p0_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Barnfield and Richard", "System name");
    author = translate("VTranslateVars", "Jo Barnfield and Andrew Richards", "Author name");
    book = translate("VTranslateVars", "Pattern Making Primer", "Book name");
    InitSystem(p1_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Friendship/Women", "System name");
    author = translate("VTranslateVars", "Elizabeth Friendship", "Author name");
    book = translate("VTranslateVars",
                     "Creating Historical Clothes - Pattern Cutting from the 16th to the 19th Centuries", "Book name");
    InitSystem(p2_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Morris, K.", "System name");
    author = translate("VTranslateVars", "Karen Morris", "Author name");
    book = translate("VTranslateVars", "Sewing Lingerie that Fits", "Book name");
    InitSystem(p3_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Castro", "System name");
    author = translate("VTranslateVars", "Lucia Mors de Castro", "Author name");
    book = translate("VTranslateVars", "Patternmaking in Practic", "Book name");
    InitSystem(p4_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Kim & Uh", "System name");
    author = translate("VTranslateVars", "Injoo Kim and Mykyung Uh", "Author name");
    book = translate("VTranslateVars", "Apparel Making in Fashion Design", "Book name");
    InitSystem(p5_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Waugh", "System name");
    author = translate("VTranslateVars", "Norah Waugh", "Author name");
    book = translate("VTranslateVars", "Corsets and Crinolines", "Book name");
    InitSystem(p6_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Grimble", "System name");
    author = translate("VTranslateVars", "Frances Grimble", "Author name");
    book = translate("VTranslateVars", "Fashions of the Gilded Age", "Book name");
    InitSystem(p7_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Thornton's International System", "System name");
    author = translate("VTranslateVars", "ed. R. L. Shep", "Author name");
    book = translate("VTranslateVars", "The Great War: Styles and Patterns of the 1910s", "Book name");
    InitSystem(p8_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Hillhouse & Mansfield", "System name");
    author = translate("VTranslateVars", "Marion S. Hillhouse and Evelyn A. Mansfield", "Author name");
    book = translate("VTranslateVars", "Dress Design: Draping and Flat Pattern Making", "Book name");
    InitSystem(p9_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Pivnick", "System name");
    author = translate("VTranslateVars", "Esther Kaplan Pivnick", "Author name");
    book = translate("VTranslateVars", "How to Design Beautiful Clothes: Designing and Pattern Making", "Book name");
    InitSystem(p10_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Minister & Son", "System name");
    author = translate("VTranslateVars", "Edward Minister & Son, ed. R. L. Shep", "Author name");
    book = translate("VTranslateVars", "The Complete Guide to Practical Cutting (1853)", "Book name");
    InitSystem(p11_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Strickland", "System name");
    author = translate("VTranslateVars", "Gertrude Strickland", "Author name");
    book = translate("VTranslateVars", "A Tailoring Manual", "Book name");
    InitSystem(p12_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Loh & Lewis", "System name");
    author = translate("VTranslateVars", "May Loh and Diehl Lewis", "Author name");
    book = translate("VTranslateVars", "Patternless Fashion Design", "Book name");
    InitSystem(p13_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Morris, F. R.", "System name");
    author = translate("VTranslateVars", "F. R. Morris", "Author name");
    book = translate("VTranslateVars", "Ladies Garment Cutting and Making", "Book name");
    InitSystem(p14_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Mason", "System name");
    author = translate("VTranslateVars", "Gertrude Mason", "Author name");
    book = translate("VTranslateVars", "Gertrude Mason's Patternmaking Book", "Book name");
    InitSystem(p15_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Kimata", "System name");
    author = translate("VTranslateVars", "K. Kimata", "Author name");
    book = translate("VTranslateVars", "K.Kimata's Simplified Drafting Book for Dressmaking", "Book name");
    InitSystem(p16_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Master Designer", "System name");
    author = translate("VTranslateVars", "The Master Designer (Chicago, IL)", "Author name");
    book = translate("VTranslateVars", "Master Designer's System of Designing, Cutting and Grading", "Book name");
    InitSystem(p17_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Kopp", "System name");
    author = translate("VTranslateVars", "Ernestine Kopp, Vittorina Rolfo, Beatrice Zelin, Lee Gross", "Author name");
    book = translate("VTranslateVars", "How to Draft Basic Patterns", "Book name");
    InitSystem(p18_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Ekern", "System name");
    author = translate("VTranslateVars", "Doris Ekern", "Author name");
    book = translate("VTranslateVars", "Slacks Cut-to-Fit for Your Figure", "Book name");
    InitSystem(p19_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Doyle", "System name");
    author = translate("VTranslateVars", "Sarah J. Doyle", "Author name");
    book = translate("VTranslateVars", "Sarah's Key to Pattern Drafting", "Book name");
    InitSystem(p20_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Shelton", "System name");
    author = translate("VTranslateVars", "Karla J. Shelton", "Author name");
    book = translate("VTranslateVars", "Design and Sew Jeans", "Book name");
    InitSystem(p21_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Lady Boutique", "System name");
    author = translate("VTranslateVars", "Lady Boutique", "Author name");
    book = translate("VTranslateVars", "Lady Boutique magazine (Japan)", "Book name");
    InitSystem(p22_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Rohr", "System name");
    author = translate("VTranslateVars", "M. Rohr", "Author name");
    book = translate("VTranslateVars", "Pattern Drafting and Grading: Women's nd Misses' Garment Design", "Book name");
    InitSystem(p23_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Moore", "System name");
    author = translate("VTranslateVars", "Dorothy Moore", "Author name");
    book = translate("VTranslateVars", "Dorothy Moore's Pattern Drafting and Dressmaking", "Book name");
    InitSystem(p24_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Abling", "System name");
    author = translate("VTranslateVars", "Bina Abling", "Author name");
    book = translate("VTranslateVars", "Integrating Draping, Drafting and Drawing", "Book name");
    InitSystem(p25_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Fukomoto", "System name");
    author = translate("VTranslateVars", "Sue S. Fukomoto", "Author name");
    book = translate("VTranslateVars",
                     "Scientific Pattern Drafting as taught at Style Center School of Costume "
                     "Design, Dressmaking and Millinery",
                     "Book name");
    InitSystem(p26_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Dressmaking International", "System name");
    author = translate("VTranslateVars", "Dressmaking International", "Author name");
    book = translate("VTranslateVars", "Dressmaking International magazine (Japan)", "Book name");
    InitSystem(p27_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Erwin", "System name");
    author = translate("VTranslateVars", "Mabel D. Erwin", "Author name");
    book = translate("VTranslateVars", "Practical Dress Design", "Book name");
    InitSystem(p28_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Gough", "System name");
    author = translate("VTranslateVars", "E. L. G. Gough", "Author name");
    book = translate("VTranslateVars", "Principles of Garment Cutting", "Book name");
    InitSystem(p29_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Allemong", "System name");
    author = translate("VTranslateVars", "Elizabeth M. Allemong", "Author name");
    book = translate("VTranslateVars", "European Cut", "Book name");
    InitSystem(p30_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "McCunn", "System name");
    author = translate("VTranslateVars", "Donald H. McCunn", "Author name");
    book = translate("VTranslateVars", "How to Make Your Own Sewing Patterns", "Book name");
    InitSystem(p31_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Zarapkar", "System name");
    author = translate("VTranslateVars", "Shri K. R. Zarapkar and Shri Arvind K. Zarapkar", "Author name");
    book = translate("VTranslateVars", "Zarapkar System of Cutting", "Book name");
    InitSystem(p32_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Kunick", "System name");
    author = translate("VTranslateVars", "Philip Kunick", "Author name");
    book = translate("VTranslateVars", "Sizing, Pattern Construction and Grading for Women's and Children's Garments",
                     "Book name");
    InitSystem(p33_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Handford", "System name");
    author = translate("VTranslateVars", "Jack Handford", "Author name");
    book = translate("VTranslateVars", "Professional Patternmaking for Designers: Women's Wear, Men's Casual Wear",
                     "Book name");
    InitSystem(p34_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Davis", "System name");
    author = translate("VTranslateVars", "R. I. Davis", "Author name");
    book = translate("VTranslateVars", "Men's 17th & 18th Century Costume, Cut & Fashion", "Book name");
    InitSystem(p35_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "MacLochlainn", "System name");
    author = translate("VTranslateVars", "Jason MacLochlainn", "Author name");
    book = translate("VTranslateVars", "The Victorian Tailor: An Introduction to Period Tailoring", "Book name");
    InitSystem(p36_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Joseph-Armstrong", "System name");
    author = translate("VTranslateVars", "Helen Joseph-Armstrong", "Author name");
    book = translate("VTranslateVars", "Patternmaking for Fashion Design", "Book name");
    InitSystem(p37_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Supreme System", "System name");
    author = translate("VTranslateVars", "Frederick T. Croonberg", "Author name");
    book = translate("VTranslateVars",
                     "The Blue Book of Men's Tailoring, Grand Edition of Supreme System for "
                     "Producing Mens Garments (1907)",
                     "Book name");
    InitSystem(p38_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Sugino", "System name");
    author = translate("VTranslateVars", "Dressmaking", "Author name");
    book = translate("VTranslateVars", "Pattern Drafting Vols. I, II, III (Japan)", "Book name");
    InitSystem(p39_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Centre Point System", "System name");
    author = translate("VTranslateVars", "Louis Devere", "Author name");
    book = translate("VTranslateVars", "The Handbook of Practical Cutting on the Centre Point System", "Book name");
    InitSystem(p40_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Aldrich/Men", "System name");
    author = translate("VTranslateVars", "Winifred Aldrich", "Author name");
    book = translate("VTranslateVars", "Metric Pattern Cutting for Menswear", "Book name");
    InitSystem(p41_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Aldrich/Women", "System name");
    author = translate("VTranslateVars", "Winifred Aldrich", "Author name");
    book = translate("VTranslateVars", "Metric Pattern Cutting for Women's Wear", "Book name");
    InitSystem(p42_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Kershaw", "System name");
    author = translate("VTranslateVars", "Gareth Kershaw", "Author name");
    book = translate("VTranslateVars", "Patternmaking for Menswear", "Book name");
    InitSystem(p43_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Gilewska", "System name");
    author = translate("VTranslateVars", "Teresa Gilewska", "Author name");
    book = translate("VTranslateVars", "Pattern-Drafting for Fashion: The Basics", "Book name");
    InitSystem(p44_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Lo", "System name");
    author = translate("VTranslateVars", "Dennic Chunman Lo", "Author name");
    book = translate("VTranslateVars", "Pattern Cutting", "Book name");
    InitSystem(p45_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Bray", "System name");
    author = translate("VTranslateVars", "Natalie Bray", "Author name");
    book = translate("VTranslateVars", "Dress Pattern Designing: The Basic Principles of Cut and Fit", "Book name");
    InitSystem(p46_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Knowles/Men", "System name");
    author = translate("VTranslateVars", "Lori A. Knowles", "Author name");
    book = translate("VTranslateVars", "The Practical Guide to Patternmaking for Fashion Designers: Menswear",
                     "Book name");
    InitSystem(p47_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Friendship/Men", "System name");
    author = translate("VTranslateVars", "Elizabeth Friendship", "Author name");
    book = translate("VTranslateVars", "Pattern Cutting for Men's Costume", "Book name");
    InitSystem(p48_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Brown", "System name");
    author = translate("VTranslateVars", "P. Clement Brown", "Author name");
    book = translate("VTranslateVars", "Art in Dress", "Book name");
    InitSystem(p49_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Mitchell", "System name");
    author = translate("VTranslateVars", "Jno. J. Mitchell", "Author name");
    book = translate("VTranslateVars",
                     "\"Standard\" Work on Cutting (Men's Garments) 1886: The Art and Science of "
                     "Garment Cutting",
                     "Book name");
    InitSystem(p50_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "GOST 17917-86", "System name");
    author = translate("VTranslateVars", "Ministry of consumer industry of the USSR", "Author name");
    book = translate("VTranslateVars", "Standard figure boys", "Book name");
    InitSystem(p51_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Eddy", "System name");
    author = translate("VTranslateVars", "Josephine F. Eddy and Elizabeth C. B. Wiley", "Author name");
    book = translate("VTranslateVars", "Pattern and Dress Design", "Book name");
    InitSystem(p52_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "Knowles/Women", "System name");
    author = translate("VTranslateVars", "Lori A. Knowles", "Author name");
    book = translate("VTranslateVars",
                     "Practical Guide to Patternmaking for Fashion Designers: Juniors, Misses, and "
                     "Women",
                     "Book name");
    InitSystem(p53_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "American Garment Cutter", "System name");
    author = translate("VTranslateVars", "ed. R. L. Shep", "Author name");
    book = translate("VTranslateVars", "The Great War: Styles and Patterns of the 1910s", "Book name");
    InitSystem(p54_S, name, author, book);
    //=================================================================================================================
    name = translate("VTranslateVars", "None", "System name");
    author = translate("VTranslateVars", "Valentina team", "Author name");
    book = translate("VTranslateVars", "Valentina's internal standard", "Book name");
    InitSystem(p998_S, name, author, book);
    //=================================================================================================================
}

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::InitVariables()
{
    variables.insert(measurement_, translate("VTranslateVars", "M_", "Left symbol _ in the name"));
    variables.insert(increment_, translate("VTranslateVars", "Increment_", "Left symbol _ in the name"));
    variables.insert(line_, translate("VTranslateVars", "Line_", "Left symbol _ in the name"));
    variables.insert(angleLine_, translate("VTranslateVars", "AngleLine_", "Left symbol _ in the name"));
    variables.insert(arc_, translate("VTranslateVars", "Arc_", "Left symbol _ in the name"));
    variables.insert(elarc_, translate("VTranslateVars", "ElArc_", "Left symbol _ in the name"));
    variables.insert(spl_, translate("VTranslateVars", "Spl_", "Left symbol _ in the name"));
    variables.insert(splPath, translate("VTranslateVars", "SplPath", "Do not add symbol _ to the end of the name"));
    variables.insert(radiusArc_, translate("VTranslateVars", "RadiusArc_", "Left symbol _ in the name"));
    variables.insert(radius1ElArc_, translate("VTranslateVars", "Radius1ElArc_", "Left symbol _ in the name"));
    variables.insert(radius2ElArc_, translate("VTranslateVars", "Radius2ElArc_", "Left symbol _ in the name"));
    variables.insert(angle1Arc_, translate("VTranslateVars", "Angle1Arc_", "Left symbol _ in the name"));
    variables.insert(angle2Arc_, translate("VTranslateVars", "Angle2Arc_", "Left symbol _ in the name"));
    variables.insert(angle1ElArc_, translate("VTranslateVars", "Angle1ElArc_", "Left symbol _ in the name"));
    variables.insert(angle2ElArc_, translate("VTranslateVars", "Angle2ElArc_", "Left symbol _ in the name"));
    variables.insert(angle1Spl_, translate("VTranslateVars", "Angle1Spl_", "Left symbol _ in the name"));
    variables.insert(angle2Spl_, translate("VTranslateVars", "Angle2Spl_", "Left symbol _ in the name"));
    variables.insert(angle1SplPath,
                     translate("VTranslateVars", "Angle1SplPath", "Do not add symbol _ to the end of the name"));
    variables.insert(angle2SplPath,
                     translate("VTranslateVars", "Angle2SplPath", "Do not add symbol _ to the end of the name"));
    variables.insert(seg_, translate("VTranslateVars", "Seg_", "Segment. Left symbol _ in the name"));
    variables.insert(currentLength, translate("VTranslateVars", "CurrentLength", "Do not add space between words"));
    variables.insert(currentSeamAllowance,
                     translate("VTranslateVars", "CurrentSeamAllowance", "Do not add space between words"));
    variables.insert(c1LengthSpl_, translate("VTranslateVars", "C1LengthSpl_", "Left symbol _ in the name"));
    variables.insert(c2LengthSpl_, translate("VTranslateVars", "C2LengthSpl_", "Left symbol _ in the name"));
    variables.insert(c1LengthSplPath,
                     translate("VTranslateVars", "C1LengthSplPath", "Do not add symbol _ to the end of the name"));
    variables.insert(c2LengthSplPath,
                     translate("VTranslateVars", "C2LengthSplPath", "Do not add symbol _ to the end of the name"));
    variables.insert(rotationElArc_, translate("VTranslateVars", "RotationElArc_", "Left symbol _ in the name"));
    variables.insert(pieceArea_, translate("VTranslateVars", "PieceArea_", "Left symbol _ in the name"));
    variables.insert(pieceSeamLineArea_,
                     translate("VTranslateVars", "PieceSeamLineArea_", "Left symbol _ in the name"));
}

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::InitFunctions()
{
    const QString oneArgument = QStringLiteral("()");
    const QString twoArguments = QStringLiteral("(;)");
    const QString threeArguments = QStringLiteral("(;;)");

    functions.insert(degTorad_F, translate("VTranslateVars", "degTorad", "converts degrees to radian"));
    functionsDescriptions.insert(degTorad_F,
                                 translate("VTranslateVars", "converts degrees to radian", "function degTorad"));
    functionsArguments.insert(degTorad_F, oneArgument);

    functions.insert(radTodeg_F, translate("VTranslateVars", "radTodeg", "converts radian to degrees"));
    functionsDescriptions.insert(radTodeg_F,
                                 translate("VTranslateVars", "converts radian to degrees", "function radTodeg"));
    functionsArguments.insert(radTodeg_F, oneArgument);

    functions.insert(sin_F, translate("VTranslateVars", "sin", "sine function working with radians"));
    functionsDescriptions.insert(sin_F,
                                 translate("VTranslateVars", "sine function working with radians", "function sin"));
    functionsArguments.insert(sin_F, oneArgument);

    functions.insert(cos_F, translate("VTranslateVars", "cos", "cosine function working with radians"));
    functionsDescriptions.insert(cos_F,
                                 translate("VTranslateVars", "cosine function working with radians", "function cos"));
    functionsArguments.insert(cos_F, oneArgument);

    functions.insert(tan_F, translate("VTranslateVars", "tan", "tangens function working with radians"));
    functionsDescriptions.insert(tan_F,
                                 translate("VTranslateVars", "tangens function working with radians", "function tan"));
    functionsArguments.insert(tan_F, oneArgument);

    functions.insert(asin_F, translate("VTranslateVars", "asin", "arcus sine function working with radians"));
    functionsDescriptions.insert(
        asin_F, translate("VTranslateVars", "arcus sine function working with radians", "function asin"));
    functionsArguments.insert(asin_F, oneArgument);

    functions.insert(acos_F, translate("VTranslateVars", "acos", "arcus cosine function working with radians"));
    functionsDescriptions.insert(
        acos_F, translate("VTranslateVars", "arcus cosine function working with radians", "function acos"));
    functionsArguments.insert(acos_F, oneArgument);

    functions.insert(atan_F, translate("VTranslateVars", "atan", "arcus tangens function working with radians"));
    functionsDescriptions.insert(
        atan_F, translate("VTranslateVars", "arcus tangens function working with radians", "function atan"));
    functionsArguments.insert(atan_F, oneArgument);

    functions.insert(sinh_F, translate("VTranslateVars", "sinh", "hyperbolic sine function"));
    functionsDescriptions.insert(sinh_F, translate("VTranslateVars", "hyperbolic sine function", "function sinh"));
    functionsArguments.insert(sinh_F, oneArgument);

    functions.insert(cosh_F, translate("VTranslateVars", "cosh", "hyperbolic cosine"));
    functionsDescriptions.insert(cosh_F, translate("VTranslateVars", "hyperbolic cosine", "function cosh"));
    functionsArguments.insert(sinh_F, oneArgument);

    functions.insert(tanh_F, translate("VTranslateVars", "tanh", "hyperbolic tangens function"));
    functionsDescriptions.insert(tanh_F, translate("VTranslateVars", "hyperbolic tangens function", "function tanh"));
    functionsArguments.insert(tanh_F, oneArgument);

    functions.insert(asinh_F, translate("VTranslateVars", "asinh", "hyperbolic arcus sine function"));
    functionsDescriptions.insert(asinh_F,
                                 translate("VTranslateVars", "hyperbolic arcus sine function", "function asinh"));
    functionsArguments.insert(asinh_F, oneArgument);

    functions.insert(acosh_F, translate("VTranslateVars", "acosh", "hyperbolic arcus cosine function"));
    functionsDescriptions.insert(acosh_F,
                                 translate("VTranslateVars", "hyperbolic arcus cosine function", "function acosh"));
    functionsArguments.insert(acosh_F, oneArgument);

    functions.insert(atanh_F, translate("VTranslateVars", "atanh", "hyperbolic arcur tangens function"));
    functionsDescriptions.insert(atanh_F,
                                 translate("VTranslateVars", "hyperbolic arcur tangens function", "function atanh"));
    functionsArguments.insert(atanh_F, oneArgument);

    functions.insert(sinD_F, translate("VTranslateVars", "sinD", "sine function working with degrees"));
    functionsDescriptions.insert(sinD_F,
                                 translate("VTranslateVars", "sine function working with degrees", "function sinD"));
    functionsArguments.insert(sinD_F, oneArgument);

    functions.insert(cosD_F, translate("VTranslateVars", "cosD", "cosine function working with degrees"));
    functionsDescriptions.insert(cosD_F,
                                 translate("VTranslateVars", "cosine function working with degrees", "function cosD"));
    functionsArguments.insert(cosD_F, oneArgument);

    functions.insert(tanD_F, translate("VTranslateVars", "tanD", "tangens function working with degrees"));
    functionsDescriptions.insert(tanD_F,
                                 translate("VTranslateVars", "tangens function working with degrees", "function tanD"));
    functionsArguments.insert(tanD_F, oneArgument);

    functions.insert(asinD_F, translate("VTranslateVars", "asinD", "arcus sine function working with degrees"));
    functionsDescriptions.insert(
        asinD_F, translate("VTranslateVars", "arcus sine function working with degrees", "function asinD"));
    functionsArguments.insert(asinD_F, oneArgument);

    functions.insert(acosD_F, translate("VTranslateVars", "acosD", "arcus cosine function working with degrees"));
    functionsDescriptions.insert(
        acosD_F, translate("VTranslateVars", "arcus cosine function working with degrees", "function acosD"));
    functionsArguments.insert(acosD_F, oneArgument);

    functions.insert(atanD_F, translate("VTranslateVars", "atanD", "arcus tangens function working with degrees"));
    functionsDescriptions.insert(
        atanD_F, translate("VTranslateVars", "arcus tangens function working with degrees", "function atanD"));
    functionsArguments.insert(atanD_F, oneArgument);

    functions.insert(log2_F, translate("VTranslateVars", "log2", "logarithm to the base 2"));
    functionsDescriptions.insert(log2_F, translate("VTranslateVars", "logarithm to the base 2", "function log2"));
    functionsArguments.insert(log2_F, oneArgument);

    functions.insert(log10_F, translate("VTranslateVars", "log10", "logarithm to the base 10"));
    functionsDescriptions.insert(log10_F, translate("VTranslateVars", "logarithm to the base 10", "function log10"));
    functionsArguments.insert(log10_F, oneArgument);

    functions.insert(log_F, translate("VTranslateVars", "log", "logarithm to the base 10"));
    functionsDescriptions.insert(log_F, translate("VTranslateVars", "logarithm to the base 10", "function log"));
    functionsArguments.insert(log_F, oneArgument);

    functions.insert(ln_F, translate("VTranslateVars", "ln", "logarithm to base e (2.71828...)"));
    functionsDescriptions.insert(ln_F, translate("VTranslateVars", "logarithm to base e (2.71828...)", "function ln"));
    functionsArguments.insert(ln_F, oneArgument);

    functions.insert(exp_F, translate("VTranslateVars", "exp", "e raised to the power of x"));
    functionsDescriptions.insert(exp_F, translate("VTranslateVars", "e raised to the power of x", "function exp"));
    functionsArguments.insert(exp_F, oneArgument);

    functions.insert(sqrt_F, translate("VTranslateVars", "sqrt", "square root of a value"));
    functionsDescriptions.insert(sqrt_F, translate("VTranslateVars", "square root of a value", "function sqrt"));
    functionsArguments.insert(sqrt_F, oneArgument);

    functions.insert(sign_F, translate("VTranslateVars", "sign", "sign function -1 if x<0; 1 if x>0"));
    functionsDescriptions.insert(sign_F,
                                 translate("VTranslateVars", "sign function -1 if x<0; 1 if x>0", "function sign"));
    functionsArguments.insert(sign_F, oneArgument);

    functions.insert(rint_F, translate("VTranslateVars", "rint", "round to nearest integer"));
    functionsDescriptions.insert(rint_F, translate("VTranslateVars", "round to nearest integer", "function rint"));
    functionsArguments.insert(rint_F, oneArgument);

    functions.insert(r2cm_F, translate("VTranslateVars", "r2cm", "round to up to 1 decimal"));
    functionsDescriptions.insert(r2cm_F, translate("VTranslateVars", "round to up to 1 decimal", "function r2cm"));
    functionsArguments.insert(r2cm_F, oneArgument);

    functions.insert(csrCm_F, translate("VTranslateVars", "csrCm",
                                        "cut, split and rotate modeling operation. Takes"
                                        " cm units."));
    functionsDescriptions.insert(csrCm_F, translate("VTranslateVars",
                                                    "cut, split and rotate modeling operation. Takes "
                                                    "cm units.",
                                                    "function csrCm"));
    functionsArguments.insert(csrCm_F, threeArguments);

    functions.insert(csrInch_F, translate("VTranslateVars", "csrInch",
                                          "cut, split and rotate modeling operation. Takes"
                                          " inch units."));
    functionsDescriptions.insert(csrInch_F, translate("VTranslateVars",
                                                      "cut, split and rotate modeling operation. "
                                                      "Takes inch units.",
                                                      "function csrInch"));
    functionsArguments.insert(csrInch_F, threeArguments);

    functions.insert(abs_F, translate("VTranslateVars", "abs", "absolute value"));
    functionsDescriptions.insert(abs_F, translate("VTranslateVars", "absolute value", "function abs"));
    functionsArguments.insert(abs_F, oneArgument);

    functions.insert(min_F, translate("VTranslateVars", "min", "min of all arguments"));
    functionsDescriptions.insert(min_F, translate("VTranslateVars", "min of all arguments", "function min"));
    functionsArguments.insert(min_F, twoArguments);

    functions.insert(max_F, translate("VTranslateVars", "max", "max of all arguments"));
    functionsDescriptions.insert(max_F, translate("VTranslateVars", "max of all arguments", "function max"));
    functionsArguments.insert(max_F, twoArguments);

    functions.insert(sum_F, translate("VTranslateVars", "sum", "sum of all arguments"));
    functionsDescriptions.insert(sum_F, translate("VTranslateVars", "sum of all arguments", "function sum"));
    functionsArguments.insert(sum_F, threeArguments);

    functions.insert(avg_F, translate("VTranslateVars", "avg", "mean value of all arguments"));
    functionsDescriptions.insert(avg_F, translate("VTranslateVars", "mean value of all arguments", "function avg"));
    functionsArguments.insert(avg_F, threeArguments);

    functions.insert(fmod_F, translate("VTranslateVars", "fmod",
                                       "Returns the floating-point remainder of numer/denom (rounded towards zero)"));
    functionsDescriptions.insert(fmod_F, translate("VTranslateVars",
                                                   "Returns the floating-point remainder of "
                                                   "numer/denom (rounded towards zero)",
                                                   "function fmod"));
    functionsArguments.insert(fmod_F, twoArguments);

    functions.insert(warning_F, translate("VTranslateVars", "warning", "Calculation warning"));
    functionsDescriptions.insert(warning_F,
                                 translate("VTranslateVars", "Show a warning in calculations", "function warning"));
    functionsArguments.insert(warning_F, twoArguments);
}

#undef translate

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::PrepareFunctionTranslations()
{
    translatedFunctions.clear();
    QMap<QString, qmu::QmuTranslation>::const_iterator i = functions.constBegin();
    while (i != functions.constEnd())
    {
        const QString translated = i.value().translate(VAbstractApplication::VApp()->Settings()->GetLocale());
        if (i.key() != translated)
        {
            translatedFunctions.insert(translated, i.key());
        }
        ++i;
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::InitSystem(const QString &code, const qmu::QmuTranslation &name, const qmu::QmuTranslation &author,
                                const qmu::QmuTranslation &book)
{
    PMSystemNames.insert(code, name);
    PMSystemAuthors.insert(code, author);
    PMSystemBooks.insert(code, book);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief CorrectionsPositions correct position tokens in expression after token translation.
 *
 * Because translated string can have different length compare to original need make correction after each translation.
 * If bias = 0 correction will not happens.
 *
 * @param position position currecnt token in expression
 * @param bias difference between original token length and translated
 * @param tokens all tokens
 * @param numbers all numbers
 */
void VTranslateVars::CorrectionsPositions(vsizetype position, vsizetype bias, QMap<vsizetype, QString> &tokens,
                                          QMap<vsizetype, QString> &numbers)
{
    if (bias == 0)
    {
        return; // Nothing to correct;
    }

    BiasTokens(position, bias, tokens);
    BiasTokens(position, bias, numbers);
}

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::TranslateVarsFromUser(QString &newFormula, QMap<vsizetype, QString> &tokens,
                                           QMap<vsizetype, QString> &numbers) const
{
    QList<vsizetype> tKeys = tokens.keys(); // Take all tokens positions
    QList<QString> tValues = tokens.values();
    for (vsizetype i = 0; i < tKeys.size(); ++i)
    {
        vsizetype bias = 0;
        if (MeasurementsFromUser(newFormula, tKeys.at(i), tValues.at(i), bias))
        {
            if (bias != 0)
            { // Translated token has different length than original. Position of next tokens need to be corrected.
                CorrectionsPositions(tKeys.at(i), bias, tokens, numbers);
                tKeys = tokens.keys();
                tValues = tokens.values();
            }
            continue;
        }

        if (VariablesFromUser(newFormula, tKeys.at(i), tValues.at(i), bias))
        {
            if (bias != 0)
            { // Translated token has different length than original. Position of next tokens need to be corrected.
                CorrectionsPositions(tKeys.at(i), bias, tokens, numbers);
                tKeys = tokens.keys();
                tValues = tokens.values();
            }
            continue;
        }

        if (FunctionsFromUser(newFormula, tKeys.at(i), tValues.at(i), bias))
        {
            if (bias != 0)
            { // Translated token has different length than original. Position of next tokens need to be corrected.
                CorrectionsPositions(tKeys.at(i), bias, tokens, numbers);
                tKeys = tokens.keys();
                tValues = tokens.values();
            }
            continue;
        }

        if (tValues.at(i) == LocaleNegativeSign(QLocale()))
        { // unary minus
            newFormula.replace(tKeys.at(i), 1, '-');
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::TranslateNumbersFromUser(QString &newFormula, QMap<vsizetype, QString> &tokens,
                                              QMap<vsizetype, QString> &numbers, bool osSeparator)
{
    QLocale loc = QLocale(); // User locale
    if (loc != QLocale::c() && osSeparator)
    {                                            // User want use Os separator
        QList<vsizetype> nKeys = numbers.keys(); // Positions for all numbers in expression
        QList<QString> nValues = numbers.values();
        for (int i = 0; i < nKeys.size(); ++i)
        {
            loc = QLocale(); // From system locale
            bool ok = false;
            const qreal d = loc.toDouble(nValues.at(i), &ok);
            if (not ok)
            {
                qDebug() << "Can't convert to double token" << nValues.at(i);
                continue; // Leave with out translation
            }

            loc = QLocale::c();                   // To internal locale
            const QString dStr = loc.toString(d); // Internal look for number
            newFormula.replace(nKeys.at(i), nValues.at(i).length(), dStr);
            const auto bias = nValues.at(i).length() - dStr.length();
            if (bias != 0)
            { // Translated number has different length than original. Position next tokens need to be corrected.
                CorrectionsPositions(nKeys.at(i), bias, tokens, numbers);
                nKeys = numbers.keys();
                nValues = numbers.values();
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::TranslateVarsToUser(QString &newFormula, QMap<vsizetype, QString> &tokens,
                                         QMap<vsizetype, QString> &numbers) const
{
    QList<vsizetype> tKeys = tokens.keys();
    QList<QString> tValues = tokens.values();
    for (vsizetype i = 0; i < tKeys.size(); ++i)
    {
        if (measurements.contains(tValues.at(i)))
        {
            newFormula.replace(
                tKeys.at(i), tValues.at(i).length(),
                measurements.value(tValues.at(i)).translate(VAbstractApplication::VApp()->Settings()->GetLocale()));
            auto bias = tValues.at(i).length() - measurements.value(tValues.at(i))
                                                     .translate(VAbstractApplication::VApp()->Settings()->GetLocale())
                                                     .length();
            if (bias != 0)
            { // Translated token has different length than original. Position next tokens need to be corrected.
                CorrectionsPositions(tKeys.at(i), bias, tokens, numbers);
                tKeys = tokens.keys();
                tValues = tokens.values();
            }
            continue;
        }

        if (functions.contains(tValues.at(i)))
        {
            newFormula.replace(
                tKeys.at(i), tValues.at(i).length(),
                functions.value(tValues.at(i)).translate(VAbstractApplication::VApp()->Settings()->GetLocale()));
            auto bias = tValues.at(i).length() - functions.value(tValues.at(i))
                                                     .translate(VAbstractApplication::VApp()->Settings()->GetLocale())
                                                     .length();
            if (bias != 0)
            { // Translated token has different length than original. Position next tokens need to be corrected.
                CorrectionsPositions(tKeys.at(i), bias, tokens, numbers);
                tKeys = tokens.keys();
                tValues = tokens.values();
            }
            continue;
        }

        vsizetype bias = 0;
        if (VariablesToUser(newFormula, tKeys.at(i), tValues.at(i), bias))
        {
            if (bias != 0)
            { // Translated token has different length than original. Position next tokens need to be corrected.
                CorrectionsPositions(tKeys.at(i), bias, tokens, numbers);
                tKeys = tokens.keys();
                tValues = tokens.values();
            }
            continue;
        }

        if (tValues.at(i) == '-'_L1)
        { // unary minus
            newFormula.replace(tKeys.at(i), 1, LocaleNegativeSign(QLocale()));
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::TranslateNumbersToUser(QString &newFormula, QMap<vsizetype, QString> &tokens,
                                            QMap<vsizetype, QString> &numbers, bool osSeparator)
{
    QLocale loc = QLocale(); // User locale
    if (loc != QLocale::C && osSeparator)
    {                                            // User want use Os separator
        QList<vsizetype> nKeys = numbers.keys(); // Positions for all numbers in expression
        QList<QString> nValues = numbers.values();
        for (int i = 0; i < nKeys.size(); ++i)
        {
            loc = QLocale::c(); // From pattern locale
            bool ok = false;
            const qreal d = loc.toDouble(nValues.at(i), &ok);
            if (not ok)
            {
                qDebug() << "Can't convert to double token" << nValues.at(i);
                continue; // Leave with out translation
            }

            loc = QLocale();                // To user locale
            QString dStr = loc.toString(d); // Number string in user locale
            if (VLocaleCharacter(LocaleGroupSeparator(loc)).isSpace())
            {
                dStr.replace(LocaleGroupSeparator(loc), QString());
            }
            newFormula.replace(nKeys.at(i), nValues.at(i).length(), dStr);
            const auto bias = nValues.at(i).length() - dStr.length();
            if (bias != 0)
            { // Translated number has different length than original. Position next tokens need to be corrected.
                CorrectionsPositions(nKeys.at(i), bias, tokens, numbers);
                nKeys = numbers.keys();
                nValues = numbers.values();
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief BiasTokens change position for each token that have position more then "position".
 * @param position token position
 * @param bias difference between original token length and translated
 * @param tokens all tokens
 */
void VTranslateVars::BiasTokens(vsizetype position, vsizetype bias, QMap<vsizetype, QString> &tokens)
{
    QMap<vsizetype, QString> newTokens;
    QMap<vsizetype, QString>::const_iterator i = tokens.constBegin();
    while (i != tokens.constEnd())
    {
        if (i.key() <= position)
        { // Tokens before position "position" did not change his positions.
            newTokens.insert(i.key(), i.value());
        }
        else
        {
            newTokens.insert(i.key() - bias, i.value());
        }
        ++i;
    }
    tokens = newTokens;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VariablesFromUser translate variable to internal look.
 * @param newFormula [in|out] expression to translate
 * @param position token position
 * @param token token to translate
 * @param bias hold change of length between translated and origin token string
 * @return true if was found variable with same name.
 */
auto VTranslateVars::VariablesFromUser(QString &newFormula, vsizetype position, const QString &token,
                                       vsizetype &bias) const -> bool
{
    const QString currentLengthTr =
        variables.value(currentLength).translate(VAbstractApplication::VApp()->Settings()->GetLocale());
    const QString currentSeamAllowanceTr =
        variables.value(currentSeamAllowance).translate(VAbstractApplication::VApp()->Settings()->GetLocale());

    QMap<QString, qmu::QmuTranslation>::const_iterator i = variables.constBegin();
    while (i != variables.constEnd())
    {
        const qmu::QmuTranslation &var = i.value();
        const QString varTr = var.translate(VAbstractApplication::VApp()->Settings()->GetLocale());

        if (token.indexOf(varTr) == 0)
        {
            if ((varTr == currentLengthTr || varTr == currentSeamAllowanceTr) && token != varTr)
            {
                ++i;
                continue;
            }

            newFormula.replace(position, varTr.length(), i.key());
            QString newToken = token;
            newToken.replace(0, varTr.length(), i.key());
            bias = token.length() - newToken.length();
            return true;
        }
        ++i;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief FunctionsFromUser translate function name to internal look.
 * @param newFormula [in|out] expression to translate
 * @param position token position
 * @param token token to translate
 * @param bias hold change of length between translated and origin token string
 * @return true if was found function with same name.
 */
auto VTranslateVars::FunctionsFromUser(QString &newFormula, vsizetype position, const QString &token,
                                       vsizetype &bias) const -> bool
{
    QMap<QString, qmu::QmuTranslation>::const_iterator i = functions.constBegin();
    while (i != functions.constEnd())
    {
        if (token == i.value().translate(VAbstractApplication::VApp()->Settings()->GetLocale()))
        {
            newFormula.replace(position, token.length(), i.key());
            bias = token.length() - i.key().length();
            return true;
        }
        ++i;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VariablesToUser translate variable name to user.
 * @param newFormula [in|out] expression to translate
 * @param position token position
 * @param token token to translate
 * @param bias hold change of length between translated and origin token string
 * @return true if was found variable with same name.
 */
auto VTranslateVars::VariablesToUser(QString &newFormula, vsizetype position, const QString &token,
                                     vsizetype &bias) const -> bool
{
    QMap<QString, qmu::QmuTranslation>::const_iterator i = variables.constBegin();
    while (i != variables.constEnd())
    {
        if (token.indexOf(i.key()) == 0)
        {
            if ((i.key() == currentLength || i.key() == currentSeamAllowance) && token != i.key())
            {
                ++i;
                continue;
            }

            newFormula.replace(position, i.key().length(),
                               i.value().translate(VAbstractApplication::VApp()->Settings()->GetLocale()));

            QString newToken = token;
            newToken.replace(0, i.key().length(),
                             i.value().translate(VAbstractApplication::VApp()->Settings()->GetLocale()));
            bias = token.length() - newToken.length();
            return true;
        }
        ++i;
    }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::InternalVarToUser(const QString &var) const -> QString
{
    VCommonSettings *settings = VAbstractApplication::VApp()->Settings();
    if (!settings->IsTranslateFormula())
    {
        return var;
    }

    QString newVar = var;
    vsizetype bias = 0;
    if (VariablesToUser(newVar, 0, var, bias))
    {
        return newVar;
    }

    return var;
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::VarToUser(const QString &var) const -> QString
{
    const QString locale = VAbstractApplication::VApp()->Settings()->GetLocale();
    if (locale == QStringLiteral("zh_CN") || locale == QStringLiteral("he_IL"))
    {
        return var; // We do not support translation of variables for these locales
    }

    VCommonSettings *settings = VAbstractApplication::VApp()->Settings();
    if (!settings->IsTranslateFormula())
    {
        return var;
    }

    if (measurements.contains(var))
    {
        return measurements.value(var).translate(VAbstractApplication::VApp()->Settings()->GetLocale());
    }

    if (functions.contains(var))
    {
        return functions.value(var).translate(VAbstractApplication::VApp()->Settings()->GetLocale());
    }

    return InternalVarToUser(var);
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::VarFromUser(const QString &var) const -> QString
{
    const QString locale = VAbstractApplication::VApp()->Settings()->GetLocale();
    if (locale == QStringLiteral("zh_CN") || locale == QStringLiteral("he_IL"))
    {
        return var; // We do not support translation of variables for Chinese
    }

    VCommonSettings *settings = VAbstractApplication::VApp()->Settings();
    if (!settings->IsTranslateFormula())
    {
        return var;
    }

    QString newVar = var;
    vsizetype bias = 0;
    if (MeasurementsFromUser(newVar, 0, var, bias))
    {
        return newVar;
    }

    if (VariablesFromUser(newVar, 0, var, bias))
    {
        return newVar;
    }

    if (FunctionsFromUser(newVar, 0, var, bias))
    {
        return newVar;
    }
    return newVar;
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::PMSystemName(const QString &code) const -> QString
{
    return PMSystemNames.value(code).translate(VAbstractApplication::VApp()->Settings()->GetLocale());
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::PMSystemAuthor(const QString &code) const -> QString
{
    return PMSystemAuthors.value(code).translate(VAbstractApplication::VApp()->Settings()->GetLocale());
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::PMSystemBook(const QString &code) const -> QString
{
    return PMSystemBooks.value(code).translate(VAbstractApplication::VApp()->Settings()->GetLocale());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief FormulaFromUser replace all known tokens in formula to internal look. Also change decimal
 * separator in numbers.
 * @param formula expression that need translate
 * @throw qmu::QmuParserError in case of a wrong expression
 * @return translated expression
 */
auto VTranslateVars::FormulaFromUser(const QString &formula, bool osSeparator) const -> QString
{
    if (formula.isEmpty())
    {
        return formula;
    }

    VCommonSettings *settings = VAbstractApplication::VApp()->Settings();
    if (!settings->IsTranslateFormula())
    {
        return formula;
    }

    // Eval formula
    QScopedPointer<qmu::QmuTokenParser> cal(
        new qmu::QmuTokenParser(formula, osSeparator, true, GetTranslatedFunctions()));
    QMap<vsizetype, QString> tokens = cal->GetTokens();   // Tokens (variables, measurements)
    QMap<vsizetype, QString> numbers = cal->GetNumbers(); // All numbers in expression for changing decimal separator
    cal.reset();

    QString newFormula = formula; // Local copy for making changes

    TranslateVarsFromUser(newFormula, tokens, numbers);
    TranslateNumbersFromUser(newFormula, tokens, numbers, osSeparator);

    return newFormula;
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::TryFormulaFromUser(const QString &formula, bool osSeparator) -> QString
{
    try
    {
        SCASSERT(VAbstractApplication::VApp()->TrVars() != nullptr)
        return VAbstractApplication::VApp()->TrVars()->FormulaFromUser(formula, osSeparator);
    }
    catch (qmu::QmuParserError &e) // In case something bad will happen
    {
        Q_UNUSED(e)
        return formula;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief FormulaToUser replace all known tokens in formula to user look. Also change decimal
 * separator in numbers.
 * @param formula expression that need translate
 * @return translated expression
 */
auto VTranslateVars::FormulaToUser(const QString &formula, bool osSeparator) const -> QString
{
    if (formula.isEmpty())
    {
        return formula;
    }

    VCommonSettings *settings = VAbstractApplication::VApp()->Settings();
    if (!settings->IsTranslateFormula())
    {
        return formula;
    }

    QString newFormula = formula; // Local copy for making changes

    QMap<vsizetype, QString> tokens;
    QMap<vsizetype, QString> numbers;
    try
    {
        QScopedPointer<qmu::QmuTokenParser> cal(new qmu::QmuTokenParser(formula, false, false)); // Eval formula
        tokens = cal->GetTokens();   // Tokens (variables, measurements)
        numbers = cal->GetNumbers(); // All numbers in expression for changing decimal separator
    }
    catch (qmu::QmuParserError &e)
    {
        qDebug() << "\nMath parser error:\n"
                 << "--------------------------------------\n"
                 << "Message:     " << e.GetMsg() << "\n"
                 << "Expression:  " << e.GetExpr() << "\n"
                 << "--------------------------------------";
        return newFormula;
    }

    TranslateVarsToUser(newFormula, tokens, numbers);
    TranslateNumbersToUser(newFormula, tokens, numbers, osSeparator);

    return newFormula;
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::TryFormulaToUser(const QString &formula, bool osSeparator) -> QString
{
    try
    {
        return VAbstractApplication::VApp()->TrVars()->FormulaToUser(formula, osSeparator);
    }
    catch (qmu::QmuParserError &e) // In case something bad will happen
    {
        Q_UNUSED(e)
        return formula;
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VTranslateVars::Retranslate()
{
    VTranslateMeasurements::Retranslate();

    PMSystemNames.clear();
    PMSystemAuthors.clear();
    PMSystemBooks.clear();
    variables.clear();
    functions.clear();
    functionsDescriptions.clear();
    stDescriptions.clear();

    InitPatternMakingSystems();
    InitVariables();
    InitFunctions();

    PrepareFunctionTranslations();
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::GetTranslatedFunctions() const -> QMap<QString, QString>
{
    return translatedFunctions;
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::GetFunctions() const -> QMap<QString, qmu::QmuTranslation>
{
    return functions;
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::GetFunctionsDescriptions() const -> QMap<QString, qmu::QmuTranslation>
{
    return functionsDescriptions;
}

//---------------------------------------------------------------------------------------------------------------------
auto VTranslateVars::GetFunctionsArguments() const -> QMap<QString, QString>
{
    return functionsArguments;
}
