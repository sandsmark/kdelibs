/* This file is part of the KDE project
 *
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 *                     1999 Lars Knoll <knoll@kde.org>
 *                     1999 Antti Koivisto <koivisto@kde.org>
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
 */

#include "khtmlview.moc"
#include "khtml_part.h"

#include "misc/loader.h"
#include "html/html_documentimpl.h"
#include "html/html_miscimpl.h"
#include "html/html_inlineimpl.h"
#include "dom/dom2_range.h"
#include "rendering/render_object.h"
#include "rendering/render_root.h"
#include "misc/htmlhashes.h"
#include "xml/dom_textimpl.h"

#include <qdragobject.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qevent.h>
#include <qdatetime.h>
#include <qclipboard.h>

#include <kapp.h>
#include <kmimetype.h>
#include <kimageio.h>
#include <kdebug.h>
#include <kglobalsettings.h>
#include <kcursor.h>

#include <qlist.h>
#include <qrect.h>
#include <qscrollview.h>
#include <qwidget.h>
#include <qapplication.h>

#include <qprinter.h>
#include <qpaintdevicemetrics.h>

#define SCROLLBARWIDTH 16

#define PAINT_BUFFER_HEIGHT 150

template class QList<KHTMLView>;

QList<KHTMLView> *KHTMLView::lstViews = 0L;

using namespace DOM;

QPixmap* KHTMLView::paintBuffer = 0L;

class KHTMLViewPrivate {
public:
    KHTMLViewPrivate()
    {
	selectionStart = 0;
	selectionEnd = 0;
	startOffset = 0;
	endOffset = 0;
	startBeforeEnd = true;
	underMouse = 0;
	linkPressed = false;
	currentNode = 0;
 	originalNode= 0;
 	tabIndex=-1;

    }
    NodeImpl *selectionStart;
    int startOffset;
    NodeImpl *selectionEnd;
    int endOffset;
    bool startBeforeEnd;
    NodeImpl *underMouse;

    NodeImpl *currentNode;
    NodeImpl *originalNode;

    bool linkPressed;
    short tabIndex;

    QPoint m_dragStartPos;

    bool m_bDnd;
};


KHTMLView::KHTMLView( KHTMLPart *part, QWidget *parent, const char *name)
    : QScrollView( parent, name)
{
    m_part = part;

    // initialize QScrollview

    enableClipper(true);

    viewport()->setMouseTracking(true);
    viewport()->setBackgroundMode(NoBackground);

    KImageIO::registerFormats();

    setCursor(arrowCursor);
    linkCursor = KCursor::handCursor();
    init();

    viewport()->show();

    setVScrollBarMode(Auto);
    setHScrollBarMode(Auto);

    d = new KHTMLViewPrivate;
    d->m_bDnd = true;
}

KHTMLView::~KHTMLView()
{
  lstViews->removeRef( this );
  if(lstViews->isEmpty())
  {
      delete lstViews;
      lstViews = 0;
      if(paintBuffer) delete paintBuffer;
      paintBuffer = 0;
  }

  delete d;
}

void KHTMLView::init()
{
    if ( lstViews == 0L )
	lstViews = new QList<KHTMLView>;
    lstViews->setAutoDelete( FALSE );
    lstViews->append( this );

    if(!paintBuffer) paintBuffer = new QPixmap();

  viewport()->setFocusPolicy( QWidget::WheelFocus );

  _marginWidth = -1; // undefined
  _marginHeight = -1;
  _width = width()- SCROLLBARWIDTH - 2*marginWidth();
  _height = 0;

  resizeContents(clipper()->width(), clipper()->height());
}

void KHTMLView::clear()
{
    resizeContents(clipper()->width(), clipper()->height());

    pressed = false;

    //setVScrollBarMode(Auto);
    //setHScrollBarMode(Auto);

    d->selectionStart = 0;
    d->selectionEnd = 0;
    d->startOffset = 0;
    d->endOffset = 0;
    d->underMouse = 0;
    d->currentNode = 0;
    d->linkPressed = false;
}

void KHTMLView::resizeEvent ( QResizeEvent * event )
{
//    kdDebug( 6000 ) << "resizeEvent" << endl;
    layout();

    DOM::HTMLDocumentImpl *doc = m_part->docImpl();

    if ( doc && doc->body() )
      resizeContents( doc->renderer()->width(), doc->renderer()->height() );

    QScrollView::resizeEvent(event);
}



