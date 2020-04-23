/************************************************************************
 **
 **  @file   vpuzzlelayoutfilereader.cpp
 **  @author Ronan Le Tiec
 **  @date   18 4, 2020
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
 ** *************************************************************************/

#include <QXmlStreamAttributes>
#include "vpuzzlelayoutfilereader.h"
#include "vpuzzlelayoutfilewriter.h"
#include "layoutliterals.h"

VPuzzleLayoutFileReader::VPuzzleLayoutFileReader()
{

}

//---------------------------------------------------------------------------------------------------------------------
VPuzzleLayoutFileReader::~VPuzzleLayoutFileReader()
{
    // TODO
}


//---------------------------------------------------------------------------------------------------------------------
bool VPuzzleLayoutFileReader::ReadFile(VPuzzleLayout *layout, QFile *file)
{
    setDevice(file);

    if (readNextStartElement())
    {

        // TODO extend the handling
        // if it doesn't start with layout, error
        // if it starts with version > than current version, error

        if (name() == ML::TagLayout)
        {
            QString versionStr = attributes().value(ML::AttrVersion).toString();
            QStringList versionStrParts = versionStr.split('.');
            m_layoutFormatVersion = FORMAT_VERSION(versionStrParts.at(0).toInt(),versionStrParts.at(1).toInt(),versionStrParts.at(2).toInt());

            if(VPuzzleLayoutFileWriter::LayoutFileFormatVersion >= m_layoutFormatVersion)
            {
                ReadLayout(layout);
            }
            else
            {
                // TODO better error handling
                raiseError(QObject::tr("You're trying to open a layout that was created with a newer version of puzzle"));
            }
        }
        else
        {
            raiseError(QObject::tr("Wrong file structure"));
        }
    }

    return !error();
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzleLayoutFileReader::ReadLayout(VPuzzleLayout *layout)
{
    Q_ASSERT(isStartElement() && name() == ML::TagLayout);

    while (readNextStartElement())
    {
        if (name() == ML::TagProperties)
        {
            ReadProperties(layout);
        }
        else if (name() == ML::TagLayers)
        {
            ReadLayers(layout);
        }
        else
        {
            // TODO error handling, we encountered a tag that isn't defined in the specification
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzleLayoutFileReader::ReadProperties(VPuzzleLayout *layout)
{
    Q_ASSERT(isStartElement() && name() == ML::TagProperties);

    while (readNextStartElement())
    {
        qDebug(name().toString().toLatin1());

        const QStringList tags = QStringList(
            {
                ML::TagUnit,
                ML::TagDescription,
                ML::TagSize,
                ML::TagMargin,
                ML::TagControl,
                ML::TagTiles
            });

        switch (tags.indexOf(name().toString()))
        {
        case 0:// unit
            qDebug("read unit");
            layout->SetUnit(StrToUnits(readElementText()));
            break;
        case 1:// description
        {
            qDebug("read description");
            QString description = readElementText();
            // TODO read the description info
            break;
        }
        case 2:// size
        {
            qDebug("read size");
            QSizeF size = ReadSize();
            layout->SetLayoutSize(size);
            readElementText();
            break;
        }
        case 3:// margin
        {
            qDebug("read margin");
            QMarginsF margins = ReadMargins();
            layout->SetLayoutMargins(margins);
            readElementText();
            break;
        }
        case 4:// control
        {
            qDebug("read control");
            QXmlStreamAttributes attribs = attributes();

            // attribs.value("followGrainLine"); // TODO

            layout->SetWarningSuperpositionOfPieces(attribs.value(ML::AttrWarningSuperposition) == "true");
            layout->SetWarningPiecesOutOfBound(attribs.value(ML::AttrWarningOutOfBound) == "true");
            layout->SetStickyEdges(attribs.value(ML::AttrStickyEdges) == "true");

            layout->SetPiecesGap(attribs.value(ML::AttrPiecesGap).toDouble());
            readElementText();
            break;
        }
        case 5:// tiles
            qDebug("read tiles");
            ReadTiles(layout);
            readElementText();
            break;
        default:
            // TODO error handling, we encountered a tag that isn't defined in the specification
            skipCurrentElement();
            break;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzleLayoutFileReader::ReadTiles(VPuzzleLayout *layout)
{
    Q_UNUSED(layout); // to be removed when used

    Q_ASSERT(isStartElement() && name() == ML::TagTiles);

//    QXmlStreamAttributes attribs = attributes();
    // attribs.value(ML::AttrVisible); // TODO
    // attribs.value(ML::AttrMatchingMarks); // TODO

    while (readNextStartElement())
    {
        if (name() == ML::TagSize)
        {
            QSizeF size = ReadSize();
            // TODO set layout tiled size
            Q_UNUSED(size);
            readElementText();
        }
        else if (name() == ML::TagMargin)
        {
            QMarginsF margins = ReadMargins();
            // TODO set layout tiled margins
            Q_UNUSED(margins);
            readElementText();
        }
        else
        {
            // TODO error handling, we encountered a tag that isn't defined in the specification
            skipCurrentElement();
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzleLayoutFileReader::ReadLayers(VPuzzleLayout *layout)
{
    Q_ASSERT(isStartElement() && name() == ML::TagLayers);

    while (readNextStartElement())
    {
        if (name() == ML::TagUnplacedPiecesLayer)
        {
            ReadLayer(layout->GetUnplacedPiecesLayer());
        }
        else if (name() == ML::TagLayer)
        {
            VPuzzleLayer *layer = layout->AddLayer();
            ReadLayer(layer);
        }
        else
        {
            // TODO error handling, we encountered a tag that isn't defined in the specification
            skipCurrentElement();
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzleLayoutFileReader::ReadLayer(VPuzzleLayer *layer)
{
    Q_ASSERT(isStartElement() && name() == ML::TagLayer);

    QXmlStreamAttributes attribs = attributes();
    layer->SetName(attribs.value(ML::AttrName).toString());
    layer->SetIsVisible(attribs.value(ML::AttrVisible) == "true");

    while (readNextStartElement())
    {
        if (name() == ML::TagPiece)
        {
            VPuzzlePiece *piece = new VPuzzlePiece();
            ReadPiece(piece);
            layer->AddPiece(piece);
        }
        else
        {
            // TODO error handling, we encountered a tag that isn't defined in the specification
            skipCurrentElement();
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzleLayoutFileReader::ReadPiece(VPuzzlePiece *piece)
{
    Q_UNUSED(piece);
    Q_ASSERT(isStartElement() && name() == ML::TagPiece);

    // TODO read the attributes

    while (readNextStartElement())
    {
        if (name() == QString("..."))
        {
            // TODO
             readElementText();
        }
        else
        {
            // TODO error handling, we encountered a tag that isn't defined in the specification
            skipCurrentElement();
        }
    }

}

//---------------------------------------------------------------------------------------------------------------------
QMarginsF VPuzzleLayoutFileReader::ReadMargins()
{
    QMarginsF margins = QMarginsF();

    QXmlStreamAttributes attribs = attributes();
    margins.setLeft(attribs.value(ML::AttrLeft).toDouble());
    margins.setTop(attribs.value(ML::AttrTop).toDouble());
    margins.setRight(attribs.value(ML::AttrRight).toDouble());
    margins.setBottom(attribs.value(ML::AttrBottom).toDouble());

    return margins;
}

//---------------------------------------------------------------------------------------------------------------------
QSizeF VPuzzleLayoutFileReader::ReadSize()
{
    QSizeF size = QSize();

    QXmlStreamAttributes attribs = attributes();
    size.setWidth(attribs.value(ML::AttrWidth).toDouble());
    size.setHeight(attribs.value(ML::AttrLength).toDouble());

    return size;
}
