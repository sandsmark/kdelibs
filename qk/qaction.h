#ifndef QACTION_H
#define QACTION_H

#ifndef QT_H
#include "qobject.h"
#include "qiconset.h"
#include "qpixmap.h"
#include "qstring.h"
#include "qstringlist.h"
#include "qwidget.h"
#include "qdialog.h"
#include "qvaluelist.h"
#include "qfontdatabase.h"
#endif // QT_H

class QToolBar;
class QMenuBar;
class QPopupMenu;
class QComboBox;
class QPoint;
class QIconView;
class QIconViewItem;
class QPoint;

class QAction : public QObject
{
    Q_OBJECT
public:
    QAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );
    QAction( const QString& text, int accel,
	     const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QAction( const QString& text, const QIconSet& pix, int accel = 0,
	     QObject* parent = 0, const char* name = 0 );
    QAction( const QString& text, const QIconSet& pix, int accel,
	     const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QAction( QObject* parent = 0, const char* name = 0 );
    virtual ~QAction();

    virtual void update();

    virtual int plug( QWidget*, int index = -1 );
    virtual void unplug( QWidget* );

    virtual bool isPlugged() const;

//    QDomElement configuration( QDomDocument& doc, bool properties ) const;

    QWidget* container( int index );
    QWidget* representative( int index );
    int containerCount() const;

    // ##### todo setPixmap
    virtual QPixmap pixmap() const;

    virtual bool hasIconSet() const;
    virtual QString plainText() const;

    virtual QObject* component();
    virtual void setComponent( QObject* );

    virtual void setText( const QString& text );
    virtual QString text() const;

    virtual void setIconSet( const QIconSet& );
    virtual QIconSet iconSet() const;

    virtual void setWhatsThis( const QString& text );
    virtual QString whatsThis() const;

    virtual bool isEnabled() const;
    virtual void setEnabled( bool );

    virtual void setGroup( const QString& );
    virtual QString group() const;

    virtual void setAccel( int );
    virtual int accel() const;

    virtual void setToolTip( const QString& );
    virtual QString toolTip() const;

protected slots:
    virtual void slotDestroyed();
    virtual void slotActivated();

protected:
    QToolBar* toolBar( int index );
    QPopupMenu* popupMenu( int index );
    int menuId( int index );
    void removeContainer( int index );
    int findContainer( QWidget* widget );

    void addContainer( QWidget* parent, int id );
    void addContainer( QWidget* parent, QWidget* representative );

    virtual void setAccel( int id, int accel );
    virtual void setGroup( int id, const QString& grp );
    virtual void setEnabled( int id, bool enable );
    virtual void setToolTip( int id, const QString& tt );
    virtual void setText( int id, const QString& text );
    virtual void setIconSet( int id, const QIconSet& iconset );
    virtual void setWhatsThis( int id, const QString& text );

signals:
    void activated();
    void enabled( bool );

private:
    QObject* m_component;

    QString m_text;
    QString m_whatsThis;
    QString m_groupText;
    QPixmap m_pixmap;
    QIconSet m_iconSet;
    bool m_bIconSet;
    QString m_group;
    int m_accel;
    QString m_toolTip;

    struct Container
    {
	Container() { m_container = 0; m_representative = 0; m_id = 0; }
	Container( const Container& s ) { m_container = s.m_container;
	                                  m_id = s.m_id; m_representative = s.m_representative; }
	QWidget* m_container;
	int m_id;
	QWidget* m_representative;
    };

    QValueList<Container> m_containers;
    bool m_enabled;
};

class QActionSeparator : public QAction
{
    Q_OBJECT
public:
    QActionSeparator( QObject* parent = 0, const char* name = 0 );
    virtual ~QActionSeparator();

    virtual int plug( QWidget*, int index = -1 );
    virtual void unplug( QWidget* );
};

class QActionMenu : public QAction
{
    Q_OBJECT
public:
    QActionMenu( const QString& text, QObject* parent = 0, const char* name = 0 );
    QActionMenu( const QString& text, const QIconSet& icon, QObject* parent = 0, const char* name = 0 );
    QActionMenu( QObject* parent = 0, const char* name = 0 );
    virtual ~QActionMenu();

    virtual int plug( QWidget*, int index = -1 );
    virtual void unplug( QWidget* );

    virtual void insert( QAction*, int index = -1 );
    virtual void remove( QAction* );

    QPopupMenu* popupMenu();
    void popup( const QPoint& global );

//    virtual bool setConfiguration( const QDomElement& element );

private:
    QPopupMenu* m_popup;
};

class QToggleAction : public QAction
{
    Q_OBJECT
public:
    QToggleAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );
    QToggleAction( const QString& text, int accel,
		   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QToggleAction( const QString& text, const QIconSet& pix, int accel = 0,
	     QObject* parent = 0, const char* name = 0 );
    QToggleAction( const QString& text, const QIconSet& pix, int accel,
		   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QToggleAction( QObject* parent = 0, const char* name = 0 );

    int plug( QWidget*, int index = -1 );

    virtual void setChecked( bool );
    bool isChecked();

    virtual void setExclusiveGroup( const QString& name );
    virtual QString exclusiveGroup() const;

protected:
    virtual void setChecked( int id, bool checked );

protected slots:
    void slotActivated();

signals:
    void toggled( bool );

private:
    bool m_checked;
    bool m_lock;
    QString m_exclusiveGroup;
};

class QSelectAction : public QAction
{
    Q_OBJECT
public:
    QSelectAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );
    QSelectAction( const QString& text, int accel,
		   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QSelectAction( const QString& text, const QIconSet& pix, int accel = 0,
	     QObject* parent = 0, const char* name = 0 );
    QSelectAction( const QString& text, const QIconSet& pix, int accel,
		   const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QSelectAction( QObject* parent = 0, const char* name = 0 );

    void setEditable( bool );
    bool isEditable() const;

    int plug( QWidget*, int index = -1 );

    virtual void setItems( const QStringList& items );
    QStringList items();
    virtual void changeItem( int index, const QString& text );
    QString currentText();
    int currentItem();
    virtual void setCurrentItem( int index );
    virtual void clear();

    QPopupMenu* popupMenu();

protected:
    virtual void setCurrentItem( int id, int index );
    virtual void changeItem( int id, int index, const QString& text );
    virtual void setItems( int id, const QStringList& items );
    virtual void clear( int id );

protected slots:
    virtual void slotActivated( int );

signals:
    void activated( int index );
    void activated( const QString& text );

private:
    bool m_edit;
    bool m_lock;
    QStringList m_list;
    QPopupMenu* m_menu;
    int m_current;
};

class QFontAction : public QSelectAction
{
    Q_OBJECT
public:
    QFontAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );
    QFontAction( const QString& text, int accel,
		 const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QFontAction( const QString& text, const QIconSet& pix, int accel = 0,
	     QObject* parent = 0, const char* name = 0 );
    QFontAction( const QString& text, const QIconSet& pix, int accel,
		 const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QFontAction( QObject* parent = 0, const char* name = 0 );

private:
    QFontDatabase m_fdb;
};

class QFontSizeAction : public QSelectAction
{
    Q_OBJECT
public:
    QFontSizeAction( const QString& text, int accel = 0, QObject* parent = 0, const char* name = 0 );
    QFontSizeAction( const QString& text, int accel,
		     const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QFontSizeAction( const QString& text, const QIconSet& pix, int accel = 0,
	     QObject* parent = 0, const char* name = 0 );
    QFontSizeAction( const QString& text, const QIconSet& pix, int accel,
		     const QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );
    QFontSizeAction( QObject* parent = 0, const char* name = 0 );

    int plug( QWidget*, int index = -1 );

    void setFontSize( int size );
    int fontSize();

protected slots:
    virtual void slotActivated( int );
    virtual void slotActivated( const QString& );

signals:
    void fontSizeChanged( int );

private:
    void init();

    bool m_lock;
};

class QActionCollection : public QObject
{
    Q_OBJECT
public:
    QActionCollection( QObject* parent = 0, const char* name = 0 );
    QActionCollection( const QActionCollection& copy );
    ~QActionCollection();

    virtual void insert( QAction* );
    virtual void remove( QAction* );
    virtual QAction* take( QAction* );

    virtual QAction* action( int index );
    virtual uint count() const;
    virtual QAction* action( const char* name, const char* classname = 0, QObject* component = 0 );

    virtual QStringList groups() const;
    virtual QValueList<QAction*> actions( const QString& group ) const;
    virtual QValueList<QAction*> actions() const;

    QActionCollection operator+ (const QActionCollection& ) const;
    QActionCollection& operator= (const QActionCollection& );
    QActionCollection& operator+= (const QActionCollection& );

//    bool setConfiguration( const QDomElement& element );
//    QDomElement configuration( QDomDocument& doc, bool properties ) const;

signals:
    void inserted( QAction* );
    void removed( QAction* );

protected:
    void childEvent( QChildEvent* );

private:
    QList<QAction> m_actions;
};

/////// This needs to be ported to the new QIconView, to make it compile
/////// We don't use it currently,anyway.
/*
class QActionWidget : public QWidget
{
  //Q_OBJECT
public:
    QActionWidget( QWidget* parent = 0, const char* name = 0 );
    QActionWidget( QActionCollection*, QWidget* parent = 0, const char* name = 0 );
    ~QActionWidget();

    virtual QAction* currentAction();

    virtual void clearSelection();
    virtual QAction* selectedAction();

    virtual QActionCollection* collection();
    virtual void setCollection( QActionCollection* );

    virtual void addGroup( const QString& group );
    virtual QString currentGroup() const;
    virtual void setCurrentGroup( const QString& grp, bool update = FALSE );

    void updateAction( QAction* );

signals:
    void rightButtonPressed( QAction*, const QPoint& );
    void selectionChanged( QAction* );

    protected slots:
    void slotDropped( QDropEvent* );
    void insertAction( QAction* );
    void removeAction( QAction* );

private slots:
    void rightButtonPressed( QIconViewItem*, const QPoint& );
    void showGroup( const QString& grp );
    void selectionChanged();

private:
    void init();

private:
    QIconView* m_icons;
    QComboBox* m_group;
    QActionCollection* m_collection;
};

class QActionDialog : public QDialog
{
  //Q_OBJECT
public:
    QActionDialog( QActionCollection*, QWidget* parent = 0, const char* name = 0, bool modal = FALSE );
    ~QActionDialog();

    QActionWidget* actionWidget();

private:
    QActionWidget* m_widget;
};
*/

#endif
