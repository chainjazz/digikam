/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-22
 * Descritpion : tags folder view.
 * 
 * Copyright (C) 2005-2006 by Joern Ahrens <joern.ahrens@kdemail.net>
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <Q3ListView>
#include <Q3ValueList>
#include <QPainter>
#include <QCursor>

// KDE includes.

#include <kmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kapplication.h>
#include <kmessagebox.h>

// Local includes.

#include "ddebug.h"
#include "album.h"
#include "albumdb.h"
#include "albummanager.h"
#include "databasetransaction.h"
#include "syncjob.h"
#include "tagcreatedlg.h"
#include "dragobjects.h"
#include "folderitem.h"
#include "dio.h"
#include "imageattributeswatch.h"
#include "imageinfo.h"
#include "metadatahub.h"
#include "albumthumbnailloader.h"
#include "statusprogressbar.h"
#include "tagfolderview.h"
#include "tagfolderview.moc"

#include "config.h"
#ifdef KDEPIMLIBS_FOUND
#include <kabc/stdaddressbook.h>
#endif

namespace Digikam
{

//-----------------------------------------------------------------------------
// TagFolderViewItem
//-----------------------------------------------------------------------------

class TagFolderViewItem : public FolderItem
{
public:

    TagFolderViewItem(Q3ListView *parent, TAlbum *tag);
    TagFolderViewItem(Q3ListViewItem *parent, TAlbum *tag);

    TAlbum* getTag() const;
    int id() const;

private:

    TAlbum      *m_tag;
};

TagFolderViewItem::TagFolderViewItem(Q3ListView *parent, TAlbum *tag)
                 : FolderItem(parent, tag->title())
{
    setDragEnabled(true);
    m_tag = tag;
}

TagFolderViewItem::TagFolderViewItem(Q3ListViewItem *parent, TAlbum *tag)
                 : FolderItem(parent, tag->title())
{
    setDragEnabled(true);
    m_tag = tag;
}

TAlbum* TagFolderViewItem::getTag() const
{
    return m_tag;
}

int TagFolderViewItem::id() const
{
    return m_tag ? m_tag->id() : 0;
}

//-----------------------------------------------------------------------------
// TagFolderViewPriv
//-----------------------------------------------------------------------------

class TagFolderViewPriv
{

public:

    TagFolderViewPriv()
    {
        ABCMenu  = 0;
        albumMan = 0;
    }

    QMenu        *ABCMenu;

    AlbumManager *albumMan;
};

//-----------------------------------------------------------------------------
// TagFolderView
//-----------------------------------------------------------------------------

TagFolderView::TagFolderView(QWidget *parent)
             : FolderView(parent, "TagFolderView")
{
    d = new TagFolderViewPriv();
    d->albumMan = AlbumManager::instance();

    addColumn(i18n("My Tags"));
    setResizeMode(Q3ListView::LastColumn);
    setRootIsDecorated(false);
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);

    // -- setup slots ---------------------------------------------------------

    connect(d->albumMan, SIGNAL(signalAlbumAdded(Album*)),
            this, SLOT(slotAlbumAdded(Album*)));

    connect(d->albumMan, SIGNAL(signalAlbumDeleted(Album*)),
            this, SLOT(slotAlbumDeleted(Album*)));

    connect(d->albumMan, SIGNAL(signalAlbumRenamed(Album*)),
            this, SLOT(slotAlbumRenamed(Album*)));

    connect(d->albumMan, SIGNAL(signalAlbumsCleared()),
            this, SLOT(slotAlbumsCleared()));

    connect(d->albumMan, SIGNAL(signalAlbumIconChanged(Album*)),
            this, SLOT(slotAlbumIconChanged(Album*)));

    connect(d->albumMan, SIGNAL(signalTAlbumMoved(TAlbum*, TAlbum*)),
            this, SLOT(slotAlbumMoved(TAlbum*, TAlbum*)));