void KHTMLView::viewportPaintEvent ( QPaintEvent* pe  )
{
    QRect r = pe->rect();

//    kdDebug( 6000 ) << "viewportPaintEvent r x=" << //    	r.x() << ",y=" << r.y() << ",w=" << r.width() << ",h=" << r.height() << endl;

    NodeImpl *body = 0;

    if( m_part->docImpl() )
	body = m_part->docImpl()->body();

    QRect rr(
	-viewport()->x(), -viewport()->y(),
	clipper()->width(), clipper()->height()
    );
    r &= rr;
    int ex = r.x() + viewport()->x() + contentsX();
    int ey = r.y() + viewport()->y() + contentsY();
    int ew = r.width();
    int eh = r.height();

    if (ew<0)	    	// events generated with repaint() are bit weird...
    	ew = pe->rect().width();
    if (eh<0)
    	eh = pe->rect().height();

    if(!body)
    {
	QPainter p(viewport());

	p.fillRect(r.x(), r.y(), ew, eh, palette().normal().brush(QColorGroup::Background));
	return;
    }
//    kdDebug( 6000 ) << "viewportPaintEvent x=" << ex << ",y=" << ey << ",w=" << ew << ",h=" << eh << endl;

    if ( paintBuffer->width() < width() )
    {
        paintBuffer->resize(width(),PAINT_BUFFER_HEIGHT);
	QPainter p(paintBuffer);
	p.fillRect(r.x(), r.y(), ew, eh, palette().normal().brush(QColorGroup::Background));
    }

    QTime qt;
    qt.start();

    int py=0;
    while (py < eh)
    {
	QPainter* tp = new QPainter;
	tp->begin( paintBuffer );
	tp->translate(-ex,-ey-py);

    	int ph = eh-py<PAINT_BUFFER_HEIGHT ? eh-py : PAINT_BUFFER_HEIGHT;	

	// ### fix this for frames...

	tp->fillRect(ex, ey+py, ew, ph, palette().normal().brush(QColorGroup::Background));

	m_part->docImpl()->renderer()->print(tp, ex, ey+py, ew, ph, 0, 0);

	
	drawContents(tp,ex,ey+py,ew,ph); // in case someone want to extend the widget
	tp->end();

	delete tp;

    	//kdDebug( 6000 ) << "bitBlt x=" << ex << ",y=" << ey+py << ",sw=" << ew << ",sh=" << ph << endl;
	bitBlt(viewport(),r.x(),r.y()+py,paintBuffer,0,0,ew,ph,Qt::CopyROP);
	
	py += PAINT_BUFFER_HEIGHT;
    }

    //kdDebug( 6000 ) << "TIME: print() dt=" << qt.elapsed() << endl;
}

void KHTMLView::layout(bool force)
{
    //### take care of frmaes (hide scrollbars,...)

    if( m_part->docImpl() )
    {	
        DOM::HTMLDocumentImpl *document = m_part->docImpl();

	NodeImpl *body = document->body();
	if(body && body->id() == ID_FRAMESET)
	{
	    setVScrollBarMode(AlwaysOff);
	    setHScrollBarMode(AlwaysOff);
	    _width = width();
	
	    document->renderer()->setMinWidth(_width);
	    document->renderer()->layout(true);
	    return;
	}

	int w = width() - SCROLLBARWIDTH - 2*marginWidth();
	int h = visibleHeight();

    	if (w != _width || h != _height || force)
	{
	    //kdDebug( 6000 ) << "layouting document" << endl;

    	    _width = w;
	    _height = h;

	    QTime qt;
	    qt.start();

	    document->renderer()->setMinWidth(_width);
	    document->renderer()->layout(true);
	    resizeContents(document->renderer()->width(), document->renderer()->height());
	    kdDebug( 6000 ) << "TIME: layout() dt=" << qt.elapsed() << endl;

	    viewport()->repaint(false);
	}
	else
	{
	    document->renderer()->layout(false);
	    viewport()->repaint(false);
	}
    }
    else
    {
	_width = width() - SCROLLBARWIDTH - 2*marginWidth();
    }
}

