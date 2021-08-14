/************************************************************************
 **
 **  @file   vpmainwindow.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   16 2, 2020
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
#include "vpmainwindow.h"

#include <QFileDialog>
#include <QCloseEvent>
#include <QtMath>
#include <QSvgGenerator>
#include <QFileSystemWatcher>

#include "ui_vpmainwindow.h"
#include "dialogs/vpdialogabout.h"
#include "xml/vplayoutfilewriter.h"
#include "xml/vplayoutfilereader.h"
#include "vpapplication.h"
#include "../vlayout/vrawlayout.h"
#include "../vmisc/vsysexits.h"
#include "../vmisc/projectversion.h"
#include "../ifc/xml/vlayoutconverter.h"
#include "../ifc/exception/vexception.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "layout/vpsheet.h"
#include "dialogs/dialogpuzzlepreferences.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
#include "../vmisc/backport/qscopeguard.h"
#else
#include <QScopeGuard>
#endif

#include <QLoggingCategory>

QT_WARNING_PUSH
QT_WARNING_DISABLE_CLANG("-Wmissing-prototypes")
QT_WARNING_DISABLE_INTEL(1418)

Q_LOGGING_CATEGORY(pWindow, "p.window")

QT_WARNING_POP

//---------------------------------------------------------------------------------------------------------------------
VPMainWindow::VPMainWindow(const VPCommandLinePtr &cmd, QWidget *parent) :
    VAbstractMainWindow(parent),
    ui(new Ui::VPMainWindow),
    m_cmd(cmd),
    m_statusLabel(new QLabel(this)),
    m_layoutWatcher(new QFileSystemWatcher(this))
{
    //    // ----- for test purposes, to be removed------------------
    m_layout->LayoutSettings().SetUnit(Unit::Cm);
    m_layout->LayoutSettings().SetWarningSuperpositionOfPieces(true);
    m_layout->LayoutSettings().SetTitle(QString("My Test Layout"));
    m_layout->LayoutSettings().SetDescription(QString("Description of my Layout"));

    m_layout->LayoutSettings().SetTilesSizeConverted(21,29.7);
    m_layout->LayoutSettings().SetTilesOrientation(PageOrientation::Portrait);
    m_layout->LayoutSettings().SetTilesMarginsConverted(1,1,1,1);
    m_layout->LayoutSettings().SetShowTiles(true);

    // --------------------------------------------------------

    ui->setupUi(this);

    // create a standard sheet
    AddSheet();

    // init the tile factory
    m_tileFactory = new VPTileFactory(m_layout, VPApplication::VApp()->Settings());
    m_tileFactory->refreshTileInfos();

    // init status bar
    statusBar()->addPermanentWidget(m_statusLabel, 1);
    
    SetupMenu();
    InitProperties();
    InitCarrousel();

    InitMainGraphics();

    InitZoomToolBar();
    InitScaleToolBar();

    SetPropertiesData();

    ReadSettings();

#if defined(Q_OS_MAC)
    // Mac OS Dock Menu
    QMenu *menu = new QMenu(this);
    connect(menu, &QMenu::aboutToShow, this, &VPMainWindow::AboutToShowDockMenu);
    AboutToShowDockMenu();
    menu->setAsDockMenu();
#endif //defined(Q_OS_MAC)

    connect(m_layoutWatcher, &QFileSystemWatcher::fileChanged, this, [this](const QString &path)
    {
        if (not curFile.isEmpty() && curFile == path)
        {
            UpdateWindowTitle();
        }
    });
}

//---------------------------------------------------------------------------------------------------------------------
VPMainWindow::~VPMainWindow()
{
    delete ui;
    delete m_carrousel;
}

//---------------------------------------------------------------------------------------------------------------------
auto VPMainWindow::CurrentFile() const -> QString
{
    return curFile;
}

//---------------------------------------------------------------------------------------------------------------------
auto VPMainWindow::LoadFile(QString path) -> bool
{
    if (not QFileInfo::exists(path))
    {
        qCCritical(pWindow, "%s", qUtf8Printable(tr("File '%1' doesn't exist!").arg(path)));
        if (m_cmd->IsTestModeEnabled())
        {
            qApp->exit(V_EX_NOINPUT);
        }
        return false;
    }

    // Check if file already opened
    QList<VPMainWindow*> list = VPApplication::VApp()->MainWindows();
    auto w = std::find_if(list.begin(), list.end(),
                          [path](VPMainWindow *window) { return window->CurrentFile() == path; });
    if (w != list.end())
    {
        (*w)->activateWindow();
        if (this != *w)
        {
            close();
            return false;
        }
        return true;
    }

    VlpCreateLock(lock, path);

    if (not lock->IsLocked())
    {
        if (not IgnoreLocking(lock->GetLockError(), path, m_cmd->IsGuiEnabled()))
        {
            return false;
        }
    }

    try
    {
        VLayoutConverter converter(path);
        m_curFileFormatVersion = converter.GetCurrentFormatVersion();
        m_curFileFormatVersionStr = converter.GetFormatVersionStr();
        path = converter.Convert();
    }
    catch (VException &e)
    {
        qCCritical(pWindow, "%s\n\n%s\n\n%s", qUtf8Printable(tr("File error.")),
                   qUtf8Printable(e.ErrorMessage()), qUtf8Printable(e.DetailedInformation()));
        return false;
    }

    QFile file(path);
    file.open(QIODevice::ReadOnly);

    VPLayoutFileReader fileReader;

    delete m_layout;
    m_layout = new VPLayout();

    fileReader.ReadFile(m_layout, &file);

    if (fileReader.hasError())
    {
        qCCritical(pWindow, "%s\n\n%s", qUtf8Printable(tr("File error.")),
                   qUtf8Printable(tr("Unable to read a layout file")));
        lock.reset();

        if (m_cmd->IsTestModeEnabled())
        {
            qApp->exit(V_EX_NOINPUT);
        }
        return false;
    }

    SetCurrentFile(path);

    m_layout->SetFocusedSheet();

    // updates the properties with the loaded data
    SetPropertiesData();

    m_carrousel->Refresh();
    m_graphicsView->RefreshLayout();
    m_graphicsView->RefreshPieces();

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::LayoutWasSaved(bool saved)
{
    setWindowModified(!saved);
    not IsLayoutReadOnly() ? ui->actionSave->setEnabled(!saved): ui->actionSave->setEnabled(false);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::SetCurrentFile(const QString &fileName)
{
    if (not curFile.isEmpty() && m_layoutWatcher->files().contains(curFile))
    {
        m_layoutWatcher->removePath(curFile);
    }

    curFile = fileName;
    if (not curFile.isEmpty())
    {
        if (not m_layoutWatcher->files().contains(curFile))
        {
            m_layoutWatcher->addPath(curFile);
        }
        auto *settings = VPApplication::VApp()->PuzzleSettings();
        QStringList files = settings->GetRecentFileList();
        files.removeAll(fileName);
        files.prepend(fileName);
        while (files.size() > MaxRecentFiles)
        {
            files.removeLast();
        }
        settings->SetRecentFileList(files);
        UpdateRecentFileActions();
    }

    UpdateWindowTitle();
}

//---------------------------------------------------------------------------------------------------------------------
auto VPMainWindow::SaveLayout(const QString &path, QString &error) -> bool
{
    QFile file(path);
    file.open(QIODevice::WriteOnly);

    VPLayoutFileWriter fileWriter;
    fileWriter.WriteFile(m_layout, &file);

    if (fileWriter.hasError())
    {
        error = tr("Fail to create layout.");
        return false;
    }

    SetCurrentFile(path);
    LayoutWasSaved(true);
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::ImportRawLayouts(const QStringList &rawLayouts)
{
    VRawLayout rawLayoutReader;

    for(const auto &path : rawLayouts)
    {
        VRawLayoutData data;
        if (rawLayoutReader.ReadFile(path, data))
        {
            for (const auto& rawPiece : data.pieces)
            {
                // TODO / FIXME: make a few tests, on the data to check for validity. If not
                //
                // If seam allowance enabled, but the path is empty — invalid.
                // If seam line path not hidden, but the path is empty — invalid.
                // If seam allowance is built-in, but the seam line path is empty — invalid.


                // TODO for feature "Update piece" : CreateOrUpdate() function indstead of CreatePiece()
                VPPiece *piece = CreatePiece(rawPiece);
                piece->SetSheet(nullptr); // just in case
                m_layout->AddPiece(piece);
            }

            m_carrousel->Refresh();
            LayoutWasSaved(false);
        }
        else
        {
            qCCritical(pWindow, "%s\n", qPrintable(tr("Could not extract data from file '%1'. %2")
                                                    .arg(path, rawLayoutReader.ErrorString())));
            if (m_cmd != nullptr && not m_cmd->IsGuiEnabled())
            {
                m_cmd->ShowHelp(V_EX_DATAERR);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitZoom()
{
    if (m_graphicsView != nullptr)
    {
        m_graphicsView->ZoomFitBest();
    }
}

//---------------------------------------------------------------------------------------------------------------------
VPPiece* VPMainWindow::CreatePiece(const VLayoutPiece &rawPiece)
{
    auto *piece = new VPPiece(rawPiece);


    // cutting line : GetMappedSeamAllowancePoints();
    // seamline : GetMappedContourPoints();

    // rawPiece.IsGrainlineEnabled() , GrainlineAngle , GetGrainline


    // TODO : set all the information we need for the piece!


    return piece;
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::SetupMenu()
{
    // most of the actions are connected through name convention (auto-connection)

    // -------------------- connects the actions for the file menu
    ui->actionNew->setShortcuts(QKeySequence::New);
    ui->actionSave->setShortcuts(QKeySequence::Save);
    ui->actionSaveAs->setShortcuts(QKeySequence::SaveAs);

    connect(ui->actionExit, &QAction::triggered, this, &VPMainWindow::close);
    ui->actionExit->setShortcuts(QKeySequence::Quit);

    // -------------------- connects the actions for the edit menu
    // TODO : initialise the undo / redo

    // -------------------- connects the actions for the windows menu
    // TODO : initialise the entries for the different windows

    // Add dock properties action
    QAction* actionDockWidgetToolOptions = ui->dockWidgetProperties->toggleViewAction();
    ui->menuEdit->addAction(actionDockWidgetToolOptions);

    // File
    m_recentFileActs.fill(nullptr);
    for (auto & recentFileAct : m_recentFileActs)
    {
        auto *action = new QAction(this);
        recentFileAct = action;
        connect(action, &QAction::triggered, this, [this]()
        {
            if (auto *senderAction = qobject_cast<QAction *>(sender()))
            {
                const QString filePath = senderAction->data().toString();
                if (not filePath.isEmpty())
                {
                    LoadFile(filePath);
                }
            }
        });
        ui->menuFile->insertAction(ui->actionPreferences, recentFileAct);
        recentFileAct->setVisible(false);
    }

    m_separatorAct = new QAction(this);
    m_separatorAct->setSeparator(true);
    m_separatorAct->setVisible(false);
    ui->menuFile->insertAction(ui->actionPreferences, m_separatorAct);

    // Actions for recent files loaded by a puzzle window application.
    UpdateRecentFileActions();

    // Window
    connect(ui->menuWindow, &QMenu::aboutToShow, this, [this]()
    {
        ui->menuWindow->clear();
        CreateWindowMenu(ui->menuWindow);
    });
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitProperties()
{
    InitPropertyTabCurrentPiece();
    InitPropertyTabCurrentSheet();
    InitPropertyTabLayout();
    InitPropertyTabTiles();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitPropertyTabCurrentPiece()
{
    // FIXME ---- For MVP we hide a few things. To be displayed when functions there
    ui->groupBoxLayoutControl->hide();
    ui->groupBoxCurrentPieceGeometry->hide();


    // ------------------------------ placement -----------------------------------
    connect(ui->doubleSpinBoxCurrentPieceBoxPositionX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_CurrentPiecePositionEdited);
    connect(ui->doubleSpinBoxCurrentPieceBoxPositionY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_CurrentPiecePositionEdited);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitPropertyTabCurrentSheet()
{
    // FIXME ---- For MVP we hide a few things. To be displayed when functions there
    ui->pushButtonSheetRemoveUnusedLength->hide();
    ui->groupBoxSheetControl->hide();

    // some of the UI Elements are connected to the slots via auto-connect
   // see https://doc.qt.io/qt-5/designer-using-a-ui-file.html#widgets-and-dialogs-with-auto-connect

    // -------------------- layout width, length, orientation  ------------------------
    connect(ui->doubleSpinBoxSheetWidth, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_SheetSizeChanged);
    connect(ui->doubleSpinBoxSheetLength, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_SheetSizeChanged);
    connect(ui->radioButtonSheetPortrait, QOverload<bool>::of(&QRadioButton::clicked), this,
            &VPMainWindow::on_SheetOrientationChanged);
    connect(ui->radioButtonSheetLandscape, QOverload<bool>::of(&QRadioButton::clicked), this,
            &VPMainWindow::on_SheetOrientationChanged);

    // -------------------- margins  ------------------------
    connect(ui->doubleSpinBoxSheetMarginTop, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_SheetMarginChanged);
    connect(ui->doubleSpinBoxSheetMarginRight, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_SheetMarginChanged);
    connect(ui->doubleSpinBoxSheetMarginBottom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_SheetMarginChanged);
    connect(ui->doubleSpinBoxSheetMarginLeft, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_SheetMarginChanged);

    // ------------------- follow grainline -----------------------
    connect(ui->radioButtonSheetFollowGrainlineNo, QOverload<bool>::of(&QRadioButton::clicked), this,
            &VPMainWindow::on_SheetFollowGrainlineChanged);
    connect(ui->radioButtonSheetFollowGrainlineVertical, QOverload<bool>::of(&QRadioButton::clicked), this,
            &VPMainWindow::on_SheetFollowGrainlineChanged);
    connect(ui->radioButtonSheetFollowGrainlineHorizontal, QOverload<bool>::of(&QRadioButton::clicked), this,
            &VPMainWindow::on_SheetFollowGrainlineChanged);

    // -------------------- sheet template ---------------------------

    // FIXME: find a nicer way to initiliase it
    QVector<PaperSizeTemplate> sheetTemplates = QVector<PaperSizeTemplate>();
    sheetTemplates.append(PaperSizeTemplate::A0);
    sheetTemplates.append(PaperSizeTemplate::A1);
    sheetTemplates.append(PaperSizeTemplate::A2);
    sheetTemplates.append(PaperSizeTemplate::A3);
    sheetTemplates.append(PaperSizeTemplate::A4);
    sheetTemplates.append(PaperSizeTemplate::Letter);
    sheetTemplates.append(PaperSizeTemplate::Legal);
    sheetTemplates.append(PaperSizeTemplate::Tabloid);
    sheetTemplates.append(PaperSizeTemplate::Roll24in);
    sheetTemplates.append(PaperSizeTemplate::Roll30in);
    sheetTemplates.append(PaperSizeTemplate::Roll36in);
    sheetTemplates.append(PaperSizeTemplate::Roll42in);
    sheetTemplates.append(PaperSizeTemplate::Roll44in);
    sheetTemplates.append(PaperSizeTemplate::Roll48in);
    sheetTemplates.append(PaperSizeTemplate::Roll62in);
    sheetTemplates.append(PaperSizeTemplate::Roll72in);
    sheetTemplates.append(PaperSizeTemplate::Custom);

    ui->comboBoxSheetTemplate->blockSignals(true);
    VPLayoutSettings::PopulateComboBox(&sheetTemplates, ui->comboBoxSheetTemplate);
    ui->comboBoxSheetTemplate->blockSignals(false);

    ui->comboBoxSheetTemplate->setCurrentIndex(0);

    // ---------------------- export format --------------------------

    VPExporter exporter;
    for (auto &v : exporter.InitFormats())
    {
        ui->comboBoxSheetExportFormat->addItem(v.first, QVariant(static_cast<int>(v.second)));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitPropertyTabTiles()
{
    // -------------------- layout width, length, orientation  ------------------------
    connect(ui->doubleSpinBoxTilesWidth, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_TilesSizeChanged);
    connect(ui->doubleSpinBoxTilesLength, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_TilesSizeChanged);
    connect(ui->radioButtonTilesPortrait, QOverload<bool>::of(&QRadioButton::clicked), this,
            &VPMainWindow::on_TilesOrientationChanged);
    connect(ui->radioButtonTilesLandscape, QOverload<bool>::of(&QRadioButton::clicked), this,
            &VPMainWindow::on_TilesOrientationChanged);

    // -------------------- tiles template
    QVector<PaperSizeTemplate> tilesTemplates = QVector<PaperSizeTemplate>();
    tilesTemplates.append(PaperSizeTemplate::A0);
    tilesTemplates.append(PaperSizeTemplate::A1);
    tilesTemplates.append(PaperSizeTemplate::A2);
    tilesTemplates.append(PaperSizeTemplate::A3);
    tilesTemplates.append(PaperSizeTemplate::A4);
    tilesTemplates.append(PaperSizeTemplate::Letter);
    tilesTemplates.append(PaperSizeTemplate::Legal);
    tilesTemplates.append(PaperSizeTemplate::Custom);

    ui->comboBoxTilesTemplate->blockSignals(true);
    VPLayoutSettings::PopulateComboBox(&tilesTemplates, ui->comboBoxTilesTemplate);
    ui->comboBoxTilesTemplate->blockSignals(false);

    ui->comboBoxTilesTemplate->setCurrentIndex(4); //A4


    // -------------------- margins  ------------------------
    connect(ui->doubleSpinBoxTilesMarginTop, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_TilesMarginChanged);
    connect(ui->doubleSpinBoxTilesMarginRight, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_TilesMarginChanged);
    connect(ui->doubleSpinBoxTilesMarginBottom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_TilesMarginChanged);
    connect(ui->doubleSpinBoxTilesMarginLeft, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &VPMainWindow::on_TilesMarginChanged);
}


//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitPropertyTabLayout()
{

    // FIXME ---- For MVP we hide a few things. To be displayed when functions there
    ui->groupBoxLayoutControl->hide();

    // -------------------- init the unit combobox ---------------------
   ui->comboBoxLayoutUnit->addItem(tr("Centimeters"), QVariant(UnitsToStr(Unit::Cm)));
   ui->comboBoxLayoutUnit->addItem(tr("Millimiters"), QVariant(UnitsToStr(Unit::Mm)));
   ui->comboBoxLayoutUnit->addItem(tr("Inches"), QVariant(UnitsToStr(Unit::Inch)));

   // set default unit - TODO when we have the setting for the unit
//    const qint32 indexUnit = -1;//ui->comboBoxLayoutUnit->findData(qApp->ValentinaSettings()->GetUnit());
//    if (indexUnit != -1)
//    {
//        ui->comboBoxLayoutUnit->setCurrentIndex(indexUnit);
//    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitCarrousel()
{
    m_carrousel = new VPCarrousel(m_layout, ui->dockWidgetCarrousel);
    ui->dockWidgetCarrousel->setWidget(m_carrousel);

    connect(ui->dockWidgetCarrousel, QOverload<Qt::DockWidgetArea>::of(&QDockWidget::dockLocationChanged), this,
              &VPMainWindow::on_CarrouselLocationChanged);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::SetPropertiesData()
{
    if(m_layout == nullptr)
    {
       // TODO : hide the tabs when there is no layout
    }
    else
    {
        SetPropertyTabCurrentPieceData();
        SetPropertyTabSheetData();
        SetPropertyTabTilesData();
        SetPropertyTabLayoutData();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::SetPropertyTabCurrentPieceData()
{
    if(m_selectedPieces.count() == 0)
    {
        // show the content "no piece selected"

         ui->containerCurrentPieceNoData->setVisible(true);
         ui->containerCurrentPieceData->setVisible(false);
         ui->containerCurrentPieceMultipleData->setVisible(false);
    }
    else if(m_selectedPieces.count() == 1)
    {
        // show the content "selected piece data"
        ui->containerCurrentPieceNoData->setVisible(false);
        ui->containerCurrentPieceData->setVisible(true);
        ui->containerCurrentPieceMultipleData->setVisible(false);

        VPPiece *selectedPiece = m_selectedPieces.first();

        // set the value to the current piece
        ui->lineEditCurrentPieceName->setText(selectedPiece->GetName());
        ui->plainTextEditCurrentPieceUUID->setPlainText(selectedPiece->GetUUID().toString());

//        ui->checkBoxCurrentPieceShowSeamline->setChecked(selectedPiece->GetShowSeamLine());
        ui->checkBoxCurrentPieceMirrorPiece->setChecked(selectedPiece->IsMirror());

        QPointF pos = selectedPiece->GetPosition();
        SetDoubleSpinBoxValue(ui->doubleSpinBoxCurrentPieceBoxPositionX,
                              UnitConvertor(pos.x(), Unit::Px, m_layout->LayoutSettings().GetUnit()));
        SetDoubleSpinBoxValue(ui->doubleSpinBoxCurrentPieceBoxPositionY,
                              UnitConvertor(pos.y(), Unit::Px, m_layout->LayoutSettings().GetUnit()));

//        qreal angle = selectedPiece->GetRotation();
//        SetDoubleSpinBoxValue(ui->doubleSpinBoxCurrentPieceAngle, angle);
    }
    else
    {
        // show the content "multiple pieces selected"

        ui->containerCurrentPieceNoData->setVisible(false);
        ui->containerCurrentPieceData->setVisible(false);
        ui->containerCurrentPieceMultipleData->setVisible(true);

        // if needed in the future, we can show some properties that coul be edited for all the pieces
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::SetPropertyTabSheetData()
{
    // set name // TODO FIXME make it better
    ui->lineEditSheetName->setText(m_layout->GetFocusedSheet()->GetName());

    // set Width / Length
    QSizeF size = m_layout->LayoutSettings().GetSheetSizeConverted();
    SetDoubleSpinBoxValue(ui->doubleSpinBoxSheetWidth, size.width());
    SetDoubleSpinBoxValue(ui->doubleSpinBoxSheetLength, size.height());

    // Set Orientation
    if(m_layout->LayoutSettings().GetOrientation() == PageOrientation::Portrait)
    {
        ui->radioButtonSheetPortrait->setChecked(true);
    }
    else
    {
        ui->radioButtonSheetLandscape->setChecked(true);
    }

    // set margins
    QMarginsF margins = m_layout->LayoutSettings().GetSheetMarginsConverted();
    SetDoubleSpinBoxValue(ui->doubleSpinBoxSheetMarginLeft, margins.left());
    SetDoubleSpinBoxValue(ui->doubleSpinBoxSheetMarginTop, margins.top());
    SetDoubleSpinBoxValue(ui->doubleSpinBoxSheetMarginRight, margins.right());
    SetDoubleSpinBoxValue(ui->doubleSpinBoxSheetMarginBottom, margins.bottom());

    // set placement grid
    SetDoubleSpinBoxValue(ui->doubleSpinBoxSheetGridColWidth, m_layout->LayoutSettings().GetGridColWidthConverted());
    SetDoubleSpinBoxValue(ui->doubleSpinBoxSheetGridRowHeight, m_layout->LayoutSettings().GetGridRowHeightConverted());
    SetCheckBoxValue(ui->checkBoxSheetShowGrid, m_layout->LayoutSettings().GetShowGrid());

    // set pieces gap
    SetDoubleSpinBoxValue(ui->doubleSpinBoxSheetPiecesGap, m_layout->LayoutSettings().GetPiecesGapConverted());

    // set the checkboxes
    SetCheckBoxValue(ui->checkBoxSheetStickyEdges, m_layout->LayoutSettings().GetStickyEdges());
}


//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::SetPropertyTabTilesData()
{
    // set Width / Length
    QSizeF size = m_layout->LayoutSettings().GetTilesSizeConverted();
    SetDoubleSpinBoxValue(ui->doubleSpinBoxTilesWidth, size.width());
    SetDoubleSpinBoxValue(ui->doubleSpinBoxTilesLength, size.height());

    // Set Orientation
    if(m_layout->LayoutSettings().GetTilesOrientation() == PageOrientation::Portrait)
    {
        ui->radioButtonSheetPortrait->setChecked(true);
    }
    else
    {
        ui->radioButtonSheetLandscape->setChecked(true);
    }

    // set margins
    QMarginsF margins = m_layout->LayoutSettings().GetTilesMarginsConverted();
    SetDoubleSpinBoxValue(ui->doubleSpinBoxTilesMarginLeft, margins.left());
    SetDoubleSpinBoxValue(ui->doubleSpinBoxTilesMarginTop, margins.top());
    SetDoubleSpinBoxValue(ui->doubleSpinBoxTilesMarginRight, margins.right());
    SetDoubleSpinBoxValue(ui->doubleSpinBoxTilesMarginBottom, margins.bottom());

    // set "show tiles" checkbox
    SetCheckBoxValue(ui->checkBoxTilesShowTiles, m_layout->LayoutSettings().GetShowTiles());
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::SetPropertyTabLayoutData()
{
    // set the title and description
    ui->lineEditLayoutName->setText(m_layout->LayoutSettings().GetTitle());
    ui->plainTextEditLayoutDescription->setPlainText(m_layout->LayoutSettings().GetDescription());

    // set Unit
    int index = ui->comboBoxLayoutUnit->findData(QVariant(UnitsToStr(m_layout->LayoutSettings().GetUnit())));
    if(index != -1)
    {
        ui->comboBoxLayoutUnit->blockSignals(true); // FIXME: is there a better way to block the signals?
        ui->comboBoxLayoutUnit->setCurrentIndex(index);
        ui->comboBoxLayoutUnit->blockSignals(false);
    }

    // set controls
    SetCheckBoxValue(ui->checkBoxLayoutWarningPiecesOutOfBound,
                     m_layout->LayoutSettings().GetWarningPiecesOutOfBound());
    SetCheckBoxValue(ui->checkBoxLayoutWarningPiecesSuperposition,
                     m_layout->LayoutSettings().GetWarningSuperpositionOfPieces());
}



//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitMainGraphics()
{
    m_graphicsView = new VPMainGraphicsView(m_layout, m_tileFactory, this);
    m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->centralWidget->layout()->addWidget(m_graphicsView);

    m_graphicsView->RefreshLayout();

    connect(m_graphicsView, &VPMainGraphicsView::ScaleChanged, this, &VPMainWindow::on_ScaleChanged);
    connect(m_graphicsView->GetScene(), &VMainGraphicsScene::mouseMove, this, &VPMainWindow::on_MouseMoved);
    connect(m_carrousel, &VPCarrousel::on_ActiveSheetChanged, m_graphicsView, &VPMainGraphicsView::RefreshPieces);
    connect(m_graphicsView, &VPMainGraphicsView::on_SheetRemoved, m_carrousel, &VPCarrousel::Refresh);
    connect(m_layout, &VPLayout::PieceSheetChanged, m_carrousel, &VPCarrousel::Refresh);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitZoomToolBar()
{
    if (not m_doubleSpinBoxScale.isNull())
    {
        delete m_doubleSpinBoxScale;
    }

    delete m_mouseCoordinate;

    // connect the zoom buttons and shortcuts to the slots
    QList<QKeySequence> zoomInShortcuts;
    zoomInShortcuts.append(QKeySequence(QKeySequence::ZoomIn));
    zoomInShortcuts.append(QKeySequence(Qt::ControlModifier + Qt::Key_Plus + Qt::KeypadModifier));
    ui->actionZoomIn->setShortcuts(zoomInShortcuts);
    connect(ui->actionZoomIn, &QAction::triggered, m_graphicsView, &VPMainGraphicsView::ZoomIn);

    QList<QKeySequence> zoomOutShortcuts;
    zoomOutShortcuts.append(QKeySequence(QKeySequence::ZoomOut));
    zoomOutShortcuts.append(QKeySequence(Qt::ControlModifier + Qt::Key_Minus + Qt::KeypadModifier));
    ui->actionZoomOut->setShortcuts(zoomOutShortcuts);
    connect(ui->actionZoomOut, &QAction::triggered, m_graphicsView, &VPMainGraphicsView::ZoomOut);

    QList<QKeySequence> zoomOriginalShortcuts;
    zoomOriginalShortcuts.append(QKeySequence(Qt::ControlModifier + Qt::Key_0));
    zoomOriginalShortcuts.append(QKeySequence(Qt::ControlModifier + Qt::Key_0 + Qt::KeypadModifier));
    ui->actionZoomOriginal->setShortcuts(zoomOriginalShortcuts);
    connect(ui->actionZoomOriginal, &QAction::triggered, m_graphicsView, &VPMainGraphicsView::ZoomOriginal);

    QList<QKeySequence> zoomFitBestShortcuts;
    zoomFitBestShortcuts.append(QKeySequence(Qt::ControlModifier + Qt::Key_Equal));
    ui->actionZoomFitBest->setShortcuts(zoomFitBestShortcuts);
    connect(ui->actionZoomFitBest, &QAction::triggered, m_graphicsView, &VPMainGraphicsView::ZoomFitBest);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::InitScaleToolBar()
{
    auto* zoomScale = new QLabel(tr("Scale:"), this);
    ui->toolBarScale->addWidget(zoomScale);

    m_doubleSpinBoxScale = new QDoubleSpinBox(this);
    m_doubleSpinBoxScale->setDecimals(1);
    m_doubleSpinBoxScale->setSuffix(QChar('%'));
    on_ScaleChanged(m_graphicsView->transform().m11());
    connect(m_doubleSpinBoxScale.data(), QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, [this](double d){m_graphicsView->Zoom(d/100.0);});
    ui->toolBarScale->addWidget(m_doubleSpinBoxScale);


    // define the mouse position
    ui->toolBarScale->addSeparator();

    m_mouseCoordinate = new QLabel(QStringLiteral("0, 0 (%1)")
                                   .arg(UnitsToStr(m_layout->LayoutSettings().GetUnit(), true)));
    ui->toolBarScale->addWidget(m_mouseCoordinate);
    ui->toolBarScale->addSeparator();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::SetDoubleSpinBoxValue(QDoubleSpinBox *spinBox, qreal value)
{
    spinBox->blockSignals(true);
    spinBox->setValue(value);
    spinBox->blockSignals(false);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::SetCheckBoxValue(QCheckBox *checkbox, bool value)
{
    checkbox->blockSignals(true);
    checkbox->setChecked(value);
    checkbox->blockSignals(false);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::UpdateWindowTitle()
{
    QString showName;
    if (not curFile.isEmpty())
    {
        showName = StrippedName(curFile);
    }
    else
    {
        int index = VPApplication::VApp()->MainWindows().indexOf(this);
        if (index != -1)
        {
            showName = tr("untitled %1.vlt").arg(index+1);
        }
        else
        {
            showName = tr("untitled.vlt");
        }
    }

    showName += QLatin1String("[*]");

    if (IsLayoutReadOnly())
    {
        showName += QStringLiteral(" (") + tr("read only") + QChar(')');
    }

    setWindowTitle(showName);
    setWindowFilePath(curFile);

#if defined(Q_OS_MAC)
    static QIcon fileIcon = QIcon(QCoreApplication::applicationDirPath() +
                                  QLatin1String("/../Resources/layout.icns"));
    QIcon icon;
    if (not curFile.isEmpty())
    {
        if (not isWindowModified())
        {
            icon = fileIcon;
        }
        else
        {
            static QIcon darkIcon;

            if (darkIcon.isNull())
            {
                darkIcon = QIcon(darkenPixmap(fileIcon.pixmap(16, 16)));
            }
            icon = darkIcon;
        }
    }
    setWindowIcon(icon);
#endif //defined(Q_OS_MAC)
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::ReadSettings()
{
    qCDebug(pWindow, "Reading settings.");
    const VPSettings *settings = VPApplication::VApp()->PuzzleSettings();

    if (settings->status() == QSettings::NoError)
    {
        restoreGeometry(settings->GetGeometry());
        restoreState(settings->GetWindowState());
        restoreState(settings->GetToolbarsState(), APP_VERSION);

        // Text under tool buton icon
        ToolBarStyles();

        ui->dockWidgetProperties->setVisible(settings->IsDockWidgetPropertiesActive());
        ui->dockWidgetPropertiesContents->setVisible(settings->IsDockWidgetPropertiesContentsActive());

        // Scene antialiasing
        m_graphicsView->SetAntialiasing(settings->GetGraphicalOutput());

        // Stack limit
//        qApp->getUndoStack()->setUndoLimit(settings->GetUndoCount());
    }
    else
    {
        qWarning() << tr("Cannot read settings from a malformed .INI file.");
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::WriteSettings()
{
    VPSettings *settings = VPApplication::VApp()->PuzzleSettings();
    settings->SetGeometry(saveGeometry());
    settings->SetWindowState(saveState());
    settings->SetToolbarsState(saveState(APP_VERSION));

    settings->SetDockWidgetPropertiesActive(ui->dockWidgetProperties->isEnabled());
    settings->SetDockWidgetPropertiesContentsActive(ui->dockWidgetPropertiesContents->isEnabled());

    settings->sync();
    if (settings->status() == QSettings::AccessError)
    {
        qWarning() << tr("Cannot save settings. Access denied.");
    }
}

//---------------------------------------------------------------------------------------------------------------------
auto VPMainWindow::MaybeSave() -> bool
{
    // TODO: Implement maybe save check
    if (this->isWindowModified())
    {
        QScopedPointer<QMessageBox> messageBox(new QMessageBox(tr("Unsaved changes"),
                                                               tr("Measurements have been modified.\n"
                                                                  "Do you want to save your changes?"),
                                                               QMessageBox::Warning, QMessageBox::Yes, QMessageBox::No,
                                                               QMessageBox::Cancel, this, Qt::Sheet));

        messageBox->setDefaultButton(QMessageBox::Yes);
        messageBox->setEscapeButton(QMessageBox::Cancel);

        messageBox->setButtonText(QMessageBox::Yes, curFile.isEmpty() || IsLayoutReadOnly() ? tr("Save…") : tr("Save"));
        messageBox->setButtonText(QMessageBox::No, tr("Don't Save"));

        messageBox->setWindowModality(Qt::ApplicationModal);
        const auto ret = static_cast<QMessageBox::StandardButton>(messageBox->exec());

        switch (ret)
        {
            case QMessageBox::Yes:
                if (IsLayoutReadOnly())
                {
                    return on_actionSaveAs_triggered();
                }
                return on_actionSave_triggered();
            case QMessageBox::No:
                return true;
            case QMessageBox::Cancel:
                return false;
            default:
                break;
        }
    }
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::generateTiledPdf(QString fileName)
{
    if(not fileName.isEmpty())
    {
        m_graphicsView->PrepareForExport();
        m_tileFactory->refreshTileInfos();

        PageOrientation tilesOrientation = m_layout->LayoutSettings().GetTilesOrientation();

        // -------------  Set up the printer
        QScopedPointer<QPrinter> printer(new QPrinter());

        printer->setCreator(QGuiApplication::applicationDisplayName()+QChar(QChar::Space)+
                            QCoreApplication::applicationVersion());
        printer->setPageOrientation(QPageLayout::Portrait); // in the pdf file the pages should always be in portrait

        // here we might need to so some rounding for the size.
        printer->setPageSize(QPageSize(m_layout->LayoutSettings().GetTilesSize(Unit::Mm),
                                                           QPageSize::Millimeter));
        printer->setFullPage(true);

        #ifdef Q_OS_MAC
        printer->setOutputFormat(QPrinter::NativeFormat);
        #else
        printer->setOutputFormat(QPrinter::PdfFormat);
        #endif

        printer->setOutputFileName(fileName);
        printer->setResolution(static_cast<int>(PrintDPI));
        printer->setDocName(m_layout->GetFocusedSheet()->GetName());

        // -------------  Set up the painter
        QPainter painter;
        if (not painter.begin(printer.data()))
        { // failed to open file
            qCritical() << tr("Failed to open file, is it writable?");
            return;
        }
        painter.setFont( QFont( QStringLiteral("Arial"), 8, QFont::Normal ) );
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setBrush ( QBrush ( Qt::NoBrush ) );

        if(tilesOrientation == PageOrientation::Landscape)
        {
            // The landscape tiles have to be rotated, because the pages
            // stay portrait in the pdf
            painter.rotate(90);
            painter.translate(0, -ToPixel(printer->pageRect(QPrinter::Millimeter).width(), Unit::Mm));
        }

        for(int row=0;row<m_tileFactory->getRowNb();row++)  // for each row of the tiling grid
        {
            for(int col=0;col<m_tileFactory->getColNb();col++) // for each column of tiling grid
            {
                if(not (row == 0 && col == 0))
                {
                    if (not printer->newPage())
                    {
                        qWarning("failed in flushing page to disk, disk full?");
                        return;
                    }
                }

                m_tileFactory->drawTile(&painter, m_graphicsView, row, col);
            }
        }

        painter.end();

        m_graphicsView->CleanAfterExport();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::CreateWindowMenu(QMenu *menu)
{
    SCASSERT(menu != nullptr)

    QAction *action = menu->addAction(tr("&New Window"));
    connect(action, &QAction::triggered, this, []()
    {
        VPApplication::VApp()->NewMainWindow()->activateWindow();
    });
    action->setMenuRole(QAction::NoRole);
    menu->addSeparator();

    const QList<VPMainWindow*> windows = VPApplication::VApp()->MainWindows();
    for (int i = 0; i < windows.count(); ++i)
    {
        VPMainWindow *window = windows.at(i);

        QString title = QStringLiteral("%1. %2").arg(i+1).arg(window->windowTitle());
        const int index = title.lastIndexOf(QLatin1String("[*]"));
        if (index != -1)
        {
            window->isWindowModified() ? title.replace(index, 3, QChar('*')) : title.replace(index, 3, QString());
        }

#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        QAction *action = menu->addAction(title, this, SLOT(ShowWindow()));
#else
        QAction *action = menu->addAction(title, this, &VPMainWindow::ShowWindow);
#endif //QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
        action->setData(i);
        action->setCheckable(true);
        action->setMenuRole(QAction::NoRole);
        if (window->isActiveWindow())
        {
            action->setChecked(true);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::AddSheet()
{
    auto *sheet = new VPSheet(m_layout);
    sheet->SetName(QObject::tr("Sheet %1").arg(m_layout->GetSheets().size()+1));
    m_layout->AddSheet(sheet);
    m_layout->SetFocusedSheet(sheet);

//    // ----- for test purposes, to be removed------------------
    m_layout->LayoutSettings().SetSheetMarginsConverted(1, 1, 1, 1);
    m_layout->LayoutSettings().SetSheetSizeConverted(84.1, 118.9);
    m_layout->LayoutSettings().SetPiecesGapConverted(1);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPMainWindow::IsLayoutReadOnly() const -> bool
{
    if (curFile.isEmpty())
    {
        return false;
    }

    QFileInfo f(curFile);

    if (not f.exists())
    {
        return false;
    }

#ifdef Q_OS_WIN32
    qt_ntfs_permission_lookup++; // turn checking on
#endif /*Q_OS_WIN32*/

    bool fileWritable = f.isWritable();

