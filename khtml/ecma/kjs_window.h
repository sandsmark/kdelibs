/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 2000 Harri Porten (porten@kde.org)
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

#ifndef _KJS_WINDOW_H_
#define _KJS_WINDOW_H_

#include <qobject.h>
#include <kjs/object.h>
#include <kjs/function.h>

#include "kjs_binding.h"

class QTimer;
class KHTMLView;
class KHTMLPart;

namespace KJS {

  class WindowFunc;

  class Window : public QObject, public DOMObject {
    Q_OBJECT
    friend class WindowFunc;
  public:
    Window(KHTMLView *w) : widget(w), timer(0L) { }
    ~Window();
    virtual KJSO tryGet(const UString &p) const;
    virtual void tryPut(const UString &p, const KJSO& v);
    void installTimeout(const UString &handler, int t);
  public slots:
    void timeout();
  private:
    KHTMLView *widget;
    QTimer *timer;
    UString timeoutHandler;
  };

  class WindowFunc : public DOMFunction {
  public:
    WindowFunc(const Window *w, int i) : window(w), id(i) { };
    Completion tryExecute(const List &);
    enum { Alert, Confirm, Prompt, Open, SetTimeout };
    static void setStatusBarText(KHTMLPart *, const QString &);
  private:
    const Window *window;
    int id;
  };

}; // namespace

#endif
