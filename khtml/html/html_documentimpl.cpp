/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
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
 * $Id$
 */
#include "html_documentimpl.h"

#include "khtmlview.h"
#include "khtml_part.h"
#include "misc/khtmldata.h"

#include "htmlparser.h"
#include "htmltokenizer.h"
#include "htmlhashes.h"

#include "dom_exception.h"
#include "html_headimpl.h"

#include <stdio.h>
#include <kurl.h>
#include <kglobal.h>

#include "css/cssstyleselector.h"
#include "css/css_stylesheetimpl.h"
#include "rendering/render_style.h"
#include "rendering/render_root.h"

#include <qstack.h>


using namespace DOM;
using namespace khtml;

template class QStack<DOM::NodeImpl>;

HTMLDocumentImpl::HTMLDocumentImpl() : DocumentImpl()
{
    printf("HTMLDocumentImpl constructor this = %p\n",this);
    parser = 0;
    tokenizer = 0;

    bodyElement = 0;

    m_loadingSheet = false;

    m_elemSheet=0;
}

HTMLDocumentImpl::HTMLDocumentImpl(KHTMLView *v)
    : DocumentImpl(v)
{
    printf("HTMLDocumentImpl constructor2 this = %p\n",this);
    parser = 0;
    tokenizer = 0;

    bodyElement = 0;

    m_styleSelector = new CSSStyleSelector(this);

    m_loadingSheet = false;

    m_elemSheet=0;
}

HTMLDocumentImpl::~HTMLDocumentImpl()
{
    printf("HTMLDocumentImpl destructor this = %p\n",this);
}

DOMString HTMLDocumentImpl::referrer() const
{
    // ### should we fix that? I vote against for privacy reasons
    return DOMString();
}

DOMString HTMLDocumentImpl::domain() const
{
    // ### do they want the host or the domain????
    KURL u(url.string());
    return u.host();
}

HTMLElementImpl *HTMLDocumentImpl::body()
{
    if(bodyElement) return bodyElement;
    if(!_first) return 0;
    NodeImpl *test = _first->firstChild();
    if(!test) return 0;
    while(test && (test->id() != ID_BODY && test->id() != ID_FRAMESET))
	test = test->nextSibling();
    if(!test) return 0;
    bodyElement = static_cast<HTMLElementImpl *>(test);
    return bodyElement;
}

void HTMLDocumentImpl::open(  )
{
    //printf("HTMLDocumentImpl::open()\n");
    clear();
    parser = new KHTMLParser(m_view, this);
    tokenizer = new HTMLTokenizer(parser, m_view);
    tokenizer->begin();
}

void HTMLDocumentImpl::close(  )
{
    if (m_render)
    	m_render->close();

    if(parser && !parser->hasQueued())
    {
	delete parser;
	parser = 0;
    }
    if(tokenizer) delete tokenizer;
    tokenizer = 0;
}

void HTMLDocumentImpl::write( const DOMString &text )
{
    if(tokenizer)
	tokenizer->write(text.string());
}

void HTMLDocumentImpl::write( const QString &text )
{
    if(tokenizer)
	tokenizer->write(text);
}

void HTMLDocumentImpl::writeln( const DOMString &text )
{
    write(text);
    write(DOMString("\n"));
}

ElementImpl *HTMLDocumentImpl::getElementById( const DOMString &elementId )
{
    QStack<NodeImpl> nodeStack;
    NodeImpl *current = _first;

    while(1)
    {
	if(!current)
	{
	    if(nodeStack.isEmpty()) break;
	    current = nodeStack.pop();
	    current = current->nextSibling();
	}
	else
	{
	    if(current->isElementNode())
	    {
		ElementImpl *e = static_cast<ElementImpl *>(current);
		if(e->getAttribute(ATTR_ID) == elementId)
		    return e;
	    }
	
	    NodeImpl *child = current->firstChild();
	    if(child)
	    {	
		nodeStack.push(current);
		current = child;
	    }
	    else
	    {
		current = current->nextSibling();
	    }
	}
    }

    return 0;
}


NodeListImpl *HTMLDocumentImpl::getElementsByName( const DOMString &elementName )
{
    return new NameNodeListImpl( this, elementName );
}

// internal. finds the first element with tagid id
NodeImpl *HTMLDocumentImpl::findElement( int id )
{
    QStack<NodeImpl> nodeStack;
    NodeImpl *current = _first;

    while(1)
    {
	if(!current)
	{
	    if(nodeStack.isEmpty()) break;
	    current = nodeStack.pop();
	    current = current->nextSibling();
	}
	else
	{
	    if(current->id() == id)
		return current;
	
	    NodeImpl *child = current->firstChild();
	    if(child)
	    {	
		nodeStack.push(current);
		current = child;
	    }
	    else
	    {
		current = current->nextSibling();
	    }
	}
    }

    return 0;
}


StyleSheetListImpl *HTMLDocumentImpl::styleSheets()
{
    // ### implement for html
    return 0;
}


// --------------------------------------------------------------------------
// not part of the DOM
// --------------------------------------------------------------------------