#ifdef Q_OS_WIN32
    qt_ntfs_permission_lookup--; // turn it off again
#endif /*Q_OS_WIN32*/

    return not fileWritable;
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_actionNew_triggered()
{
    VPApplication::VApp()->NewMainWindow();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::ShowToolTip(const QString &toolTip)
{
    m_statusLabel->setText(toolTip);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::closeEvent(QCloseEvent *event)
{
#if defined(Q_OS_MAC) && QT_VERSION < QT_VERSION_CHECK(5, 11, 1)
    // Workaround for Qt bug https://bugreports.qt.io/browse/QTBUG-43344
    static int numCalled = 0;
    if (numCalled++ >= 1)
    {
        return;
    }
#endif

    if (MaybeSave())
    {
        WriteSettings();
        event->accept();
        deleteLater();
    }
    else
    {
        event->ignore();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        // retranslate designer form (single inheritance approach)
        ui->retranslateUi(this);

        WindowsLocale();
        UpdateWindowTitle();
    }

    // remember to call base class implementation
    QMainWindow::changeEvent(event);
}

//---------------------------------------------------------------------------------------------------------------------
QStringList VPMainWindow::RecentFileList() const
{
    return VPApplication::VApp()->PuzzleSettings()->GetRecentFileList();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_actionOpen_triggered()
{
    qCDebug(pWindow, "Openning puzzle layout file.");

    const QString filter(tr("Layout files") + QLatin1String(" (*.vlt)"));
    //Use standard path to individual measurements
    const QString pathTo = VPApplication::VApp()->PuzzleSettings()->GetPathManualLayouts();

    bool usedNotExistedDir = false;
    QDir directory(pathTo);
    if (not directory.exists())
    {
        usedNotExistedDir = directory.mkpath(QChar('.'));
    }

    const QString mPath = QFileDialog::getOpenFileName(this, tr("Open file"), pathTo, filter, nullptr,
                                                       VAbstractApplication::VApp()->NativeFileDialog());

    if (not mPath.isEmpty())
    {
        VPApplication::VApp()->NewMainWindow()->LoadFile(mPath);
    }

    if (usedNotExistedDir)
    {
        QDir(pathTo).rmpath(QChar('.'));
    }
}

//---------------------------------------------------------------------------------------------------------------------
bool VPMainWindow::on_actionSave_triggered()
{
    if (curFile.isEmpty() || IsLayoutReadOnly())
    {
        return on_actionSaveAs_triggered();
    }

    if (m_curFileFormatVersion < VLayoutConverter::LayoutMaxVer
            && not ContinueFormatRewrite(m_curFileFormatVersionStr, VLayoutConverter::LayoutMaxVerStr))
    {
        return false;
    }

    if (not CheckFilePermissions(curFile, this))
    {
        return false;
    }

    QString error;
    if (not SaveLayout(curFile, error))
    {
        QMessageBox messageBox;
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setText(tr("Could not save the file"));
        messageBox.setDefaultButton(QMessageBox::Ok);
        messageBox.setDetailedText(error);
        messageBox.setStandardButtons(QMessageBox::Ok);
        messageBox.exec();
        return false;
    }

    m_curFileFormatVersion = VLayoutConverter::LayoutMaxVer;
    m_curFileFormatVersionStr = VLayoutConverter::LayoutMaxVerStr;

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
bool VPMainWindow::on_actionSaveAs_triggered()
{
    QString filters = tr("Layout files") + QStringLiteral(" (*.vlt)");
    QString suffix = QStringLiteral("vlt");
    QString fName = tr("layout") + QChar('.') + suffix;

    QString dir;
    if (curFile.isEmpty())
    {
        dir = VPApplication::VApp()->PuzzleSettings()->GetPathManualLayouts();
    }
    else
    {
        dir = QFileInfo(curFile).absolutePath();
    }

    bool usedNotExistedDir = false;
    QDir directory(dir);
    if (not directory.exists())
    {
        usedNotExistedDir = directory.mkpath(QChar('.'));
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"), dir + QChar('/') + fName, filters, nullptr,
                                                    VAbstractApplication::VApp()->NativeFileDialog());

    auto RemoveTempDir = qScopeGuard([usedNotExistedDir, dir]()
    {
        if (usedNotExistedDir)
        {
            QDir(dir).rmpath(QChar('.'));
        }
    });

    if (fileName.isEmpty())
    {
        return false;
    }

    QFileInfo f( fileName );
    if (f.suffix().isEmpty() && f.suffix() != suffix)
    {
        fileName += QChar('.') + suffix;
    }

    if (not CheckFilePermissions(fileName, this))
    {
        return false;
    }

    if (QFileInfo::exists(fileName) && curFile != fileName)
    {
        // Temporary try to lock the file before saving
        VLockGuard<char> tmp(fileName);
        if (not tmp.IsLocked())
        {
            qCCritical(pWindow, "%s",
                       qUtf8Printable(tr("Failed to lock. This file already opened in another window.")));
            return false;
        }
    }

    QString error;
    bool result = SaveLayout(fileName, error);
    if (not result)
    {
        QMessageBox messageBox;
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setInformativeText(tr("Could not save file"));
        messageBox.setDefaultButton(QMessageBox::Ok);
        messageBox.setDetailedText(error);
        messageBox.setStandardButtons(QMessageBox::Ok);
        messageBox.exec();
        return false;
    }

    m_curFileFormatVersion = VLayoutConverter::LayoutMaxVer;
    m_curFileFormatVersionStr = VLayoutConverter::LayoutMaxVerStr;

    UpdateWindowTitle();

    if (curFile == fileName && not lock.isNull())
    {
        lock->Unlock();
    }
    VlpCreateLock(lock, fileName);
    if (not lock->IsLocked())
    {
        qCCritical(pWindow, "%s", qUtf8Printable(tr("Failed to lock. This file already opened in another window. "
                                                    "Expect collissions when run 2 copies of the program.")));
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_actionImportRawLayout_triggered()
{
    const QString filter(tr("Raw Layout files") + QLatin1String(" (*.rld)"));

    const QString filePath = QFileDialog::getOpenFileName(this, tr("Open file"), QDir::homePath(), filter, nullptr,
                                                          VAbstractApplication::VApp()->NativeFileDialog());

    if (not filePath.isEmpty())
    {
        ImportRawLayouts({filePath});
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_actionAboutPuzzle_triggered()
{
    auto *aboutDialog = new VPDialogAbout(this);
    aboutDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    aboutDialog->show();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_comboBoxLayoutUnit_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    QVariant comboBoxValue = ui->comboBoxLayoutUnit->currentData();

    if(comboBoxValue == QVariant(UnitsToStr(Unit::Cm)))
    {
        m_layout->LayoutSettings().SetUnit(Unit::Cm);
    }
    else if(comboBoxValue == QVariant(UnitsToStr(Unit::Mm)))
    {
        m_layout->LayoutSettings().SetUnit(Unit::Mm);
    }
    else if(comboBoxValue == QVariant(UnitsToStr(Unit::Inch)))
    {
        m_layout->LayoutSettings().SetUnit(Unit::Inch);
    }

    SetPropertyTabCurrentPieceData();
    SetPropertyTabSheetData();
    SetPropertyTabTilesData();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_lineEditSheetName_textChanged(const QString &text)
{
    m_layout->GetFocusedSheet()->SetName(text);

    if(m_carrousel != nullptr)
    {
        m_carrousel->RefreshSheetNames();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_comboBoxSheetTemplate_currentIndexChanged(int index)
{
    PaperSizeTemplate tmpl = static_cast<PaperSizeTemplate>(
                ui->comboBoxSheetTemplate->itemData(index).toInt()
                );

    QSizeF tmplSize = VPLayoutSettings::GetTemplateSize(tmpl);
    if(!tmplSize.isEmpty())
    {
        ui->doubleSpinBoxSheetWidth->blockSignals(true);
        ui->doubleSpinBoxSheetLength->blockSignals(true);

        ui->doubleSpinBoxSheetWidth->setValue(UnitConvertor(tmplSize.width(), Unit::Px,
                                                            m_layout->LayoutSettings().GetUnit()));
        ui->doubleSpinBoxSheetLength->setValue(UnitConvertor(tmplSize.height(), Unit::Px,
                                                             m_layout->LayoutSettings().GetUnit()));

        on_SheetSizeChanged(false);

        ui->doubleSpinBoxSheetWidth->blockSignals(false);
        ui->doubleSpinBoxSheetLength->blockSignals(false);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_SheetSizeChanged(bool changedViaSizeCombobox)
{
    m_layout->LayoutSettings().SetSheetSizeConverted(
                ui->doubleSpinBoxSheetWidth->value(),
                ui->doubleSpinBoxSheetLength->value()
                );

    if(changedViaSizeCombobox)
    {
        ui->comboBoxSheetTemplate->blockSignals(true);

        // we don't try to get the right size, because it doesn't work well because of mm / inch conversion
        int index = ui->comboBoxSheetTemplate->findData(
                    QVariant(static_cast<int>(PaperSizeTemplate::Custom)));

        ui->comboBoxSheetTemplate->setCurrentIndex(index);
        ui->comboBoxSheetTemplate->blockSignals(false);
    }

    m_tileFactory->refreshTileInfos();

    // TODO Undo / Redo

    m_graphicsView->RefreshLayout();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_SheetOrientationChanged()
{
    // Updates the orientation
    if(ui->radioButtonSheetPortrait->isChecked())
    {
        m_layout->LayoutSettings().SetOrientation(PageOrientation::Portrait);
    }
    else
    {
        m_layout->LayoutSettings().SetOrientation(PageOrientation::Landscape);
    }
    m_tileFactory->refreshTileInfos();

    // TODO Undo / Redo

    m_graphicsView->RefreshLayout();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_pushButtonSheetRemoveUnusedLength_clicked()
{
    // just for test purpuses, to be removed:
    QMessageBox msgBox;
    msgBox.setText("TODO VPMainWindow::on_pushButtonSheetRemoveUnusedLength_clicked");
    int ret = msgBox.exec();

    Q_UNUSED(ret);

    // TODO
}


//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_SheetMarginChanged()
{
    m_layout->LayoutSettings().SetSheetMarginsConverted(
                ui->doubleSpinBoxSheetMarginLeft->value(),
                ui->doubleSpinBoxSheetMarginTop->value(),
                ui->doubleSpinBoxSheetMarginRight->value(),
                ui->doubleSpinBoxSheetMarginBottom->value()
            );

    // TODO Undo / Redo

    m_graphicsView->RefreshLayout();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_checkBoxSheetShowGrid_toggled(bool checked)
{
    m_layout->LayoutSettings().SetShowGrid(checked);
    m_graphicsView->RefreshLayout();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_doubleSpinBoxSheetGridColWidth_valueChanged(double value)
{
    m_layout->LayoutSettings().SetGridColWidthConverted(value);
    m_graphicsView->RefreshLayout();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_doubleSpinBoxSheetGridRowHeight_valueChanged(double value)
{
    m_layout->LayoutSettings().SetGridRowHeightConverted(value);
    m_graphicsView->RefreshLayout();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_comboBoxTilesTemplate_currentIndexChanged(int index)
{
    PaperSizeTemplate tmpl = static_cast<PaperSizeTemplate>(
                ui->comboBoxTilesTemplate->itemData(index).toInt()
                );

    QSizeF tmplSize = VPLayoutSettings::GetTemplateSize(tmpl);
    if(!tmplSize.isEmpty())
    {
        ui->doubleSpinBoxTilesWidth->blockSignals(true);
        ui->doubleSpinBoxTilesLength->blockSignals(true);

        ui->doubleSpinBoxTilesWidth->setValue(UnitConvertor(tmplSize.width(), Unit::Px,
                                                            m_layout->LayoutSettings().GetUnit()));
        ui->doubleSpinBoxTilesLength->setValue(UnitConvertor(tmplSize.height(), Unit::Px,
                                                             m_layout->LayoutSettings().GetUnit()));

        on_TilesSizeChanged(false);

        ui->doubleSpinBoxTilesWidth->blockSignals(false);
        ui->doubleSpinBoxTilesLength->blockSignals(false);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_TilesSizeChanged(bool changedViaSizeCombobox)
{
    m_layout->LayoutSettings().SetTilesSizeConverted(ui->doubleSpinBoxTilesWidth->value(),
                                                     ui->doubleSpinBoxTilesLength->value());
    m_tileFactory->refreshTileInfos();

    if(changedViaSizeCombobox)
    {
        ui->comboBoxTilesTemplate->blockSignals(true);

        // we don't try to get the right size, because it doesn't work well because of mm / inch conversion
        int index = ui->comboBoxTilesTemplate->findData(
                    QVariant(static_cast<int>(PaperSizeTemplate::Custom)));

        ui->comboBoxTilesTemplate->setCurrentIndex(index);
        ui->comboBoxTilesTemplate->blockSignals(false);
    }

    // TODO Undo / Redo

    if(m_graphicsView != nullptr)
    {
        m_graphicsView->RefreshLayout();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_TilesOrientationChanged()
{
    // Updates the orientation
    if(ui->radioButtonTilesPortrait->isChecked())
    {
        m_layout->LayoutSettings().SetTilesOrientation(PageOrientation::Portrait);
    }
    else
    {
        m_layout->LayoutSettings().SetTilesOrientation(PageOrientation::Landscape);
    }
    m_tileFactory->refreshTileInfos();

    // TODO Undo / Redo

    m_graphicsView->RefreshLayout();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_TilesMarginChanged()
{
    m_layout->LayoutSettings().SetTilesMarginsConverted(
                ui->doubleSpinBoxTilesMarginLeft->value(),
                ui->doubleSpinBoxTilesMarginTop->value(),
                ui->doubleSpinBoxTilesMarginRight->value(),
                ui->doubleSpinBoxTilesMarginBottom->value()
            );
    m_tileFactory->refreshTileInfos();

    // TODO Undo / Redo

    m_graphicsView->RefreshLayout();
}


//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_checkBoxTilesShowTiles_toggled(bool checked)
{
    m_layout->LayoutSettings().SetShowTiles(checked);

    // TODO Undo / Redo

    m_graphicsView->RefreshLayout();
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_pushButtonTilesExport_clicked()
{
    QString dir = QDir::homePath();
    QString filters(tr("PDF Files") + QLatin1String("(*.pdf)"));
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save as"),
                                                    dir + QLatin1String("/") + m_layout->GetFocusedSheet()->GetName() + QLatin1String(".pdf"),
                                                    filters, nullptr
#ifdef Q_OS_LINUX
                                                    , QFileDialog::DontUseNativeDialog
#endif
                                                    );

    generateTiledPdf(fileName);
}



//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_SheetFollowGrainlineChanged()
{
    // just for test purpuses, to be removed:
    QMessageBox msgBox;
    msgBox.setText("TODO VPMainWindow::on_SheetFollowGrainlineChanged");
    int ret = msgBox.exec();

    Q_UNUSED(ret);

    // TODO
}


//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_doubleSpinBoxSheetPiecesGap_valueChanged(double value)
{
    m_layout->LayoutSettings().SetPiecesGapConverted(value);

    // TODO Undo / Redo
    // TODO update the QGraphicView
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_checkBoxLayoutWarningPiecesSuperposition_toggled(bool checked)
{
    m_layout->LayoutSettings().SetWarningSuperpositionOfPieces(checked);

    // TODO Undo / Redo
    // TODO update the QGraphicView
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_checkBoxLayoutWarningPiecesOutOfBound_toggled(bool checked)
{
    m_layout->LayoutSettings().SetWarningPiecesOutOfBound(checked);

    // TODO Undo / Redo
    // TODO update the QGraphicView
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_checkBoxSheetStickyEdges_toggled(bool checked)
{
    m_layout->LayoutSettings().SetStickyEdges(checked);

    // TODO Undo / Redo
    // TODO update the QGraphicView
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_pushButtonSheetExport_clicked()
{
    LayoutExportFormats format = static_cast<LayoutExportFormats>(ui->comboBoxSheetExportFormat->currentData().toInt());

    VPExporter exporter;
    exporter.Export(m_layout, format, m_graphicsView);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_checkBoxCurrentPieceShowSeamline_toggled(bool checked)
{
    if(m_selectedPieces.count() == 1)
    {
//        m_selectedPieces.first()->SetShowSeamLine(checked);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_checkBoxCurrentPieceMirrorPiece_toggled(bool checked)
{
    if(m_selectedPieces.count() == 1)
    {
        m_selectedPieces.first()->SetMirror(checked);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_pushButtonCurrentPieceRotate90Anticlockwise_clicked()
{
    if(m_selectedPieces.count() == 1)
    {
//         m_selectedPieces.first()->RotateBy(90);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_pushButtonCurrentPieceRotate90Clockwise_clicked()
{
    if(m_selectedPieces.count() == 1)
    {
//         m_selectedPieces.first()->RotateBy(-90);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_pushButtonCurrentPieceRotateGrainlineVertical_clicked()
{
    if(m_selectedPieces.count() == 1)
    {
//         m_selectedPieces.first()->RotateToGrainline(90, true);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_pushButtonCurrentPieceRotateGrainlineHorizontal_clicked()
{
    if(m_selectedPieces.count() == 1)
    {
//         m_selectedPieces.first()->RotateToGrainline(0, true);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_doubleSpinBoxCurrentPieceAngle_valueChanged(double value)
{
    if(m_selectedPieces.count() == 1)
    {
//        VPPiece *piece = m_selectedPieces.first();
//        piece->SetRotation(value);
    }
}


//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_CurrentPiecePositionEdited()
{
    if(m_selectedPieces.count() == 1)
    {
        VPPiece *piece = m_selectedPieces.first();
        QPointF pos(UnitConvertor(ui->doubleSpinBoxCurrentPieceBoxPositionX->value(),
                                  m_layout->LayoutSettings().GetUnit(), Unit::Px),
                    UnitConvertor(ui->doubleSpinBoxCurrentPieceBoxPositionY->value(),
                                  m_layout->LayoutSettings().GetUnit(), Unit::Px));
        piece->SetPosition(pos);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_CarrouselLocationChanged(Qt::DockWidgetArea area)
{
    if(area == Qt::BottomDockWidgetArea || area == Qt::TopDockWidgetArea)
    {
        m_carrousel->SetOrientation(Qt::Horizontal);
    }
    else if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea)
    {
        m_carrousel->SetOrientation(Qt::Vertical);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_PieceSelectionChanged()
{
//    m_selectedPieces = m_layout->GetSelectedPieces();

    // update the property of the piece currently selected
    SetPropertyTabCurrentPieceData();
}


//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_PiecePositionChanged()
{
    if(m_selectedPieces.count() == 1)
    {
        VPPiece *piece = m_selectedPieces.first();
        QPointF pos = piece->GetPosition();

        SetDoubleSpinBoxValue(ui->doubleSpinBoxCurrentPieceBoxPositionX,
                              UnitConvertor(pos.x(), Unit::Px, m_layout->LayoutSettings().GetUnit()));
        SetDoubleSpinBoxValue(ui->doubleSpinBoxCurrentPieceBoxPositionY,
                              UnitConvertor(pos.y(), Unit::Px, m_layout->LayoutSettings().GetUnit()));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_PieceRotationChanged()
{
    if(m_selectedPieces.count() == 1)
    {
//        VPPiece *piece = m_selectedPieces.first();
//        qreal angle = piece->GetRotation();

//        SetDoubleSpinBoxValue(ui->doubleSpinBoxCurrentPieceAngle, angle);
    }
}


//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_ScaleChanged(qreal scale)
{
    if (not m_doubleSpinBoxScale.isNull())
    {
        m_doubleSpinBoxScale->blockSignals(true);
        m_doubleSpinBoxScale->setMaximum(qFloor(VPMainGraphicsView::MaxScale()*1000)/10.0);
        m_doubleSpinBoxScale->setMinimum(qFloor(VPMainGraphicsView::MinScale()*1000)/10.0);
        m_doubleSpinBoxScale->setValue(qFloor(scale*1000)/10.0);
        m_doubleSpinBoxScale->setSingleStep(1);
        m_doubleSpinBoxScale->blockSignals(false);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_MouseMoved(const QPointF &scenePos)
{
    if (m_mouseCoordinate != nullptr)
    {
        m_mouseCoordinate->setText(QStringLiteral("%1, %2 (%3)")
                                   .arg(static_cast<qint32>(FromPixel(scenePos.x(),
                                                                      m_layout->LayoutSettings().GetUnit())))
                                   .arg(static_cast<qint32>(FromPixel(scenePos.y(),
                                                                      m_layout->LayoutSettings().GetUnit())))
                                   .arg(UnitsToStr(m_layout->LayoutSettings().GetUnit(), true)));
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::ShowWindow() const
{
    if (auto *action = qobject_cast<QAction*>(sender()))
    {
        const QVariant v = action->data();
        if (v.canConvert<int>())
        {
            const int offset = qvariant_cast<int>(v);
            const QList<VPMainWindow*> windows = VPApplication::VApp()->MainWindows();
            windows.at(offset)->raise();
            windows.at(offset)->activateWindow();
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_actionPreferences_triggered()
{
    // Calling constructor of the dialog take some time. Because of this user have time to call the dialog twice.
    static QPointer<DialogPuzzlePreferences> guard;// Prevent any second run
    if (guard.isNull())
    {
        QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        auto *preferences = new DialogPuzzlePreferences(this);
        // QScopedPointer needs to be sure any exception will never block guard
        QScopedPointer<DialogPuzzlePreferences> dlg(preferences);
        guard = preferences;
        // Must be first
        connect(dlg.data(), &DialogPuzzlePreferences::UpdateProperties, this, &VPMainWindow::WindowsLocale);
        connect(dlg.data(), &DialogPuzzlePreferences::UpdateProperties, this, &VPMainWindow::ToolBarStyles);
        QGuiApplication::restoreOverrideCursor();
        dlg->exec();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::ToolBarStyles()
{
    ToolBarStyle(ui->mainToolBar);
    ToolBarStyle(ui->toolBarZoom);
}

//---------------------------------------------------------------------------------------------------------------------
void VPMainWindow::on_actionAddSheet_triggered()
{
    AddSheet();
    m_carrousel->Refresh();
}

//---------------------------------------------------------------------------------------------------------------------
#if defined(Q_OS_MAC)
void VPMainWindow::AboutToShowDockMenu()
{
    if (QMenu *menu = qobject_cast<QMenu *>(sender()))
    {
        menu->clear();
        CreateWindowMenu(menu);

        menu->addSeparator();

        QAction *actionPreferences = menu->addAction(tr("Preferences"));
        actionPreferences->setMenuRole(QAction::NoRole);
        connect(actionPreferences, &QAction::triggered, this, &VPMainWindow::on_actionPreferences_triggered);
    }
}
#endif //defined(Q_OS_MAC)
