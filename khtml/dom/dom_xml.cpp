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


#include "dom_xml.h"
#include "dom_string.h"
using namespace DOM;


CDATASection::CDATASection()
{
}

CDATASection::CDATASection(const CDATASection &) : Text()
{
}

CDATASection &CDATASection::operator = (const Node &other)
{
    if(other.nodeType() != CDATA_SECTION_NODE)
    {
	impl = 0;
	return *this;
    }
    Node::operator =(other);
    return *this;
}

CDATASection &CDATASection::operator = (const CDATASection &other)
{
    Node::operator =(other);
    return *this;
}

CDATASection::~CDATASection()
{
}

// ----------------------------------------------------------------------------
Entity::Entity()
{
}

Entity::Entity(const Entity &) : Node()
{
}

Entity &Entity::operator = (const Node &other)
{
    if(other.nodeType() != ENTITY_NODE)
    {
	impl = 0;
	return *this;
    }
    Node::operator =(other);
    return *this;
}

Entity &Entity::operator = (const Entity &other)
{
    Node::operator =(other);
    return *this;
}

Entity::~Entity()
{
}

DOMString Entity::publicId() const
{
    return 0;
}

DOMString Entity::systemId() const
{
    return 0;
}

DOMString Entity::notationName() const
{
    return 0;
}


EntityReference::EntityReference()
{
}

EntityReference::EntityReference(const EntityReference &) : Node()
{
}

EntityReference &EntityReference::operator = (const Node &other)
{
    if(other.nodeType() != ENTITY_REFERENCE_NODE)
    {
	impl = 0;
	return *this;
    }
    Node::operator =(other);
    return *this;
}

EntityReference &EntityReference::operator = (const EntityReference &other)
{
    Node::operator =(other);
    return *this;
}

EntityReference::~EntityReference()
{
}



Notation::Notation()
{
}

Notation::Notation(const Notation &) : Node()
{
}

Notation &Notation::operator = (const Node &other)
{
    if(other.nodeType() != NOTATION_NODE)
    {
	impl = 0;
	return *this;
    }
    Node::operator =(other);
    return *this;
}

Notation &Notation::operator = (const Notation &other)
{
    Node::operator =(other);
    return *this;
}

Notation::~Notation()
{
}

DOMString Notation::publicId() const
{
    return 0;
}

DOMString Notation::systemId() const
{
    return 0;
}


ProcessingInstruction::ProcessingInstruction()
{
}

ProcessingInstruction::ProcessingInstruction(const ProcessingInstruction &)
    : Node()
{
}

ProcessingInstruction &ProcessingInstruction::operator = (const Node &other)
{
    if(other.nodeType() != PROCESSING_INSTRUCTION_NODE)
    {
	impl = 0;
	return *this;
    }
    Node::operator =(other);
    return *this;
}

ProcessingInstruction &ProcessingInstruction::operator = (const ProcessingInstruction &other)
{
    Node::operator =(other);
    return *this;
}

ProcessingInstruction::~ProcessingInstruction()
{
}

DOMString ProcessingInstruction::target() const
{
    return 0;
}

DOMString ProcessingInstruction::data() const
{
    return 0;
}

void ProcessingInstruction::setData( const DOMString & )
{
}

