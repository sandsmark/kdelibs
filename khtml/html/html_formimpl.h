/*
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
#ifndef HTML_FORMIMPL_H
#define HTML_FORMIMPL_H

#include "html_elementimpl.h"
//#include "dtd.h"
#include "html_element.h"

#include <qlist.h>

class KHTMLView;

namespace khtml
{
    class RenderFormElement;
    class RenderTextArea;
    class RenderSelect;
}

namespace DOM {

class HTMLFormElement;
class DOMString;
class HTMLGenericFormElementImpl;

class HTMLFormElementImpl : public HTMLElementImpl
{
public:
    HTMLFormElementImpl(DocumentImpl *doc);
    virtual ~HTMLFormElementImpl();

    virtual const DOMString nodeName() const { return "FORM"; }
    virtual ushort id() const;

    virtual tagStatus startTag() { return FORMStartTag; }
    virtual tagStatus endTag() { return FORMEndTag; }

    long length() const;
    void submit (  );
    void reset (  );

    QByteArray formData( );

    DOMString enctype() const { return _enctype; }
    void setEnctype( const DOMString & );

    DOMString boundary() const { return _boundary; }
    void setBoundary( const DOMString & );

    virtual void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);
    virtual void detach();

    void radioClicked( khtml::RenderFormElement *caller );
    void maybeSubmit();

    void registerFormElement(khtml::RenderFormElement *);
    void removeFormElement(khtml::RenderFormElement *);


protected:
    DOMString url;
    DOMString target;
    DOMString _enctype;
    DOMString _boundary;
    bool post;
    bool multipart;
    KHTMLView *view;
    QList<khtml::RenderFormElement> formElements;
};

// -------------------------------------------------------------------------

class HTMLGenericFormElementImpl : public HTMLElementImpl
{
    friend class HTMLFormElementImpl;

public:
    HTMLGenericFormElementImpl(DocumentImpl *doc);
    HTMLGenericFormElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLGenericFormElementImpl();

    HTMLFormElementImpl *form() { return _form; }

    virtual NodeImpl *addChild(NodeImpl *newChild) { return HTMLElementImpl::addChild(newChild); }

    virtual void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);
    virtual void detach();

    virtual void reset() {}
    bool disabled() const { return m_disabled; }

protected:
    HTMLFormElementImpl *getForm() const;

    DOMString _name;
    HTMLFormElementImpl *_form;
    KHTMLView *view;
    bool m_disabled, m_readonly;
};

// -------------------------------------------------------------------------

class HTMLButtonElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLButtonElementImpl(DocumentImpl *doc);
    HTMLButtonElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual ~HTMLButtonElementImpl();

    enum typeEnum {
	SUBMIT,
	RESET,
	BUTTON
    };

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return BUTTONStartTag; }
    virtual tagStatus endTag() { return BUTTONEndTag; }

    long tabIndex() const;
    void setTabIndex( long );

    DOMString type() const;

    void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);

    virtual NodeImpl *addChild(NodeImpl *newChild) { return HTMLElementImpl::addChild(newChild); }

protected:
    DOMString _value;
    bool _clicked;
    typeEnum _type;
    QString currValue;
    bool dirty;
};

// -------------------------------------------------------------------------

class HTMLFieldSetElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLFieldSetElementImpl(DocumentImpl *doc);
    HTMLFieldSetElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual ~HTMLFieldSetElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return FIELDSETStartTag; }
    virtual tagStatus endTag() { return FIELDSETEndTag; }
};

// -------------------------------------------------------------------------

class HTMLInputElementImpl : public HTMLGenericFormElementImpl
{
public:
    enum typeEnum {
	TEXT,
	PASSWORD,
	CHECKBOX,
	RADIO,
	SUBMIT,
	RESET,
	FILE,
	HIDDEN,
	IMAGE,
	BUTTON
    };

    HTMLInputElementImpl(DocumentImpl *doc);
    HTMLInputElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLInputElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return INPUTStartTag; }
    virtual tagStatus endTag() { return INPUTEndTag; }

    bool checked() { return m_checked; }
    long maxLength() const { return _maxLen; }
    int size() const { return _size; }
    long tabIndex() const;
    DOMString type() const;
    DOMString value() const { return _value; }

    QString state();

    void blur();
    void focus();
    void select();
    void click();

    virtual void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);

protected:
    typeEnum _type;
    DOMString _value;
    QString currValue;
    bool m_checked;
    // ### move _size and _maxLen to render objects?
    int _maxLen;
    int _size;
    DOMString _src;
    bool _clicked;
};

// -------------------------------------------------------------------------

class HTMLLabelElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLLabelElementImpl(DocumentImpl *doc);
    HTMLLabelElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLLabelElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return LABELStartTag; }
    virtual tagStatus endTag() { return LABELEndTag; }
};

// -------------------------------------------------------------------------

class HTMLLegendElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLLegendElementImpl(DocumentImpl *doc);
    HTMLLegendElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLLegendElementImpl();

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return LEGENDStartTag; }
    virtual tagStatus endTag() { return LEGENDEndTag; }
};


// -------------------------------------------------------------------------

class HTMLSelectElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLSelectElementImpl(DocumentImpl *doc);
    HTMLSelectElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual const DOMString nodeName() const { return "SELECT"; }
    virtual ushort id() const;

    virtual tagStatus startTag() { return SELECTStartTag; }
    virtual tagStatus endTag() { return SELECTEndTag; }

    DOMString type() const;

    long selectedIndex() const;
    void setSelectedIndex( long index );

    long length() const;

    long size() const { return m_size; }

    bool multiple() { return m_multiple; }

    void add ( const HTMLElement &element, const HTMLElement &before );
    void remove ( long index );

    QString state();

    void blur (  );
    void focus (  );

    virtual NodeImpl *insertBefore ( NodeImpl *newChild, NodeImpl *refChild );
    virtual NodeImpl *replaceChild ( NodeImpl *newChild, NodeImpl *oldChild );
    virtual NodeImpl *removeChild ( NodeImpl *oldChild );
    virtual NodeImpl *appendChild ( NodeImpl *newChild );


    virtual void parseAttribute(AttrImpl *attr);

    virtual void attach(KHTMLView *w);

protected:
    int m_size;
    bool m_multiple;
    int m_selectedIndex;
};


// -------------------------------------------------------------------------

class HTMLOptGroupElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLOptGroupElementImpl(DocumentImpl *doc);
    HTMLOptGroupElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);
    virtual ~HTMLOptGroupElementImpl();

    virtual const DOMString nodeName() const { return "OPTGROUP"; }
    virtual ushort id() const;

    virtual tagStatus startTag() { return OPTGROUPStartTag; }
    virtual tagStatus endTag() { return OPTGROUPEndTag; }

    virtual NodeImpl *insertBefore ( NodeImpl *newChild, NodeImpl *refChild );
    virtual NodeImpl *replaceChild ( NodeImpl *newChild, NodeImpl *oldChild );
    virtual NodeImpl *removeChild ( NodeImpl *oldChild );
    virtual NodeImpl *appendChild ( NodeImpl *newChild );
    virtual void parseAttribute(AttrImpl *attr);
    void recalcSelectOptions();
};


// ---------------------------------------------------------------------------

class HTMLOptionElementImpl : public HTMLGenericFormElementImpl
{
public:
    HTMLOptionElementImpl(DocumentImpl *doc);
    HTMLOptionElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual const DOMString nodeName() const;
    virtual ushort id() const;

    virtual tagStatus startTag() { return OPTIONStartTag; }
    virtual tagStatus endTag() { return OPTIONEndTag; }

    DOMString text() const;

    long index() const;
    void setIndex( long );
    virtual void parseAttribute(AttrImpl *attr);
    DOMString value() { return m_value; }

    bool selected() const;

protected:
    bool m_selected;
    DOMString m_value;

//    friend khtml::RenderSelect;
};


// -------------------------------------------------------------------------

class HTMLTextAreaElementImpl : public HTMLGenericFormElementImpl
{
public:
    enum WrapMethod {
        ta_NoWrap,
        ta_Virtual,
        ta_Physical
    };

    HTMLTextAreaElementImpl(DocumentImpl *doc);
    HTMLTextAreaElementImpl(DocumentImpl *doc, HTMLFormElementImpl *f);

    virtual const DOMString nodeName() const { return "TEXTAREA"; }
    virtual ushort id() const;

    virtual tagStatus startTag() { return TEXTAREAStartTag; }
    virtual tagStatus endTag() { return TEXTAREAEndTag; }

    long cols() const { return m_cols; }

    long rows() const { return m_rows; }

    WrapMethod wrap() { return m_wrap; }

    DOMString type() const;

    QString state();

    void blur (  );
    void focus (  );
    void select (  );

    virtual void parseAttribute(AttrImpl *attr);
    virtual void attach(KHTMLView *w);

protected:
    int m_rows;
    int m_cols;
    WrapMethod m_wrap;

    friend khtml::RenderTextArea;
};

}; //namespace

#endif