void KHTMLView::paintElement( khtml::RenderObject *o, int xPos, int yPos )
{
    int yOff = contentsY();
    if(yOff > yPos + o->height() ||
       yOff + visibleHeight() < yPos)
	return;

    QWidget *vp = viewport();
    QPainter p(vp);
    int xOff = contentsX()+vp->x();
    yOff += vp->y();
    p.translate( -xOff, -yOff );

    o->printObject( &p , xOff, yOff, vp->width(), vp->height(),
		    xPos , yPos );
}

//
// Event Handling
//
/////////////////

void KHTMLView::viewportMousePressEvent( QMouseEvent *_mouse )
{
    if(!m_part->docImpl()) return;

    d->m_dragStartPos = _mouse->pos();

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    //kdDebug( 6000 ) << "\nmousePressEvent: x=" << xm << ", y=" << ym << endl;


    // Make this frame the active one
    // ### need some visual indication for the active frame.
    /* ### use PartManager (Simon)
    if ( _isFrame && !_isSelected )
    {
	kdDebug( 6000 ) << "activating frame!" << endl;
	topView()->setFrameSelected(this);
    }*/

    DOMString url;
    NodeImpl *innerNode=0;
    long offset=0;
    m_part->docImpl()->mouseEvent( xm, ym, _mouse->stateAfter(), DOM::NodeImpl::MousePress, 0, 0, url, innerNode, offset );

    kdDebug( 6000 ) << "Her har vi long'n: " << offset << " " << endl;
    if(m_part->mousePressHook(_mouse, xm, ym, url, Node(innerNode), offset)) return;

    if(url != 0)
    {
	//kdDebug( 6000 ) << "mouseEvent: overURL " << url.string() << endl;
	m_strSelectedURL = url.string();
    }
    else
	m_strSelectedURL = QString::null;

    if ( _mouse->button() == LeftButton || _mouse->button() == MidButton )
    {
    	pressed = TRUE;
	if(_mouse->button() == LeftButton) {
    	    if(innerNode) {
		d->selectionStart = innerNode;
		d->startOffset = offset;
		d->selectionEnd = innerNode;
		d->endOffset = offset;
		m_part->docImpl()->clearSelection();
		kdDebug( 6000 ) << "setting start of selection to " << innerNode << "/" << offset << endl;
	    } else {
		d->selectionStart = 0;
		d->selectionEnd = 0;
	    }
	    // ### emit some signal
	    emit selectionChanged();
	}
    }

    if( _mouse->button() == RightButton )
    {
	m_part->popupMenu( m_strSelectedURL );
    }
    else if ( _mouse->button() == MidButton )
    {
      KURL u( m_part->url(), m_strSelectedURL );
      if ( !u.isMalformed() )
        emit m_part->browserExtension()->createNewWindow( u );
      m_strSelectedURL = QString::null; //reset it, in order to avoid opening the url in the current window! (in mouseReleaseEvent) (Simon)
    }
}

void KHTMLView::viewportMouseDoubleClickEvent( QMouseEvent *_mouse )
{
    if(!m_part->docImpl()) return;

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    kdDebug( 6000 ) << "\nmouseDblClickEvent: x=" << xm << ", y=" << ym << endl;

    DOMString url;
    NodeImpl *innerNode=0;
    long offset=0;
    m_part->docImpl()->mouseEvent( xm, ym, _mouse->stateAfter(), DOM::NodeImpl::MouseDblClick, 0, 0, url, innerNode, offset );

    if(m_part->mouseDoubleClickHook(_mouse, xm, ym, url, Node(innerNode), offset)) return;

    // ###
    //if ( url.length() )
    //emit doubleClick( url.string(), _mouse->button() );
}

