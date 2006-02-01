/* ============================================================
 * Author: Caulier Gilles <caulier dot gilles at free.fr>
 * Date  : 2004-11-17
 * Description :
 *
 * Copyright 2004-2006 by Gilles Caulier
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
 
#ifndef IMAGEPROPERTIESSIDEBAR_H
#define IMAGEPROPERTIESSIDEBAR_H

// KDE includes.

#include <kurl.h>

// Local includes.

#include "sidebar.h"
#include "digikam_export.h"

class QSplitter;
class QWidget;
class QRect;

namespace Digikam
{

class DImg;
class ImagePropertiesEXIFTab;
class ImagePropertiesColorsTab;

class DIGIKAM_EXPORT ImagePropertiesSideBar : public Digikam::Sidebar
{
    Q_OBJECT

public:

    ImagePropertiesSideBar(QWidget* parent, const char *name, QSplitter *splitter, 
                           Side side=Left, bool mimimizedDefault=false);
                    
    ~ImagePropertiesSideBar();
    
    virtual void itemChanged(const KURL& url, QRect *rect=0, DImg *img=0);
                    
public slots:

    void slotImageSelectionChanged(QRect *rect);                 
    virtual void slotNoCurrentItem(void);                               


protected slots:

    virtual void slotChangedTab(QWidget* tab);

protected:

    bool                      m_dirtyExifTab;
    bool                      m_dirtyColorTab;
 
    QRect                    *m_currentRect;
 
    KURL                      m_currentURL;
 
    DImg                     *m_image;
     
    ImagePropertiesEXIFTab   *m_exifTab;
    ImagePropertiesColorsTab *m_colorTab;
        
};

}  // NameSpace Digikam

#endif  // IMAGEPROPERTIESSIDEBAR_H
