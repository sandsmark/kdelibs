/*
 * This file is part of the KDE Libraries
 * Copyright (C) 1999-2000 Mirko Sucker (mirko@kde.org) and 
 * Espen Sand (espen@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef _KABOUTDIALOG_PRIVATE_H_
#define _KABOUTDIALOG_PRIVATE_H_

#include <qlabel.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtabwidget.h>

/**
 * @internal
 */
class KImageTrackLabel : public QLabel
{
  Q_OBJECT

  public:
    enum MouseMode
    {
      MousePress = 1,
      MouseRelease,
      MouseDoubleClick,
      MouseMove
    };

  public:
    KImageTrackLabel( QWidget * parent, const char * name=0, WFlags f=0 );

  signals:
    void mouseTrack( int mode, const QMouseEvent *e );

  protected:
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mouseDoubleClickEvent( QMouseEvent *e );
    virtual void mouseMoveEvent ( QMouseEvent *e );
};

class KAboutContainerPrivate;
/**
 *  @internal
 */
class KAboutContainer : public QFrame
{
  Q_OBJECT

  public:
    KAboutContainer( QWidget *parent=0, const char *name=0,
		     int margin=0, int spacing=0,
		     int childAlignment = AlignCenter,
		     int innerAlignment = AlignCenter );

    void addWidget( QWidget *widget );
    void addPerson( const QString &name, const QString &email,
		    const QString &url, const QString &task,
		    bool showHeader = false, bool showframe = false,
		    bool showBold = false );
    void addTitle(  const QString &title, int alignment=AlignLeft,
		    bool showframe = false, bool showBold = false );
    void addImage( const QString &fileName, int alignment=AlignLeft );

    virtual QSize sizeHint( void ) const;
    virtual QSize minimumSizeHint( void ) const;

  protected:
    virtual void childEvent( QChildEvent *e );

  signals:
    void urlClick( const QString &url );
    void mailClick( const QString &name, const QString &address );


  private:
    QVBoxLayout *mVbox;
    int mAlignment;
    KAboutContainerPrivate *d;
};

class KAboutContainerBasePrivate;
/**
 *  @internal
 */
class KAboutContainerBase : public QWidget
{
  Q_OBJECT

  public:
    enum LayoutType
    {
      AbtPlain         = 0x0001,
      AbtTabbed        = 0x0002,
      AbtTitle         = 0x0004,
      AbtImageLeft     = 0x0008,
      AbtImageRight    = 0x0010,
      AbtImageOnly     = 0x0020,
      AbtProduct       = 0x0040,
      AbtKDEStandard   = AbtTabbed|AbtTitle|AbtImageLeft,
      AbtAppStandard   = AbtTabbed|AbtTitle|AbtProduct,
      AbtImageAndTitle = AbtPlain|AbtTitle|AbtImageOnly
    };

  public:
    KAboutContainerBase( int layoutType, QWidget *parent = 0, char *name = 0 );
    virtual void show( void );
    virtual QSize sizeHint( void ) const;

    void setTitle( const QString &title );
    void setImage( const QString &fileName );
    void setImageBackgroundColor( const QColor &color );
    void setImageFrame( bool state );
    void setProduct( const QString &appName, const QString &version,
		     const QString &author, const QString &year );

    QFrame *addTextPage( const QString &title, const QString &text,
			 bool richText=false, int numLines=10 );
    KAboutContainer *addContainerPage( const QString &title,
      int childAlignment = AlignCenter, int innerAlignment = AlignCenter );
    KAboutContainer *addScrolledContainerPage( const QString &title,
      int childAlignment = AlignCenter, int innerAlignment = AlignCenter );

    QFrame *addEmptyPage( const QString &title );

    KAboutContainer *addContainer( int childAlignment, int innerAlignment );

  public slots:
    virtual void slotMouseTrack( int mode, const QMouseEvent *e );
    virtual void slotUrlClick( const QString &url );
    virtual void slotMailClick( const QString &name, const QString &address );

  protected:
    virtual void fontChange( const QFont &oldFont );

  signals:
    void mouseTrack( int mode, const QMouseEvent *e );
    void urlClick( const QString &url );
    void mailClick( const QString &name, const QString &address );

  private:
    QArray<QWidget*>  mContainerList;

    QVBoxLayout *mTopLayout;
    KImageTrackLabel *mImageLabel;
    QLabel  *mTitleLabel;
    QLabel  *mIconLabel;
    QLabel  *mVersionLabel;
    QLabel  *mAuthorLabel;
    QFrame  *mImageFrame;
    QTabWidget *mPageTab;
    QFrame  *mPlainSpace;

    KAboutContainerBasePrivate *d;
};

class KAboutContributorPrivate;
/**
 *  @internal
 */
class KAboutContributor : public QFrame
{
  Q_OBJECT

  public:
    KAboutContributor( QWidget *parent=0, const char *name=0,
		       const QString &username=QString::null,
		       const QString &email=QString::null,
		       const QString &url=QString::null,
		       const QString &work=QString::null,
		       bool showHeader=false, bool showFrame=true,
		       bool showBold=false );

    void setName( const QString &text, const QString &header=QString::null,
		  bool update = true );
    void setEmail( const QString &text, const QString &header=QString::null,
		   bool update = true );
    void setURL( const QString &text, const QString &header=QString::null,
		 bool update = true );
    void setWork( const QString &text, const QString &header=QString::null,
		  bool update = true );
    QString getName( void );
    QString getEmail( void );
    QString getURL( void );
    QString getWork( void );

    virtual QSize sizeHint( void ) const;

  protected:
    virtual void fontChange( const QFont &oldFont );

  protected slots:
    void urlClickedSlot( const QString& );
    void emailClickedSlot( const QString& emailaddress );

  private:
    void updateLayout( void );

  signals:
    void sendEmail(const QString& name, const QString& email);
    void openURL(const QString& url);

  private:
    QLabel *mLabel[4];
    QLabel *mText[4];
    bool mShowHeader;
    bool mShowBold;

    KAboutContributorPrivate *d;
};

#endif
