/*  -*- c++ -*-
    This file is part of the KDE libraries
    Copyright (C) 1998 Stephan Kulow <coolo@kde.org>
                  1998 Daniel Grana <grana@ie.iwi.unibe.ch>
                  2000 Werner Trobin <wtrobin@carinthia.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef _KFILEPREVIEW_H
#define _KFILEPREVIEW_H

#include <kfileview.h>
#include <kfileviewitem.h>
#include <kfilepreview.h>
#include <kdirlistbox.h>
#include <kfileiconview.h>
#include <kfiledetailview.h>
#include <config-kfile.h>

#include <qsplitter.h>
#include <qwidget.h>

#include <qstring.h>
#include <qlabel.h>

#include <klocale.h>
#include <kdebug.h>
#include <kurl.h>

class KFilePreview : public QSplitter, public KFileView
{
    Q_OBJECT

public:
    KFilePreview(QWidget *parent, const char *name);
    virtual ~KFilePreview();

    virtual QWidget *widget() { return this; }
    virtual void clearView();

    virtual void updateView( bool );
    virtual void updateView(const KFileViewItem*);

    virtual void setSelectMode( KFileView::SelectionMode sm );

    virtual void clear();
    virtual void clearSelection();
    virtual bool isSelected( const KFileViewItem * ) const;

    void setPreviewWidget(const QWidget *w, const KURL &u);

signals:
    void showPreview(const KURL &);

protected:
    virtual void insertItem(KFileViewItem *);
    virtual void highlightItem(const KFileViewItem *);

protected slots:
    void activatedMenu(const KFileViewItem*);
    void selectDir(const KFileViewItem*);
    void highlightFile(const KFileViewItem*);
    void selectFile(const KFileViewItem*);

private:
    bool deleted, previewMode;
    KFileView *left;
    QWidget *preview;
    QString viewname;
};
#endif
