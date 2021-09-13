/************************************************************************
 **
 **  @file   vpcarrouselpiece.cpp
 **  @author Ronan Le Tiec
 **  @date   25 4, 2020
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

#include "vpcarrouselpiece.h"

#include <QApplication>
#include <QMenu>
#include <QPainter>

#include "vpmimedatapiece.h"
#include "vpcarrouselpiecelist.h"
#include "vpcarrousel.h"
#include "../layout/vpsheet.h"
#include "../layout/vppiece.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(pCarrouselPiece, "p.carrouselPiece")


//---------------------------------------------------------------------------------------------------------------------
VPCarrouselPiece::VPCarrouselPiece(const VPPiecePtr &piece, QListWidget* parent) :
    QListWidgetItem(parent, Type),
    m_piece(piece)
{
    SCASSERT(m_piece != nullptr)
    int width = 120 - 8;
    QFontMetrics metrix = QFontMetrics(QFont());
    QString clippedText = metrix.elidedText(piece->GetName(), Qt::ElideRight, width);
    setIcon(CreatePieceIcon(QSize(120, 120)));
    setText(clippedText);
}

//---------------------------------------------------------------------------------------------------------------------
auto VPCarrouselPiece::GetPiece() const -> VPPiecePtr
{
    return m_piece;
}

//---------------------------------------------------------------------------------------------------------------------
void VPCarrouselPiece::RefreshSelection()
{
    VPPiecePtr piece = GetPiece();
    if (not piece.isNull())
    {
        setSelected(piece->IsSelected());
    }
}

//---------------------------------------------------------------------------------------------------------------------
auto VPCarrouselPiece::CreatePieceIcon(const QSize &size, bool isDragIcon) const -> QIcon
{
    VPPiecePtr piece = GetPiece();
    if (piece.isNull())
    {
        return {};
    }

    QRectF boundingRect = piece->DetailBoundingRect();
    qreal canvasSize = qMax(boundingRect.height(), boundingRect.width());
    QRectF canvas = QRectF(0, 0, canvasSize, canvasSize);

    qreal dx = canvas.center().x() - boundingRect.center().x();
    qreal dy = canvas.center().y() - boundingRect.center().y();

    QVector<QIcon::Mode> iconModes;
    iconModes.append(QIcon::Normal);

    if(not isDragIcon)
    {
        iconModes.append(QIcon::Selected);
    }

    QIcon icon;

    for(auto iconMode : iconModes)
    {
        QPixmap pixmap(size);

        if(not isDragIcon)
        {
            pixmap.fill(QColor(Qt::white));
        }
        else
        {
            pixmap.fill(QColor(Qt::transparent));
        }

        QPainter painter;
        painter.begin(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        int spacing = 2;

        painter.translate(spacing, spacing);

        qreal scaleFactorX = canvasSize * 100 / (size.width() - spacing*2) / 100;
        qreal scaleFactorY = canvasSize * 100 / (size.height() - spacing*2) / 100;
        painter.scale(1./scaleFactorX, 1./scaleFactorY);
        painter.setPen(QPen(Qt::black, 0.8*qMax(scaleFactorX, scaleFactorY)));

        if(not isDragIcon)
        {
            painter.translate(dx, dy);
        }
        else
        {
            painter.translate(-boundingRect.topLeft().x()+spacing, -boundingRect.topLeft().y()+spacing);
        }

        if(iconMode == QIcon::Selected)
        {
            painter.setBrush(QBrush(QColor(255,160,160,60)));
        }
        else
        {
            painter.setBrush(QBrush(Qt::white));
        }

        piece->DrawMiniature(painter);

        painter.end();

        icon.addPixmap(pixmap,iconMode);
    }

    return icon;
}
