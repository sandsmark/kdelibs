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

#include "dom_nodeimpl.h"
#include "render_object.h"
#include "render_box.h"
#include "render_flow.h"
#include "render_style.h"
#include "render_table.h"
#include "render_list.h"

#include "qnamespace.h"
#include "qpainter.h"

#include <kdebug.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpen.h>
#include <qsize.h>

using namespace DOM;
using namespace khtml;

RenderObject *RenderObject::createObject(DOM::NodeImpl *node)
{
    RenderStyle *style = node->style();
    switch(style->display())
    {
    case INLINE:
    case BLOCK:
	return new RenderFlow(style);
    case LIST_ITEM:
	return new RenderListItem(style);
    case RUN_IN:
    case COMPACT:
    case MARKER:
	break;
    case TABLE:
    case INLINE_TABLE:
	// ### set inline/block right
	kdDebug(300) << "creating RenderTable" << endl;
	return new RenderTable(style);
    case TABLE_ROW_GROUP:
    case TABLE_HEADER_GROUP:
    case TABLE_FOOTER_GROUP:
	return new RenderTableSection(style);
    case TABLE_ROW:
	return new RenderTableRow(style);
    case TABLE_COLUMN_GROUP:
    case TABLE_COLUMN:
	return new RenderTableCol(style);
    case TABLE_CELL:
	return new RenderTableCell(style);
    case TABLE_CAPTION:
	return new RenderTableCaption(style);
    case NONE:
	return 0;
    }
    return 0;
}


RenderObject::RenderObject(RenderStyle* style)
{
    m_style = style;

    m_layouted = false;
    m_parsing = false;
    m_minMaxKnown = false;

    m_parent = 0;
    m_previous = 0;
    m_next = 0;
    m_first = 0;
    m_last = 0;

    m_bgImage = m_style->backgroundImage();
    if(m_bgImage) m_bgImage->ref(this);

    m_floating = false;
    m_positioned = false;

    m_printSpecial = false;
    if( m_style->backgroundColor().isValid() || m_style->hasBorder() || m_bgImage )
	m_printSpecial = true;

    // ### this might be the wrong place to call it !!! We mght get the wrong containing block
    m_containingBlock = 0;
}

RenderObject::~RenderObject()
{
    // previous and next node may still reference this!!!
    // hope this fix is fine...
    if(m_previous) m_previous->setNextSibling(0);
    if(m_next) m_next->setPreviousSibling(0);

    RenderObject *n;
    RenderObject *next;

    for( n = m_first; n != 0; n = next )
    {
	n->setParent(0);
	next = n->nextSibling();
	if(n->deleteMe()) delete n;
    }

    if(m_bgImage) m_bgImage->deref(this);
}

bool RenderObject::deleteMe()
{
    if(!m_parent && _ref <= 0) return true;
    return false;
}

void RenderObject::addChild(RenderObject *newChild)
{
#ifdef DEBUG_LAYOUT
    kdDebug(300) << renderName() << "(RenderObject)::addChild( " << newChild->renderName() << " )" << endl;
#endif

    newChild->setParsing();

    // just add it...
    newChild->setParent(this);

    if(m_last)
    {
	newChild->setPreviousSibling(m_last);
	m_last->setNextSibling(newChild);
	m_last = newChild;
    }
    else
    {
	m_first = m_last = newChild;
    }
    newChild->calcWidth();
}

RenderObject *RenderObject::containingBlock() const
{
    if(!m_containingBlock) const_cast<RenderObject *>(this)->setContainingBlock();
    return m_containingBlock;
}

void RenderObject::setContainingBlock()
{
    RenderObject *o = parent();
    if(m_style->position() == FIXED)
    {
	// ### containing block is viewport
    }
    else if(m_style->position() == ABSOLUTE)
    {
	while(o && m_style->position() == STATIC) o = o->parent();
    }
    else
    {
	while(o && o->style()->display() == INLINE) o = o->parent();
    }
    // this is just to make sure we return a valid element.
    // the case below should never happen...
    if(!o)
    {
	kdDebug(300) << renderName() << "(RenderObject)::setContainingBlock() containingBlock == 0, setting to this" << endl;
	m_containingBlock = this;
    }
    else
	m_containingBlock = o;
}

