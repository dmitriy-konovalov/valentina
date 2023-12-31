/************************************************************************
 **
 **  @file   vtranslatemeasurements.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   2 8, 2015
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

#ifndef VTRANSLATEMEASUREMENTS_H
#define VTRANSLATEMEASUREMENTS_H

#include <QMap>
#include <QString>
#include <QtGlobal>

#include "../qmuparser/qmutranslation.h"
#include "../vmisc/defglobal.h"

class VTranslateMeasurements
{
public:
    VTranslateMeasurements();
    virtual ~VTranslateMeasurements();

    auto MeasurementsFromUser(QString &newFormula, vsizetype position, const QString &token, vsizetype &bias) const
        -> bool;

    auto MFromUser(const QString &measurement) const -> QString;
    auto MToUser(const QString &measurement) const -> QString;
    auto MNumber(const QString &measurement) const -> QString;
    auto MFormula(const QString &measurement) const -> QString;
    auto GuiText(const QString &measurement) const -> QString;
    auto Description(const QString &measurement) const -> QString;

    virtual void Retranslate();

protected:
    QMap<QString, qmu::QmuTranslation> measurements;

private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(VTranslateMeasurements) // NOLINT
    QMap<QString, qmu::QmuTranslation> guiTexts;
    QMap<QString, qmu::QmuTranslation> descriptions;
    QMap<QString, QString> numbers;
    QMap<QString, QString> formulas;

    void InitGroupA(); // Direct Height
    void InitGroupB(); // Direct Width
    void InitGroupC(); // Indentation
    void InitGroupD(); // Circumference and Arc
    void InitGroupE(); // Vertical
    void InitGroupF(); // Horizontal
    void InitGroupG(); // Bust
    void InitGroupH(); // Balance
    void InitGroupI(); // Arm
    void InitGroupJ(); // Leg
    void InitGroupK(); // Crotch and Rise
    void InitGroupL(); // Hand
    void InitGroupM(); // Foot
    void InitGroupN(); // Head
    void InitGroupO(); // Men & Tailoring
    void InitGroupP(); // Historical & Specialty
    void InitGroupQ(); // Patternmaking measurements

    void InitMeasurements();

    void InitMeasurement(const QString &name, const qmu::QmuTranslation &m, const qmu::QmuTranslation &g,
                         const qmu::QmuTranslation &d, const QString &number, const QString &formula = QString());
};

#endif // VTRANSLATEMEASUREMENTS_H
