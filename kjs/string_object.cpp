// -*- c-basic-offset: 2 -*-
/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2001 Harri Porten (porten@kde.org)
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
 *
 *  $Id$
 */

#include "value.h"
#include "object.h"
#include "types.h"
#include "interpreter.h"
#include "operations.h"
#include "regexp.h"
#include "regexp_object.h"
#include "string_object.h"
#include "error_object.h"
#include <stdio.h>

using namespace KJS;

// ------------------------------ StringInstanceImp ----------------------------

const ClassInfo StringInstanceImp::info = {"String", 0, 0, 0};

StringInstanceImp::StringInstanceImp(const Object &proto)
  : ObjectImp(proto)
{
  setInternalValue(String(""));
}

// ------------------------------ StringPrototypeImp ---------------------------
// ECMA 15.5.4

StringPrototypeImp::StringPrototypeImp(ExecState *exec,
                                       ObjectPrototypeImp *objProto,
                                       FunctionPrototypeImp *funcProto)
  : StringInstanceImp(objProto)
{
  Value protect(this);
  // The constructor will be added later in StringObject's constructor
  put(exec,"length",Number(0),DontDelete|ReadOnly|DontEnum);

  put(exec,"toString",    new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::ToString,    0), DontEnum);
  put(exec,"valueOf",     new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::ValueOf,     0), DontEnum);
  put(exec,"charAt",      new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::CharAt,      1), DontEnum);
  put(exec,"charCodeAt",  new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::CharCodeAt,  1), DontEnum);
  put(exec,"indexOf",     new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::IndexOf,     2), DontEnum);
  put(exec,"lastIndexOf", new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::LastIndexOf, 2), DontEnum);
  put(exec,"match",       new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Match,       1), DontEnum);
  put(exec,"replace",     new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Replace,     2), DontEnum);
  put(exec,"search",      new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Search,      1), DontEnum);
  put(exec,"slice",       new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Slice,       0), DontEnum);
  put(exec,"split",       new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Split,       1), DontEnum);
  put(exec,"substr",      new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Substr,      2), DontEnum);
  put(exec,"substring",   new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Substring,   2), DontEnum);
  put(exec,"toLowerCase", new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::ToLowerCase, 0), DontEnum);
  put(exec,"toUpperCase", new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::ToUpperCase, 0), DontEnum);
#ifndef KJS_PURE_ECMA
  put(exec,"big",         new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Big,         0), DontEnum);
  put(exec,"small",       new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Small,       0), DontEnum);
  put(exec,"blink",       new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Blink,       0), DontEnum);
  put(exec,"bold",        new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Bold,        0), DontEnum);
  put(exec,"fixed",       new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Fixed,       0), DontEnum);
  put(exec,"italics",     new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Italics,     0), DontEnum);
  put(exec,"strike",      new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Strike,      0), DontEnum);
  put(exec,"sub",         new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Sub,         0), DontEnum);
  put(exec,"sup",         new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Sup,         0), DontEnum);
  put(exec,"fontcolor",   new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Fontcolor,   1), DontEnum);
  put(exec,"fontsize",    new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Fontsize,    1), DontEnum);
  put(exec,"anchor",      new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Anchor,      1), DontEnum);
  put(exec,"link",        new StringProtoFuncImp(exec,funcProto,StringProtoFuncImp::Link,        1), DontEnum);
#endif
}

// ------------------------------ StringProtoFuncImp ---------------------------

StringProtoFuncImp::StringProtoFuncImp(ExecState *exec,
                                       FunctionPrototypeImp *funcProto, int i, int len)
  : InternalFunctionImp(funcProto), id(i)
{
  Value protect(this);
  put(exec,"length",Number(len),DontDelete|ReadOnly|DontEnum);
}

bool StringProtoFuncImp::implementsCall() const
{
  return true;
}

