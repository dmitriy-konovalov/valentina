/************************************************************************
 **
 **  @file   vbackgroundpixmapitem.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   15 1, 2022
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2022 Valentina project
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
#ifndef VBACKGROUNDPIXMAPITEM_H
#define VBACKGROUNDPIXMAPITEM_H

#include "vbackgroundimageitem.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 13, 0)
#include "../vmisc/defglobal.h"
#endif // QT_VERSION < QT_VERSION_CHECK(5, 13, 0)

enum class ShapeMode
{
    MaskShape,
    BoundingRectShape,
    HeuristicMaskShape
};

class VBackgroundPixmapItem : public VBackgroundImageItem // clazy:exclude=ctor-missing-parent-argument,missing-qobject-macro
{
public:
    VBackgroundPixmapItem(const VBackgroundPatternImage &image, VAbstractPattern *doc, QGraphicsItem *parent = nullptr);
    ~VBackgroundPixmapItem() override = default;

    auto type() const -> int override {return Type;}
    enum {Type = UserType + static_cast<int>(Tool::BackgroundPixmapImage)};

    auto boundingRect() const -> QRectF override;
    auto shape() const -> QPainterPath override;
    auto contains(const QPointF &point) const -> bool override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    auto isObscuredBy(const QGraphicsItem *item) const -> bool override;
    auto opaqueArea() const -> QPainterPath override;

    auto GetShapeMode() const -> ShapeMode;
    void SetShapeMode(enum ShapeMode mode);

protected:
    auto supportsExtension(Extension extension) const -> bool override;
    void setExtension(Extension extension, const QVariant &variant) override;
    auto extension(const QVariant &variant) const -> QVariant override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    Q_DISABLE_COPY_MOVE(VBackgroundPixmapItem) // NOLINT

    mutable QPixmap m_pixmap{};
    Qt::TransformationMode m_transformationMode{Qt::SmoothTransformation};
    enum ShapeMode m_shapeMode{ShapeMode::MaskShape};
    mutable QPainterPath m_shape{};
    mutable bool m_hasShape{false};

    void SetTransformationMode(Qt::TransformationMode mode);

    void UpdateShape() const;

    auto Pixmap() const -> QPixmap;
};

#endif // VBACKGROUNDPIXMAPITEM_H
