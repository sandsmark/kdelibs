/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *	     (C) 2000 Simon Hausmann <hausmann@kde.org>
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
// -------------------------------------------------------------------------
#include "html_inlineimpl.h"

#include "html_documentimpl.h"
using namespace DOM;

#include <kdebug.h>

#include "htmlhashes.h"
#include "css/cssproperties.h"
#include "css/cssstyleselector.h"

#include "rendering/render_br.h"

using namespace khtml;

HTMLAnchorElementImpl::HTMLAnchorElementImpl(DocumentImpl *doc)
: HTMLAreaElementImpl(doc)
{
}

HTMLAnchorElementImpl::~HTMLAnchorElementImpl()
{
}

const DOMString HTMLAnchorElementImpl::nodeName() const
{
    return "A";
}

ushort HTMLAnchorElementImpl::id() const
{
    return ID_A;
}

void HTMLAnchorElementImpl::blur(  )
{
  setKeyboardFocus(DOM::ActivationOff);
}

void HTMLAnchorElementImpl::focus(  )
{
  setKeyboardFocus(DOM::ActivationPassive);
}

bool HTMLAnchorElementImpl::mouseEvent( int _x, int _y, int button, MouseEventType type,
				  int _tx, int _ty, DOMString &_url,
                                        NodeImpl *&innerNode, long &offset)
{
    bool inside = HTMLElementImpl::mouseEvent( _x, _y, button, type, _tx, _ty, _url, innerNode, offset);

    if(inside && _url==0)
    {
	//kdDebug() << "HTMLAnchorElementImpl::mouseEvent" << _tx << "/" << _ty <<endl; 
	// set the url
	if(target && href) {
	    DOMString s = DOMString("target://") + DOMString(target) + DOMString("/#") + DOMString(href);
	    _url = s;
	}
	else
	    _url = href;
    }

    return inside;
}

void HTMLAnchorElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_HREF:
    default:
	HTMLAreaElementImpl::parseAttribute(attr);
    }
}

// -------------------------------------------------------------------------

HTMLBRElementImpl::HTMLBRElementImpl(DocumentImpl *doc) : HTMLElementImpl(doc)
{
}

HTMLBRElementImpl::~HTMLBRElementImpl()
{
}

const DOMString HTMLBRElementImpl::nodeName() const
{
    return "BR";
}

ushort HTMLBRElementImpl::id() const
{
    return ID_BR;
}

void HTMLBRElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_CLEAR:
	if ( strcasecmp( attr->value(), "left" ) == 0 )
	    addCSSProperty(CSS_PROP_CLEAR, "left", false);
	else if ( strcasecmp( attr->value(), "right" ) == 0 )
	    addCSSProperty(CSS_PROP_CLEAR, "right", false);
	else if ( strcasecmp( attr->value(), "all" ) == 0 )
	    addCSSProperty(CSS_PROP_CLEAR, "both", false);
	break;
    default:
    	HTMLElementImpl::parseAttribute(attr);
    }
}

void HTMLBRElementImpl::attach(KHTMLView *w)
{
    //kdDebug( 6030 ) << "HTMLBRElementImpl::attach" << endl;
    m_style = document->styleSelector()->styleForElement(this);
    khtml::RenderObject *r = _parent->renderer();
    if(r)
    {
	m_render = new RenderBR();
	m_render->setStyle(m_style);
	r->addChild(m_render, _next ? _next->renderer() : 0);
    }
    NodeBaseImpl::attach( w );
}

// -------------------------------------------------------------------------

HTMLFontElementImpl::HTMLFontElementImpl(DocumentImpl *doc)
    : HTMLElementImpl(doc)
{
}

HTMLFontElementImpl::~HTMLFontElementImpl()
{
}

const DOMString HTMLFontElementImpl::nodeName() const
{
    return "FONT";
}

ushort HTMLFontElementImpl::id() const
{
    return ID_FONT;
}

void HTMLFontElementImpl::parseAttribute(AttrImpl *attr)
{
    switch(attr->attrId)
    {
    case ATTR_SIZE:
    {
	DOMString s = attr->value();
	if(s != 0) {
	    int num = s.toInt();
	    if ( *s.unicode() == '+' || *s.unicode() == '-' ) {
		num += 3;
	    }
	    switch (num)
	    {
		// size = 3 is the normal size according to html specs
		case 1:
		    addCSSProperty(CSS_PROP_FONT_SIZE, "x-small", false);
		    break;
		case 2:
		    addCSSProperty(CSS_PROP_FONT_SIZE, "small", false);
		    break;
		case 3:
		    addCSSProperty(CSS_PROP_FONT_SIZE, "medium", false);
		    break;
		case 4:
		    addCSSProperty(CSS_PROP_FONT_SIZE, "large", false);
		    break;
		case 5:
		    addCSSProperty(CSS_PROP_FONT_SIZE, "x-large", false);
		    break;
		case 6:
		    addCSSProperty(CSS_PROP_FONT_SIZE, "xx-large", false);
		    break;
		case 7:
		    addCSSProperty(CSS_PROP_FONT_SIZE, "xx-large", false);
		    break;
		default:
		    if (num >= 6) {
			addCSSProperty(CSS_PROP_FONT_SIZE, "xx-large", false);
		    } else if (num < 1) {
			addCSSProperty(CSS_PROP_FONT_SIZE, "xx-small", false);
		    }
		    break;
		}
	}
	break;
    }
    case ATTR_COLOR:
	addCSSProperty(CSS_PROP_COLOR, attr->value(), false);
	// HTML4 compatibility hack
	addCSSProperty(CSS_PROP_TEXT_DECORATION_COLOR, attr->value(), false);
	break;
    case ATTR_FACE:
	addCSSProperty(CSS_PROP_FONT_FAMILY, attr->value(), false);
	break;
    default:
	HTMLElementImpl::parseAttribute(attr);
    }
}


void HTMLFontElementImpl::attach(KHTMLView *w)
{
    HTMLElementImpl::attach(w);
#if 0
    // the font element needs special handling because it has to behave like
    // an inline or block level element depending on context.

    m_style = document->styleSelector()->styleForElement(this);
    if(_parent && _parent->renderer())
    {
	if(_parent->style()->display() != INLINE)
	    m_style->setDisplay(BLOCK);
	m_render = khtml::RenderObject::createObject(this);

	if(m_render)
	{
	    _parent->renderer()->addChild(m_render, _next ? _next->renderer() : 0);
	}
    }

    NodeBaseImpl::attach(w);
#endif
}


// -------------------------------------------------------------------------

HTMLModElementImpl::HTMLModElementImpl(DocumentImpl *doc, ushort tagid) : HTMLElementImpl(doc)
{
    _id = tagid;
}

HTMLModElementImpl::~HTMLModElementImpl()
{
}

const DOMString HTMLModElementImpl::nodeName() const
{
    return getTagName(_id);
}

ushort HTMLModElementImpl::id() const
{
    return _id;
}

// -------------------------------------------------------------------------

HTMLQuoteElementImpl::HTMLQuoteElementImpl(DocumentImpl *doc)
    : HTMLElementImpl(doc)
{
}

HTMLQuoteElementImpl::~HTMLQuoteElementImpl()
{
}

const DOMString HTMLQuoteElementImpl::nodeName() const
{
    return "Q";
}

ushort HTMLQuoteElementImpl::id() const
{
    return ID_Q;
}