    AlbumThumbnailLoader *loader = AlbumThumbnailLoader::instance();

    connect(loader, SIGNAL(signalThumbnail(Album *, const QPixmap&)),
            this, SLOT(slotGotThumbnailFromIcon(Album *, const QPixmap&)));

    connect(loader, SIGNAL(signalFailed(Album *)),
            this, SLOT(slotThumbnailLost(Album *)));

    connect(loader, SIGNAL(signalReloadThumbnails()),
            this, SLOT(slotReloadThumbnails()));

    connect(this, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int)),
            SLOT(slotContextMenu(Q3ListViewItem*, const QPoint&, int)));

    connect(this, SIGNAL(selectionChanged()),
            this, SLOT(slotSelectionChanged()));
}

TagFolderView::~TagFolderView()
{
    delete d;
}

void TagFolderView::slotAlbumAdded(Album *album)
{
    if(!album)
        return;

    TAlbum *tag = dynamic_cast<TAlbum*>(album);
    if(!tag)
        return;

    TagFolderViewItem *item;

    if(tag->isRoot())
    {
        item = new TagFolderViewItem(this, tag);
        tag->setExtraData(this, item);
        // Toplevel tags are all children of root, and should always be visible - set root to open
        item->setOpen(true);
    }
    else
    {
        TagFolderViewItem *parent =
                (TagFolderViewItem*)tag->parent()->extraData(this);

        if (!parent)
        {
            DWarning() << " Failed to find parent for Tag "
                       << tag->title() << endl;
            return;
        }

        item = new TagFolderViewItem(parent, tag);
        tag->setExtraData(this, item);
    }

    setTagThumbnail(tag);
}

void TagFolderView::slotAlbumDeleted(Album *album)
{
    if(!album)
        return;

    TAlbum *tag = dynamic_cast<TAlbum*>(album);
    if(!tag)
        return;

    TagFolderViewItem *item = (TagFolderViewItem*)album->extraData(this);
    if(item)
    {
        TagFolderViewItem *itemParent = dynamic_cast<TagFolderViewItem*>(item->parent());

        if(itemParent)
            itemParent->takeItem(item);
        else
            takeItem(item);

        delete item;
    }
}

void TagFolderView::slotAlbumsCleared()
{
    clear();
}

void TagFolderView::slotAlbumMoved(TAlbum* tag, TAlbum* newParent)
{
    if (!tag || !newParent)
        return;

    TagFolderViewItem* item = (TagFolderViewItem*)tag->extraData(this);
    if (!item)
        return;

    if (item->parent())
    {
        Q3ListViewItem* oldPItem = item->parent();
        oldPItem->takeItem(item);
    }
    else
    {
        takeItem(item);
    }

    TagFolderViewItem* newPItem = (TagFolderViewItem*)newParent->extraData(this);
    if (newPItem)
        newPItem->insertItem(item);
    else
        insertItem(item);
}

void TagFolderView::slotAlbumRenamed(Album* album)
{
    if (!album)
        return;

    TAlbum* tag = dynamic_cast<TAlbum*>(album);
    if (!tag)
        return;

    TagFolderViewItem* item = (TagFolderViewItem*)(tag->extraData(this));
    if (item)
    {
        item->setText(0, tag->title());
    }
}

void TagFolderView::setTagThumbnail(TAlbum *album)
{
    if(!album)
        return;

    TagFolderViewItem* item = (TagFolderViewItem*) album->extraData(this);

    if(!item)
        return;

    AlbumThumbnailLoader *loader = AlbumThumbnailLoader::instance();
    QPixmap icon;
    if (!loader->getTagThumbnail(album, icon))
    {
        if (icon.isNull())
        {
            item->setPixmap(0, loader->getStandardTagIcon(album));
        }
        else
        {
            QPixmap blendedIcon = loader->blendIcons(loader->getStandardTagIcon(), icon);
            item->setPixmap(0, blendedIcon);
        }
    }
    else
    {
        // for the time being, set standard icon
        item->setPixmap(0, loader->getStandardTagIcon(album));
    }
}