// ECMA 15.5.4.2 - 15.5.4.20
Value StringProtoFuncImp::call(ExecState *exec, Object &thisObj, const List &args)
{
  Value result;

  // toString and valueOf are no generic function.
  if (id == ToString || id == ValueOf) {
    if (thisObj.isNull() || !thisObj.inherits(&StringInstanceImp::info)) {
      Object err = Error::create(exec,TypeError);
      exec->setException(err);
      return err;
    }

    return thisObj.internalValue().toString(exec);
  }

  String s2;
  Number n, m;
  UString u, u2, u3;
  int pos, p0, i;
  double d, d2;

  Value tv = thisObj;
  String s = tv.toString(exec);

  int len = s.value().size();
  Value a0 = args[0];
  Value a1 = args[1];

  switch (id) {
  case ToString:
  case ValueOf:
    // handled above
    break;
  case CharAt:
    n = a0.toInteger(exec);
    pos = (int) n.value();
    if (pos < 0 || pos >= len)
      u = "";
    else
      u = s.value().substr(pos, 1);
    result = String(u);
    break;
  case CharCodeAt:
    n = a0.toInteger(exec);
    pos = (int) n.value();
    if (pos < 0 || pos >= len)
      d = NaN;
    else {
      UChar c = s.value()[pos];
      d = (c.high() << 8) + c.low();
    }
    result = Number(d);
    break;
  case IndexOf:
    s2 = a0.toString(exec);
    if (a1.type() == UndefinedType)
      pos = 0;
    else
      pos = a1.toInteger(exec).intValue();
    d = s.value().find(s2.value(), pos);
    result = Number(d);
    break;
  case LastIndexOf:
    s2 = a0.toString(exec);
    d = a1.toNumber(exec).value();
    if (a1.type() == UndefinedType || KJS::isNaN(d) || KJS::isPosInf(d))
      pos = len;
    else
      pos = a1.toInteger(exec).intValue();
    if (pos < 0)
      pos = 0;
    d = s.value().rfind(s2.value(), pos);
    result = Number(d);
    break;
  case Match:
  case Search: {
    u = s.value();
    RegExp* reg = 0;
    if (a0.isA(ObjectType) && a0.toObject(exec).inherits(&RegExpImp::info))
      {
        //s2 = a0.get("source").toString();     // this loses the flags
        RegExpImp* imp = static_cast<RegExpImp *>( a0.toObject(exec).imp() );
        reg = imp->regExp();
      }
    else if (a0.isA(StringType))
      {
        //s2 = a0.toString();
        reg = new RegExp( a0.toString(exec).value(), RegExp::None );
      }
    else
      {
        printf("KJS: Match/Search. Argument is not a RegExp nor a String - returning Undefined\n");
        result = Undefined(); // No idea what to do here
        break;
      }
    {
      //RegExp reg(s2.value());
      UString mstr = reg->match(u, -1, &pos);
      if (a0.isA(StringType))
        delete reg;
      if (id == Search) {
        result = Number(pos);
        break;
      }
      if (mstr.isNull()) {
        result = Null();
        break;
      }
      /* TODO return an array, with the matches, etc. */
      result = String(mstr);
    }
  }
    break;
  case Replace:
    // TODO: this is just a hack to get the most common cases going
    u = s.value();
    if (a0.type() == ObjectType && a0.toObject(exec).inherits(&RegExpImp::info)) {
      s2 = Object::dynamicCast(a0).get(exec,"source").toString(exec);
      RegExp reg(s2.value());
      UString mstr = reg.match(u, -1, &pos);
      len = mstr.size();
    } else {
      s2 = a0.toString(exec);
      u2 = s2.value();
      pos = u.find(u2);
      len = u2.size();
    }
    if (pos == -1)
        result = s;
    else {
        u3 = u.substr(0, pos) + a1.toString(exec).value() +
             u.substr(pos + len);
        result = String(u3);
    }
    break;
  case Slice: // http://developer.netscape.com/docs/manuals/js/client/jsref/string.htm#1194366
    {
        // The arg processing is very much like ArrayProtoFunc::Slice
        // We return a new array
        result = exec->interpreter()->builtinArray().construct(exec,List::empty());
        int begin = args[0].toUInt32(exec);
        int end = len;
        if (args[1].type() != UndefinedType)
        {
          end = args[1].toUInt32(exec);
          if ( end < 0 )
            end += len;
        }
        // safety tests
        if ( begin < 0 || end < 0 || begin >= end ) {
            result = String();
            break;
        }
        //printf( "Slicing from %d to %d \n", begin, end );
        result = String( s.value().substr(begin, end-begin) );
        break;
    }
    case Split: {
    Object constructor = exec->interpreter()->builtinArray();
    Object res = Object::dynamicCast(constructor.construct(exec,List::empty()));
    result = res;
    u = s.value();
    i = p0 = 0;
    d = (a1.type() != UndefinedType) ? a1.toInteger(exec).intValue() : -1; // optional max number
    if (a0.type() == ObjectType && Object::dynamicCast(a0.imp()).inherits(&RegExpImp::info)) {
      Object obj0 = Object::dynamicCast(a0);
      RegExp reg(obj0.get(exec,"source").toString(exec).value());
      if (u.isEmpty() && !reg.match(u, 0).isNull()) {
	// empty string matched by regexp -> empty array
	res.put(exec,"length", Number(0));
	break;
      }
      int mpos;
      pos = 0;
      while (1) {
	// TODO: back references
	UString mstr = reg.match(u, pos, &mpos);
	if (mpos < 0)
	  break;
	pos = mpos + (mstr.isEmpty() ? 1 : mstr.size());
	if (mpos != p0 || !mstr.isEmpty()) {
	  res.put(exec,UString::from(i), String(u.substr(p0, mpos-p0)));
	  p0 = mpos + mstr.size();
	  i++;
	}
      }
    } else if (a0.type() != UndefinedType) {
      u2 = a0.toString(exec).value();
      if (u2.isEmpty()) {
	if (u.isEmpty()) {
	  // empty separator matches empty string -> empty array
	  put(exec,"length", Number(0));
	  break;
	} else {
	  while (i != d && i < u.size()-1)
	    res.put(exec,UString::from(i++), String(u.substr(p0++, 1)));
	}
      } else {
	while (i != d && (pos = u.find(u2, p0)) >= 0) {
	  res.put(exec,UString::from(i), String(u.substr(p0, pos-p0)));
	  p0 = pos + u2.size();
	  i++;
	}
      }
    }
    // add remaining string, if any
    if (i != d)
      res.put(exec,UString::from(i++), String(u.substr(p0)));
    res.put(exec,"length", Number(i));
    }
    break;
  case Substr:
    n = a0.toInteger(exec);
    m = a1.toInteger(exec);
    if (n.value() >= 0)
      d = n.value();
    else
      d = max(len + n.value(), 0);
    if (a1.type() == UndefinedType)
      d2 = len - d;
    else
      d2 = min(max(m.value(), 0), len - d);
    result = String(s.value().substr((int)d, (int)d2));
    break;
  case Substring: {
    n = a0.toInteger(exec);
    m = a1.toInteger(exec);
    double start = n.value();
    double end = m.value();
    if (KJS::isNaN(start))
      start = 0;
    if (KJS::isNaN(end))
      end = 0;
    if (start > len)
      start = len;
    if (end > len)
      end = len;
    if (a1.type() == UndefinedType)
      end = len;
    if (start > end) {
      double temp = end;
      end = start;
      start = temp;
    }
    result = String(s.value().substr((int)start, (int)(end-start)));
    }
    break;
  case ToLowerCase:
    u = UString(s.value());
    for (i = 0; i < len; i++)
      u[i] = u[i].toLower();
    result = String(u);
    break;
  case ToUpperCase:
    u = UString(s.value());
    for (i = 0; i < len; i++)
      u[i] = u[i].toUpper();
    result = String(u);
    break;
#ifndef KJS_PURE_ECMA
  case Big:
    result = String("<BIG>" + s.value() + "</BIG>");
    break;
  case Small:
    result = String("<SMALL>" + s.value() + "</SMALL>");
    break;
  case Blink:
    result = String("<BLINK>" + s.value() + "</BLINK>");
    break;
  case Bold:
    result = String("<B>" + s.value() + "</B>");
    break;
  case Fixed:
    result = String("<TT>" + s.value() + "</TT>");
    break;
  case Italics:
    result = String("<I>" + s.value() + "</I>");
    break;
  case Strike:
    result = String("<STRIKE>" + s.value() + "</STRIKE>");
    break;
  case Sub:
    result = String("<SUB>" + s.value() + "</SUB>");
    break;
  case Sup:
    result = String("<SUP>" + s.value() + "</SUP>");
    break;
  case Fontcolor:
    result = String("<FONT COLOR=" + a0.toString(exec).value() + ">"
		    + s.value() + "</FONT>");
    break;
  case Fontsize:
    result = String("<FONT SIZE=" + a0.toString(exec).value() + ">"
		    + s.value() + "</FONT>");
    break;
  case Anchor:
    result = String("<a name=" + a0.toString(exec).value() + ">"
		    + s.value() + "</a>");
    break;
  case Link:
    result = String("<a href=" + a0.toString(exec).value() + ">"
		    + s.value() + "</a>");
    break;
#endif
  }

  return result;
}