void KHTMLView::viewportMouseMoveEvent( QMouseEvent * _mouse )
{
    if(!m_part->docImpl()) return;


    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    DOMString url;
    NodeImpl *innerNode=0;
    long offset=0;
    m_part->docImpl()->mouseEvent( xm, ym, _mouse->stateAfter(), DOM::NodeImpl::MouseMove, 0, 0, url, innerNode, offset );

    d->underMouse = innerNode;
    if(m_part->mouseMoveHook(_mouse, xm, ym, url, Node(innerNode), offset)) return;

    // drag of URL

    if(pressed && !m_strSelectedURL.isEmpty() &&
       ( d->m_dragStartPos - _mouse->pos() ).manhattanLength() > KGlobalSettings::dndEventDelay() &&
       d->m_bDnd )
    {
	QStringList uris;
	KURL u( m_part->completeURL( m_strSelectedURL) );
	uris.append( u.url() );
	QUriDrag *drag = new QUriDrag( viewport() );
	drag->setUnicodeUris( uris );
	
	QPixmap p = KMimeType::pixmapForURL(u, 0, KIcon::SizeMedium);
	
	if ( !p.isNull() )
    	  drag->setPixmap(p);
	else
	  kdDebug( 6000 ) << "null pixmap" << endl;
 	
	drag->drag();

	// when we finish our drag, we need to undo our mouse press
	pressed = false;
        m_strSelectedURL = "";
	return;
    }



    if ( !pressed && url.length() )
    {
	QString surl = url.string();
	if ( overURL.isEmpty() )
	{
	    setCursor( linkCursor );
	    overURL = surl;
	    m_part->overURL( overURL );
	}
	else if ( overURL != surl )
	{
	    m_part->overURL( overURL );
	    overURL = surl;
	}
	return;
    }
    else if( overURL.length() && !url.length() )
    {
	setCursor( arrowCursor );
	m_part->overURL( QString::null );
	overURL = "";
    }

    // selection stuff
    if( pressed && innerNode && innerNode->isTextNode()) {
	d->selectionEnd = innerNode;
	d->endOffset = offset;
	kdDebug( 6000 ) << "setting end of selection to " << innerNode << "/" << offset << endl;

	// we have to get to know if end is before start or not...
	NodeImpl *n = d->selectionStart;
	d->startBeforeEnd = false;
	while(n) {
	    if(n == d->selectionEnd) {
		d->startBeforeEnd = true;
		break;
	    }
	    NodeImpl *next = n->firstChild();
	    if(!next) next = n->nextSibling();
	    while( !next && (n = n->parentNode()) ) {
		next = n->nextSibling();
	    }
	    n = next;
	    //viewport()->repaint(false);
	}
	
	if (d->selectionEnd == d->selectionStart && d->endOffset < d->startOffset)
	     m_part->docImpl()
	    	->setSelection(d->selectionStart,d->endOffset,d->selectionEnd,d->startOffset);
	else if (d->startBeforeEnd)
	    m_part->docImpl()
	    	->setSelection(d->selectionStart,d->startOffset,d->selectionEnd,d->endOffset);
	else
	    m_part->docImpl()
	    	->setSelection(d->selectionEnd,d->endOffset,d->selectionStart,d->startOffset);
	
    }
}