void TagFolderView::slotGotThumbnailFromIcon(Album *album, const QPixmap& thumbnail)
{
    if(!album || album->type() != Album::TAG)
        return;

    TagFolderViewItem* item = (TagFolderViewItem*)album->extraData(this);

    if(!item)
        return;

    AlbumThumbnailLoader *loader = AlbumThumbnailLoader::instance();
    QPixmap blendedIcon = loader->blendIcons(loader->getStandardTagIcon(), thumbnail);
    item->setPixmap(0, blendedIcon);
}

void TagFolderView::slotThumbnailLost(Album *)
{
    // we already set the standard icon before loading
}

void TagFolderView::slotReloadThumbnails()
{
    AlbumList tList = AlbumManager::instance()->allTAlbums();
    for (AlbumList::iterator it = tList.begin(); it != tList.end(); ++it)
    {
        TAlbum* tag  = (TAlbum*)(*it);
        setTagThumbnail(tag);
    }
}

void TagFolderView::slotAlbumIconChanged(Album* album)
{
    if(!album || album->type() != Album::TAG)
        return;

    setTagThumbnail((TAlbum *)album);
}

void TagFolderView::slotSelectionChanged()
{
    if (!active())
        return;

    Q3ListViewItem* selItem = 0;
    Q3ListViewItemIterator it(this);
    while (it.current())
    {
        if (it.current()->isSelected())
        {
            selItem = it.current();
            break;
        }
        ++it;
    }

    if (!selItem)
    {
        d->albumMan->setCurrentAlbum(0);
        return;
    }

    TagFolderViewItem *tagitem = dynamic_cast<TagFolderViewItem*>(selItem);
    if(!tagitem)
    {
        d->albumMan->setCurrentAlbum(0);
        return;
    }

    d->albumMan->setCurrentAlbum(tagitem->getTag());
}

void TagFolderView::slotContextMenu(Q3ListViewItem *item, const QPoint &, int)
{
    TagFolderViewItem *tag = dynamic_cast<TagFolderViewItem*>(item);

    KMenu popmenu(this);
    popmenu.addTitle(SmallIcon("digikam"), i18n("My Tags"));

    QAction *newAction, *editAction=0, *resetIconAction=0, *deleteAction=0;

    newAction     = popmenu.addAction(SmallIcon("tag-new"), i18n("New Tag..."));

#ifdef KDEPIMLIBS_FOUND
    d->ABCMenu = new QMenu(this);

    connect( d->ABCMenu, SIGNAL( aboutToShow() ),
             this, SLOT( slotABCContextMenu() ) );

    popmenu.addMenu(d->ABCMenu);
    d->ABCMenu->menuAction()->setIcon(SmallIcon("tag-addressbook"));
    d->ABCMenu->menuAction()->setText(i18n("Create Tag From AddressBook"));
#endif

    if(tag && tag->parent())
    {
        editAction       = popmenu.addAction(SmallIcon("tag-properties"), i18n("Edit Tag Properties..."));
        resetIconAction  = popmenu.addAction(SmallIcon("tag-reset"),      i18n("Reset Tag Icon"));
        popmenu.addSeparator();
        deleteAction = popmenu.addAction(SmallIcon("tag-delete"),     i18n("Delete Tag"));
    }

    QAction *choice = popmenu.exec((QCursor::pos()));
    if (choice)
    {
        if (choice == newAction)
        {
            tagNew(tag);
        }
        if (choice == editAction)
        {
            tagEdit(tag);
        }
        if (choice == deleteAction)
        {
            tagDelete(tag);
        }
        if (choice == resetIconAction)
        {
            QString errMsg;
            AlbumManager::instance()->updateTAlbumIcon(tag->getTag(), QString("tag"), 0, errMsg);
        }
        else
        {
            tagNew( tag, choice->text(), "tag-people" );
        }
    }

    delete d->ABCMenu;
    d->ABCMenu = 0;
}

