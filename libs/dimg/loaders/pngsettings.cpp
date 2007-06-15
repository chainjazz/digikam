/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2007-08-02
 * Description : save PNG image options.
 *
 * Copyright (C) 2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include <QString>
#include <QLabel>
#include <QLayout>
#include <QGridLayout>

// KDE includes.

#include <klocale.h>
#include <kdialog.h>
#include <knuminput.h>

// Local includes.

#include "pngsettings.h"
#include "pngsettings.moc"

namespace Digikam
{

class PNGSettingsPriv
{

public:

    PNGSettingsPriv()
    {
        PNGGrid             = 0;
        labelPNGcompression = 0;
        PNGcompression      = 0;
    }

    QGridLayout  *PNGGrid;

    QLabel       *labelPNGcompression;

    KIntNumInput *PNGcompression;
};

PNGSettings::PNGSettings(QWidget *parent)
           : QWidget(parent)
{
    d = new PNGSettingsPriv;
    setAttribute(Qt::WA_DeleteOnClose);

    d->PNGGrid        = new QGridLayout(this);
    d->PNGcompression = new KIntNumInput(9, this);
    d->PNGcompression->setRange(1, 9, 1, true );
    d->labelPNGcompression = new QLabel(i18n("PNG compression:"), this);

    d->PNGcompression->setWhatsThis( i18n("<p>The compression value for PNG images:<p>"
                                          "<b>1</b>: low compression (large file size but "
                                          "short compression duration - default)<p>"
                                          "<b>5</b>: medium compression<p>"
                                          "<b>9</b>: high compression (small file size but "
                                          "long compression duration)<p>"
                                          "<b>Note: PNG is always a lossless image "
                                          "compression format.</b>"));
    d->PNGGrid->addWidget(d->labelPNGcompression, 0, 0, 0, 0);
    d->PNGGrid->addWidget(d->PNGcompression, 0, 0, 1, 1);
    d->PNGGrid->setColumnStretch(1, 10);
    d->PNGGrid->setSpacing(KDialog::spacingHint());
}

PNGSettings::~PNGSettings()
{
    delete d;
}

void PNGSettings::setCompressionValue(int val)
{
    d->PNGcompression->setValue(val);
}

int PNGSettings::getCompressionValue()
{
    return d->PNGcompression->value();
}

}  // namespace Digikam
