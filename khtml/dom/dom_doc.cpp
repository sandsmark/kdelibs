/**
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999 Lars Knoll (knoll@kde.org)
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

#include "dom_string.h"
#include "dom_doc.h"
#include "dom_docimpl.h"
#include "dom_node.h"
#include "dom_nodeimpl.h"
#include "dom_exception.h"
#include "dom_element.h"
#include "dom_text.h"
#include "dom_xml.h"
#include "dom2_range.h"
#include "dom2_traversal.h"
#include <kdebug.h>
using namespace DOM;


DOMImplementation::DOMImplementation()
{
}

DOMImplementation::DOMImplementation(const DOMImplementation &)
{
}

DOMImplementation &DOMImplementation::operator = (const DOMImplementation &)
{
    return *this;
}

DOMImplementation::~DOMImplementation()
{
}

bool DOMImplementation::hasFeature( const DOMString &feature, const DOMString &version )
{
    // no valid implementation at the moment... ;-)
    if(feature == "HTML" && version == "1") return true;

    return false;
}

// ----------------------------------------------------------------------------

Document::Document() : Node()
{
    // we always wan't an implementation
    impl = new DocumentImpl();
    impl->ref();
    kdDebug(6090) << "Document::Document()" << endl;
}

Document::Document(bool create) : Node()
{
    if(create)
    {
	impl = new DocumentImpl();
	impl->ref();
    }
    else
	impl = 0;
    kdDebug(6090) << "Document::Document(bool)" << endl;
}

Document::Document(const Document &other) : Node(other)
{
    kdDebug(6090) << "Document::Document(Document &)" << endl;
}

Document::Document(DocumentImpl *i) : Node(i)
{
    kdDebug(6090) << "Document::Document(DocumentImpl)" << endl;
}

Document &Document::operator = (const Node &other)
{
    if(other.nodeType() != DOCUMENT_NODE)
    {
	impl = 0;
	return *this;
    }
    Node::operator =(other);
    return *this;
}

Document &Document::operator = (const Document &other)
{
    Node::operator =(other);
    return *this;
}

Document::~Document()
{
    kdDebug(6090) << "Document::~Document\n" << endl;
}

DocumentType Document::doctype() const
{
    return DocumentType();
}

DOMImplementation Document::implementation() const
{
   return DOMImplementation();
}

Element Document::documentElement() const
{
    return ((DocumentImpl *)impl)->documentElement();
}

Element Document::createElement( const DOMString &tagName )
{
    return ((DocumentImpl *)impl)->createElement(tagName);
}

DocumentFragment Document::createDocumentFragment(  )
{
    return ((DocumentImpl *)impl)->createDocumentFragment();
}

Text Document::createTextNode( const DOMString &data )
{
    return ((DocumentImpl *)impl)->createTextNode( data );
}

Comment Document::createComment( const DOMString & )
{
    return Comment( );
}

CDATASection Document::createCDATASection( const DOMString & )
{
    return CDATASection();
}

ProcessingInstruction Document::createProcessingInstruction( const DOMString &, const DOMString & )
{
    return ProcessingInstruction();
}

Attr Document::createAttribute( const DOMString &name )
{
    return ((DocumentImpl *)impl)->createAttribute( name );
}

EntityReference Document::createEntityReference( const DOMString & )
{
    return EntityReference();
}

NodeList Document::getElementsByTagName( const DOMString &tagName )
{
    return ((DocumentImpl *)impl)->getElementsByTagName( tagName );
}

bool Document::isHTMLDocument()
{
    if(!impl) return false;
    return ((DocumentImpl *)impl)->isHTMLDocument();
}

Range Document::createRange()
{
    return Range( *this );
}

Range Document::createRange(const Node &sc, const long so, const Node &ec, const long eo)
{
    Range r;
    r.setStart( sc, so );
    r.setEnd( ec, eo );
    return r;
}

NodeIterator Document::createNodeIterator()
{
  return NodeIterator( *this );
}

NodeIterator Document::createNodeIterator(long _whatToShow, NodeFilter *filter)
{
  return NodeIterator( *this, _whatToShow, filter );
}

TreeWalker Document::createTreeWalker()
{
  return TreeWalker( *this );
}

TreeWalker Document::createTreeWalker(long _whatToShow, NodeFilter *filter )
{
  return TreeWalker( *this, _whatToShow, filter);
}


// ----------------------------------------------------------------------------

DocumentFragment::DocumentFragment() : Node()
{
}

DocumentFragment::DocumentFragment(const DocumentFragment &other) : Node(other)
{
}

DocumentFragment &DocumentFragment::operator = (const Node &other)
{
    if(other.nodeType() != DOCUMENT_FRAGMENT_NODE)
    {
	impl = 0;
	return *this;
    }
    Node::operator =(other);
    return *this;
}

DocumentFragment &DocumentFragment::operator = (const DocumentFragment &other)
{
    Node::operator =(other);
    return *this;
}

DocumentFragment::~DocumentFragment()
{
}

DocumentFragment::DocumentFragment(DocumentFragmentImpl *i) : Node(i)
{
}

// ----------------------------------------------------------------------------

DocumentType::DocumentType()
{
}

DocumentType::DocumentType(const DocumentType &other) : Node(other)
{
}

DocumentType &DocumentType::operator = (const Node &other)
{
    if(other.nodeType() != DOCUMENT_TYPE_NODE)
    {
	impl = 0;
	return *this;
    }

    Node::operator =(other);
    return *this;
}

DocumentType &DocumentType::operator = (const DocumentType &other)
{
    Node::operator =(other);
    return *this;
}

DocumentType::~DocumentType()
{
}

DOMString DocumentType::name() const
{
    // ###
    return 0;
}

NamedNodeMap DocumentType::entities() const
{
    return NamedNodeMap();
    // ###
}

NamedNodeMap DocumentType::notations() const
{
    return NamedNodeMap();
    // ###
}

