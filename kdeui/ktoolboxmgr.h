/*
    This file is part of the KDE libraries
    Copyright (C) 1998 Sven Radej (sven@lisa.exp.univie.ac.at)
              
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

#ifndef K_OWN_WM
#define K_OWN_WM

#include <qwindefs.h>
#include <qwidget.h>
#include <qtimer.h>
#include <qlist.h>
#include <qrect.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

 // $Id$
 // $Log$

 /**
  * KToolBoxManager is a class for self management of small windows.
  * Windows can be resized or moved. During move/resize it will emit
  * all changes and you can recompute and accept or discard new geometry.
  * You can define hot spots on the screen, and you will receive signal
  * when your widget enters it. KToolBoxManager doesn't draw any decoration.
  *
  * You will call (after constructing KToolBoxManager instance) doMove or
  * doResize when mouse presses some handle point of your widget. Then,
  * KToolBoxManager will grab the mouse and move (or resize) hollow rectangle
  * on the screen. On mouse release event or on stop() call it will actually
  * resize/move your widget unless your widget is a child widget.
  * if your widget is a child widget you will have to do the moving.<br>
  * Provided that your widget is top-level, the simplest use is like this:
  * <pre>
  * mousePressevent(QMouseEvent *)
  * {
  *  KToolBoxManager *mgr = new KToolBoxManager(this);
  *  doMove();
  * }
  * </pre>
  * For now, moving/resizing is always transparent. Word "resizer" refers to
  * hollow rectangle which is actually resized or moved.
  *
  * Functions doMove and doResize do not return untill mouse is released or
  * stop() function is called. However, this is QTimer driven so you will
  * receive signals, and Qt-engine will operate normally. Halting does not
  * hog CPU (it's not an empty for(;;) loop).
  *
  * @short Class for own window management.
  * @author Sven Radej <sven@lisa.exp.univie.ac.at>
  *
  */
class KToolBoxManager : public QObject
{
  Q_OBJECT
    
public:
  /**
   * Constructor. widget is the widget to be managed. It can be any custom
   * widget with QWidget as a base class.
   * blockme is internal thing and changing it has no efect.
   * @ref #doMove or @ref #doResize won't return till mouseRelease, or
   * @ref #stop . Qt will run normaly, because this thing is QTimer driven.
   * You can get position and size calling @ref #x , @ref #y , @ref width and
   * @ref #height
   */
  KToolBoxManager (QWidget *widget, bool blockme=true);

  /**
   * Destructor. If resizer is working, it will stop working, release mouse,
   * and move/resize the widget (i.e. will call @ref #stop) before death.
   */
  ~KToolBoxManager ();

  /**
   * Starts moving. If dynamic is true signal @ref #posChanged
   * will be emitted whenever position changes. If dynamic is false no signals
   * are emitted except @ref #onHotSpot when resizer enters a hot spot area.
   * Function will grab the mouse and will not return till end of drag. You
   * can call @ref #setGeometry and resizer will adapt to it whenever you want.
   * If dontmove is false, widget is moved to resizer's position when dragging
   * If dontmove is false, no move is done you have to do it. But if widget is
   * a child widget it will not be moved, regardless of parameter dontmove.
   * If KToolBoxManager already moves or resizes widget when you
   * call this function, it will return and do nothing.<br>
   * When dynamic is true, signal @ref #posChanged is emitted when resizer
   * changes position.<br>
   * When in_hotspot_static is true, resizer is not moved while in hotspot - only
   * mouse. Also @ref #posChanged is not emitted.
   * changes position.<br>
   
   * Hint: Call this function with dynamic=false and define hot spots.
   * @see #x
   * @see #y
   * @see #width
   * @see #height
   */
  void doMove(bool in_hotspot_static=false, bool dynamic = false, bool dontmove=false);

  /**
   * Starts resizing. If dynamic is true (default) signal @ref #sizeChanged
   * will be emitted whenever size changes. If dynamic is false, no signals
   * are emitted. Function will grab the mouse and will not return until
   * button mouse is released or @ref #stop function is called. You can call
   * @ref #setGeometry and resizer will adapt to it whenever you want.
   * If dontresize is false, widget is resized to resizer's size on the end.
   * If dontresize is true, widget is not resized, you have to do it. Widget
   * will not be resized if it is child widget. If KToolBoxManager already
   * moves or resizes widget when you  call this function, it will return
   * and do nothing.  When dynamic is true, signal @ref #sizeChanged is
   * emitted only when resizers changes size. You can resize the resize
   * with @ref #setGeometry or @ref #resize .
   * @see #x
   * @see #y
   * @see #width
   * @see #height
   */
  void doResize(bool dynamic = false, bool dontresize=false);