void KHTMLView::viewportMouseReleaseEvent( QMouseEvent * _mouse )
{
    if ( !m_part->docImpl() ) return;

    int xm, ym;
    viewportToContents(_mouse->x(), _mouse->y(), xm, ym);

    //kdDebug( 6000 ) << "\nmouseReleaseEvent: x=" << xm << ", y=" << ym << endl;

    DOMString url=0;
    NodeImpl *innerNode=0;
    long offset;
    m_part->docImpl()->mouseEvent( xm, ym, _mouse->stateAfter(), DOM::NodeImpl::MouseRelease, 0, 0, url, innerNode, offset );

    if(m_part->mouseReleaseHook(_mouse, xm, ym, url, Node(innerNode), offset)) return;


    if ( pressed )
    {
	// in case we started an autoscroll in MouseMove event
	// ###
	//stopAutoScrollY();
	//disconnect( this, SLOT( slotUpdateSelectText(int) ) );
    }

    // Used to prevent mouseMoveEvent from initiating a drag before
    // the mouse is pressed again.
    pressed = false;


    if ( !m_strSelectedURL.isEmpty() && _mouse->button() != RightButton )
    {
	KURL u(m_strSelectedURL);
	QString pressedTarget;
	if(u.protocol() == "target")
	{
	    m_strSelectedURL = u.ref();
	    pressedTarget = u.host();
	}	
	kdDebug( 6000 ) << "m_strSelectedURL='" << m_strSelectedURL << "' target=" << pressedTarget << endl;

	m_part->urlSelected( m_strSelectedURL, _mouse->button(), _mouse->state(), pressedTarget );
   }
	//###: is this neccessary? m_part->openURL(u);

    if(innerNode && innerNode->isTextNode()) {
	kdDebug( 6000 ) << "final range of selection to " << d->selectionStart << "/" << d->startOffset << " --> " << innerNode << "/" << offset << endl;
	d->selectionEnd = innerNode;
	d->endOffset = offset;
    }

    // delete selection in case start and end position are at the same point
    if(d->selectionStart == d->selectionEnd && d->startOffset == d->endOffset) {
	d->selectionStart = 0;
	d->selectionEnd = 0;
	d->startOffset = 0;
	d->endOffset = 0;
	emit selectionChanged();
    } else {
	// we have to get to know if end is before start or not...
	NodeImpl *n = d->selectionStart;
	d->startBeforeEnd = false;
	while(n) {
	    if(n == d->selectionEnd) {
		d->startBeforeEnd = true;
		break;
	    }
	    NodeImpl *next = n->firstChild();
	    if(!next) next = n->nextSibling();
	    while( !next && (n = n->parentNode()) ) {
		next = n->nextSibling();
	    }	
	    n = next;
	}
	if(!d->startBeforeEnd)
	{
	    NodeImpl *tmpNode = d->selectionStart;
	    int tmpOffset = d->startOffset;
	    d->selectionStart = d->selectionEnd;
	    d->startOffset = d->endOffset;
	    d->selectionEnd = tmpNode;
	    d->endOffset = tmpOffset;
	    d->startBeforeEnd = true;
	}
	// get selected text and paste to the clipboard
	QString text = selectedText();
	QClipboard *cb = QApplication::clipboard();
	cb->setText(text);
	//kdDebug( 6000 ) << "selectedText = " << text << endl;
	emit selectionChanged();
    }
}

QString KHTMLView::selectedText() const
{
    QString text;
    NodeImpl *n = d->selectionStart;
    while(n) {
	if(n->isTextNode()) {
	    QString str = static_cast<TextImpl *>(n)->data().string();
	    if(n == d->selectionStart && n == d->selectionEnd)
		text = str.mid(d->startOffset, d->endOffset - d->startOffset);
	    else if(n == d->selectionStart)
		text = str.mid(d->startOffset);
	    else if(n == d->selectionEnd)
		text += str.left(d->endOffset);
	    else
		text += str;
	}
	else if(n->id() == ID_BR)
	    text += "\n";
	else if(n->id() == ID_P || n->id() == ID_TD)
	    text += "\n\n";
	if(n == d->selectionEnd) break;
	NodeImpl *next = n->firstChild();
	if(!next) next = n->nextSibling();
	while( !next && (n = n->parentNode()) ) {
	    next = n->nextSibling();
	}

	n = next;
    }
    return text;
}

bool KHTMLView::hasSelection() const
{
  return ( d->selectionStart != 0 && d->selectionEnd != 0 );
}

void KHTMLView::keyPressEvent( QKeyEvent *_ke )
{
    if(m_part->keyPressHook(_ke)) return;


    int offs = (clipper()->height() < 30) ? clipper()->height() : 30;
    switch ( _ke->key() )
    {
    case Key_Down:
    case Key_J:
	scrollBy( 0, 10 );
	break;

    case Key_Next:
    case Key_Space:
	scrollBy( 0, clipper()->height() - offs );
	break;

    case Key_Up:
    case Key_K:
	scrollBy( 0, -10 );
	break;

    case Key_Prior:
	scrollBy( 0, -clipper()->height() + offs );
	break;
    case Key_Right:
    case Key_L:
	scrollBy( 10, 0 );
	break;
    case Key_Left:
    case Key_H:
	scrollBy( -10, 0 );
	break;
    case Key_N:
        gotoNextLink();
        break;
    case Key_P:
        gotoPrevLink();
        break;
    case Key_Enter:
    case Key_Return:
        toggleActLink(false);
        break;
    default:
	QScrollView::keyPressEvent( _ke );
    }
}

void KHTMLView::keyReleaseEvent( QKeyEvent *_ke )
{
    if(m_part->keyReleaseHook(_ke)) return;
     switch(_ke->key())
       {
       case Key_Enter:
       case Key_Return:
	 toggleActLink(true);
	 break;
       default:
    QScrollView::keyReleaseEvent( _ke);
       }
}