void TagFolderView::slotABCContextMenu()
{
#ifdef KDEPIMLIBS_FOUND
    d->ABCMenu->clear();

    KABC::AddressBook* ab = KABC::StdAddressBook::self();
    QStringList names;
    for ( KABC::AddressBook::Iterator it = ab->begin(); it != ab->end(); ++it )
    {
        names.push_back(it->formattedName());
    }
    qSort(names);

    for ( QStringList::Iterator it = names.begin(); it != names.end(); ++it )
    {
        QString name = *it;
        if (!name.isNull() )
            d->ABCMenu->addAction(name);
    }

    if (d->ABCMenu->isEmpty())
    {
        QAction *nothingFound = d->ABCMenu->addAction(i18n("No AddressBook entries found"));
        nothingFound->setEnabled(false);
    }
#endif
}

void TagFolderView::tagNew()
{
    TagFolderViewItem *item = dynamic_cast<TagFolderViewItem*>(selectedItem());
    tagNew(item);
}

void TagFolderView::tagNew( TagFolderViewItem *item, const QString& _title, const QString& _icon )
{
    QString title = _title;
    QString icon  = _icon;
    TAlbum *parent;

    if(!item)
        parent = d->albumMan->findTAlbum(0);
    else
        parent = item->getTag();

    if (title.isNull())
    {
        if(!TagCreateDlg::tagCreate(kapp->activeWindow(), parent, title, icon))
            return;
    }

    QString errMsg;
    TAlbum* newAlbum = d->albumMan->createTAlbum(parent, title, icon, errMsg);

    if( !newAlbum )
        KMessageBox::error(0, errMsg);
    else
    {
        TagFolderViewItem *item = (TagFolderViewItem*)newAlbum->extraData(this);
        if ( item )
            ensureItemVisible( item );
    }
}

void TagFolderView::tagEdit()
{
    TagFolderViewItem *item = dynamic_cast<TagFolderViewItem*>(selectedItem());
    tagEdit(item);
}

void TagFolderView::tagEdit(TagFolderViewItem *item)
{
    if(!item)
        return;

    TAlbum *tag = item->getTag();
    if(!tag)
        return;

    QString title, icon;
    if(!TagEditDlg::tagEdit(kapp->activeWindow(), tag, title, icon))
    {
        return;
    }

    if(tag->title() != title)
    {
        QString errMsg;
        if(!d->albumMan->renameTAlbum(tag, title, errMsg))
            KMessageBox::error(0, errMsg);
        else
            item->setText(0, title);
    }

    if(tag->icon() != icon)
    {
        QString errMsg;
        if (!d->albumMan->updateTAlbumIcon(tag, icon, 0, errMsg))
            KMessageBox::error(0, errMsg);
        else
            setTagThumbnail(tag);
    }
}

void TagFolderView::tagDelete()
{
    TagFolderViewItem *item = dynamic_cast<TagFolderViewItem*>(selectedItem());
    tagDelete(item);
}

void TagFolderView::tagDelete(TagFolderViewItem *item)
{
    if(!item)
        return;

    TAlbum *tag = item->getTag();
    if (!tag || tag->isRoot())
        return;

    // find number of subtags
    int children = 0;
    AlbumIterator iter(tag);
    while(iter.current())
    {
        children++;
        ++iter;
    }

    if(children)
    {
        int result = KMessageBox::warningContinueCancel(this,
                       i18np("Tag '%1' has one subtag. "
                             "Deleting this will also delete "
                             "the subtag. "
                             "Do you want to continue?",
                             "Tag '%1' has %n subtags. "
                             "Deleting this will also delete "
                             "the subtags. "
                             "Do you want to continue?",
                             children, 
                             tag->title()),
                       i18n("Delete Tag"), 
                       KGuiItem(i18n("Delete"),
                       "edit-delete"));

        if(result == KMessageBox::Continue)
        {
            QString errMsg;
            if (!d->albumMan->deleteTAlbum(tag, errMsg))
                KMessageBox::error(0, errMsg);
        }
    }
    else
    {
        int result =
            KMessageBox::warningContinueCancel(0, i18n("Delete '%1' tag?",
                                               tag->title()), i18n("Delete Tag"),
                                               KGuiItem(i18n("Delete"), "edit-delete"));

        if(result == KMessageBox::Continue)
        {
            QString errMsg;
            if (!d->albumMan->deleteTAlbum(tag, errMsg))
                KMessageBox::error(0, errMsg);
        }
    }
}

