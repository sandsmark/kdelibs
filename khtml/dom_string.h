/*
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
#ifndef _DOM_DOMString_h_
#define _DOM_DOMString_h_

#include "qstring.h"
#include <stdlib.h>

class Attribute;

namespace DOM {

class DOMStringImpl;

/**
 * This class implements the basic string we use in the DOM. We do not use
 * QString for 2 reasons: Memory overhead, and the missing explicit sharing
 * of strings we need for the DOM.
 *
 * All DOMStrings are explicitly shared (they behave like pointers), meaning
 * that modifications to one instance will also modify all others. If you
 * wish to get a DOMString that is independent, use @ref copy().
 */
class DOMString
{
    friend class CharacterDataImpl;
    friend class Attribute;

public:
    /** default constructor. Gives an empty DOMString */
    DOMString();
    /* constructs an empty DOMString. Like this assignment to 0 works */
    DOMString(int);
    /** if copy == false, takes over the QChar * array. It'll get deleted,
     * when the string gets deleted...
     */
    DOMString(QChar *str, uint len, bool copy = true);
    DOMString(const QChar *str, uint len);
    DOMString(const QString &);
    DOMString(const char *str);
    DOMString(DOMStringImpl *i);
    virtual ~DOMString();

    // assign and copy
    DOMString(const DOMString &str);
    DOMString &operator =(const DOMString &str);

    /**
     * append str to this string
     */
    DOMString &operator += (const DOMString &str);
    /**
     * add two DOMString's
     */
    DOMString operator + (const DOMString &str);

    void insert(DOMString str, uint pos);

    /**
     * The character at position i of the DOMString. If i > length(), the
     * character returned will be 0.
     */
    const QChar &operator [](unsigned int i);

    int find(const QChar c, int start = 0);

    uint length() const;
    void truncate( unsigned int len );

    QChar *unicode() const;
    const QString string() const;

    int toInt() const;
    bool percentage(int &_percentage) const;

    DOMString copy() const;

    bool isNull() { return (impl == 0); }
    bool isEmpty();

    /**
     * @internal get a handle to the imlementation of the DOMString
     * Use at own risk!!!
     */
    DOMStringImpl *implementation() const { return impl; }
protected:
    DOMStringImpl *impl;
};

bool operator==( const DOMString &a, const DOMString &b );
bool operator==( const DOMString &a, const QString &b );
bool operator==( const DOMString &a, const char *b );
bool operator==( const DOMString &a, int );
bool operator!=( const DOMString &a, const DOMString &b );
bool operator!=( const DOMString &a, const QString &b );
bool operator!=( const DOMString &a, const char *b );
bool operator!=( const DOMString &a, int );

bool strncmp( const DOMString &a, const DOMString &b, unsigned int len );

int strncasecmp( const DOMString &a, const DOMString &b, unsigned int l );
int strcasecmp( const DOMString &a, const DOMString &b );

};
#endif
