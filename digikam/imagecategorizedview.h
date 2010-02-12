/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-04-22
 * Description : Qt item view for images
 *
 * Copyright (C) 2009 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#ifndef IMAGECATEGORIZEDVIEW_H
#define IMAGECATEGORIZEDVIEW_H

// Local includes

#include "imageinfo.h"
#include "dcategorizedview.h"
#include "thumbnailsize.h"

namespace Digikam
{

class Album;
class ImageAlbumModel;
class ImageAlbumFilterModel;
class ImageModel;
class ImageFilterModel;
class ImageDelegate;
class ImageDelegateOverlay;
class ImageThumbnailModel;
class ImageCategorizedViewPriv;

class ImageCategorizedView : public DCategorizedView
{
    Q_OBJECT

public:

    ImageCategorizedView(QWidget *parent = 0);
    ~ImageCategorizedView();

    void setModels(ImageModel *model, ImageFilterModel *filterModel);

    ImageModel *imageModel() const;
    ImageFilterModel *imageFilterModel() const;

    /// Returns 0 if the ImageModel is not an ImageThumbnailModel
    ImageThumbnailModel *imageThumbnailModel() const;

    /// Returns 0 if the ImageModel is not an ImageAlbumModel
    ImageAlbumModel *imageAlbumModel() const;
    ImageAlbumFilterModel *imageAlbumFilterModel() const;

    ImageDelegate *delegate() const;

    Album *currentAlbum() const;

    ImageInfo currentInfo() const;
    KUrl currentUrl() const;

    QList<ImageInfo> selectedImageInfos() const;
    QList<ImageInfo> selectedImageInfosCurrentFirst() const;
    KUrl::List selectedUrls() const;

    QList<ImageInfo> imageInfos() const;
    KUrl::List urls() const;

    /** Selects the index as current and scrolls to it */
    void toIndex(const KUrl& url);
    /** Returns the n-th info after the given one.
     *  Specifically, return the previous info for nth = -1
     *  and the next info for n = 1.
     *  Returns a null info if either startingPoint or the nth info are
     *  not contained in the model */
    ImageInfo nextInOrder(const ImageInfo &startingPoint, int nth);
    ImageInfo previousInfo(const ImageInfo &info) { return nextInOrder(info, -1); }
    ImageInfo nextInfo(const ImageInfo &info) { return nextInOrder(info, 1); }

    ThumbnailSize thumbnailSize() const;
    void setThumbnailSize(const ThumbnailSize& size);

    /** If the model is categorized by an album, returns the album of the category
     *  that contains the position.
     *  If this is not applicable, return the current album. May return 0. */
    Album *albumAt(const QPoint& pos);

    /// Add and remove an overlay. It will as well be removed automatically when destroyed.
    void addOverlay(ImageDelegateOverlay *overlay);
    void removeOverlay(ImageDelegateOverlay *overlay);

    void addSelectionOverlay();

public Q_SLOTS:

    void openAlbum(Album *album);

    void setThumbnailSize(int size);
    /** Scroll the view to the given item when it becomes available */
    void setCurrentWhenAvailable(qlonglong imageId);
    /** Set as current item the item identified by its file url */
    void setCurrentUrl(const KUrl& url);
    /** Set selected items identified by their file urls */
    void setSelectedUrls(const KUrl::List& urlList);

Q_SIGNALS:

    void currentChanged(const ImageInfo& info);
    /// Emitted when new items are selected. The parameter includes only the newly selected infos,
    /// there may be other already selected infos.
    void selected(const QList<ImageInfo>& newSelectedInfos);
    /// Emitted when items are deselected. There may be other selected infos left.
    /// This signal is not emitted when the model is reset; then only selectionCleared is emitted.
    void deselected(const QList<ImageInfo>& nowDeselectedInfos);

protected Q_SLOTS:

    void slotImageInfosAdded();

protected:

    /// install default ImageAlbumModel and filter model, ready for use
    void installDefaultModels();

    /// Reimplement these in a subclass
    virtual void activated(const ImageInfo& info);
    virtual void showContextMenuOnInfo(QContextMenuEvent *event, const ImageInfo& info);
    virtual void showContextMenuOnIndex(QContextMenuEvent *event, const QModelIndex& index);

    // reimplemented from parent class
    void setItemDelegate(ImageDelegate *delegate);
    void indexActivated(const QModelIndex& index);
    QSortFilterProxyModel *filterModel() const;
    void currentChanged(const QModelIndex& index, const QModelIndex& previous);
    void paintEvent(QPaintEvent *e);
    void selectionChanged(const QItemSelection &, const QItemSelection &);
    ImageModelDragDropHandler *dragDropHandler() const;

private Q_SLOTS:

    void slotFileChanged(const QString& filePath);

private:

    void scrollToStoredItem();

private:

    ImageCategorizedViewPriv* const d;
};

} // namespace Digikam

#endif /* IMAGECATEGORIZEDVIEW_H */