Q3DragObject* TagFolderView::dragObject()
{
    TagFolderViewItem *item = dynamic_cast<TagFolderViewItem*>(dragItem());
    if(!item)
        return 0;

    if(!item->parent())
        return 0;

    TagDrag *t = new TagDrag(item->getTag()->id(), this);
    t->setPixmap(*item->pixmap(0));

    return t;
}

bool TagFolderView::acceptDrop(const QDropEvent *e) const
{
    QPoint vp = contentsToViewport(e->pos());
    TagFolderViewItem *itemDrop = dynamic_cast<TagFolderViewItem*>(itemAt(vp));
    TagFolderViewItem *itemDrag = dynamic_cast<TagFolderViewItem*>(dragItem());

    if(TagDrag::canDecode(e) || TagListDrag::canDecode(e))
    {
        // Allow dragging at the root, to move the tag to the root
        if(!itemDrop)
            return true;

        // Dragging an item on itself makes no sense
        if(itemDrag == itemDrop)
            return false;

        // Dragging a parent on its child makes no sense
        if(itemDrag && itemDrag->getTag()->isAncestorOf(itemDrop->getTag()))
            return false;

        return true;
    }

    if (ItemDrag::canDecode(e) && itemDrop && itemDrop->parent())
    {
        // Only other possibility is image items being dropped
        // And allow this only if there is a Tag to be dropped
        // on and also the Tag is not root.
        return true;
    }

    return false;
}

