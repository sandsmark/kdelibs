/*
    This file is part of the KDE libraries

    Copyright (C) 1997 Martin Jones (mjones@kde.org)
              (C) 1997 Torben Weis (weis@kde.org)
              (C) 1998 Waldo Bastian (bastian@kde.org)

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
//----------------------------------------------------------------------------
//
// KDE HTML Widget - Tokenizers
// $Id$

#include "khtmltoken.h"
#include "khtmltags.h"
#include "khtmlattrs.h"

// defined in khtmltokenizer
int getAttrID(const char *tagStr, int len);

// ---------------------------------------------------------------------------

Attribute::Attribute(const DOMString &name, const DOMString &val)
{
    setName(name);
    v = val.implementation();
    v->ref();
}

Attribute::Attribute(const DOMStringImpl *name, DOMStringImpl *val)
{
    setName(name);
    v = val;
    v->ref();
}

const DOMString &Attribute::name() const
{
    return getAttrName(id);
}

void Attribute::setName(const DOMString &name)
{
    id = getAttrID(name.string().ascii(), name.length());
}

void Attribute::setName(const DOMStringImpl *name)
{
    id = getAttrID(QConstString(name->s, name->l).string().ascii(), name->l);
}

//-----------------------------------------------------------------------------

AttributeList::AttributeList()
{
    _list = 0;
    _len = 0;
}

AttributeList::AttributeList( const AttributeList &other)
{
    if(!other._len)
    {
	_len = 0;
	_list = 0;
    }
    _len = other._len;
    _list = new Attribute [ _len ];
    int i = 0;
    while(i < (int)_len)
    {
	_list[i].setValue(other._list[i].val());
	_list[i].id = other._list[i].id;
	i++;
    }
}

AttributeList &AttributeList::operator = (const AttributeList &other )
{
    if(_list) delete _list;
    if(!other._len)
    {
	_len = 0;
	_list = 0;
    }
    _len = other._len;
    _list = new Attribute [ _len ];
    int i = 0;
    while(i < (int)_len)
    {
	_list[i].setValue(other._list[i].val());
	_list[i].id = other._list[i].id;
	i++;
    }
    return *this;
}

AttributeList::AttributeList( Attribute *list, unsigned int len)
{
    _list = list;
    _len = len;
}

AttributeList::~AttributeList()
{
    if(_list) delete [] _list;
}


ushort AttributeList::id(uint index) const
{
    return _list[index].id;
}

DOMString AttributeList::name(uint index) const
{
    if(index >= _len) return 0;

    return _list[index].name();
}

DOMString AttributeList::value(uint index) const
{
    if(index >= _len) return 0;

    return _list[index].value();
}

void AttributeList::add(const Attribute &a)
{
    int index = find(a.id);

    if(index == -1)
    {
	Attribute *nList = new Attribute [ _len+1 ];
	if(_list)
	{
	    int i = 0;
	    while(i < (int)_len)
	    {
		nList[i].setValue(_list[i].val());
		nList[i].id = _list[i].id;
		i++;
	    }
	    delete [] _list;
	}
	_list = nList;
	_list[_len].id = a.id;
	_list[_len].setValue(a.val());
	_len++;
    }
    else
	_list[index].setValue(a.val());
}

unsigned int AttributeList::length() const
{
    return _len;
}

int AttributeList::find(const DOMString &name)
{
    int i = 0;
    while( i<(int)_len )
    {
	if(_list[i].name() == name)
	    return i;
	i++;
    }
    return -1;
}

int AttributeList::find(ushort id)
{
    int i = 0;
    while( i<(int)_len )
    {
	if(_list[i].id == id)
	    return i;
	i++;
    }
    return -1;
}


DOMString AttributeList::operator [] (const DOMString &name)
{
    int i = find(name);
    if(i == -1) return 0;
    return value((uint) i);
}

Attribute *AttributeList::operator [] (uint index)
{
    if(index > _len) return 0;
    return _list+index;
}

void AttributeList::remove(DOMString name)
{
    int index = find(name);

    if(index == -1)
	return;

    remove(index);
}

void AttributeList::remove(uint index)
{
    if(index > _len) return;

    Attribute *nList = new Attribute [ _len-1 ];
    uint i = 0;
    while(i < index)
    {
	nList[i].setValue(_list[i].val());
	nList[i].id = _list[i].id;
	i++;
    }
    while(i < _len-1)
    {
	nList[i].setValue(_list[i+1].val());
	nList[i].id = _list[i+1].id;
	i++;
    }

    delete [] _list;
    _list = nList;
    _len--;
}

// ---------------------------------------------------------------------------

// contructor for a text token
Token::Token(QChar *t, uint len)
{
    id = ID_TEXT;
    text = DOMString(t, len);
}

// tag without attributes
Token::Token(ushort _id)
{
    id = _id;
}

// tag with attributes
Token::Token(ushort _id, Attribute *lst, uint len)
{
    id = _id;
    attrs = AttributeList(lst, len);
}
