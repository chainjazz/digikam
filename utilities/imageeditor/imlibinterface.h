/* ============================================================
 * File  : imlibinterface.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2003-01-15
 * Description : 
 * 
 * Copyright 2003 by Renchi Raju

 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published bythe Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef IMLIBINTERFACE_H
#define IMLIBINTERFACE_H

#include <qobject.h>

class QWidget;
class QString;

namespace Digikam
{

class ImlibInterfacePrivate;

class ImlibInterface : public QObject
{

    Q_OBJECT
    
public:

    static ImlibInterface* instance();

    ~ImlibInterface();

    void load(const QString& filename);
    void zoom(double val);

    void paint(int dx, int dy, int dw, int dh,
               int sx, int sy);
    void paint(QPaintDevice *w, int dx, int dy, int dw, int dh,
               int sx, int sy);

    int  width();
    int  height();
    int  origWidth();
    int  origHeight();
    bool hasAlpha();    

    void setSelectedArea(int x, int y, int w, int h);
    void getSelectedArea(int& x, int& y, int& w, int& h);
    
    void rotate90();
    void rotate180();
    void rotate270();

    void flipHoriz();
    void flipVert();
    
    void crop(int x, int y, int w, int h);

    void resize(int w, int h);
    
    void restore();
    void save(const QString& file);
    void saveAs(const QString& file);

    void changeGamma(double gamma);
    void changeBrightness(double brightness);
    void changeContrast(double contrast);

    void setBCG(double brightness, double contrast, double gamma);
    
    uint* getData();
    void  putData(uint* data);
    uint* getSelectedData();
    void  putSelectedData(uint* data);
    
signals:

    void signalRequestUpdate();
    
private:

    ImlibInterface();
    void render();

    ImlibInterfacePrivate *d;
    
    static ImlibInterface *m_instance;
};

}

#endif /* IMLIBINTERFACE_H */
