/************************************************************************
 **
 **  @file   tst_vdetail.h
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   9 1, 2016
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2016 Valentina project
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

#ifndef TST_VDETAIL_H
#define TST_VDETAIL_H

#include "../vtest/abstracttest.h"
#include "../vmisc/defglobal.h"

class TST_VPiece : public AbstractTest
{
    Q_OBJECT // NOLINT
public:
    explicit TST_VPiece(QObject *parent = nullptr);

private slots:
    void Issue620();
    void TestSAPassmark_data();
    void TestSAPassmark();

private:
    Q_DISABLE_COPY_MOVE(TST_VPiece) // NOLINT
};

#endif // TST_VDETAIL_H
