/************************************************************************
 **
 **  @file   vdxfpaintdevice.h
 **  @author Valentina Zhuravska <zhuravska19(at)gmail.com>
 **  @date   12 812, 2015
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

#ifndef VDXFPAINTDEVICE_H
#define VDXFPAINTDEVICE_H

#include <qcompilerdetection.h>
#include <QPaintDevice>
#include <QSize>
#include <QString>
#include <QtGlobal>

#include "dxfdef.h"
#include "libdxfrw/drw_base.h"

class VDxfEngine;
class VLayoutPiece;

class VDxfPaintDevice : public QPaintDevice
{
public:
    VDxfPaintDevice();
    virtual ~VDxfPaintDevice() override;
    virtual QPaintEngine *paintEngine() const override;

    QString getFileName() const;
    void setFileName(const QString &value);

    QSize getSize();
    void setSize(const QSize &size);

    double getResolution() const;
    void   setResolution(double dpi);

    DRW::Version GetVersion() const;
    void         SetVersion(DRW::Version version);

    void SetBinaryFormat(bool binary);
    bool IsBinaryFromat() const;

    void setMeasurement(const VarMeasurement &var);
    void setInsunits(const VarInsunits &var);

    qreal GetXScale() const;
    void  SetXScale(const qreal &xscale);

    qreal GetYScale() const;
    void  SetYScale(const qreal &yscale);

    bool ExportToAAMA(const QVector<VLayoutPiece> &details) const;
    bool ExportToASTM(const QVector<VLayoutPiece> &details) const;

    QString ErrorString() const;

protected:
    virtual int metric(PaintDeviceMetric metric) const override;
private:
    // cppcheck-suppress unknownMacro
    Q_DISABLE_COPY_MOVE(VDxfPaintDevice) // NOLINT
    VDxfEngine *engine;
    QString     fileName;
};

#endif // VDXFPAINTDEVICE_H
