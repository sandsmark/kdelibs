/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999-2000 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA 02111-1307, USA.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#ifndef HAVE_FLOAT_H   /* just for !Windows */
#define HAVE_FLOAT_H 0
#define HAVE_FUNC__FINITE 0
#endif

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#ifndef HAVE_FUNC_ISINF
#ifdef HAVE_IEEEFP_H
#include <ieeefp.h>
#endif
#endif /* HAVE_FUNC_ISINF */

#if HAVE_FLOAT_H
#include <float.h>
#endif

#include "operations.h"
#include "object.h"

using namespace KJS;

bool KJS::isNaN(double d)
{
#ifdef HAVE_FUNC_ISNAN
  return isnan(d);
#elif defined HAVE_FLOAT_H
  return _isnan(d) != 0;
#else
  return !(d == d);
#endif
}

bool KJS::isInf(double d)
{
#if defined(HAVE_FUNC_ISINF)
  return isinf(d);
#elif HAVE_FUNC_FINITE
  return finite(d) == 0 && d == d;
#elif HAVE_FUNC__FINITE
  return _finite(d) == 0 && d == d;
#else
  return false;
#endif
}

bool KJS::isPosInf(double d)
{
#if defined(HAVE_FUNC_ISINF)
  return (isinf(d) == 1);
#elif HAVE_FUNC_FINITE
  return finite(d) == 0 && d == d; // ### can we distinguish between + and - ?
#elif HAVE_FUNC__FINITE
  return _finite(d) == 0 && d == d; // ###
#else
  return false;
#endif
}

bool KJS::isNegInf(double d)
{
#if defined(HAVE_FUNC_ISINF)
  return (isinf(d) == -1);
#elif HAVE_FUNC_FINITE
  return finite(d) == 0 && d == d; // ###
#elif HAVE_FUNC__FINITE
  return _finite(d) == 0 && d == d; // ###
#else
  return false;
#endif
}

// ECMA 11.9.3
bool KJS::equal(ExecState *exec, const Value& v1, const Value& v2)
{
  Type t1 = v1.type();
  Type t2 = v2.type();

  if (t1 == t2) {
    if (t1 == UndefinedType || t1 == NullType)
      return true;
    if (t1 == NumberType)
      return (v1.toNumber(exec).value() == v2.toNumber(exec).value()); /* TODO: NaN, -0 ? */
    if (t1 == StringType)
      return (v1.toString(exec).value() == v2.toString(exec).value());
    if (t1 == BooleanType)
      return (v1.toBoolean(exec).value() == v2.toBoolean(exec).value());

    // types are Object
    return (v1.imp() == v2.imp());
  }

  // different types
  if ((t1 == NullType && t2 == UndefinedType) || (t1 == UndefinedType && t2 == NullType))
    return true;
  if (t1 == NumberType && t2 == StringType) {
    Number n2 = v2.toNumber(exec);
    return equal(exec,v1, n2);
  }
  if ((t1 == StringType && t2 == NumberType) || t1 == BooleanType) {
    Number n1 = v1.toNumber(exec);
    return equal(exec,n1, v2);
  }
  if (t2 == BooleanType) {
    Number n2 = v2.toNumber(exec);
    return equal(exec,v1, n2);
  }
  if ((t1 == StringType || t1 == NumberType) && t2 >= ObjectType) {
    Value p2 = v2.toPrimitive(exec);
    return equal(exec,v1, p2);
  }
  if (t1 >= ObjectType && (t2 == StringType || t2 == NumberType)) {
    Value p1 = v1.toPrimitive(exec);
    return equal(exec,p1, v2);
  }

  return false;
}

bool KJS::strictEqual(ExecState *exec, const Value &v1, const Value &v2)
{
  Type t1 = v1.type();
  Type t2 = v2.type();

  if (t1 != t2)
    return false;
  if (t1 == UndefinedType || t1 == NullType)
    return true;
  if (t1 == NumberType) {
    double n1 = v1.toNumber(exec).value();
    double n2 = v2.toNumber(exec).value();
    if (isNaN(n1) || isNaN(n2))
      return false;
    if (n1 == n2)
      return true;
    /* TODO: +0 and -0 */
    return false;
  } else if (t1 == StringType) {
    return v1.toString(exec).value() == v2.toString(exec).value();
  } else if (t2 == BooleanType) {
    return v1.toBoolean(exec).value() == v2.toBoolean(exec).value();
  }
  if (v1.imp() == v2.imp())
    return true;
  /* TODO: joined objects */

  return false;
}

int KJS::relation(ExecState *exec, const Value& v1, const Value& v2)
{
  Value p1 = v1.toPrimitive(exec,NumberType);
  Value p2 = v2.toPrimitive(exec,NumberType);

  if (p1.type() == StringType && p2.type() == StringType)
    return p1.toString(exec).value() < p2.toString(exec).value() ? 1 : 0;

  Number n1 = p1.toNumber(exec);
  Number n2 = p2.toNumber(exec);
  /* TODO: check for NaN */
  if (n1.value() == n2.value())
    return 0;
  /* TODO: +0, -0 and Infinity */
  return (n1.value() < n2.value());
}

double KJS::max(double d1, double d2)
{
  /* TODO: check for NaN */
  return (d1 > d2) ? d1 : d2;
}

double KJS::min(double d1, double d2)
{
  /* TODO: check for NaN */
  return (d1 < d2) ? d1 : d2;
}

// ECMA 11.6
Value KJS::add(ExecState *exec, const Value &v1, const Value &v2, char oper)
{
  Value p1 = v1.toPrimitive(exec);
  Value p2 = v2.toPrimitive(exec);

  if ((p1.type() == StringType || p2.type() == StringType) && oper == '+') {
    String s1 = p1.toString(exec);
    String s2 = p2.toString(exec);

    UString s = s1.value() + s2.value();

    return String(s);
  }

  Number n1 = p1.toNumber(exec);
  Number n2 = p2.toNumber(exec);

  if (oper == '+')
    return Number(n1.value() + n2.value());
  else
    return Number(n1.value() - n2.value());
}

// ECMA 11.5
Value KJS::mult(ExecState *exec, const Value &v1, const Value &v2, char oper)
{
  Number n1 = v1.toNumber(exec);
  Number n2 = v2.toNumber(exec);

  double result;

  if (oper == '*')
    result = n1.value() * n2.value();
  else if (oper == '/')
    result = n1.value() / n2.value();
  else
    result = fmod(n1.value(), n2.value());

  return Number(result);
}
