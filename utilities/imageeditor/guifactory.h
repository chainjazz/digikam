/* ============================================================
 * File  : guifactory.h
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2004-02-11
 * Description : 
 * 
 * Copyright 2004 by Renchi Raju

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

#ifndef GUIFACTORY_H
#define GUIFACTORY_H

class QWidget;

namespace Digikam
{

class GUIClient;
class GUIFactoryPriv;
class GUIElement;

class GUIFactory
{

public:

    GUIFactory();
    ~GUIFactory();

    void insertClient(GUIClient *client);
    void removeClient(GUIClient *client);

    void buildGUI(QWidget *widget);
    
private:

    void buildGUI(GUIElement *gui, QWidget *widget);
    
    void parseDefinition();
    void buildTree(GUIClient *client, GUIElement *parent,
                   QStringList guiDef);

    GUIFactoryPriv *d;
};

}

#endif /* GUIFACTORY_H */