bool KHTMLView::focusNextPrevChild( bool next )
{
    return QScrollView::focusNextPrevChild( next );
}

void KHTMLView::drawContents ( QPainter * p, int clipx, int clipy, int clipw, int cliph )
{
    m_part->drawContentsHook(p);
}


DOM::NodeImpl *KHTMLView::nodeUnderMouse() const
{
    return d->underMouse;
}

bool KHTMLView::gotoLink()
{
    // let's ignore non anchors for the moment
    if(!d->currentNode || d->currentNode->id() != ID_A) return false;
    HTMLAnchorElementImpl *n=static_cast<HTMLAnchorElementImpl*>(d->currentNode);
    //printf("current item:%s\n", n->areaHref().string().latin1());

  if (d->linkPressed)
    n->setKeyboardFocus(DOM::ActivationActive);
  else
    n->setKeyboardFocus(DOM::ActivationPassive);

//calculate x- and ypos
  int x = 0, y = 0;
  n->getAnchorPosition(x,y);
    ensureVisible(x, y);

  return true;
}

bool KHTMLView::gotoNextLink()
{
    if(d->currentNode) d->currentNode->setKeyboardFocus(DOM::ActivationOff);

    if (d->tabIndex!=-1)
      {
	d->tabIndex++;
	if (d->tabIndex>m_part->docImpl()->findHighestTabIndex())
	  d->tabIndex=0;
      }
    // find next link
    NodeImpl *n = d->currentNode;
    NodeImpl *begin = 0;
    if(!n) n = m_part->docImpl()->body();
    while(n) {
	// find next Node
	if(n->firstChild())
	    n = n->firstChild();
	else if (n->nextSibling())
	    n = n->nextSibling();
	else {
	    NodeImpl *next = 0;
	    while(!next) {
		n = n->parentNode();
		if(!n) {
		    d->currentNode = 0;
		    return false;
		}
		next = n->nextSibling();
	    }
	    n = next;
	}
	if(n->id() == ID_A) {
	    //here, additional constraints for the previous link are checked.
	    HTMLAreaElementImpl *a=static_cast<HTMLAreaElementImpl *>(n);
	    if ((d->tabIndex==-1)||((a->tabIndex()==d->tabIndex)))
	      {
		d->currentNode = n;
		return gotoLink();
	      }
	    else if (!begin)
	      {
		begin=n;
	      }
	    else if (begin==n)
	      {
		if (d->tabIndex<m_part->docImpl()->findHighestTabIndex())
		  d->tabIndex=-1;
		else
		  {
		    d->currentNode=n;
		    return gotoLink();
		  }
	      }
	}
    }
    return false;
}

bool KHTMLView::gotoPrevLink()
{
    if(d->currentNode) d->currentNode->setKeyboardFocus(DOM::ActivationOff);

    // ###
    if(d->tabIndex!=-1)
      {
	d->tabIndex--;
	if (d->tabIndex==-1)
	  {
	    //<tobias>
	    //in case execution reaches this point, the highest tabindex of the
	    //document needs to be determined. this is a function that IMO
	    //should be implemented in HTMLDocumentImpl. i placed it there
	    //under the name "findHighestTabIndex()".
	    //Lars: is that desirable?
	    //if so, remove this comment.
	    //please remove the function itself and the call in any other case.
	    //</tobias>
	    d->tabIndex=m_part->docImpl()->findHighestTabIndex();
	  }
      }
    // find next link
    NodeImpl *n = d->currentNode;
    NodeImpl *begin=0;
    if(!n) n = m_part->docImpl()->body();

    while(n) {
	// find next Node
	if(n->lastChild())
	    n = n->lastChild();
	else if (n->previousSibling())
	    n = n->previousSibling();
	else {
	    NodeImpl *prev = 0;
	    while(!prev) {
		n = n->parentNode();
		if(!n) {
		    d->currentNode = 0;
		    return false;
		}
		prev = n->previousSibling();
	    }
	    n = prev;
	}
	// ### add handling of form elements here!
	if(n->id() == ID_A) {
	    //here, additional constraints for the previous link are checked.
	    HTMLAreaElementImpl *a=static_cast<HTMLAreaElementImpl *>(n);
	    if ((d->tabIndex==-1)||(a->tabIndex()==d->tabIndex))
	      {
		d->currentNode = n;
		return gotoLink();
	      }
	    else if (!begin)
	      {
		begin=n;
	      }
	    else if (begin==n)
	      {
		if (d->tabIndex>0)
		  d->tabIndex--;
		else // if (d->tabIndex==0)
		  {
		    d->currentNode=n;
		    return gotoLink();
		  }
	      }
    }
    }
    return false;
    //} else {
    //  NodeImpl *n=d->currentNode;
    // ### insert search for tabindex here
    //}
}