QSize RenderObject::containingBlockSize() const
{
    RenderObject *o = containingBlock();

    if(m_style->position() == ABSOLUTE)
    {
	if(o->isInline())
	{
	    // ### fixme
	}
	else
	    return o->paddingSize();
    }

    return o->contentSize();
}

short RenderObject::containingBlockWidth() const
{
    // ###
    return containingBlock()->contentWidth();
}

int RenderObject::containingBlockHeight() const
{
    // ###
    return containingBlock()->contentHeight();
}


QSize RenderObject::contentSize() const
{
    return QSize(0, 0);
}

QSize RenderObject::contentOffset() const
{
    return QSize(0, 0);
}

QSize RenderObject::paddingSize() const
{
    return QSize(0, 0);
}

QSize RenderObject::size() const
{
    return QSize(0, 0);
}

void RenderObject::drawBorder(QPainter *p, int x1, int y1, int x2, int y2, int width, BorderSide s, const QColor &c, EBorderStyle style)
{
    switch(style)
    {
    case BNONE:
    case BHIDDEN:
	return;
    case DOTTED:
	p->setPen(QPen(c, width, Qt::DotLine));
	break;
    case DASHED:
	p->setPen(QPen(c, width, Qt::DashLine));
	break;
    case DOUBLE:
    case GROOVE:
    case RIDGE:
    case INSET:
    case OUTSET:
	// ### don't treat them as solid
    case SOLID:
	p->setPen(QPen(c, width, Qt::SolidLine));
	break;
    }

    int half = width/2;

    switch(s)
    {
    case BSTop:
	y1 += half; y2 += half; break;
    case BSBottom:
	y1 -= half; y2 -= half; break;
    case BSLeft:
	x1 += half; x2 += half; break;
    case BSRight:
	x1 -= half; x2 -= half; break;
    }
	
    p->drawLine(x1, y1, x2, y2);
}

void RenderObject::repaintRectangle(int x, int y, int w, int h)
{
    if(m_parent) m_parent->repaintRectangle(x, y, w, h);
}

void RenderObject::repaintObject(RenderObject *o, int x, int y)
{
    if(m_parent) m_parent->repaintObject(o, x, y);
}

// used for clear property & to layout replaced elements
bool RenderObject::isSpecial() const
{
    return (!isInline() || isReplaced() || isBR());
}

void RenderObject::printTree(int indent) const
{
    for (int i=0; i<indent; i++) kdDebug(300) << " " << endl;
    kdDebug(300) << renderName() << ": " << (void*)this << " il=" << isInline()
                 << " fl=" << isFloating() << " rp=" << isReplaced()
                 << " laytd=" << layouted()
                 << " (" << xPos() << "," << yPos() << "," << width() << "," << height() << ")" << endl;
    RenderObject *child = firstChild();
    while( child != 0 )
    {    	
	child->printTree(indent+2);
	child = child->nextSibling();
    }
}

void RenderObject::selectionStartEnd(int& spos, int& epos)
{
    if (parent())
    	parent()->selectionStartEnd(spos, epos);
}


void RenderObject::styleChanged(RenderStyle *newStyle)
{
    if(newStyle)
    {
        if(m_style) delete m_style;
        m_style = newStyle;
    }
     containingBlock()->updateSize();
     //     repaintObject(containingBlock(), 0, 0);
     repaint();
}

void RenderObject::relativePositionOffset(int &tx, int &ty)
{
    if(!m_style->left().isUndefined())
	tx += m_style->left().width(containingBlockWidth());
    else if(!m_style->right().isUndefined())
	tx -= m_style->right().width(containingBlockWidth());
    if(!m_style->top().isUndefined())
	ty += m_style->top().width(containingBlockHeight());
    else if(!m_style->bottom().isUndefined())
	ty -= m_style->bottom().width(containingBlockHeight());
}