void TagFolderView::contentsDropEvent(QDropEvent *e)
{
    FolderView::contentsDropEvent(e);

    if(!acceptDrop(e))
        return;

    QPoint vp = contentsToViewport(e->pos());
    TagFolderViewItem *itemDrop = dynamic_cast<TagFolderViewItem*>(itemAt(vp));

    if (!itemDrop)
        return;

    if(TagDrag::canDecode(e))
    {
        QByteArray ba = e->encodedData("digikam/tag-id");
        QDataStream ds(&ba, QIODevice::ReadOnly);
        int tagID;
        ds >> tagID;

        AlbumManager* man = AlbumManager::instance();
        TAlbum* talbum    = man->findTAlbum(tagID);

        if(!talbum)
            return;

        if (talbum == itemDrop->getTag())
            return;

        KMenu popMenu(this);
        popMenu.addTitle(SmallIcon("digikam"), i18n("My Tags"));

        QAction *gotoAction = popMenu.addAction(SmallIcon("goto"), i18n("&Move Here"));
        popMenu.addSeparator();
        popMenu.addAction(SmallIcon("cancel"), i18n("C&ancel"));
        popMenu.setMouseTracking(true);
        QAction *choice = popMenu.exec(QCursor::pos());

        if(choice == gotoAction)
        {
            TAlbum *newParentTag = 0;

            if (!itemDrop)
            {
                // move dragItem to the root
                newParentTag = d->albumMan->findTAlbum(0);
            }
            else
            {
                // move dragItem as child of dropItem
                newParentTag = itemDrop->getTag();
            }

            QString errMsg;
            if (!d->albumMan->moveTAlbum(talbum, newParentTag, errMsg))
            {
                KMessageBox::error(this, errMsg);
            }

            if(itemDrop && !itemDrop->isOpen())
                itemDrop->setOpen(true);
        }

        return;
    }

    if (ItemDrag::canDecode(e))
    {
        TAlbum *destAlbum = itemDrop->getTag();
        TAlbum *srcAlbum;

        KUrl::List      urls;
        KUrl::List      kioURLs;        
        Q3ValueList<int> albumIDs;
        Q3ValueList<int> imageIDs;

        if (!ItemDrag::decode(e, urls, kioURLs, albumIDs, imageIDs))
            return;

        if (urls.isEmpty() || kioURLs.isEmpty() || albumIDs.isEmpty() || imageIDs.isEmpty())
            return;

        // all the albumids will be the same
        int albumID = albumIDs.first();
        srcAlbum = d->albumMan->findTAlbum(albumID);
        if (!srcAlbum)
        {
            DWarning() << "Could not find source album of drag"
                       << endl;
            return;
        }

        if(srcAlbum == destAlbum)
        {
            // Setting the dropped image as the album thumbnail
            // If the ctrl key is pressed, when dropping the image, the
            // thumbnail is set without a popup menu
            bool set = false;
            if (e->keyboardModifiers() == Qt::ControlModifier)
            {
                set = true;
            }
            else
            {
                KMenu popMenu(this);
                popMenu.addTitle(SmallIcon("digikam"), i18n("My Tags"));
                QAction *setAction = popMenu.addAction(i18n("Set as Tag Thumbnail"));
                popMenu.addSeparator();
                popMenu.addAction( SmallIcon("cancel"), i18n("C&ancel") );

                popMenu.setMouseTracking(true);
                QAction *choice = popMenu.exec(QCursor::pos());
                set = (choice == setAction);
            }

            if(set)
            {
                QString errMsg;
                AlbumManager::instance()->updateTAlbumIcon(destAlbum, QString(),
                                                           imageIDs.first(), errMsg);
            }
            return;
        }

        // If a ctrl key is pressed while dropping the drag object,
        // the tag is assigned to the images without showing a
        // popup menu.
        bool assign = false;
        if (e->keyboardModifiers() == Qt::ControlModifier)
        {
            assign = true;
        }
        else
        {
            KMenu popMenu(this);
            popMenu.addTitle(SmallIcon("digikam"), i18n("My Tags"));
            QAction * assignAction =
                    popMenu.addAction( SmallIcon("tag"), i18n("Assign Tag '%1' to Items", destAlbum->prettyUrl()));
            popMenu.addSeparator();
            popMenu.addAction( SmallIcon("cancel"), i18n("C&ancel") );

            popMenu.setMouseTracking(true);
            QAction *choice = popMenu.exec(QCursor::pos());
            assign = (choice == assignAction);
        }

        if (assign)
        {
            emit signalProgressBarMode(StatusProgressBar::ProgressBarMode, 
                                       i18n("Assigning image tags. Please wait..."));

            int i=0;
            {
                DatabaseTransaction transaction;
                for (Q3ValueList<int>::const_iterator it = imageIDs.begin();
                    it != imageIDs.end(); ++it)
                {
                    // create temporary ImageInfo object
                    ImageInfo info(*it);

                    MetadataHub hub;
                    hub.load(info);
                    hub.setTag(destAlbum, true);
                    hub.write(info, MetadataHub::PartialWrite);
                    hub.write(info.filePath(), MetadataHub::FullWriteIfChanged);

                    emit signalProgressValue((int)((i++/(float)imageIDs.count())*100.0));
                    kapp->processEvents();
                }
            }

            emit signalProgressBarMode(StatusProgressBar::TextMode, QString());
        }
    }
}

void TagFolderView::selectItem(int id)
{
    TAlbum* tag = d->albumMan->findTAlbum(id);
    if(!tag)
        return;

    TagFolderViewItem *item =
            (TagFolderViewItem*)tag->extraData(this);
    if(item)
    {
        setSelected(item, true);
        ensureItemVisible(item);
    }
}

}  // namespace Digikam
