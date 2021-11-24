/************************************************************************
 **
 **  @file   dialogtoolbox.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   25 1, 2019
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2019 Valentina project
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
#ifndef DIALOGTOOLBOX_H
#define DIALOGTOOLBOX_H

#include <QString>
#include <QtGlobal>
#include <QSharedPointer>

#include "../vpatterndb/variables/vinternalvariable.h"
#include "../vmisc/typedef.h"
#include "../vgeometry/vgeometrydef.h"

class QPlainTextEdit;
class QPushButton;
class QDialog;
class QObject;
class QEvent;
class QLabel;
class QWidget;
class QColor;
class QLineEdit;
class VContainer;
class QListWidget;
class VPieceNode;

constexpr int formulaTimerTimeout = 300;

extern const QColor errorColor;

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Weffc++")

struct FormulaData
{
    QString formula;
    const QHash<QString, QSharedPointer<VInternalVariable> > *variables{nullptr};
    QLabel *labelEditFormula{nullptr};
    QLabel *labelResult{nullptr};
    QString postfix;
    bool checkZero{true};
    bool checkLessThanZero{false};
};

QT_WARNING_POP

VPieceNode RowNode(QListWidget *listWidget, int i);
void   MoveCursorToEnd(QPlainTextEdit *plainTextEdit);
void   DeployFormula(QDialog *dialog, QPlainTextEdit *formula, QPushButton *buttonGrowLength, int formulaBaseHeight);
bool   FilterObject(QObject *object, QEvent *event);
qreal  EvalToolFormula(QDialog *dialog, const FormulaData &data, bool &flag);
void   ChangeColor(QWidget *widget, const QColor &color);
QColor OkColor(QWidget *widget);
void   CheckPointLabel(QDialog *dialog, QLineEdit* edit, QLabel *labelEditNamePoint, const QString &pointName,
                       const VContainer *data, bool &flag);
int    FindNotExcludedNodeDown(QListWidget *listWidget, int candidate);
int    FindNotExcludedNodeUp(QListWidget *listWidget, int candidate);
int    FindNotExcludedPointDown(QListWidget *listWidget, int start);
int    FindNotExcludedCurveDown(QListWidget *listWidget, int start);
bool   FirstPointEqualLast(QListWidget *listWidget, const VContainer *data, QString &error);
bool   DoublePoints(QListWidget *listWidget, const VContainer *data, QString &error);
bool   DoubleCurves(QListWidget *listWidget, const VContainer *data, QString &error);
bool   EachPointLabelIsUnique(QListWidget *listWidget);
bool   InvalidSegment(QListWidget *listWidget, const VContainer *data, QString &error);
QString DialogWarningIcon();
QFont  NodeFont(QFont font, bool nodeExcluded = false);
void   CurrentCurveLength(vidtype curveId, VContainer *data);
void   SetTabStopDistance(QPlainTextEdit *edit, int tabWidthChar=4);
QIcon  LineColor(int size, const QString &color);
auto SegmentAliases(GOType curveType, const QString &alias1, const QString &alias2) -> QPair<QString, QString>;
QString GetNodeName(const VContainer *data, const VPieceNode &node, bool showPassmarkDetails = false);

#endif // DIALOGTOOLBOX_H
