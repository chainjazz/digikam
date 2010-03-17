/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-05-25
 * Description : border threaded image filter.
 *
 * Copyright 2005-2010 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright 2006-2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright 2009-2010 by Andi Clemens <andi dot clemens at gmx dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef BORDERFILTER_H
#define BORDERFILTER_H

// Qt includes

#include <QColor>
#include <QImage>
#include <QString>

// Local includes

#include "digikam_export.h"
#include "dimgthreadedfilter.h"
#include "globals.h"

namespace Digikam
{

class BorderFilterPriv;

class DIGIKAM_EXPORT BorderFilter : public DImgThreadedFilter
{

public:

    enum BorderTypes
    {
        SolidBorder = 0,
        NiepceBorder,
        BeveledBorder,
        PineBorder,
        WoodBorder,
        PaperBorder,
        ParqueBorder,
        IceBorder,
        LeafBorder,
        MarbleBorder,
        RainBorder,
        CratersBorder,
        DriedBorder,
        PinkBorder,
        StoneBorder,
        ChalkBorder,
        GraniteBorder,
        RockBorder,
        WallBorder
    };

public:

    /** Constructor using settings to preserve aspect ratio of image. 
     */
    explicit BorderFilter(DImg* orgImage, QObject* parent=0, 
                          int   orgWidth=0, 
                          int   orgHeight=0,
                          const QString& borderPath=QString(), 
                          int   borderType=SolidBorder, 
                          float borderPercent=0.1,
                          const DColor& solidColor = DColor(),
                          const DColor& niepceBorderColor = DColor(),
                          const DColor& niepceLineColor = DColor(),
                          const DColor& bevelUpperLeftColor = DColor(),
                          const DColor& bevelLowerRightColor = DColor(),
                          const DColor& decorativeFirstColor = DColor(),
                          const DColor& decorativeSecondColor = DColor());

    /** Constructor using settings to not-preserve aspect ratio of image. 
     */
    explicit BorderFilter(DImg* orgImage, QObject* parent=0, 
                          int   orgWidth=0, 
                          int   orgHeight=0,
                          const QString& borderPath=QString(), 
                          int   borderType=SolidBorder,
                          int   borderWidth1=100, int borderWidth2=20, int borderWidth3=20, int borderWidth4=10,
                          const DColor& solidColor = DColor(),
                          const DColor& niepceBorderColor = DColor(),
                          const DColor& niepceLineColor = DColor(),
                          const DColor& bevelUpperLeftColor = DColor(),
                          const DColor& bevelLowerRightColor = DColor(),
                          const DColor& decorativeFirstColor = DColor(),
                          const DColor& decorativeSecondColor = DColor());

    ~BorderFilter();

private:

    void filterImage();

    /** Methods to preserve aspect ratio of image. */
    void solid(DImg& src, DImg& dest, const DColor& fg, int borderWidth);
    void niepce(DImg& src, DImg& dest, const DColor& fg, int borderWidth,
                const DColor& bg, int lineWidth);
    void bevel(DImg& src, DImg& dest, const DColor& topColor,
               const DColor& btmColor, int borderWidth);
    void pattern(DImg& src, DImg& dest, int borderWidth, const DColor& firstColor,
                 const DColor& secondColor, int firstWidth, int secondWidth);

    /** Methods to not-preserve aspect ratio of image. */
    void solid2(DImg& src, DImg& dest, const DColor& fg, int borderWidth);
    void niepce2(DImg& src, DImg& dest, const DColor& fg, int borderWidth,
                 const DColor& bg, int lineWidth);
    void bevel2(DImg& src, DImg& dest, const DColor& topColor,
                const DColor& btmColor, int borderWidth);
    void pattern2(DImg& src, DImg& dest, int borderWidth, const DColor& firstColor,
                  const DColor& secondColor, int firstWidth, int secondWidth);

private:

    BorderFilterPriv* const d;
};

}  // namespace Digikam

#endif /* BORDERFILTER_H */