  /**
   * Adds region x, y, w, h to the lists of hot spots, and returns
   * index of that hot spot. When resizer enters that hot spot, signal
   * @ref #onHotSpot will be emitted. Only first hotspot is reported
   * That mean if you have overlaping hotspots the one with the lower index
   * will be reported. There is a special hotspot with index -1; exterior
   * of all hotspots(the whole screen minus defined hotspots). When resizer
   * leaves defined all hotspots @ref #onHotSpot (-1) will be emitted.
   *
   * Note that x and y must be global screen coordinates.
   * @see #removeHotSpot
   */
  int addHotSpot(int x, int y, int w, int h);

  /**
   * Removes hot spot index.
   * @see #addHotSpot
   */
  void removeHotSpot(int index);

    /**
   * Sets geometry of resizer. Does nothing if manager is not working (i.e.
   * if @ref #doMove or @ref #doResize were not called first). This function
   * will not emit signals @ref #sizeChanged ,  @ref #posChanged or
   * @ref #onHotSpot . Call this after you received some of those signals,
   * and you have to change geometry. For example, toolbar will change
   * geometry if dragged on hot spots - to tell user that it will -  if left
   * on that place -  re-embed itself into parent window. Note that x and y
   * must be global screen coordinates.
   */
  void setGeometry(int x, int y, int w, int h);

  /**
   * Sets geometry of resizer to geometry of hotspot index. This is
   * provided for conevenience. Does nothing is index is false.
   */
  void setGeometry(int index);

  /**
   * Resizes the resizer. rw is width and rh is height
   */
  void resize(int rw, int rh) {setGeometry(xp, yp, rw, rh);};

  /**
   * Returns global x coordinate of a mouse.
   */
   int mouseX() {return rx;};

  /**
   * Returns global y coordinate of a mouse.
   */
  int mouseY() {return ry;};
  
  /**
   * Returns global x coordinate of resizer.
   */
  int x() {return xp;};

  /**
   * Returns global y coordinate of resizer.
   */
  int y() {return yp;};

  /**
   * Returns width of resizer.
   */
  int width() {return w;};

  /**
   * Returns height resizer.
   */
  int height() {return h;};

  
public slots:

  /**
   * Calling this slot will stop the process of moving or resizing. It is
   * equal as if user releases the mouse button.
   */
  void stop ();

protected:

  /**
   * Internal - draws rectangle on the screen
   */
  void drawRectangle (int x, int y, int w, int h);

  /**
   * Internal - deletes last rectangle, if there is one.
   */
  void deleteLastRectangle ();

  /**
   * Internal - mode.
   */
  enum Mode {Nothing=0, Moving=1, Resizing=2};
  

protected slots:
  /**
   * Internal, QTimer driven mover.
   */
  void doMoveInternal();

  /**
   * Internal, QTimer driven sizer.
   */
  void doResizeInternal();
  
private:
  int xp, yp, w, h;
  int ox, oy, ow, oh;
  int orig_w, orig_h;
  bool noLast;
  bool working;
  bool dynamic;
  bool geometryChanged;
  bool blockme;
  bool dontmoveres;
  bool deepSpace;
  bool hotspot_static;
  Mode mode;
  
  QWidget *widget;
  QTimer *timer;

  QList<QRect> hotspots;
  QRect *last_hsp;
  
  int rx, ry, sx, sy;
  int offX, offY;

  /* X-stuff */

  Window root;
  GC rootgc;
  int scr;
  XEvent ev;

signals:
    
    /**
     * This signal is emitted when resizer changes position. Note:
     * x and y are global screen coordinates
     */
    void posChanged (int x, int y);

    /**
     * This signal is emitted when resizer changes size.
     */
    void sizeChanged (int w, int h);

    /**
     * This signal is emitted when resizer enter hot spot index.
     * It is also emited with index = -1 in the moment when resizer
     * leaves a hot spot but doesn't enter another hotspot. That is,
     * when it goes to "free space".
     */
    void onHotSpot (int id);

};

#endif