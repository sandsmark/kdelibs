/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _HTML_OBJECT_H_
#define _HTML_OBJECT_H_

#include <html_document.h>
#include <html_base.h>
#include <html_misc.h>

#include <kjs/object.h>
#include <kjs/function.h>

#include "kjs_binding.h"

class KHTMLPart;
class HTMLElement;
class HTMLCollection;

namespace KJS {

  class HTMLDocFunction : public DOMFunction {
  public:
    HTMLDocFunction(DOM::HTMLDocument d, int i) : doc(d), id(i) { };
    virtual KJSO tryGet(const UString &p) const;
    Completion tryExecute(const List &);
    enum { Images, Applets, Links, Forms, Anchors, Open, Close,
	   Write, WriteLn, GetElementById, GetElementsByName };
  private:
    DOM::HTMLDocument doc;
    int id;
  };

  class HTMLDocument : public NodeObject {
  public:
    HTMLDocument(KHTMLPart *p) : part(p) { }
    virtual KJSO tryGet(const UString &p) const;
    virtual void tryPut(const UString &p, const KJSO& v);
    virtual DOM::Node toNode() const;
    virtual const TypeInfo* typeInfo() const { return &info; }
    static const TypeInfo info;
  private:
    KHTMLPart *part;
  };

  class HTMLElement : public NodeObject {
  public:
    HTMLElement(DOM::HTMLElement e) : element(e) { }
    virtual KJSO tryGet(const UString &p) const;
    virtual void tryPut(const UString &p, const KJSO& v);
    virtual DOM::Node toNode() const { return element; }
    virtual const TypeInfo* typeInfo() const { return &info; }
    static const TypeInfo info;
  private:
    DOM::HTMLElement element;
  };


  class HTMLElementFunction : public DOMFunction {
  public:
    HTMLElementFunction(DOM::HTMLElement e, int i) : element(e), id(i) { };
    Completion tryExecute(const List &);
    enum { Submit, Reset, Add, Remove, Blur, Focus, Select, Click,
           CreateTHead, DeleteTHead, CreateTFoot, DeleteTFoot,
           CreateCaption, DeleteCaption, InsertRow, DeleteRow,
           InsertCell, DeleteCell };
  private:
    DOM::HTMLElement element;
    int id;
  };

  class HTMLCollection : public DOMObject {
  public:
    HTMLCollection(DOM::HTMLCollection c) : collection(c) { }
    virtual KJSO tryGet(const UString &p) const;
  private:
    DOM::HTMLCollection collection;
  };

  class HTMLCollectionFunc : public DOMFunction {
  public:
    HTMLCollectionFunc(DOM::HTMLCollection c, int i) : coll(c), id(i) { };
    Completion tryExecute(const List &);
    enum { Item, NamedItem };
  private:
    DOM::HTMLCollection coll;
    int id;
  };

  ////////////////////// Image Object ////////////////////////

  class ImageObject : public DOMFunction {
  public:
    ImageObject(const Global &global);
    Completion tryExecute(const List &);
  private:
    UString src;
  };

  class ImageConstructor : public ConstructorImp {
  public:
    ImageConstructor(const Global& global);
    Object construct(const List &);
  private:
    Global global;
  };

  class Image : public DOMObject {
  public:
    virtual KJSO tryGet(const UString &p) const;
    virtual void tryPut(const UString &p, const KJSO& v);
  private:
    UString src;
  };
}; // namespace

#endif
