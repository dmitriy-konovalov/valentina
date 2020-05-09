/************************************************************************
 **
 **  @file   vpuzzlepiece.cpp
 **  @author Ronan Le Tiec
 **  @date   13 4, 2020
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
#include "vpuzzlepiece.h"

//---------------------------------------------------------------------------------------------------------------------
VPuzzlePiece::VPuzzlePiece()
{

}

//---------------------------------------------------------------------------------------------------------------------
VPuzzlePiece::~VPuzzlePiece()
{

}


//---------------------------------------------------------------------------------------------------------------------
QString VPuzzlePiece::GetName() const
{
    return m_name;
}


//---------------------------------------------------------------------------------------------------------------------
void VPuzzlePiece::SetName(const QString &name)
{
    m_name = name;
}


//---------------------------------------------------------------------------------------------------------------------
QUuid VPuzzlePiece::GetUuid() const
{
    return m_uuid;
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzlePiece::SetUuid(const QUuid &uuid)
{
    m_uuid = uuid;
}


//---------------------------------------------------------------------------------------------------------------------
QVector<QPointF> VPuzzlePiece::GetCuttingLine() const
{
    return m_cuttingLine;
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzlePiece::SetCuttingLine(const QVector<QPointF> &cuttingLine)
{
    m_cuttingLine = cuttingLine;
}


//---------------------------------------------------------------------------------------------------------------------
bool VPuzzlePiece::GetShowSeamLine() const
{
    return m_showSeamline;
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzlePiece::SetShowSeamLine(bool value)
{
    m_showSeamline = value;
}

//---------------------------------------------------------------------------------------------------------------------
bool VPuzzlePiece::GetPieceMirrored() const
{
    return m_mirrorPiece;
}

//---------------------------------------------------------------------------------------------------------------------
void VPuzzlePiece::SetPieceMirrored(bool value)
{
    m_mirrorPiece = value;
}