void KHTMLView::print()
{
    if(!m_part->docImpl()) return;
    khtml::RenderRoot *root = static_cast<khtml::RenderRoot *>(m_part->docImpl()->renderer());
    if(!root) return;

    QPrinter *printer = new QPrinter;
    if(printer->setup(this)) {
	// set up QPrinter
	printer->setFullPage(true);
	printer->setCreator("KDE 2.0 HTML Library");
	//printer->setDocName(m_part->url());
	
	QPainter *p = new QPainter;
	p->begin( printer );
	
	QPaintDeviceMetrics metrics( printer );
	
	// this is a simple approximation... we layout the document
	// according to the width of the page, then just cut
	// pages without caring about the content. We should do better
	// in the future, but for the moment this is better than no
	// printing support
	kdDebug(6000) << "printing: physical page width = " << metrics.width()
		      << " height = " << metrics.height() << endl;
	root->setPrintingMode(true);
	root->setWidth(metrics.width());
	root->setMinWidth(metrics.width());
	root->setMaxWidth(metrics.width());
	QValueList<int> oldSizes = m_part->fontSizes();

	const int printFontSizes[] = { 6, 7, 8, 10, 12, 14, 18, 24,
				       28, 34, 40, 48, 56, 68, 82, 100, 0 };
	QValueList<int> fontSizes;
	for ( int i = 0; printFontSizes[i] != 0; i++ )
	    fontSizes << printFontSizes[ i ];
	m_part->setFontSizes(fontSizes);
	m_part->docImpl()->applyChanges();

	// ok. now print the pages.
	kdDebug(6000) << "printing: html page width = " << root->width()
		      << " height = " << root->height() << endl;
	// if the width is too large to fit on the paper we just scale
	// the whole thing.
	int pageHeight = metrics.height();
	int pageWidth = metrics.width();
	if(root->width() > metrics.width()) {
	    double scale = ((double) metrics.width())/((double) root->width());
	    p->scale(scale, scale);
	    pageHeight = (int) (pageHeight/scale);
	    pageWidth = (int) (pageWidth/scale);
	}	
	int top = 0;
	while(top < root->height()) {
	    if(top > 0) printer->newPage();
	    root->print(p, 0, top, pageWidth, pageHeight, 0, 0);
	    p->translate(0,-pageHeight);
	    top += pageHeight;
	}

	p->end();
	delete p;

	// and now reset the layout to the usual one...
	root->setPrintingMode(false);
	m_part->setFontSizes(oldSizes);
	m_part->docImpl()->applyChanges();
    }
    delete printer;
}

void KHTMLView::setDNDEnabled( bool b )
{
  d->m_bDnd = b;
}

bool KHTMLView::dndEnabled() const
{
  return d->m_bDnd;
}

 
void KHTMLView::toggleActLink(bool actState)
{
  if ( d->currentNode )
    {
      if (!actState) // inactive->active
	{
	  d->currentNode->setKeyboardFocus(DOM::ActivationActive);
	  d->originalNode=d->currentNode;
	  d->linkPressed=true;
	}
      else //active->inactive
	{
	  d->currentNode->setKeyboardFocus(DOM::ActivationOff);
	  d->linkPressed=false;
	  if (d->currentNode==d->originalNode)
	    {
	      //retrieve url
	      HTMLAnchorElementImpl *actLink = static_cast<HTMLAnchorElementImpl *>(d->currentNode);
	      d->currentNode=0;
	      KURL href = KURL(m_part->url(),actLink->areaHref().string());
	      m_part->openURL(href);
	    }
	  d->originalNode=0;
	}
     }
}
