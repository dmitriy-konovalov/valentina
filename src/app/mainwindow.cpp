/************************************************************************
 **
 **  @file   mainwindow.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013 Valentina project
 **  <https://bitbucket.org/dismine/valentina> All Rights Reserved.
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "geometry/vspline.h"
#include "geometry/vequidistant.h"
#include "exception/vexceptionobjecterror.h"
#include "exception/vexceptionconversionerror.h"
#include "exception/vexceptionemptyparameter.h"
#include "exception/vexceptionwrongid.h"
#include "exception/vexceptionundo.h"
#include "version.h"
#include "xml/vstandardmeasurements.h"
#include "xml/vindividualmeasurements.h"
#include "widgets/vapplication.h"
#include "widgets/undoevent.h"
#include "undocommands/renamepp.h"

#include <QInputDialog>
#include <QDebug>
#include <QMessageBox>
#include <QShowEvent>
#include <QScrollBar>
#include <QFileDialog>
#include <QSourceLocation>
#include <QUndoStack>
#include <QAction>
#include <QProcess>
#include <QSettings>

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief MainWindow constructor.
 * @param parent parent widget.
 */
MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent), ui(new Ui::MainWindow), pattern(nullptr), doc(nullptr), tool(Tool::ArrowTool),
      currentScene(nullptr), sceneDraw(nullptr), sceneDetails(nullptr), mouseCoordinate(nullptr), helpLabel(nullptr),
      view(nullptr), isInitialized(false), dialogTable(0), dialogTool(nullptr), dialogHistory(nullptr),
      comboBoxDraws(nullptr), curFile(QString()), mode(Draw::Calculation), currentDrawIndex(0),
      currentToolBoxIndex(0), drawMode(true), recentFileActs{nullptr, nullptr, nullptr, nullptr, nullptr},
      separatorAct(nullptr), autoSaveTimer(nullptr), guiEnabled(true)
{
    CreateActions();
    CreateMenus();
    ToolBarDraws();
    InitToolButtons();

    sceneDraw = new VMainGraphicsScene();
    currentScene = sceneDraw;
    qApp->setCurrentScene(currentScene);
    connect(sceneDraw, &VMainGraphicsScene::mouseMove, this, &MainWindow::mouseMove);
    sceneDetails = new VMainGraphicsScene();
    connect(sceneDetails, &VMainGraphicsScene::mouseMove, this, &MainWindow::mouseMove);

    view = new VMainGraphicsView();
    view->setDisabled(true);
    ui->LayoutView->addWidget(view);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    view->setScene(currentScene);

    sceneDraw->setTransform(view->transform());
    sceneDetails->setTransform(view->transform());

    connect(view, &VMainGraphicsView::NewFactor, sceneDraw, &VMainGraphicsScene::SetFactor);
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHorizontalStretch(12);
    view->setSizePolicy(policy);
    qApp->setSceneView(view);
    helpLabel = new QLabel(QObject::tr("Create new pattern piece to start working."));
    ui->statusBar->addWidget(helpLabel);

    ToolBarTools();

    pattern = new VContainer();

    doc = new VPattern(pattern, &mode, sceneDraw, sceneDetails);
    connect(doc, &VPattern::ClearMainWindow, this, &MainWindow::Clear);
    connect(doc, &VPattern::patternChanged, this, &MainWindow::PatternWasModified);
    connect(doc, &VPattern::UndoCommand, this, &MainWindow::FullParseFile);
    connect(doc, &VPattern::SetEnabledGUI, this, &MainWindow::SetEnabledGUI);
    connect(doc, &VPattern::CheckLayout, this, &MainWindow::Layout);

    connect(qApp->getUndoStack(), &QUndoStack::cleanChanged, this, &MainWindow::PatternWasModified);

    InitAutoSave();

    ui->toolBox->setCurrentIndex(0);

    ReadSettings();

    setCurrentFile("");
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ActionNewPP add to scene new pattern piece.
 */
void MainWindow::ActionNewPP()
{
    QString patternPieceName = QString(tr("Pattern piece %1")).arg(comboBoxDraws->count()+1);
    QString path;
    if (comboBoxDraws->count() == 0)
    {
        DialogMeasurements measurements(this);
        if (measurements.exec() == QDialog::Rejected)
        {
            return;
        }
        if (measurements.type() == MeasurementsType::Standard)
        {
            qApp->setPatternType(MeasurementsType::Standard);
            DialogStandardMeasurements stMeasurements(pattern, patternPieceName, this);
            if (stMeasurements.exec() == QDialog::Accepted)
            {
                patternPieceName = stMeasurements.name();
                path = stMeasurements.tablePath();
                VStandardMeasurements m(pattern);
                m.setContent(path);
                m.SetSize();
                m.SetHeight();
                m.Measurements();
            }
            else
            {
                return;
            }
        }
        else
        {
            qApp->setPatternType(MeasurementsType::Individual);
            DialogIndividualMeasurements indMeasurements(pattern, patternPieceName, this);
            if (indMeasurements.exec() == QDialog::Accepted)
            {
                patternPieceName = indMeasurements.name();
                path = indMeasurements.tablePath();
                VIndividualMeasurements m(pattern);
                m.setContent(path);
                m.Measurements();
            }
            else
            {
                return;
            }
        }

        //Set scene size to size scene view
        VAbstractTool::NewSceneRect(sceneDraw, view);
        VAbstractTool::NewSceneRect(sceneDetails, view);
        ToolBarOption();
    }
    else
    {
        patternPieceName = PatternPieceName(patternPieceName);
        if (patternPieceName.isEmpty())
        {
            return;
        }
        path = doc->MPath();
    }
    if (doc->appendPP(patternPieceName) == false)
    {
        qDebug()<<"Error creating pattern piece with the name "<<patternPieceName<<".";
        return;
    }
    disconnect(comboBoxDraws,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
               this, &MainWindow::currentPPChanged);
    comboBoxDraws->addItem(patternPieceName);

    pattern->ClearGObjects();
    //Create single point
    const quint32 id = pattern->AddGObject(new VPointF(qApp->toPixel((10+comboBoxDraws->count()*5)), qApp->toPixel(10),
                                                       "А", 5, 10));
    VToolSinglePoint *spoint = new VToolSinglePoint(doc, pattern, id, Source::FromGui, patternPieceName, path);
    sceneDraw->addItem(spoint);
    connect(spoint, &VToolPoint::ChoosedTool, sceneDraw, &VMainGraphicsScene::ChoosedItem);
    connect(sceneDraw, &VMainGraphicsScene::NewFactor, spoint, &VToolSinglePoint::SetFactor);
    QHash<quint32, VDataTool*>* tools = doc->getTools();
    SCASSERT(tools != nullptr);
    tools->insert(id, spoint);
    VDrawTool::AddRecord(id, Tool::SinglePointTool, doc);
    SetEnableTool(true);
    SetEnableWidgets(true);

    const qint32 index = comboBoxDraws->findText(patternPieceName);
    if ( index != -1 )
    { // -1 for not found
        comboBoxDraws->setCurrentIndex(index);
    }
    else
    {
        comboBoxDraws->setCurrentIndex(0);
    }
    connect(comboBoxDraws,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::currentPPChanged);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief OptionDraw help change name of pattern piece.
 */
void MainWindow::OptionDraw()
{
    const QString activDraw = doc->GetNameActivPP();
    const QString nameDraw = PatternPieceName(activDraw);
    if (nameDraw.isEmpty())
    {
        return;
    }
    RenamePP *renamePP = new RenamePP(doc, nameDraw, comboBoxDraws);
    qApp->getUndoStack()->push(renamePP);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetToolButton set tool and show dialog.
 * @param checked true if tool button checked.
 * @param t tool type.
 * @param cursor path tool cursor icon.
 * @param toolTip first tooltipe.
 * @param closeDialogSlot function what handle after close dialog.
 */
template <typename Dialog, typename Func>
void MainWindow::SetToolButton(bool checked, Tool t, const QString &cursor, const QString &toolTip,
                               Func closeDialogSlot)
{
    if (checked)
    {
        CancelTool();
        tool = t;
        QPixmap pixmap(cursor);
        QCursor cur(pixmap, 2, 3);
        view->setCursor(cur);
        helpLabel->setText(toolTip);
        dialogTool = new Dialog(pattern, 0, this);
        connect(currentScene, &VMainGraphicsScene::ChoosedObject, dialogTool, &DialogTool::ChosenObject);
        connect(dialogTool, &DialogTool::DialogClosed, this, closeDialogSlot);
        connect(dialogTool, &DialogTool::ToolTip, this, &MainWindow::ShowToolTip);
        connect(doc, &VPattern::FullUpdateFromFile, dialogTool, &DialogTool::UpdateList);
    }
    else
    {
        if (QToolButton *tButton = qobject_cast< QToolButton * >(this->sender()))
        {
            SCASSERT(tButton != nullptr);
            tButton->setChecked(true);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
template <typename Dialog, typename Func, typename Func2>
/**
 * @brief SetToolButtonWithApply set tool and show dialog.
 * @param checked true if tool button checked.
 * @param t tool type.
 * @param cursor path tool cursor icon.
 * @param toolTip first tooltipe.
 * @param closeDialogSlot function to handle close of dialog.
 * @param applyDialogSlot function to handle apply in dialog.
 */
void MainWindow::SetToolButtonWithApply(bool checked, Tool t, const QString &cursor, const QString &toolTip,
                               Func closeDialogSlot, Func2 applyDialogSlot)
{
    if (checked)
    {
        CancelTool();
        tool = t;
        QPixmap pixmap(cursor);
        QCursor cur(pixmap, 2, 3);
        view->setCursor(cur);
        helpLabel->setText(toolTip);
        dialogTool = new Dialog(pattern, 0, this);
        connect(currentScene, &VMainGraphicsScene::ChoosedObject, dialogTool, &DialogTool::ChosenObject);
        connect(dialogTool, &DialogTool::DialogClosed, this, closeDialogSlot);
        connect(dialogTool, &DialogTool::DialogApplied, this, applyDialogSlot);
        connect(dialogTool, &DialogTool::ToolTip, this, &MainWindow::ShowToolTip);
        connect(doc, &VPattern::FullUpdateFromFile, dialogTool, &DialogTool::UpdateList);
        connect(view, &VMainGraphicsView::MouseRelease, this, &MainWindow::ClickEndVisualization);
    }
    else
    {
        if (QToolButton *tButton = qobject_cast< QToolButton * >(this->sender()))
        {
            SCASSERT(tButton != nullptr);
            tButton->setChecked(true);
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ClosedDialog handle close dialog
 * @param result result working dialog.
 */
template <typename DrawTool>
void MainWindow::ClosedDialog(int result)
{
    SCASSERT(dialogTool != nullptr);
    if (result == QDialog::Accepted)
    {
        DrawTool::Create(dialogTool, currentScene, doc, pattern);
    }
    ArrowTool();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ClosedDialogWithApply handle close dialog that has apply button
 * @param result result working dialog.
 */
template <typename DrawTool>
void MainWindow::ClosedDialogWithApply(int result)
{
    SCASSERT(dialogTool != nullptr);
    if (result == QDialog::Accepted)
    {
        // Only create tool if not already created with apply
        if (dialogTool->GetAssociatedTool() == nullptr)
        {
            dialogTool->SetAssociatedTool(
                    dynamic_cast<VAbstractTool * > (DrawTool::Create(dialogTool, currentScene, doc, pattern)));
        }
        else
        { // Or update associated tool with data
            VDrawTool * vtool= static_cast<VDrawTool *>(dialogTool->GetAssociatedTool());
            vtool->FullUpdateFromGuiApply();
        }
    }
    if (dialogTool->GetAssociatedTool() != nullptr)
    {
        VDrawTool * vtool= static_cast<VDrawTool *>(dialogTool->GetAssociatedTool());
        vtool->DialogLinkDestroy();
    }
    ArrowTool();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ApplyDialog handle apply in dialog
 */
template <typename DrawTool>
void MainWindow::ApplyDialog()
{
    SCASSERT(dialogTool != nullptr);

    // Only create tool if not already created with apply
    if (dialogTool->GetAssociatedTool() == nullptr)
    {
        dialogTool->SetAssociatedTool(
                static_cast<VAbstractTool * > (DrawTool::Create(dialogTool, currentScene, doc, pattern)));
    }
    else
    { // Or update associated tool with data
        VDrawTool * vtool= static_cast<VDrawTool *>(dialogTool->GetAssociatedTool());
        SCASSERT(vtool != nullptr);
        vtool->FullUpdateFromGuiApply();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolEndLine handler tool endLine.
 * @param checked true - button checked.
 */
void MainWindow::ToolEndLine(bool checked)
{
    SetToolButtonWithApply<DialogEndLine>(checked, Tool::EndLineTool, ":/cursor/endline_cursor.png", tr("Select point"),
                                          &MainWindow::ClosedDialogWithApply<VToolEndLine>,
                                          &MainWindow::ApplyDialog<VToolEndLine>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolLine handler tool line.
 * @param checked true - button checked.
 */
void MainWindow::ToolLine(bool checked)
{
    SetToolButtonWithApply<DialogLine>(checked, Tool::LineTool, ":/cursor/line_cursor.png", tr("Select first point"),
                                       &MainWindow::ClosedDialogWithApply<VToolLine>,
                                       &MainWindow::ApplyDialog<VToolLine>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolAlongLine handler tool alongLine.
 * @param checked true - button checked.
 */
void MainWindow::ToolAlongLine(bool checked)
{
    SetToolButtonWithApply<DialogAlongLine>(checked, Tool::AlongLineTool, ":/cursor/alongline_cursor.png",
                                            tr("Select point"), &MainWindow::ClosedDialogWithApply<VToolAlongLine>,
                                            &MainWindow::ApplyDialog<VToolAlongLine>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolShoulderPoint handler tool shoulderPoint.
 * @param checked true - button checked.
 */
void MainWindow::ToolShoulderPoint(bool checked)
{
    SetToolButtonWithApply<DialogShoulderPoint>(checked, Tool::ShoulderPointTool, ":/cursor/shoulder_cursor.png",
                                                tr("Select point of shoulder"),
                                                &MainWindow::ClosedDialogWithApply<VToolShoulderPoint>,
                                                &MainWindow::ApplyDialog<VToolShoulderPoint>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolNormal handler tool normal.
 * @param checked true - button checked.
 */
void MainWindow::ToolNormal(bool checked)
{
    SetToolButtonWithApply<DialogNormal>(checked, Tool::NormalTool, ":/cursor/normal_cursor.png",
                                         tr("Select first point of line"),
                                         &MainWindow::ClosedDialogWithApply<VToolNormal>,
                                         &MainWindow::ApplyDialog<VToolNormal>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolBisector handler tool bisector.
 * @param checked true - button checked.
 */
void MainWindow::ToolBisector(bool checked)
{
    SetToolButtonWithApply<DialogBisector>(checked, Tool::BisectorTool, ":/cursor/bisector_cursor.png",
                                           tr("Select first point of angle"),
                                           &MainWindow::ClosedDialogWithApply<VToolBisector>,
                                           &MainWindow::ApplyDialog<VToolBisector>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolLineIntersect handler tool lineIntersect.
 * @param checked true - button checked.
 */
void MainWindow::ToolLineIntersect(bool checked)
{
    SetToolButton<DialogLineIntersect>(checked, Tool::LineIntersectTool, ":/cursor/intersect_cursor.png",
                                       tr("Select first point of first line"),
                                       &MainWindow::ClosedDialog<VToolLineIntersect>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolSpline handler tool spline.
 * @param checked true - button checked.
 */
void MainWindow::ToolSpline(bool checked)
{
    SetToolButton<DialogSpline>(checked, Tool::SplineTool, ":/cursor/spline_cursor.png",
                                tr("Select first point curve"), &MainWindow::ClosedDialog<VToolSpline>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolCutSpline handler tool CutSpline.
 * @param checked true - button is checked
 */
void MainWindow::ToolCutSpline(bool checked)
{
    SetToolButton<DialogCutSpline>(checked, Tool::CutSplineTool, ":/cursor/spline_cut_point_cursor.png",
                                   tr("Select simple curve"), &MainWindow::ClosedDialog<VToolCutSpline>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolArc handler tool arc.
 * @param checked true - button checked.
 */
void MainWindow::ToolArc(bool checked)
{
    SetToolButtonWithApply<DialogArc>(checked, Tool::ArcTool, ":/cursor/arc_cursor.png",
                                      tr("Select point of center of arc"), &MainWindow::ClosedDialogWithApply<VToolArc>,
                                      &MainWindow::ApplyDialog<VToolArc>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolSplinePath handler tool splinePath.
 * @param checked true - button checked.
 */
void MainWindow::ToolSplinePath(bool checked)
{
    SetToolButton<DialogSplinePath>(checked, Tool::SplinePathTool, ":/cursor/splinepath_cursor.png",
                                    tr("Select point of curve path"), &MainWindow::ClosedDialog<VToolSplinePath>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolCutSplinePath handler tool CutSplinePath.
 * @param checked true - button is checked
 */
void MainWindow::ToolCutSplinePath(bool checked)
{
    SetToolButton<DialogCutSplinePath>(checked, Tool::CutSplinePathTool,
                                       ":/cursor/splinepath_cut_point_cursor.png", tr("Select curve path"),
                                       &MainWindow::ClosedDialog<VToolCutSplinePath>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolPointOfContact handler tool pointOfContact.
 * @param checked true - button checked.
 */
void MainWindow::ToolPointOfContact(bool checked)
{
    SetToolButtonWithApply<DialogPointOfContact>(checked, Tool::PointOfContact, ":/cursor/pointcontact_cursor.png",
                                                 tr("Select first point of line"),
                                                 &MainWindow::ClosedDialogWithApply<VToolPointOfContact>,
                                                 &MainWindow::ApplyDialog<VToolPointOfContact>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolDetail handler tool detail.
 * @param checked true - button checked.
 */
void MainWindow::ToolDetail(bool checked)
{
    SetToolButton<DialogDetail>(checked, Tool::DetailTool, "://cursor/new_detail_cursor.png",
                                tr("Select points, arcs, curves clockwise."), &MainWindow::ClosedDialogDetail);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ClosedDialogDetail actions after closing DialogDetail.
 * @param result result of dialog working.
 */
void MainWindow::ClosedDialogDetail(int result)
{
    if (result == QDialog::Accepted)
    {
        VToolDetail::Create(dialogTool, sceneDetails, doc, pattern);
    }
    ArrowTool();
    doc->LiteParseTree(Document::LiteParse);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolHeight handler tool height.
 * @param checked true - button checked.
 */
void MainWindow::ToolHeight(bool checked)
{
    SetToolButton<DialogHeight>(checked, Tool::Height, ":/cursor/height_cursor.png", tr("Select base point"),
                                &MainWindow::ClosedDialog<VToolHeight>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolTriangle handler tool triangle.
 * @param checked true - button checked.
 */
void MainWindow::ToolTriangle(bool checked)
{
    SetToolButton<DialogTriangle>(checked, Tool::Triangle, ":/cursor/triangle_cursor.png",
                                  tr("Select first point of axis"), &MainWindow::ClosedDialog<VToolTriangle>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolPointOfIntersection handler tool pointOfIntersection.
 * @param checked true - button checked.
 */
void MainWindow::ToolPointOfIntersection(bool checked)
{
    SetToolButton<DialogPointOfIntersection>(checked, Tool::PointOfIntersection,
                                             ":/cursor/pointofintersect_cursor.png", tr("Select point vertically"),
                                             &MainWindow::ClosedDialog<VToolPointOfIntersection>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolUnionDetails handler tool unionDetails.
 * @param checked true - button checked.
 */
void MainWindow::ToolUnionDetails(bool checked)
{
    SetToolButton<DialogUnionDetails>(checked, Tool::UnionDetails, ":/cursor/union_cursor.png",
                                      tr("Select detail"), &MainWindow::ClosedDialogUnionDetails);
    //Must disconnect this signal here.
    disconnect(doc, &VPattern::FullUpdateFromFile, dialogTool, &DialogTool::UpdateList);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ClosedDialogUnionDetails actions after closing DialogUnionDetails.
 * @param result result of dialog working.
 */
void MainWindow::ClosedDialogUnionDetails(int result)
{
    ClosedDialog<VToolUnionDetails>(result);
    doc->LiteParseTree(Document::LiteParse);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolCutArc handler tool cutArc.
 * @param checked true - button checked.
 */
void MainWindow::ToolCutArc(bool checked)
{
    SetToolButtonWithApply<DialogCutArc>(checked, Tool::CutArcTool, ":/cursor/arc_cut_cursor.png",
                                         tr("Select arc"), &MainWindow::ClosedDialogWithApply<VToolCutArc>,
                                         &MainWindow::ApplyDialog<VToolCutArc>);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief About show widows about.
 */
void MainWindow::About()
{
    DialogAboutApp * about_dialog = new DialogAboutApp(this);
    about_dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    about_dialog->show();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief AboutQt show widows aboutQt.
 */
void MainWindow::AboutQt()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ShowTool  highlight tool.Tip show tools tooltip.
 * @param toolTip tooltip text.
 */
void MainWindow::ShowToolTip(const QString &toolTip)
{
    helpLabel->setText(toolTip);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief tableClosed handle after close layout window.
 */
void MainWindow::tableClosed()
{
    show();
    MinimumScrollBar();
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::OpenRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
    {
        OpenPattern(action->data().toString());
    }
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::PatternProperties()
{
    DialogPatternProperties proper(doc, this);
    proper.exec();
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::EditPatternCode()
{
    DialogPatternXmlEdit *Pattern = new DialogPatternXmlEdit (this, doc);
    Pattern->setAttribute(Qt::WA_DeleteOnClose, true);
    Pattern->show();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief showEvent handle after show window.
 * @param event show event.
 */
void MainWindow::showEvent( QShowEvent *event )
{
    QMainWindow::showEvent( event );
    if ( event->spontaneous() )
    {
        return;
    }

    if (isInitialized)
    {
        return;
    }
    // do your init stuff here

    MinimumScrollBar();

    isInitialized = true;//first show windows are held
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief closeEvent handle after close window.
 * @param event close event.
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (MaybeSave())
    {
        WriteSettings();
        event->accept();
        qApp->closeAllWindows();
    }
    else
    {
        event->ignore();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::customEvent(QEvent *event)
{
    if(event->type() == UNDO_EVENT)
    {
        qApp->getUndoStack()->undo();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolBarOption enable option toolbar.
 */
void MainWindow::ToolBarOption()
{
    if (qApp->patternType() == MeasurementsType::Standard)
    {
        const QStringList listHeights = VMeasurement::ListHeights(doc->GetGradationHeights());
        const QStringList listSizes = VMeasurement::ListSizes(doc->GetGradationSizes());

        SetGradationList(tr("Height: "), listHeights, &MainWindow::ChangedHeight);
        SetGradationList(tr("Size: "), listSizes, &MainWindow::ChangedSize);

        ui->toolBarOption->addSeparator();
    }

    mouseCoordinate = new QLabel(QString("0, 0 (%1)").arg(doc->UnitsToStr(qApp->patternUnit())));
    ui->toolBarOption->addWidget(mouseCoordinate);
}

//---------------------------------------------------------------------------------------------------------------------
template <typename Func>
void MainWindow::SetGradationList(const QString &label, const QStringList &list, Func changeSlot)
{
    ui->toolBarOption->addWidget(new QLabel(label));

    QComboBox *comboBox = new QComboBox;
    comboBox->addItems(list);
    comboBox->setCurrentIndex(14);//176 cm
    ui->toolBarOption->addWidget(comboBox);
    connect(comboBox, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
            this, changeSlot);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ToolBarDraws enable draw toolbar.
 */
void MainWindow::ToolBarDraws()
{
    QLabel *labelPtternPieceName = new QLabel(tr("Pattern Piece: "));
    ui->toolBarDraws->addWidget(labelPtternPieceName);

    comboBoxDraws = new QComboBox;
    ui->toolBarDraws->addWidget(comboBoxDraws);
    comboBoxDraws->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(comboBoxDraws,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::currentPPChanged);

    ui->toolBarDraws->addAction(ui->actionOptionDraw);
    ui->actionOptionDraw->setEnabled(false);

    ui->toolBarDraws->addAction(ui->actionTable);
    ui->actionTable->setEnabled(false);

    ui->toolBarDraws->addAction(ui->actionHistory);
    ui->actionHistory->setEnabled(false);
    connect(ui->actionHistory, &QAction::triggered, this, &MainWindow::ActionHistory);

    ui->toolBarDraws->addAction(ui->actionLayout);
    connect(ui->actionLayout, &QAction::triggered, this, &MainWindow::ActionLayout);
    ui->actionLayout->setEnabled(false);
}

void MainWindow::ToolBarTools()
{
    /*First we will try use Standard Shortcuts from Qt, but because keypad "-" and "+" not the same keys like in main
    keypad, shortcut Ctrl+"-" or "+" from keypad will not working with standard shortcut (QKeySequence::ZoomIn or
    QKeySequence::ZoomOut). For examle "+" is Qt::Key_Plus + Qt::KeypadModifier for keypad.
    Also for me don't work Qt:CTRL and work Qt::ControlModifier.*/

    const QList<QKeySequence> zoomInShortcuts = {QKeySequence::ZoomIn,
                                                 Qt::ControlModifier + Qt::Key_Plus + Qt::KeypadModifier};
    ui->actionZoomIn->setShortcuts(zoomInShortcuts);
    connect(ui->actionZoomIn, &QAction::triggered, view, &VMainGraphicsView::ZoomIn);

    const QList<QKeySequence> zoomOutShortcuts = {QKeySequence::ZoomOut,
                                                 Qt::ControlModifier + Qt::Key_Minus + Qt::KeypadModifier};
    ui->actionZoomOut->setShortcuts(zoomOutShortcuts);
    connect(ui->actionZoomOut, &QAction::triggered, view, &VMainGraphicsView::ZoomOut);

    connect(ui->actionZoomOriginal, &QAction::triggered, view, &VMainGraphicsView::ZoomOriginal);
    connect(ui->actionZoomFitBest, &QAction::triggered, view, &VMainGraphicsView::ZoomFitBest);
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::InitToolButtons()
{
    connect(ui->toolButtonEndLine, &QToolButton::clicked, this, &MainWindow::ToolEndLine);
    connect(ui->toolButtonLine, &QToolButton::clicked, this, &MainWindow::ToolLine);
    connect(ui->toolButtonAlongLine, &QToolButton::clicked, this, &MainWindow::ToolAlongLine);
    connect(ui->toolButtonShoulderPoint, &QToolButton::clicked, this, &MainWindow::ToolShoulderPoint);
    connect(ui->toolButtonNormal, &QToolButton::clicked, this, &MainWindow::ToolNormal);
    connect(ui->toolButtonBisector, &QToolButton::clicked, this, &MainWindow::ToolBisector);
    connect(ui->toolButtonLineIntersect, &QToolButton::clicked, this, &MainWindow::ToolLineIntersect);
    connect(ui->toolButtonSpline, &QToolButton::clicked, this, &MainWindow::ToolSpline);
    connect(ui->toolButtonArc, &QToolButton::clicked, this, &MainWindow::ToolArc);
    connect(ui->toolButtonSplinePath, &QToolButton::clicked, this, &MainWindow::ToolSplinePath);
    connect(ui->toolButtonPointOfContact, &QToolButton::clicked, this, &MainWindow::ToolPointOfContact);
    connect(ui->toolButtonNewDetail, &QToolButton::clicked, this, &MainWindow::ToolDetail);
    connect(ui->toolButtonHeight, &QToolButton::clicked, this, &MainWindow::ToolHeight);
    connect(ui->toolButtonTriangle, &QToolButton::clicked, this, &MainWindow::ToolTriangle);
    connect(ui->toolButtonPointOfIntersection, &QToolButton::clicked, this, &MainWindow::ToolPointOfIntersection);
    connect(ui->toolButtonSplineCutPoint, &QToolButton::clicked, this, &MainWindow::ToolCutSpline);
    connect(ui->toolButtonSplinePathCutPoint, &QToolButton::clicked, this, &MainWindow::ToolCutSplinePath);
    connect(ui->toolButtonUnionDetails, &QToolButton::clicked, this, &MainWindow::ToolUnionDetails);
    connect(ui->toolButtonArcCutPoint, &QToolButton::clicked, this, &MainWindow::ToolCutArc);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief currentPPChanged change active pattern piece.
 * @param index index in combobox.
 */
void MainWindow::currentPPChanged(int index)
{
    ChangePP(index);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief mouseMove save mouse position and show user.
 * @param scenePos position mouse.
 */
void MainWindow::mouseMove(const QPointF &scenePos)
{
    QString string = QString("%1, %2 (%3)").arg(static_cast<qint32>(qApp->fromPixel(scenePos.x())))
                                      .arg(static_cast<qint32>(qApp->fromPixel(scenePos.y())))
                                      .arg(doc->UnitsToStr(qApp->patternUnit()));
    if (mouseCoordinate != nullptr)
    {
        mouseCoordinate->setText(string);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief CanselTool cansel tool.
 */
void MainWindow::CancelTool()
{
    delete dialogTool;
    dialogTool = nullptr;
    switch ( tool )
    {
        case Tool::ArrowTool:
            ui->actionArrowTool->setChecked(false);
            helpLabel->setText("");
            break;
        case Tool::SinglePointTool:
            Q_UNREACHABLE();
            //Nothing to do here because we can't create this tool from main window.
            break;
        case Tool::EndLineTool:
            ui->toolButtonEndLine->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::LineTool:
            ui->toolButtonLine->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearFocus();
            break;
        case Tool::AlongLineTool:
            ui->toolButtonAlongLine->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::ShoulderPointTool:
            ui->toolButtonShoulderPoint->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::NormalTool:
            ui->toolButtonNormal->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::BisectorTool:
            ui->toolButtonBisector->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::LineIntersectTool:
            ui->toolButtonLineIntersect->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::SplineTool:
            ui->toolButtonSpline->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::ArcTool:
            ui->toolButtonArc->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::SplinePathTool:
            ui->toolButtonSplinePath->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::PointOfContact:
            ui->toolButtonPointOfContact->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::DetailTool:
            ui->toolButtonNewDetail->setChecked(false);
            break;
        case Tool::Height:
            ui->toolButtonHeight->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::Triangle:
            ui->toolButtonTriangle->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::PointOfIntersection:
            ui->toolButtonPointOfIntersection->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::CutSplineTool:
            ui->toolButtonSplineCutPoint->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::CutSplinePathTool:
            ui->toolButtonSplinePathCutPoint->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::UnionDetails:
            ui->toolButtonUnionDetails->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::CutArcTool:
            ui->toolButtonArcCutPoint->setChecked(false);
            currentScene->setFocus(Qt::OtherFocusReason);
            currentScene->clearSelection();
            break;
        case Tool::NodePoint:
        case Tool::NodeArc:
        case Tool::NodeSpline:
        case Tool::NodeSplinePath:
        default:
            qDebug()<<"Got wrong tool type. Ignored.";
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ArrowTool enable arrow tool.
 */
void  MainWindow::ArrowTool()
{
    CancelTool();
    ui->actionArrowTool->setChecked(true);
    tool = Tool::ArrowTool;
    QCursor cur(Qt::ArrowCursor);
    view->setCursor(cur);
    helpLabel->setText("");
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ActionAroowTool set arrow tool. Cansel tool what was before.
 */
void MainWindow::ActionAroowTool()
{
    ArrowTool();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief keyPressEvent handle key press events.
 * @param event key event.
 */
void MainWindow::keyPressEvent ( QKeyEvent * event )
{
    switch (event->key())
    {
        case Qt::Key_Escape:
            ArrowTool();
            break;
        case Qt::Key_Return:
            EndVisualization();
            break;
        case Qt::Key_Enter:
            EndVisualization();
            break;
        default:
            break;
    }
    QMainWindow::keyPressEvent ( event );
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SaveCurrentScene save scene options before set another.
 */
void MainWindow::SaveCurrentScene()
{
    /*Save transform*/
    currentScene->setTransform(view->transform());
    /*Save scroll bars value for previous scene.*/
    QScrollBar *horScrollBar = view->horizontalScrollBar();
    currentScene->setHorScrollBar(horScrollBar->value());
    QScrollBar *verScrollBar = view->verticalScrollBar();
    currentScene->setVerScrollBar(verScrollBar->value());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief RestoreCurrentScene restore scene options after change.
 */
void MainWindow::RestoreCurrentScene()
{
    /*Set transform for current scene*/
    view->setTransform(currentScene->transform());
    /*Set value for current scene scroll bar.*/
    QScrollBar *horScrollBar = view->horizontalScrollBar();
    horScrollBar->setValue(currentScene->getHorScrollBar());
    QScrollBar *verScrollBar = view->verticalScrollBar();
    verScrollBar->setValue(currentScene->getVerScrollBar());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ActionDraw show draw scene.
 * @param checked true - button checked.
 */
void MainWindow::ActionDraw(bool checked)
{
    if (checked)
    {
        ui->actionDetails->setChecked(false);
        SaveCurrentScene();

        currentScene = sceneDraw;
        view->setScene(currentScene);
        connect(view, &VMainGraphicsView::NewFactor, sceneDraw, &VMainGraphicsScene::SetFactor);
        RestoreCurrentScene();

        mode = Draw::Calculation;
        comboBoxDraws->setEnabled(true);
        comboBoxDraws->setCurrentIndex(currentDrawIndex);//restore current pattern peace
        drawMode = true;

        SetEnableTool(true);
        doc->setCurrentData();
        ui->toolBox->setCurrentIndex(currentToolBoxIndex);

        ui->actionHistory->setEnabled(true);
        ui->actionOptionDraw->setEnabled(true);
        ui->actionNewDraw->setEnabled(true);
    }
    else
    {
        ui->actionDraw->setChecked(true);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ActionDetails show details scene.
 * @param checked true - button checked.
 */
void MainWindow::ActionDetails(bool checked)
{
    if (checked)
    {
        ui->actionDraw->setChecked(false);
        SaveCurrentScene();

        currentScene = sceneDetails;
        view->setScene(sceneDetails);
        disconnect(view, &VMainGraphicsView::NewFactor, sceneDraw, &VMainGraphicsScene::SetFactor);
        RestoreCurrentScene();

        drawMode = false;
        currentDrawIndex = comboBoxDraws->currentIndex();//save current pattern peace
        comboBoxDraws->setCurrentIndex(comboBoxDraws->count()-1);
        comboBoxDraws->setEnabled(false);


        mode = Draw::Modeling;
        SetEnableTool(true);
        currentToolBoxIndex = ui->toolBox->currentIndex();
        ui->toolBox->setCurrentIndex(4);

        ui->actionHistory->setEnabled(false);
        ui->actionOptionDraw->setEnabled(false);
        ui->actionNewDraw->setEnabled(false);
    }
    else
    {
        ui->actionDetails->setChecked(true);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SaveAs save as pattern file.
 * @return true for successes saving.
 */
bool MainWindow::SaveAs()
{
    QString filters(tr("Pattern files (*.val)"));
    QString path = qApp->getSettings()->value("paths/pattern", QDir::homePath()).toString();
    QString dir;
    if (curFile.isEmpty())
    {
        dir = path + "/" + tr("pattern") + ".val";
    }
    else
    {
        dir = QFileInfo(curFile).absolutePath() + tr("/pattern.val");
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), dir, filters);

    if (fileName.isEmpty())
    {
        return false;
    }
    QFileInfo f( fileName );
    if (f.suffix().isEmpty() && f.suffix() != "val")
    {
        fileName += ".val";
    }
    return SavePattern(fileName);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Save save pattern file.
 * @return true for successes saving.
 */
bool MainWindow::Save()
{
    if (curFile.isEmpty())
    {
        return SaveAs();
    }
    else
    {
        bool result = SavePattern(curFile);
        if (result)
        {
            QString autofile = curFile +".autosave";
            QFile file(autofile);
            file.remove();
        }
        return result;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Open ask user select pattern file.
 */
void MainWindow::Open()
{
    const QString filter(tr("Pattern files (*.val)"));
    //Get list last open files
    const QStringList files = qApp->getSettings()->value("recentFileList").toStringList();
    QString dir;
    if (files.isEmpty())
    {
        dir = QDir::homePath();
    }
    else
    {
        //Absolute path to last open file
        dir = QFileInfo(files.first()).absolutePath();
    }
    const QString filePath = QFileDialog::getOpenFileName(this, tr("Open file"), dir, filter);
    OpenPattern(filePath);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Preferences config dialog.
 */
void MainWindow::Preferences()
{
    ConfigDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        InitAutoSave();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Clear reset to default window.
 */
void MainWindow::Clear()
{
    setCurrentFile("");
    pattern->Clear();
    doc->clear();
    sceneDraw->clear();
    sceneDetails->clear();
    CancelTool();
    comboBoxDraws->clear();
    ui->actionOptionDraw->setEnabled(false);
    ui->actionSave->setEnabled(false);
    ui->actionPattern_properties->setEnabled(false);
    ui->actionZoomIn->setEnabled(false);
    ui->actionZoomOut->setEnabled(false);
    SetEnableTool(false);
    qApp->setPatternUnit(Unit::Cm);
    qApp->setPatternType(MeasurementsType::Individual);
    ui->toolBarOption->clear();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
    Layout();
}

void MainWindow::FullParseFile()
{
    try
    {
        SetEnabledGUI(true);
        doc->Parse(Document::FullParse);
    }
    catch (const VExceptionUndo &e)
    {
        Q_UNUSED(e);
        /* If user want undo last operation before undo we need finish broken redo operation. For those we post event
         * myself. Later in method customEvent call undo.*/
        QApplication::postEvent(this, new UndoEvent());
        return;
    }
    catch (const VExceptionObjectError &e)
    {
        e.CriticalMessageBox(tr("Error parsing file."), this);
        SetEnabledGUI(false);
        return;
    }
    catch (const VExceptionConversionError &e)
    {
        e.CriticalMessageBox(tr("Error can't convert value."), this);
        SetEnabledGUI(false);
        return;
    }
    catch (const VExceptionEmptyParameter &e)
    {
        e.CriticalMessageBox(tr("Error empty parameter."), this);
        SetEnabledGUI(false);
        return;
    }
    catch (const VExceptionWrongId &e)
    {
        e.CriticalMessageBox(tr("Error wrong id."), this);
        SetEnabledGUI(false);
        return;
    }
    catch (VException &e)
    {
        e.CriticalMessageBox(tr("Error parsing file."), this);
        SetEnabledGUI(false);
        return;
    }
    catch (const std::bad_alloc &)
    {
#ifndef QT_NO_CURSOR
        QApplication::restoreOverrideCursor();
#endif
        QMessageBox::critical(this, tr("Critical error!"), tr("Error parsing file (std::bad_alloc)."), QMessageBox::Ok,
                              QMessageBox::Ok);
#ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
        SetEnabledGUI(false);
        return;
    }

    QString patternPiece = QString();
    if (comboBoxDraws->currentIndex() != -1)
    {
        patternPiece = comboBoxDraws->itemText(comboBoxDraws->currentIndex());
    }
    disconnect(comboBoxDraws,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
               this, &MainWindow::currentPPChanged);
    comboBoxDraws->clear();
    comboBoxDraws->addItems(doc->getPatternPieces());
    connect(comboBoxDraws,  static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::currentPPChanged);
    ui->actionPattern_properties->setEnabled(true);

    qint32 index = comboBoxDraws->findText(patternPiece);
    try
    {
        if ( index != -1 )
        { // -1 for not found
            ChangePP(index, false);
        }
        else
        {
            ChangePP(0, false);
        }
    }
    catch (VExceptionBadId &e)
    {
        e.CriticalMessageBox(tr("Bad id."), this);
        SetEnabledGUI(false);
        return;
    }
    catch (const VExceptionEmptyParameter &e)
    {
        e.CriticalMessageBox(tr("Error empty parameter."), this);
        SetEnabledGUI(false);
        return;
    }

    if (comboBoxDraws->count() > 0)
    {
        SetEnableTool(true);
    }
    else
    {
        SetEnableTool(false);
    }
    SetEnableWidgets(true);
}


//---------------------------------------------------------------------------------------------------------------------
void MainWindow::SetEnabledGUI(bool enabled)
{
    if (guiEnabled != enabled)
    {
        if (enabled == false)
        {
            CancelTool();
        }
        comboBoxDraws->setEnabled(enabled);
        ui->actionOptionDraw->setEnabled(enabled);
        ui->actionSave->setEnabled(enabled);
        ui->actionSaveAs->setEnabled(enabled);
        ui->actionPattern_properties->setEnabled(enabled);
        ui->actionEdit_pattern_code->setEnabled(enabled);
        ui->actionZoomIn->setEnabled(enabled);
        ui->actionZoomOut->setEnabled(enabled);
        ui->actionArrowTool->setEnabled(enabled);
        ui->actionHistory->setEnabled(enabled);
        ui->actionNewDraw->setEnabled(enabled);
        ui->actionDraw->setEnabled(enabled);
        ui->actionDetails->setEnabled(enabled);
        ui->actionTable->setEnabled(enabled);
        guiEnabled = enabled;

        sceneDraw->SetDisable(!enabled);
        view->setEnabled(enabled);

        SetEnableTool(enabled);
        ui->toolBarOption->setEnabled(enabled);
    #ifndef QT_NO_CURSOR
        QApplication::setOverrideCursor(Qt::ArrowCursor);
    #endif
    }
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::ClickEndVisualization()
{
    EndVisualization(true);
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::Layout()
{
    const QHash<quint32, VDetail> *details = pattern->DataDetails();
    if (details->size() > 0)
    {
        ui->actionLayout->setEnabled(true);
    }
    else
    {
        ui->actionLayout->setEnabled(false);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief NewPattern create new empty pattern.
 */
void MainWindow::NewPattern()
{
    if (this->isWindowModified() || curFile.isEmpty() == false)
    {
        QProcess *v = new QProcess(this);
        v->startDetached(QCoreApplication::applicationFilePath ());
        delete v;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief haveChange enable action save if we have unsaved change.
 */
void MainWindow::PatternWasModified(bool saved)
{
    if (guiEnabled)
    {
        setWindowModified(!saved);
        ui->actionSave->setEnabled(!saved);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ChangedSize change new size value.
 * @param text value size.
 */
void MainWindow::ChangedSize(const QString & text)
{
    qint32 size = text.toInt();
    pattern->SetSize(size);
    doc->LiteParseTree(Document::LiteParse);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ChangedGrowth change new height value.
 * @param text value height.
 */
void MainWindow::ChangedHeight(const QString &text)
{
    qint32 growth = text.toInt();
    pattern->SetHeight(growth);
    doc->LiteParseTree(Document::LiteParse);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetEnableWidgets enable action button.
 * @param enable enable value.
 */
void MainWindow::SetEnableWidgets(bool enable)
{
    ui->actionSaveAs->setEnabled(enable);
    ui->actionDraw->setEnabled(enable);
    ui->actionDetails->setEnabled(enable);
    ui->actionOptionDraw->setEnabled(enable);
    if (enable == true && curFile.isEmpty() == false)
    {
        ui->actionSave->setEnabled(enable);
    }
    ui->actionTable->setEnabled(enable);
    ui->actionHistory->setEnabled(enable);
    ui->actionPattern_properties->setEnabled(enable);
    ui->actionEdit_pattern_code->setEnabled(enable);
    ui->actionZoomIn->setEnabled(enable);
    ui->actionZoomOut->setEnabled(enable);
    ui->actionZoomFitBest->setEnabled(enable);
    ui->actionZoomOriginal->setEnabled(enable);
    //Now we want allow user call context menu
    view->setEnabled(enable);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ActionTable show table with variables.
 * @param checked true - button checked.
 */
void MainWindow::ActionTable(bool checked)
{
    if (checked)
    {
        dialogTable = new DialogIncrements(pattern, doc, this);
        connect(dialogTable, &DialogIncrements::DialogClosed, this, &MainWindow::ClosedActionTable);
        dialogTable->show();
    }
    else
    {
        ui->actionTable->setChecked(true);
        dialogTable->activateWindow();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ClosedActionTable actions after closing table with variables.
 */
void MainWindow::ClosedActionTable()
{
    ui->actionTable->setChecked(false);
    delete dialogTable;
    dialogTable = nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ActionHistory show tool history.
 * @param checked true - button checked.
 */
void MainWindow::ActionHistory(bool checked)
{
    if (checked)
    {
        dialogHistory = new DialogHistory(pattern, doc, this);
        dialogHistory->setWindowFlags(Qt::Window);
        connect(dialogHistory, &DialogHistory::DialogClosed, this, &MainWindow::ClosedActionHistory);
        dialogHistory->show();
    }
    else
    {
        ui->actionHistory->setChecked(true);
        dialogHistory->activateWindow();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ActionLayout begin creation layout.
 * @param checked true - button checked.
 */
void MainWindow::ActionLayout(bool checked)
{
    Q_UNUSED(checked);
    hide();
    QVector<VItem*> listDetails;
    const QHash<quint32, VDetail> *details = pattern->DataDetails();
    QHashIterator<quint32, VDetail> idetail(*details);
    while (idetail.hasNext())
    {
        idetail.next();
        QPainterPath path = VEquidistant().ContourPath(idetail.key(), pattern);
        listDetails.append(new VItem(path, listDetails.size()));
    }
    QString description = doc->UniqueTagText("description");
    emit ModelChosen(listDetails, curFile, description);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ClosedActionHistory actions after closing history window with variables.
 */
void MainWindow::ClosedActionHistory()
{
    ui->actionHistory->setChecked(false);
    delete dialogHistory;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SetEnableTool enable button.
 * @param enable enable value.
 */
void MainWindow::SetEnableTool(bool enable)
{
    bool drawTools = false;
    bool modelingTools = false;
    if (mode == Draw::Calculation)
    {
        drawTools = enable;
    }
    else
    {
        modelingTools = enable;
    }
    //Drawing Tools
    ui->toolButtonEndLine->setEnabled(drawTools);
    ui->toolButtonLine->setEnabled(drawTools);
    ui->toolButtonAlongLine->setEnabled(drawTools);
    ui->toolButtonShoulderPoint->setEnabled(drawTools);
    ui->toolButtonNormal->setEnabled(drawTools);
    ui->toolButtonBisector->setEnabled(drawTools);
    ui->toolButtonLineIntersect->setEnabled(drawTools);
    ui->toolButtonSpline->setEnabled(drawTools);
    ui->toolButtonArc->setEnabled(drawTools);
    ui->toolButtonSplinePath->setEnabled(drawTools);
    ui->toolButtonPointOfContact->setEnabled(drawTools);
    ui->toolButtonNewDetail->setEnabled(drawTools);
    ui->toolButtonHeight->setEnabled(drawTools);
    ui->toolButtonTriangle->setEnabled(drawTools);
    ui->toolButtonPointOfIntersection->setEnabled(drawTools);
    ui->toolButtonSplineCutPoint->setEnabled(drawTools);
    ui->toolButtonSplinePathCutPoint->setEnabled(drawTools);
    ui->toolButtonArcCutPoint->setEnabled(drawTools);

    //Modeling Tools
    ui->toolButtonUnionDetails->setEnabled(modelingTools);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief MinimumScrollBar set scroll bar to minimum.
 */
void MainWindow::MinimumScrollBar()
{
    QScrollBar *horScrollBar = view->horizontalScrollBar();
    horScrollBar->setValue(horScrollBar->minimum());
    QScrollBar *verScrollBar = view->verticalScrollBar();
    verScrollBar->setValue(verScrollBar->minimum());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SavePattern save pattern file.
 * @param fileName pattern file name.
 * @return true if all is good.
 */
bool MainWindow::SavePattern(const QString &fileName)
{
    QFileInfo tempInfo(fileName);
    const bool result = doc->SaveDocument(fileName);
    if (result)
    {
        if (tempInfo.suffix() != "autosave")
        {
            setCurrentFile(fileName);
            helpLabel->setText(tr("File saved"));
            qApp->getUndoStack()->clear();
        }
    }
    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief AutoSavePattern start safe saving.
 */
void MainWindow::AutoSavePattern()
{
    if (curFile.isEmpty() == false && this->isWindowModified() == true)
    {
        QString autofile = curFile +".autosave";
        if (SavePattern(autofile) == false)
        {
            qDebug()<<"Can not save pattern"<<Q_FUNC_INFO;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief setCurrentFile the function is called to reset the state of a few variables when a file
 * is loaded or saved, or when the user starts editing a new file (in which case fileName is empty).
 * @param fileName file name.
 */
void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowModified(false);

    QString shownName = strippedName(curFile);
    if (curFile.isEmpty())
    {
        shownName = tr("untitled.val");
    }
    else
    {
        QStringList files = qApp->getSettings()->value("recentFileList").toStringList();
        files.removeAll(fileName);
        files.prepend(fileName);
        while (files.size() > MaxRecentFiles)
        {
            files.removeLast();
        }

        qApp->getSettings()->setValue("recentFileList", files);
        UpdateRecentFileActions();
    }
    shownName+="[*]";
    setWindowTitle(shownName);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief strippedName the function call around curFile to exclude the path to the file.
 * @param fullFileName full path to the file.
 * @return file name.
 */
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ReadSettings read setting for app.
 */
void MainWindow::ReadSettings()
{
    QPoint pos = qApp->getSettings()->value("pos", QPoint(10, 10)).toPoint();
    QSize size = qApp->getSettings()->value("size", QSize(1000, 800)).toSize();
    resize(size);
    move(pos);

    // Scene antialiasing
    bool graphOutputValue = qApp->getSettings()->value("pattern/graphicalOutput", 1).toBool();
    view->setRenderHint(QPainter::Antialiasing, graphOutputValue);
    view->setRenderHint(QPainter::SmoothPixmapTransform, graphOutputValue);

    // Stack limit
    bool ok = true;
    qint32 count = qApp->getSettings()->value("pattern/undo", 0).toInt(&ok);
    if (ok == false)
    {
        count = 0;
    }
    qApp->getUndoStack()->setUndoLimit(count);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief WriteSettings save setting for app.
 */
void MainWindow::WriteSettings()
{
    qApp->getSettings()->setValue("pos", pos());
    qApp->getSettings()->setValue("size", size());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief MaybeSave The function is called to save pending changes.
 * @return returns true in all cases, except when the user clicks Cancel.
 */
bool MainWindow::MaybeSave()
{
    if (this->isWindowModified() && guiEnabled)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Unsaved change"), tr("The pattern has been modified.\n"
                                                             "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            return Save();
        }
        else if (ret == QMessageBox::Cancel)
        {
            return false;
        }
    }
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::UpdateRecentFileActions()
{
    QStringList files = qApp->getSettings()->value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), static_cast<int>(MaxRecentFiles));

    for (int i = 0; i < numRecentFiles; ++i)
    {
       QString text = QString("&%1 %2").arg(i + 1).arg(strippedName(files.at(i)));
       recentFileActs[i]->setText(text);
       recentFileActs[i]->setData(files.at(i));
       recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {
       recentFileActs[j]->setVisible(false);
    }

    separatorAct->setVisible(numRecentFiles > 0);
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::CreateMenus()
{
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        ui->menuFile->insertAction(ui->actionPreferences, recentFileActs[i]);
    }
    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);
    ui->menuFile->insertAction(ui->actionPreferences, separatorAct);
    UpdateRecentFileActions();

    //Add Undo/Redo actions.
    QAction *undoAction = qApp->getUndoStack()->createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);
    undoAction->setIcon(QIcon::fromTheme("edit-undo"));
    ui->menuDrawing->insertAction(ui->actionPattern_properties, undoAction);
    ui->toolBarTools->addAction(undoAction);

    QAction *redoAction = qApp->getUndoStack()->createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);
    redoAction->setIcon(QIcon::fromTheme("edit-redo"));
    ui->menuDrawing->insertAction(ui->actionPattern_properties, redoAction);
    ui->toolBarTools->addAction(redoAction);

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);
    ui->menuDrawing->insertAction(ui->actionPattern_properties, separatorAct);
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::CreateActions()
{
    ui->setupUi(this);

    connect(ui->actionArrowTool, &QAction::triggered, this, &MainWindow::ActionAroowTool);
    connect(ui->actionDraw, &QAction::triggered, this, &MainWindow::ActionDraw);
    connect(ui->actionDetails, &QAction::triggered, this, &MainWindow::ActionDetails);
    connect(ui->actionNewDraw, &QAction::triggered, this, &MainWindow::ActionNewPP);
    connect(ui->actionOptionDraw, &QAction::triggered, this, &MainWindow::OptionDraw);
    connect(ui->actionSaveAs, &QAction::triggered, this, &MainWindow::SaveAs);
    connect(ui->actionSave, &QAction::triggered, this, &MainWindow::Save);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::Open);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::NewPattern);
    connect(ui->actionTable, &QAction::triggered, this, &MainWindow::ActionTable);
    connect(ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::AboutQt);
    connect(ui->actionAbout_Valentina, &QAction::triggered, this, &MainWindow::About);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionPreferences, &QAction::triggered, this, &MainWindow::Preferences);
    connect(ui->actionPattern_properties, &QAction::triggered, this, &MainWindow::PatternProperties);
    ui->actionPattern_properties->setEnabled(false);
    connect(ui->actionEdit_pattern_code, &QAction::triggered, this, &MainWindow::EditPatternCode);
    ui->actionEdit_pattern_code->setEnabled(false);

    //Actions for recent files loaded by a main window application.
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], &QAction::triggered, this, &MainWindow::OpenRecentFile);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::InitAutoSave()
{
    //Autosaving file each 5 minutes
    delete autoSaveTimer;
    autoSaveTimer = nullptr;

    autoSaveTimer = new QTimer(this);
    autoSaveTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(autoSaveTimer, &QTimer::timeout, this, &MainWindow::AutoSavePattern);
    autoSaveTimer->stop();

    bool autoSave = qApp->getSettings()->value("configuration/autosave/state", 1).toBool();
    if (autoSave)
    {
        bool ok = true;
        qint32 autoTime = qApp->getSettings()->value("configuration/autosave/time", 5).toInt(&ok);
        if (ok == false)
        {
            autoTime = 5;
        }
        autoSaveTimer->start(autoTime*60000);

    }
    qApp->setAutoSaveTimer(autoSaveTimer);
}

//---------------------------------------------------------------------------------------------------------------------
QString MainWindow::PatternPieceName(const QString &text)
{
    QInputDialog *dlg = new QInputDialog(this);
    dlg->setInputMode( QInputDialog::TextInput );
    dlg->setLabelText(tr("Pattern piece:"));
    dlg->setTextEchoMode(QLineEdit::Normal);
    dlg->setWindowTitle(tr("Enter a new label for the pattern piece."));
    dlg->resize(300, 100);
    dlg->setTextValue(text);
    QString nameDraw;
    while (1)
    {
        const bool bOk = dlg->exec();
        nameDraw = dlg->textValue();
        if (bOk == false || nameDraw.isEmpty())
        {
            delete dlg;
            return QString();
        }
        if (comboBoxDraws->findText(nameDraw) == -1)
        {
            break;//repeate show dialog
        }
    }
    delete dlg;
    return nameDraw;
}

//---------------------------------------------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    CancelTool();
    delete ui;

    delete pattern;
    delete doc;
    delete sceneDetails;
    delete sceneDraw;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief LoadPattern open pattern file.
 * @param fileName name of file.
 */
void MainWindow::LoadPattern(const QString &fileName)
{
    // On this stage scene empty. Fit scene size to view size
    VAbstractTool::NewSceneRect(sceneDraw, view);
    VAbstractTool::NewSceneRect(sceneDetails, view);

    qApp->setOpeningPattern();//Begin opening file
    try
    {
        VDomDocument::ValidateXML("://schema/pattern.xsd", fileName);
        doc->setContent(fileName);

        qApp->setPatternUnit(doc->MUnit());
        qApp->setPatternType(doc->MType());
        QString path = doc->MPath();

        path = CheckPathToMeasurements(path, qApp->patternType());
        if (path.isEmpty())
        {
            Clear();
            return;
        }

        if (qApp->patternType() == MeasurementsType::Standard)
        {
            VStandardMeasurements m(pattern);
            m.setContent(path);
            if (m.MUnit() == Unit::Inch)
            {
                QMessageBox::critical(this, tr("Wrong units."),
                                      tr("Application doesn't support standard table with inches."));
                Clear();
                return;
            }
            m.SetSize();
            m.SetHeight();
        }
        ToolBarOption();
    }
    catch (VException &e)
    {
        e.CriticalMessageBox(tr("File error."), this);
        Clear();
        return;
    }

    FullParseFile();

    bool patternModified = this->isWindowModified();
    setCurrentFile(fileName);
    if (patternModified)
    {
        //For situation where was fixed wrong formula need return for document status was modified.
        PatternWasModified(!patternModified);
    }
    helpLabel->setText(tr("File loaded"));

    qApp->setOpeningPattern();// End opening file

    //Fit scene size to best size for first show
    ZoomFirstShow();
}

//---------------------------------------------------------------------------------------------------------------------
QString MainWindow::CheckPathToMeasurements(const QString &path, const MeasurementsType &patternType)
{
    QFile table(path);
    if (table.exists() == false)
    {
        QString text = tr("The measurements file <br/><br/> <b>%1</b> <br/><br/> %3").arg(path)
                .arg(tr("could not be found. Do you want to update the file location"));
        QMessageBox::StandardButton res = QMessageBox::question(this, "Loading measurements file", text,
                                                                QMessageBox::Yes | QMessageBox::No,
                                                                QMessageBox::Yes);
        if (res == QMessageBox::No)
        {
            return QString();
        }
        else
        {
            QString filter;
            if (patternType == MeasurementsType::Standard)
            {
                filter = tr("Standard measurements (*.vst)");
            }
            else
            {
                filter = tr("Individual measurements (*.vit)");
            }
            QString mPath = QFileDialog::getOpenFileName(this, tr("Open file"), qApp->pathToTables(), filter);

            if (mPath.isEmpty())
            {
                return mPath;
            }
            else
            {
                if (patternType == MeasurementsType::Standard)
                {
                    VDomDocument::ValidateXML("://schema/standard_measurements.xsd", mPath);
                }
                else
                {
                    VDomDocument::ValidateXML("://schema/individual_measurements.xsd", mPath);
                }
                doc->SetPath(mPath);
                PatternWasModified(false);
                return mPath;
            }
        }
    }
    return path;
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::OpenPattern(const QString &filePath)
{
    if (filePath.isEmpty() == false && filePath != curFile)
    {
        if (curFile.isEmpty() && this->isWindowModified() == false)
        {
            LoadPattern(filePath);
        }
        else
        {
            VApplication::NewValentina(filePath);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::ChangePP(int index, bool zoomBestFit)
{
    if (index != -1)
    {
        doc->ChangeActivPP(comboBoxDraws->itemText(index));
        doc->setCurrentData();
        if (drawMode)
        {
            ArrowTool();
            if (zoomBestFit)
            {
                view->fitInView(doc->ActiveDrawBoundingRect(), Qt::KeepAspectRatio);
                view->NewFactor(view->transform().m11());
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void MainWindow::EndVisualization(bool click)
{
    if (dialogTool != nullptr)
    {
        dialogTool->ShowDialog(click);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ZoomFirstShow Fit scene size to best size for first show
 */
void MainWindow::ZoomFirstShow()
{
    /* If don't call ZoomFitBest() twice, after first scaling or moving pattern piece, scene change coordinate and whole
     * pattern will be moved. Looks very ugly. It is best solution that i have now.
     */
    ActionDetails(true);
    view->ZoomFitBest();

    ActionDraw(true);
    view->ZoomFitBest();

    VAbstractTool::NewSceneRect(sceneDraw, view);
    VAbstractTool::NewSceneRect(sceneDetails, view);

    ActionDetails(true);
    view->ZoomFitBest();

    ActionDraw(true);
    view->ZoomFitBest();
}