void HTMLDocumentImpl::clear()
{
    if(parser) delete parser;
    if(tokenizer) delete tokenizer;
    parser = 0;
    tokenizer = 0;

    // #### clear tree
}


NodeImpl *HTMLDocumentImpl::addChild(NodeImpl *newChild)
{
#ifdef DEBUG_LAYOUT
    printf("Document::addChild( %s )\n", newChild->nodeName().string().ascii());
#endif

    // short check for consistency with DTD
    if(newChild->id() != ID_HTML)
    {
	printf("AddChild failed! id=#document, child->id=%d\n", newChild->id());
	throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
    }

    if(_first)
    {
	printf("AddChild failed! id=#document, child->id=%d. Already have a HTML element!\n", newChild->id());
	throw DOMException(DOMException::HIERARCHY_REQUEST_ERR);
    }

    // just add it...
    newChild->setParent(this);
    _first = _last = newChild;
    return newChild;
}

bool HTMLDocumentImpl::mouseEvent( int _x, int _y, int button, MouseEventType type,
				  int, int, DOMString &url,
                                   NodeImpl *&innerNode, long &offset)
{
    if(body())
    {
	return bodyElement->mouseEvent(_x, _y, button, type, 0, 0, url, innerNode, offset);
    }
    return false;
}

void HTMLDocumentImpl::attach(KHTMLView *w)
{
    m_view = w;
    if(!m_styleSelector) createSelector();
    m_style = new RenderStyle();
    m_style->setDisplay(BLOCK);
    // ### make the font stuff _really_ work!!!!
    const QString *families = w->part()->settings()->families();
    QValueList<int> fs = w->part()->settings()->fontSizes();
    QFont f(families[0], fs[3]);
    m_style->setFont(families[0]);

    m_style->setHtmlHacks(true); // enable html specific rendering tricks

    m_render = new RenderRoot(m_style, w);
    m_render->ref();

    NodeBaseImpl::attach(w);
}

void HTMLDocumentImpl::detach()
{
    m_view = 0;

    NodeBaseImpl::detach();
}

void HTMLDocumentImpl::setVisuallyOrdered()
{
    if(!m_style) return;
    m_style->setVisuallyOrdered(true);
}

void HTMLDocumentImpl::createSelector()
{
    //printf("document::createSelector\n");
    if(!headLoaded()) return;
    //printf("document::createSelector2\n");

    if(m_styleSelector) delete m_styleSelector;
    m_styleSelector = new CSSStyleSelector(this);

    if(!parser) return;
    //printf("document::createSelector3\n");
    parser->processQueue();
    // parsing is finished if the tokenizer is already deleted
    if(!tokenizer)
    {
	delete parser;
    }
    parser = 0;
}

bool HTMLDocumentImpl::headLoaded()
{
    printf("checking for headLoaded()\n");
    if(parser && !parser->parsingBody()) return false;
    //if(m_loadingSheet) return false;

    NodeImpl *test = _first;
    if(!test) return true;
    test = test->firstChild();
    while(test && (test->id() != ID_HEAD))
	test = test->nextSibling();
    if(!test) return true; // no head element, so nothing than can be loaded in there
    HTMLHeadElementImpl *head = static_cast<HTMLHeadElementImpl *>(test);

    // all LINK and STYLE elements have to be direct children of the HEAD element
    test = head->firstChild();
    while(test)
    {
	//printf("searching link\n");

	if(test->id() == ID_LINK)
	{
	    printf("found link\n");
	    HTMLLinkElementImpl *link = static_cast<HTMLLinkElementImpl *>(test);
	    if(link->isLoading())
	    {
		printf("--> not loaded\n");
		return false;
	    }
	}
	else if(test->id() == ID_STYLE)
	{
	    printf("found style\n");
	    HTMLStyleElementImpl *style = static_cast<HTMLStyleElementImpl *>(test);
	    if(style->isLoading()) // can still load because of @import rules
	    {
		printf("--> not loaded\n");
		return false;
	    }
	}
	test = test->nextSibling();
    }
    printf("head loaded\n");
    return true;
}

void HTMLDocumentImpl::setStyleSheet(const DOM::DOMString &url, const DOM::DOMString &sheet)
{
    printf("HTMLDocument::setStyleSheet()\n");
    m_sheet = new CSSStyleSheetImpl(this, url);
    m_sheet->ref();
    m_sheet->parseString(sheet);
    m_loadingSheet = false;

    createSelector();
}

CSSStyleSheetImpl* HTMLDocumentImpl::elementSheet()
{
    if (!m_elemSheet)
    	m_elemSheet = new CSSStyleSheetImpl(this, url);
    return m_elemSheet;
}


void HTMLDocumentImpl::setSelection(NodeImpl* s, int sp, NodeImpl* e, int ep)
{
    static_cast<RenderRoot*>(m_render)
    	->setSelection(s->renderer(),sp,e->renderer(),ep);
}

void HTMLDocumentImpl::clearSelection()
{
    static_cast<RenderRoot*>(m_render)
    	->clearSelection();
}
