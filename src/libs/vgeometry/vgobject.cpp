/************************************************************************
 **
 **  @file   vgobject.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   27 12, 2013
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Valentina project
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

#include "vgobject.h"

#include <QJsonObject>
#include <QLine>
#include <QLineF>
#include <QPoint>
#include <QPointF>
#include <QRectF>
#include <QTransform>

#include "../vmisc/def.h"
#include "../vmisc/compatibility.h"
#include "vgobject_p.h"
#include "../vmisc/vabstractapplication.h"

namespace
{
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief PerpDotProduct Calculates the area of the parallelogram of the three points.
 * This is actually the same as the area of the triangle defined by the three points, multiplied by 2.
 * @return 2 * triangleArea(a,b,c)
 */
auto PerpDotProduct(const QPointF &p1, const QPointF &p2, const QPointF &t) -> double
{
    return (p1.x() - t.x()) * (p2.y() - t.y()) - (p1.y() - t.y()) * (p2.x() - t.x());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetEpsilon solve the floating-point accuraccy problem.
 *
 * There is the floating-point accuraccy problem, so instead of checking against zero, some epsilon value has to be
 * used. Because the size of the pdp value depends on the length of the vectors, no static value can be used. One
 * approach is to compare the pdp/area value to the fraction of another area which also depends on the length of the
 * line e1=(p1, p2), e.g. the minimal area calucalted with PerpDotProduc() if point still not on the line. This distance
 * is controled by variable accuracyPointOnLine
 */
auto GetEpsilon(const QPointF &t, QPointF p1, QPointF p2, qreal accuracy) -> double
{
    QLineF edge1(p1, p2);
    QLineF edge2(p1, t);
    if (edge2.length() > edge1.length())
    {
        edge1.setLength(edge2.length());
        p1 = edge1.p1();
        p2 = edge1.p2();
    }

    QLineF line(p1, p2);
    line.setAngle(line.angle() + 90);
    line.setLength(accuracy); // less than accuracy means the same point

    return qAbs(PerpDotProduct(p1, p2, line.p2()));
}
}  // namespace

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VGObject default constructor.
 */
VGObject::VGObject()
    :d(new VGObjectData)
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VGObject constructor.
 * @param type type graphical object.
 * @param idObject id parent object.
 * @param mode mode creation. Used in modeling mode.
 */
VGObject::VGObject(const GOType &type, const quint32 &idObject, const Draw &mode)
    :d(new VGObjectData(type, idObject, mode))
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VGObject copy constructor.
 * @param obj object.
 */
VGObject::VGObject(const VGObject &obj) // NOLINT(modernize-use-equals-default)
    :d (obj.d)
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief operator = assignment operator.
 * @param obj object
 * @return object
 */
auto VGObject::operator=(const VGObject &obj) -> VGObject &
{
    if ( &obj == this )
    {
        return *this;
    }
    d = obj.d;
    return *this;
}

#ifdef Q_COMPILER_RVALUE_REFS
//---------------------------------------------------------------------------------------------------------------------
VGObject::VGObject(VGObject &&obj) Q_DECL_NOTHROW
    :d (std::move(obj.d))
{}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::operator=(VGObject &&obj) Q_DECL_NOTHROW -> VGObject &
{
    std::swap(d, obj.d);
    return *this;
}
#endif

//---------------------------------------------------------------------------------------------------------------------
VGObject::~VGObject() // NOLINT(modernize-use-equals-default)
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief getIdObject return parent id.
 * @return parent id or 0 if object don't have parent.
 */
