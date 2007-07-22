/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2004-08-22
 * Description : a generic widget to display a panel to choose
 *               a rectangular image area.
 * 
 * Copyright (C) 2004-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef PANICONWIDGET_H
#define PANICONWIDGET_H

// Qt includes.

#include <QWidget>
#include <QRect>
#include <QImage>
#include <QPixmap>
#include <QHideEvent>
#include <QMouseEvent>
#include <QTimerEvent>
#include <QPaintEvent>

// Local includes

#include "digikam_export.h"
#include "dimg.h"

namespace Digikam
{

class PanIconWidgetPriv;

class DIGIKAM_EXPORT PanIconWidget : public QWidget
{
Q_OBJECT

public:

    PanIconWidget(QWidget *parent=0, Qt::WidgetAttribute attribute=Qt::WA_DeleteOnClose);
    ~PanIconWidget();

    void setImage(int previewWidth, int previewHeight, const QImage& image);
    void setImage(int previewWidth, int previewHeight, const DImg& image);

    void  setRegionSelection(QRect regionSelection);
    QRect getRegionSelection(void);
    void  setCenterSelection(void);

    void  setCursorToLocalRegionSelectionCenter(void);
    void  setMouseFocus(void);

signals:

    // Used with ImagePreview widget. 
    // Emit when selection have been moved with mouse. 'targetDone' booleen 
    // value is used for indicate if the mouse have been released.
    void signalSelectionMoved( QRect rect, bool targetDone );

    void signalSelectionTakeFocus(void);

    void signalHiden(void);

public slots:

    void slotZoomFactorChanged( double );

protected:

    void hideEvent(QHideEvent *);
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void timerEvent(QTimerEvent *);

    /** Recalculate the target selection position and emit 'signalSelectionMoved'.*/
    void regionSelectionMoved( bool targetDone );

    virtual void updatePixmap();

protected:

    bool     m_flicker;

    int      m_timerID;
    int      m_width;
    int      m_height;
    int      m_zoomedOrgWidth;
    int      m_zoomedOrgHeight;
    int      m_orgWidth;
    int      m_orgHeight;

    double   m_zoomFactor;

    QRect    m_rect;
    QRect    m_localRegionSelection;    // Thumbnail size selection.

    QPixmap *m_pixmap;

private:

    PanIconWidgetPriv* d;
};

}  // NameSpace Digikam

#endif /* PANICONWIDGET_H */