// ------------------------------ StringObjectImp ------------------------------

StringObjectImp::StringObjectImp(ExecState *exec,
                                 FunctionPrototypeImp *funcProto,
                                 StringPrototypeImp *stringProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  // ECMA 15.5.3.1 String.prototype
  put(exec,"prototype",stringProto,DontEnum|DontDelete|ReadOnly);

  put(exec,"fromCharCode", new StringObjectFuncImp(exec,funcProto), DontEnum);

  // no. of arguments for constructor
  put(exec,"length", Number(1), ReadOnly|DontDelete|DontEnum);
}


bool StringObjectImp::implementsConstruct() const
{
  return true;
}

// ECMA 15.5.2
Object StringObjectImp::construct(ExecState *exec, const List &args)
{
  Object proto = exec->interpreter()->builtinStringPrototype();
  Object obj(new StringInstanceImp(proto ));

  String s;
  if (args.size() > 0)
    s = args.begin()->toString(exec);
  else
    s = String("");

  obj.setInternalValue(s);
  obj.put(exec,"length",Number(s.value().size()),ReadOnly|DontEnum|DontDelete);

  return obj;
}

bool StringObjectImp::implementsCall() const
{
  return true;
}

// ECMA 15.5.1
Value StringObjectImp::call(ExecState *exec, Object &/*thisObj*/, const List &args)
{
  if (args.isEmpty())
    return String("");
  else {
    Value v = args[0];
    return v.toString(exec);
  }
}

// ------------------------------ StringObjectFuncImp --------------------------

// ECMA 15.5.3.2 fromCharCode()
StringObjectFuncImp::StringObjectFuncImp(ExecState *exec, FunctionPrototypeImp *funcProto)
  : InternalFunctionImp(funcProto)
{
  Value protect(this);
  put(exec,"length",Number(1),DontDelete|ReadOnly|DontEnum);
}

bool StringObjectFuncImp::implementsCall() const
{
  return true;
}

Value StringObjectFuncImp::call(ExecState *exec, Object &/*thisObj*/, const List &args)
{
  UString s;
  if (args.size()) {
    UChar *buf = new UChar[args.size()];
    UChar *p = buf;
    ListIterator it = args.begin();
    while (it != args.end()) {
      unsigned short u = it->toUInt16(exec);
      *p++ = UChar(u);
      it++;
    }
    s = UString(buf, args.size(), false);
  } else
    s = "";

  return String(s);
}