auto VGObject::getIdObject() const -> quint32
{
    return d->idObject;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief setIdObject set parent id.
 * @param value parent id.
 */
void VGObject::setIdObject(const quint32 &value)
{
    d->idObject = value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief name return name graphical object.
 * @return name
 */
auto VGObject::name() const -> QString
{
    return d->_name;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief setName set name graphical object.
 * @param name name graphical object.
 */
void VGObject::setName(const QString &name)
{
    d->_name = name;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief getMode return mode creation.
 * @return mode.
 */
auto VGObject::getMode() const -> Draw
{
    return d->mode;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief setMode set mode creation.
 * @param value mode.
 */
void VGObject::setMode(const Draw &value)
{
    d->mode = value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief getType return object type.
 * @return type.
 */
auto VGObject::getType() const -> GOType
{
    return d->type;
}

//---------------------------------------------------------------------------------------------------------------------
// cppcheck-suppress unusedFunction
void VGObject::setType(const GOType &type)
{
    d->type = type;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief id return id object.
 * @return id
 */
auto VGObject::id() const -> quint32
{
    return d->_id;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief setId set id object.
 * @param id id.
 */
void VGObject::setId(const quint32 &id)
{
    d->_id = id;
}

//---------------------------------------------------------------------------------------------------------------------
void VGObject::SetAlias(const QString &alias)
{
    d->m_alias = alias;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::GetAlias() const -> QString
{
    return d->m_alias;
}

//---------------------------------------------------------------------------------------------------------------------
void VGObject::SetAliasSuffix(const QString &aliasSuffix)
{
    d->m_aliasSuffix = aliasSuffix;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::GetAliasSuffix() const -> QString
{
    return d->m_aliasSuffix;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::ObjectName() const -> QString
{
    const QString alias = VAbstractApplication::VApp()->TrVars()->VarToUser(d->m_alias);
    const QString name = VAbstractApplication::VApp()->TrVars()->VarToUser(d->_name);
    return not d->m_alias.isEmpty() ? QStringLiteral("%1 (%2)").arg(alias, name) : name;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::getIdTool() const -> quint32
{
    if (d->mode == Draw::Calculation)
    {
        if (d->idObject != NULL_ID)
        {
            return d->idObject;
        }
        return d->_id;
    }

    return d->_id;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::ToJson() const -> QJsonObject
{
    QJsonObject object
    {
        {"id", static_cast<qint64>(id())},
        {"type", static_cast<int>(getType())},
    };

    return object;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::BuildLine(const QPointF &p1, const qreal &length, const qreal &angle) -> QLineF
{
    QLineF line = QLineF();
    line.setP1(p1);
    line.setAngle(angle);// First set angle then length. Length can have negative value.
    line.setLength(length);
    return line;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::BuildRay(const QPointF &firstPoint, const qreal &angle, const QRectF &scRect) -> QPointF
{
    QRectF rect = scRect;
    if (not rect.isValid())
    {
        rect = QRectF(0, 0, 1200, 700);
    }

    if (not rect.contains(firstPoint))
    {
        // If point outside of scene rect create one around point and unite two rects.
        QRectF rectangle(firstPoint.x()-rect.width()/2, firstPoint.y()-rect.height()/2, rect.width(), rect.height());
        rect = rect.united(rectangle);
    }
    const qreal diagonal = qSqrt(pow(rect.height(), 2) + pow(rect.width(), 2));
    const QLineF line = BuildLine(firstPoint, diagonal, angle);

    return LineIntersectRect(rect, line);
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::BuildAxis(const QPointF &p, const qreal &angle, const QRectF &scRect) -> QLineF
{
    const QPointF endP1 = BuildRay(p, angle+180, scRect);
    const QPointF endP2 = BuildRay(p, angle, scRect);
    return {endP1, endP2};
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::BuildAxis(const QPointF &p1, const QPointF &p2, const QRectF &scRect) -> QLineF
{
    return BuildAxis(p1, QLineF(p1, p2).angle(), scRect);
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::ContactPoints(const QPointF &p, const QPointF &center, qreal radius, QPointF &p1, QPointF &p2) -> int
{
    const int flag = PointInCircle(p, center, radius);

    if (flag == 0)
    {
        return 0;
    }

    if (flag == 1)
    {
        p1 = p;
        return 1;
    }

    const double d = QLineF (p, center).length();
    const double k = sqrt (d * d - radius * radius);
    return IntersectionCircles(p, k, center, radius, p1, p2);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief LineIntersectRect find point intersection line and rect.
 * @param rec rect.
 * @param line line.
 * @return point intersection.
 */
auto VGObject::LineIntersectRect(const QRectF &rec, const QLineF &line) -> QPointF
{
    qreal x1, y1, x2, y2;
    rec.getCoords(&x1, &y1, &x2, &y2);
    QPointF point;
    QLineF::IntersectType type = Intersects(line, QLineF(QPointF(x1, y1), QPointF(x1, y2)), &point);

    if ( type == QLineF::BoundedIntersection )
    {
        return point;
    }

    type = Intersects(line, QLineF(QPointF(x1, y1), QPointF(x2, y1)), &point);

    if ( type == QLineF::BoundedIntersection )
    {
        return point;
    }

    type = Intersects(line, QLineF(QPointF(x1, y2), QPointF(x2, y2)), &point);

    if ( type == QLineF::BoundedIntersection )
    {
        return point;
    }

    type = Intersects(line, QLineF(QPointF(x2, y1), QPointF(x2, y2)), &point);

    if ( type == QLineF::BoundedIntersection )
    {
        return point;
    }
    return point;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::IntersectionCircles(const QPointF &c1, double r1, const QPointF &c2, double r2, QPointF &p1, QPointF &p2) -> int
{
    if (VFuzzyComparePossibleNulls(c1.x(), c2.x()) && VFuzzyComparePossibleNulls(c1.y(), c2.y())
            && VFuzzyComparePossibleNulls(r1, r2))
    {
        return 3;// Circles are equal
    }
    const double a = - 2.0 * (c2.x() - c1.x());
    const double b = - 2.0 * (c2.y() - c1.y());
    const double c = (c2.x() - c1.x())* (c2.x() - c1.x()) + (c2.y() - c1.y()) * (c2.y() - c1.y()) + r1 * r1 - r2 * r2;

    const double x0 = -a*c/(a*a+b*b);
    const double y0 = -b*c/(a*a+b*b);

    if (c*c > r1*r1*(a*a+b*b))
    {
        return 0;
    }

    if (VFuzzyComparePossibleNulls(c*c, r1*r1*(a*a+b*b)))
    {
        p1 = QPointF(x0 + c1.x(), y0  + c1.y());
        return 1;
    }

    const double d = r1*r1 - c*c/(a*a+b*b);
    const double mult = sqrt (d / (a*a+b*b));

    const double ax = x0 + b * mult;
    const double bx = x0 - b * mult;
    const double ay = y0 - a * mult;
    const double by = y0 + a * mult;

    p1 = QPointF(ax + c1.x(), ay + c1.y());
    p2 = QPointF(bx + c1.x(), by + c1.y());
    return 2;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief LineIntersectCircle find point intersection line and circle.
 * @param center arc center.
 * @param radius arc radius.
 * @param line line
 * @param p1 first intersection point.
 * @param p2 second intersection point.
 * @return 0 - intersection doesn't exist, 1 - one intersection point, 2 - two intersection points.
 */
auto VGObject::LineIntersectCircle(const QPointF &center, qreal radius, const QLineF &line, QPointF &p1,
                                   QPointF &p2) -> qint32
{
    // Fix for issue #485. https://bitbucket.org/dismine/valentina/issues/485/error-when-drawing-a-curved-path
    if (qFuzzyIsNull(line.length()))
    {
        return 0;
    }

    //coefficient for equation of segment
    qreal a = 0, b = 0, c = 0;
    LineCoefficients(line, &a, &b, &c);
    // projection center of circle on to line
    const QPointF p = ClosestPoint (line, center);
    // how many solutions?
    qint32 flag = 0;
    const qreal d = QLineF (center, p).length();
    if (VFuzzyComparePossibleNulls(d, radius))
    {
        flag = 1;
    }
    else
    {
        if (radius > d)
        {
            flag = 2;
        }
        else
        {
            return 0;
        }
    }
    // find distance from projection to points of intersection
    const qreal k = qSqrt (qAbs(radius * radius - d * d));
    const qreal t = QLineF (QPointF (0, 0), QPointF (b, - a)).length();
    // add to projection a vectors aimed to points of intersection
    p1 = addVector (p, QPointF (0, 0), QPointF (- b, a), k / t);
    p2 = addVector (p, QPointF (0, 0), QPointF (b, - a), k / t);
    return flag;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ClosestPoint find point projection of point onto line.
 * @param line line.
 * @return point on line or extended line if origin size too small.
 */
auto VGObject::ClosestPoint(const QLineF &line, const QPointF &point) -> QPointF
{
    qreal a = 0, b = 0, c = 0;
    LineCoefficients(line, &a, &b, &c);
    qreal x = point.x() + a;
    qreal y = b + point.y();
    QLineF lin (point, QPointF(x, y));
    QPointF p;
    QLineF::IntersectType intersect = Intersects(line, lin, &p);

    if (intersect == QLineF::UnboundedIntersection || intersect == QLineF::BoundedIntersection)
    {
        return p;
    }

    return point;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::addVector(const QPointF &p, const QPointF &p1, const QPointF &p2, qreal k) -> QPointF
{
    return {p.x() + (p2.x() - p1.x()) * k, p.y() + (p2.y() - p1.y()) * k};
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief LineCoefficients coefficient for equation of segment. Segment equestion ax+by+c=0.
 * @param line line
 * @param a a value
 * @param b b value
 * @param c c value
 */
void VGObject::LineCoefficients(const QLineF &line, qreal *a, qreal *b, qreal *c)
{
    //coefficient for equation of segment
    QPointF p1 = line.p1();
    *a = line.p2().y() - p1.y();
    *b = p1.x() - line.p2().x();
    *c = - *a * p1.x() - *b * p1.y();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief IsPointOnLineSegment Check if the point is on the line segment.
 */
auto VGObject::IsPointOnLineSegment(const QPointF &t, const QPointF &p1, const QPointF &p2, qreal accuracy) -> bool
{
    // Because of accuracy issues, this operation is slightly different from ordinary checking point on segment.
    // Here we deal with more like cigar shape.

    // Front and rear easy to check
    if (VFuzzyComparePoints(p1, t, accuracy) || VFuzzyComparePoints(p2, t, accuracy))
    {
        return true;
    }

    // Check if we have a segment. On previous step we already confirmed that we don't have intersection
    if (VFuzzyComparePoints(p1, p2, accuracy))
    {
        return false;
    }

    // Calculate the main rectangle shape. QLineF is not 100% accurate in calculating positions for points, but this
    // should be good enough for us.
    const qreal degrees = 90;
    QLineF edge(p1, p2);
    edge.setAngle(edge.angle()+degrees);
    edge.setLength(accuracy);
    QPointF sP1 = edge.p2();

    edge = QLineF(p2, p1);
    edge.setAngle(edge.angle()-degrees);
    edge.setLength(accuracy);
    QPointF sP2 = edge.p2();

    edge = QLineF(p2, p1);
    edge.setAngle(edge.angle()+degrees);
    edge.setLength(accuracy);
    QPointF sP3 = edge.p2();

    edge = QLineF(p1, p2);
    edge.setAngle(edge.angle()-degrees);
    edge.setLength(accuracy);
    QPointF sP4 = edge.p2();

    QVector<QPointF> shape{sP1, sP2, sP3, sP4, sP1};

    for (int i=0; i < shape.size()-1; ++i)
    {
        const QPointF &sp1 = shape.at(i);
        const QPointF &sp2 = shape.at(i+1);
        // This formula helps to determine on which side of a vector lies a point.
        qreal position = (sp2.x()-sp1.x())*(t.y()-sp1.y())-(sp2.y()-sp1.y())*(t.x()-sp1.x());
        if (position < 0)
        {
            return false;
        }
    }

    return true;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::IsLineSegmentOnLineSegment(const QLineF &seg1, const QLineF &seg2, qreal accuracy) -> bool
{
    const bool onLine = IsPointOnLineviaPDP(seg1.p1(), seg2.p1(), seg2.p2(), accuracy) &&
            IsPointOnLineviaPDP(seg1.p2(), seg2.p1(), seg2.p2(), accuracy);
    if (onLine)
    {
        return IsPointOnLineSegment(seg1.p1(), seg2.p1(), seg2.p2(), accuracy) ||
                IsPointOnLineSegment(seg1.p2(), seg2.p1(), seg2.p2(), accuracy) ||
                IsPointOnLineSegment(seg2.p1(), seg1.p1(), seg1.p2(), accuracy) ||
                IsPointOnLineSegment(seg2.p2(), seg1.p1(), seg1.p2(), accuracy);
    }

    return onLine;
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::CorrectDistortion(const QPointF &t, const QPointF &p1, const QPointF &p2) -> QPointF
{
    if (not VFuzzyComparePoints(p1, p2))
    {
        return VGObject::ClosestPoint(QLineF(p1, p2), t);
    }

    return t;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief IsPointOnLineviaPDP use the perp dot product (PDP) way.
 *
 *  The pdp is zero only if the t lies on the line e1 = vector from p1 to p2.
 * @return true if point is on line
 */
auto VGObject::IsPointOnLineviaPDP(const QPointF &t, const QPointF &p1, const QPointF &p2, qreal accuracy) -> bool
{
    const double p = qAbs(PerpDotProduct(p1, p2, t));
    const double e = GetEpsilon(t, p1, p2, accuracy);

    // We can't use common "<=" here because of the floating-point accuraccy problem
    return p < e || VFuzzyComparePossibleNulls(p, e);
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::PointInCircle(const QPointF &p, const QPointF &center, qreal radius) -> int
{
    const double d = QLineF (p, center).length();
    if (VFuzzyComparePossibleNulls(radius, d))
    {
        return 1; // on circle
    }

    if (radius > d)
    {
        return 0; // outside circle
    }

    return 2; // inside circle
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief GetLengthContour return length of contour.
 * @param contour container with points of contour.
 * @param newPoints point whos we try to add to contour.
 * @return length length of contour.
 */
// cppcheck-suppress unusedFunction
auto VGObject::GetLengthContour(const QVector<QPointF> &contour, const QVector<QPointF> &newPoints) -> int
{
    qreal length = 0;
    QVector<QPointF> points;
    points << contour << newPoints;
    for (qint32 i = 0; i < points.size()-1; ++i)
    {
        QLineF line(points.at(i), points.at(i+1));
        length += line.length();
    }
    return qFloor(length);
}

//---------------------------------------------------------------------------------------------------------------------
auto VGObject::FlippingMatrix(const QLineF &axis) -> QTransform
{
    QTransform matrix;

    if (axis.isNull())
    {
        return matrix;
    }

    const QLineF axisOX = QLineF(axis.x2(), axis.y2(), axis.x2() + 100, axis.y2()); // Ox axis

    const qreal angle = axis.angleTo(axisOX);
    const QPointF p2 = axis.p2();

    QTransform m;
    m.translate(p2.x(), p2.y());
    m.rotate(-angle);
    m.translate(-p2.x(), -p2.y());
    matrix *= m;

    m.reset();
    m.translate(p2.x(), p2.y());
    m.scale(m.m11(), m.m22()*-1);
    m.translate(-p2.x(), -p2.y());
    matrix *= m;

    m.reset();
    m.translate(p2.x(), p2.y());
    m.rotate(-(360-angle));
    m.translate(-p2.x(), -p2.y());
    matrix *= m;

    return matrix;
}
