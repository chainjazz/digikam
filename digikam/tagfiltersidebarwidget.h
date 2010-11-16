/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2000-12-05
 * Description : tag filter view for the right sidebar
 *
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2010 by Andi Clemens <andi dot clemens at gmx dot net>
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

#ifndef TAGFILTERSIDEBARWIDGET_H
#define TAGFILTERSIDEBARWIDGET_H

// Qt includes
#include <qwidget.h>

// Local includes
#include "imagefiltersettings.h"
#include "statesavingobject.h"
#include "tagcheckview.h"

namespace Digikam
{

class TagModel;

class TagFilterViewPriv;

/**
 * A view to filter the currently displayed album by tags.
 *
 * @author jwienke
 */
class TagFilterView : public TagCheckView
{
    Q_OBJECT
public:

    /**
     * Constructor.
     *
     * @param parent parent for qt parent child mechanism
     * @param tagFilterModel tag model to work on
     */
    TagFilterView(QWidget *parent, TagModel *tagFilterModel);

    /**
     * Destructor.
     */
    virtual ~TagFilterView();

protected:
    virtual void addCustomContextMenuActions(ContextMenuHelper &cmh, Album *album);
    virtual void handleCustomContextMenuAction(QAction *action, AlbumPointer<Album> album);

private:
    TagFilterViewPriv *d;

};

class TagFilterSideBarWidgetPriv;

/**
 * Sidebar widget containing the tag filtering.
 *
 * @author jwienke
 */
class TagFilterSideBarWidget : public QWidget, public StateSavingObject
{
    Q_OBJECT
public:

    /**
     * Constructor.
     *
     * @param parent parent for qt parent child mechanism
     * @param tagFilterModel tag model to work on
     */
    TagFilterSideBarWidget(QWidget *parent, TagModel *tagFilterModel);

    /**
     * Destructor.
     */
    virtual ~TagFilterSideBarWidget();

    virtual void setConfigGroup(KConfigGroup group);
    virtual void doLoadState();
    virtual void doSaveState();

Q_SIGNALS:

    /**
     * Emitted if the selected filter has changed.
     *
     * @param includedTags a list of included tag ids
     * @param excludedTags a list of excluded tag ids
     * @param matchingCond condition to join the selected tags
     * @param showUnTagged if this is true, only photos without a tag shall be
     *                     shown
     */
    void tagFilterChanged(const QList<int>& includedTags, const QList<int>& excludedTags,
                          ImageFilterSettings::MatchingCondition matchingCond,
                          bool showUnTagged);

public Q_SLOTS:

    /**
     * Resets all selected tag filters.
     */
    void slotResetTagFilters();

private Q_SLOTS:

    void slotMatchingConditionChanged(int index);
    void slotCheckedTagsChanged(const QList<TAlbum*> &includedTags, const QList<TAlbum*> &excludedTags);
    void slotWithoutTagChanged(int newState);

private:
    void filterChanged();

private:
    TagFilterSideBarWidgetPriv *d;

};

}

#endif /* TAGFILTERSIDEBARWIDGET_H*/
