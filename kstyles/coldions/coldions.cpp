/*
  This file is part of the KDE libraries
  Copyright (c) 1999 Matthias Elter <elter@kde.org>
  Copyright (c) 1999 Mosfet <mosfet@kde.org>
 
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

#ifndef INCLUDE_MENUITEM_DEF
#define INCLUDE_MENUITEM_DEF
#endif

#include <qmenudata.h>
#include <qpalette.h>
#include <qbitmap.h>
#include <qtabbar.h>

#include <kapp.h>

#include "coldions.h"

#define QCOORDARRLEN(x) sizeof(x)/(sizeof(QCOORD)*2)

static unsigned char combodeco_bits[] = {
    0xff, 0xff, 0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18};

static unsigned char checkfill_bits[] = {
    0x00, 0x00, 0x80, 0x01, 0x80, 0x00, 0xc0, 0x00, 0x40, 0x00, 0x60, 0x00,
    0x22, 0x00, 0x36, 0x00, 0x1c, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00};

static unsigned char checkoutline_bits[] = {
    0x80, 0x03, 0x40, 0x02, 0x40, 0x01, 0x20, 0x01, 0xa1, 0x00, 0x93, 0x00,
    0x55, 0x00, 0x49, 0x00, 0x22, 0x00, 0x24, 0x00, 0x18, 0x00, 0x10, 0x00};

static unsigned char up_bits[] = {
    0x00, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x00, 0x00};

static unsigned char down_bits[] = {
    0x00, 0x00, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x00};

static unsigned char left_bits[] = {
    0x30, 0x38, 0x3c, 0x3e, 0x3e, 0x3c, 0x38, 0x30};

static unsigned char right_bits[] = {
    0x0c, 0x1c, 0x3c, 0x7c, 0x7c, 0x3c, 0x1c, 0x0c};
    
ColdIonsStyle::ColdIonsStyle()
    :KStyle()
{
    checkOutline = new QBitmap(10, 12, checkoutline_bits, true);
    checkOutline->setMask(*checkOutline);
    checkFill = new QBitmap(10, 12, checkfill_bits, true);
    checkFill->setMask(*checkFill);
}

ColdIonsStyle::~ColdIonsStyle()
{
    delete checkOutline;
    delete checkFill;
}

void ColdIonsStyle::polish(QWidget *w)
{
    if ( !w->isTopLevel() ) {
        if (w->inherits("QLabel") || w->inherits("QPushButton")
            || w->inherits("QSlider"))
            w->setAutoMask(true);
    }
}

void ColdIonsStyle::unPolish(QWidget *w)
{
    if ( !w->isTopLevel() ) {
        if (w->inherits("QLabel") || w->inherits("QPushButton")
            || w->inherits("QSlider"))
            w->setAutoMask(false);
    }
}

void ColdIonsStyle::drawPushButtonLabel(QPushButton *btn, QPainter *p)
{
    // hack to bring round button text up one pixel
    int x = btn->x();
    int y = btn->y();
    int w = btn->width();
    int h = btn->height();
    if(w > 16 && h > 16)
        --x, --y, w-=2, h-=2;
    bool act = btn->isOn() || btn->isDown();
    drawItem(p, x+act?1:0, y+act?1:0, w, h,
             AlignCenter | ShowPrefix, btn->colorGroup(), btn->isEnabled(),
             btn->pixmap(), btn->text(), -1,
             act ? &btn->colorGroup().light() : &btn->colorGroup().buttonText());
}

void ColdIonsStyle::drawPushButton(QPushButton *btn, QPainter *p)
{
    int x, y, x2, y2;
    QColorGroup cg = btn->colorGroup();
    bool on = btn->isOn() || btn->isDown();
    QRect r = btn->rect();
    r.coords(&x, &y, &x2, &y2);

    if(r.width() > 16 && r.height() > 16){
        p->fillRect(r, on ? cg.brush(QColorGroup::Mid) :
                   cg.brush(QColorGroup::Button));
        QPointArray hPntArray, lPntArray;
        hPntArray.putPoints(0, 12, x+4,y+1, x+5,y+1, // top left
                            x+3,y+2, x+2,y+3, x+1,y+4, x+1,y+5,
                            x+1,y2-5, x+1,y2-4, x+2,y2-3, // half corners
                            x2-5,y+1, x2-4,y+1, x2-3,y+2);

        lPntArray.putPoints(0, 17, x2-5,y2-1, x2-4,y2-1, // btm right
                            x2-3,y2-2, x2-2,y2-3, x2-1,y2-5, x2-1,y2-4,
 
                            x+3,y2-2, x+4,y2-1, x+5,y2-1, //half corners
                            x2-2,y+3, x2-1,y+4, x2-1,y+5,

                            x2-5,y2-2, x2-4,y2-2, // testing
                            x2-3,y2-3,
                            x2-2,y2-5, x2-2,y2-4);

        p->setPen(on ? cg.dark() : cg.light());
        p->drawLine(x+6, y, x2-6, y);
        p->drawLine(0, y+6, 0, y2-6);
        p->drawPoints(hPntArray);

        p->setPen(on ? cg.light() : cg.dark());
        p->drawLine(x+6, y2, x2-6, y2);
        p->drawLine(x+6, y2-1, x2-6, y2-1);
        p->drawLine(x2, y+6, x2, y2-6);
        p->drawLine(x2-1, y+6, x2-1, y2-6);
        p->drawPoints(lPntArray);
    }
    else
        drawBevelButton(p, x, y, r.width(), r.height(),  cg, on,
                        &cg.brush(QColorGroup::Button));
}


void ColdIonsStyle::drawButtonMask(QPainter *p, int x, int y, int w, int h)
{
    // round edge fills
    static QCOORD btm_left_fill[]={ 0,0,1,0,2,0,3,0,4,0,0,1,1,1,2,1,3,1,4,1,
    1,2,2,2,3,2,4,2,2,3,3,3,4,3,3,4,4,4 };

    static QCOORD btm_right_fill[]={ 0,0,1,0,2,0,3,0,4,0,0,1,1,1,2,1,3,1,4,
    1,0,2,1,2,2,2,3,2,0,3,1,3,2,3,0,4,1,4 };

    static QCOORD top_left_fill[]={ 3,0,4,0,2,1,3,1,4,1,1,2,2,2,3,2,4,2,0,3,
    1,3,2,3,3,3,4,3,0,4,1,4,2,4,3,4,4,4 };

    static QCOORD top_right_fill[]={ 0,0,1,0,0,1,1,1,2,1,0,2,1,2,2,2,3,2,0,
    3,1,3,2,3,3,3,4,3,0,4,1,4,2,4,3,4,4,4 };

    QBrush fillBrush(color1, SolidPattern);
    p->setPen(color1);
    if(w > 16 && h > 16){
        int x2 = x+w-1;
        int y2 = y+h-1;
        QPointArray a(QCOORDARRLEN(top_left_fill), top_left_fill);
        a.translate(1, 1);
        p->drawPoints(a);
        a.setPoints(QCOORDARRLEN(btm_left_fill), btm_left_fill);
        a.translate(1, h-6);
        p->drawPoints(a);
        a.setPoints(QCOORDARRLEN(top_right_fill), top_right_fill);
        a.translate(w-6, 1);
        p->drawPoints(a);
        a.setPoints(QCOORDARRLEN(btm_right_fill), btm_right_fill);
        a.translate(w-6, h-6);
        p->drawPoints(a);

        p->fillRect(x+6, y, w-12, h, fillBrush);
        p->fillRect(x, y+6, x+6, h-12, fillBrush);
        p->fillRect(x2-6, y+6, x2, h-12, fillBrush);
        p->drawLine(x+6, y, x2-6, y);
        p->drawLine(x+6, y2, x2-6, y2);
        p->drawLine(x, y+6, x, y2-6);
        p->drawLine(x2, y+6, x2, y2-6);

    }
    else
        p->fillRect(x, y, w, h, fillBrush);
}

void ColdIonsStyle::drawButton(QPainter *p, int x, int y, int w, int h,
                            const QColorGroup &g, bool sunken,
                            const QBrush *fill)
{
    QPen oldPen = p->pen();
    int x2 = x+w-1;
    int y2 = y+h-1;

    if(!sunken){
        p->fillRect(x+1, y+1, w-2, h-2,
                    fill ? *fill : g.brush(QColorGroup::Button));
        p->setPen(g.light());
        p->drawLine(x, y, x2-1, y);
        p->drawLine(x, y, x, y2-1);
        //p->setPen(g.mid());
        //p->drawLine(x+1, y2-1, x2-1, y2-1);
        //p->drawLine(x2-1, y+1, x2-1, y2-1);
        p->setPen(g.dark());
        p->drawLine(x, y2, x2, y2);
        p->drawLine(x2, y, x2, y2);
    }
    else{
        p->fillRect(x+1, y+1, w-2, h-2,
                    fill ? *fill : g.brush(QColorGroup::Mid));
        p->setPen(g.dark());
        p->drawLine(x, y, x2-1, y);
        p->drawLine(x, y, x, y2-1);
        p->setPen(g.light());
        p->drawLine(x, y2, x2, y2);
        p->drawLine(x2, y, x2, y2);
    }
    p->setPen(oldPen);
}

QRect ColdIonsStyle::buttonRect(int x, int y, int w, int h)
{
    return(QRect(x+6, y+6, w-12, h-12));
}

void ColdIonsStyle::drawComboButton(QPainter *p, int x, int y, int w, int h,
                                  const QColorGroup &cg, bool sunken,
                                  bool, bool, const QBrush *fill)
{
    static QBitmap comboDeco(8, 8, combodeco_bits, true);
    if(!comboDeco.mask())
        comboDeco.setMask(comboDeco);
    
    drawButton(p, x, y, w, h, cg, sunken, fill ? fill :
               &cg.brush(QColorGroup::Background));
    int right = x+w-1;
    int bottom = y+h-1;
    
    p->setPen(cg.light());
    p->drawLine(right-16, y+5, right-16, bottom-5);
    p->setPen(cg.dark());
    p->drawLine(right-15, y+5, right-15, bottom-5);

    p->setPen(Qt::black);
    p->drawPixmap(right-12, y+(h-8)/2, comboDeco);
}

QRect ColdIonsStyle::comboButtonRect(int x, int y, int w, int h)
{
    return(QRect(x+4, y+4, w-23, h-8));
}

void ColdIonsStyle::drawComboButtonMask(QPainter *p, int x, int y, int w, int h)
{
    p->fillRect(x, y, w, h, QBrush(color1, SolidPattern));
}

void ColdIonsStyle::drawBevelButton(QPainter *p, int x, int y, int w, int h,
                                 const QColorGroup &g, bool sunken,
                                 const QBrush *fill)
{
    if(w > 2 && h > 2){
        p->setPen(Qt::black);
        p->drawRect(x, y, w, h);
        qDrawShadeRect(p, x+1, y+1, w-2, h-2, g, sunken, 1, 0,
                       fill ? fill : &g.brush(QColorGroup::Button));
    }
    else
        qDrawShadeRect(p, x, y, w, h, g, sunken, 1, 0,
                       fill ? fill : &g.brush(QColorGroup::Button));
}

void ColdIonsStyle::drawKMenuBar(QPainter *p, int x, int y, int w, int h,
                              const QColorGroup &cg, QBrush *)
{
    drawButton(p, x, y, w, h, cg, false);
}

void ColdIonsStyle::drawKToolBar(QPainter *p, int x, int y, int w, int h,
                              const QColorGroup &cg, bool)
{
    drawButton(p, x, y, w, h, cg, false);
}

void ColdIonsStyle::drawLightShadeRect(QPainter *p, int x, int y, int w, int h,
                                     const QColorGroup &g, const QBrush *fill)
{
    int x2 = x+w-1, y2 = y+h-1;
    p->setPen(g.dark());
    p->drawLine(x, y, x2, y);
    p->drawLine(x, y, x, y2);
    p->setPen(g.midlight());
    p->drawLine(x, y2, x2, y2);
    p->drawLine(x2, y, x2, y2);
    p->fillRect(x+1, y+1, x2-x-2, y2-y-2, fill ? *fill :
                g.brush(QColorGroup::Background));
}
    
void ColdIonsStyle::drawKBarHandle(QPainter *p, int x, int y, int w, int h,
                                 const QColorGroup &g, bool,
                                 QBrush *)
{
  drawButton(p, x, y, w, h, g, false, &g.brush(QColorGroup::Mid));

  if(h > w)
    {
      x += 2;
      y += 3;
      w = 5;
      h = 9;
    }
  else
    {
      x += 3;
      y += 2;
      w = 9;
      h = 5;
    }

  drawButton(p, x, y, w, h, g, false, &g.brush(QColorGroup::Midlight));
}

void ColdIonsStyle::drawKMenuItem(QPainter *p, int x, int y, int w, int h,
                                const QColorGroup &g, bool active,
                                QMenuItem *mi, QBrush *fill)
{
    if(active)
        qDrawShadeRect(p, x, y, w, h, g, false, 1, 0,
                       fill ? fill : &g.brush(QColorGroup::Midlight));
    else
        p->fillRect(x, y, w, h, fill ? *fill :
                    g.brush(QColorGroup::Background));
    
    QApplication::style().drawItem(p, x, y, w, h,
                                   AlignCenter|ShowPrefix|DontClip|SingleLine,
                                   g, mi->isEnabled(), mi->pixmap(), mi->text(),
                                   -1, active ? &g.dark() : &g.buttonText());
}

void ColdIonsStyle::drawKToolBarButton(QPainter *p, int x, int y, int w, int h,
                                     const QColorGroup &g, bool sunken,
                                     bool raised, bool enabled, bool popup,
                                     KToolButtonType icontext,
                                     const QString& btext, const QPixmap *pixmap,
                                     QFont *font)
{
  int x2 = x+w-1;
  int y2 = y+h-1;

    if (sunken)
      {
	p->setPen(g.dark());
	p->drawLine(x, y, x2, y);
	p->drawLine(x, y, x, y2);
    
	p->setPen(g.light());
	p->drawLine(x, y2, x2-1, y2);
	p->drawLine(x2, y, x2, y2-1);
	
	p->fillRect(x+1, y+1, w-2, h-2, g.brush(QColorGroup::Mid));

      }
    else if (raised)
      {
	p->setPen(g.light());
	p->drawLine(x, y, x2, y);
	p->drawLine(x, y, x, y2);
    
	p->setPen(g.dark());
	p->drawLine(x, y2, x2-1, y2);
	p->drawLine(x2, y, x2, y2-1);
	p->fillRect(x+1, y+1, w-2, h-2, g.brush(QColorGroup::Midlight));
      }
   
    int dx, dy;

    if (icontext == Icon){ // icon only
        if (pixmap){
            dx = ( w - pixmap->width() ) / 2;
            dy = ( h - pixmap->height() ) / 2;
            if ( sunken )
            {
                ++dx;
                ++dy;
            }
            p->drawPixmap( x+dx, y+dy, *pixmap );
        }
    }
    else if (icontext == IconTextRight){ // icon and text (if any)
        if (pixmap){
            dx = 1;
            dy = ( h - pixmap->height() ) / 2;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            p->drawPixmap( x+dx, y+dy, *pixmap );
        }
        if (!btext.isNull()){
            int tf = AlignVCenter|AlignLeft;
            if (!enabled)
                p->setPen(g.dark());
            if (pixmap)
                dx= pixmap->width();
            else
                dx= 1;
            dy = 0;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            if (font)
                p->setFont(*font);
            if(raised)
                p->setPen(Qt::blue);
            p->drawText(x+dx, y+dy, w-dx, h, tf, btext);
        }
    }
    else if (icontext == Text){ // only text, even if there is a icon
        if (!btext.isNull()){
            int tf = AlignVCenter|AlignLeft;
            if (!enabled)
                p->setPen(g.dark());
            dx= 1;
            dy= 0;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            if (font)
                p->setFont(*font);
            if(raised)
                p->setPen(Qt::blue);
            p->drawText(x+dx, y+dy, w-dx, h, tf, btext);
        }
    }
    else if (icontext == IconTextBottom){
        if (pixmap){
            dx = (w - pixmap->width()) / 2;
            dy = 1;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            p->drawPixmap( x+dx, y+dy, *pixmap );
        }
        if (!btext.isNull()){
            int tf = AlignBottom|AlignHCenter;
            if (!enabled)
                p->setPen(g.dark());
            dy= pixmap->height();
            dx = 2;
            if ( sunken ){
                ++dx;
                ++dy;
            }
            if (font)
                p->setFont(*font);
            if(raised)
                p->setPen(Qt::blue);
            p->drawText(x, y, w, h-4, tf, btext);
        }
    }
    if (popup){
        if (enabled)
            qDrawArrow (p, Qt::DownArrow, Qt::WindowsStyle, false, w-5, h-5, 0, 0,
                        g, true);
        else
            qDrawArrow (p, Qt::DownArrow, Qt::WindowsStyle, false, w-5, h-5,
                        0, 0, g, false);
    }
}

void ColdIonsStyle::drawScrollBarControls( QPainter *p, const QScrollBar *sb,
                                         int sliderStart,
                                         unsigned int controls,
                                         unsigned int activeControl )
{
    QRect addB, subB, addPageR, subPageR, sliderR;
    bool horiz = sb->orientation() == QScrollBar::Horizontal;
    int sliderMin, sliderMax, sliderLength, buttonDim;
    int addX, addY, subX, subY;
    int length = horiz ? sb->width()  : sb->height();
    int extent = horiz ? sb->height() : sb->width();
    QColorGroup g  = sb->colorGroup();

    scrollBarMetrics( sb, sliderMin, sliderMax, sliderLength, buttonDim );

    if(sliderStart > sliderMax)
        sliderStart = sliderMax;

    if(horiz){
        subY = addY = (extent - buttonDim)/2;
        subX = length - (buttonDim*2);
        addX = length - buttonDim;
    }
    else{
        subX = addX = (extent - buttonDim) / 2;
        subY = length - (buttonDim*2);
        addY = length - buttonDim;
    }

    subB.setRect(subX, subY, buttonDim, buttonDim);
    addB.setRect(addX, addY, buttonDim, buttonDim);

    int sliderEnd = sliderStart + sliderLength;
    int sliderW = extent;
    if(horiz){
        subPageR.setRect(1, 0, sliderStart - 1 , sliderW);
	addPageR.setRect(sliderEnd, 0, subX - sliderEnd, sliderW);
	sliderR.setRect(sliderStart, 0, sliderLength, sliderW);
    }
    else{
        subPageR.setRect(0, 1, sliderW, sliderStart - 1);
	addPageR.setRect(0, sliderEnd, sliderW, subY - sliderEnd);
	sliderR.setRect(0, sliderStart, sliderW, sliderLength);
    }

    bool maxedOut = (sb->maxValue() == sb->minValue());
    if(controls & AddLine){
        drawBevelButton(p, addB.x(), addB.y(), addB.width(), addB.height(), g,
                        activeControl == AddLine);
        p->setPen(g.shadow());
        p->drawRect(addB);
        drawArrow(p, horiz ? RightArrow : DownArrow, false, addB.x()+2,
                  addB.y()+2,  addB.width()-4, addB.height()-4, g,
                  !maxedOut, activeControl == AddLine ?
                  &g.brush( QColorGroup::Mid) : &g.brush(QColorGroup::Button));
    }
    if(controls & SubLine){
        drawBevelButton(p, subB.x(), subB.y(), subB.width(), subB.height(), g,
                        activeControl == SubLine);
        p->setPen(g.shadow());
	p->drawRect(subB);
        drawArrow(p, horiz ? LeftArrow : UpArrow, false, subB.x()+2,
                  subB.y()+2, subB.width()-4, subB.height()-4, g, !maxedOut,
                  activeControl == SubLine ? &g.brush(QColorGroup::Mid) :
                  &g.brush(QColorGroup::Button));
    }
    if(controls & SubPage)
        drawScrollBarBackground(p, subPageR.x(), subPageR.y(), subPageR.width(),
                                subPageR.height(), g, horiz );
    if(controls & AddPage)
        drawScrollBarBackground(p, addPageR.x(), addPageR.y(), addPageR.width(),
                                addPageR.height(), g, horiz );
    if(controls & Slider){
        drawBevelButton(p, sliderR.x(), sliderR.y(), sliderR.width(),
                        sliderR.height(), g, false,
                        &g.brush(QColorGroup::Button));

        if(sliderR.width() >= 14 && sliderR.height() >= 14){
            int x = sliderR.x() + (sliderR.width()-8)/2;
            int y = sliderR.y() + (sliderR.height()-8)/2;
            drawLightShadeRect(p, x, y, 8, 8, g, activeControl == Slider ?
                               &g.brush(QColorGroup::Midlight) :
                               &g.brush(QColorGroup::Mid));
        }
    }
}

QSize ColdIonsStyle::indicatorSize() const
{
    return(QSize(16, 16));
}

void ColdIonsStyle::drawIndicator(QPainter *p, int x, int y, int w, int h,
                               const QColorGroup &cg, int state, bool down, bool)
{
    drawButton(p, x, y, w, h, cg, down);
    if(state != QButton::Off){
        QPen oldPen = p->pen();
	p->setPen(cg.light());
        p->drawLine(x+5, y+6, x+5, y+10);
        p->drawLine(x+5, y+10, x+w-4, y+3);
        p->setPen(cg.dark());
        p->drawLine(x+5, y+11, x+w-4, y+4);
        p->drawLine(x+6, y+6, x+6, y+7);
        p->setPen(cg.mid());
        p->drawLine(x+6, y+11, x+w-4, y+5);
        p->drawLine(x+6, y+8, x+7, y+7);
        p->setPen(oldPen);
    }
}

QSize ColdIonsStyle::exclusiveIndicatorSize() const
{
    return(QSize(15,15));
}

void ColdIonsStyle::drawExclusiveIndicator(QPainter *p, int x, int y, int w,
                                        int h, const QColorGroup &cg, bool on,
                                        bool down, bool)
{
    static QCOORD circle_dark[] = {5,1, 6,1, 7,1, 8,1, 9,1, 10,1,
    3,2, 4,2,
    2,3,
    2,4,
    1,5,
    1,6,
    1,7,
    1,8,
    1,9,
    2,10
    };

    static QCOORD circle_mid[] = {5,0, 6,0, 7,0, 8,0, 9,0,
    3,1, 4,1, 11,1,
    2,2, 10,2, 12,2,
    1,3, 3,3,
    1,4,
    0,5,
    0,6,
    0,7,
    0,8,
    0,9,
    1,10,
    1,11,
    2,12
    };

    static QCOORD circle_light[] = {14,5,
    14,6,
    14,7,
    14,8,
    14,9,
    13,10,
    13,11,
    12,12,
    4,13, 5,13, 10,13, 11,13,
    5,14, 6,14, 7,14, 8,14, 9,14
    };

    static QCOORD fill_lines[] = {6,3, 9,3, 3,6, 3,9, 12,6, 12,9, 6,12, 9,12};

    QPen oldPen = p->pen();
    p->fillRect( x, y, w, h, cg.brush(QColorGroup::Background));
    QPointArray a( QCOORDARRLEN(circle_dark), circle_dark );
    a.translate( x, y );
    p->setPen(cg.dark() );
    p->drawPoints( a );
    a.setPoints( QCOORDARRLEN(circle_mid), circle_mid );
    a.translate( x, y );
    p->setPen(cg.mid() );
    p->drawPoints( a );
    a.setPoints( QCOORDARRLEN(circle_light), circle_light );
    a.translate( x, y );
    p->setPen(cg.light() );
    p->drawPoints( a );
    if(on || down){
        if(down)
            p->setPen(cg.mid());
        a.setPoints(QCOORDARRLEN(fill_lines), fill_lines);
        a.translate(x,y);
        p->drawLineSegments(a);
        p->fillRect(4, 4, 8, 8, (down)? cg.brush(QColorGroup::Mid) :
                    cg.brush(QColorGroup::Light));
    }
    p->setPen(oldPen);
}

void ColdIonsStyle::drawSliderGroove(QPainter *p, int x, int y, int w, int h,
                                    const QColorGroup &g, QCOORD,
                                    Orientation)
{
    qDrawShadePanel(p, x, y, w, h, g, true, 1, &g.brush(QColorGroup::Dark));
}

int ColdIonsStyle::sliderLength() const
{
    return(18);
}

void ColdIonsStyle::drawSlider(QPainter *p, int x, int y, int w, int h,
                            const QColorGroup &g, Orientation orient,
                            bool, bool)
{
    p->setPen(Qt::black);
    p->drawRect(x, y, w, h);

    if(orient == Horizontal){
        qDrawShadeRect(p, x+1, y+1, 4, h-2, g, false, 1, 0,
                       &g.brush(QColorGroup::Button));
        qDrawShadeRect(p, x+6, y+1, 6, h-2, g, false, 1, 0,
                       &g.brush(QColorGroup::Mid));
        qDrawShadeRect(p, x+13, y+1, 4, h-2, g, false, 1, 0,
                       &g.brush(QColorGroup::Button));
    }
    else{
        qDrawShadeRect(p, x+1, y+1, w-2, 4, g, false, 1, 0,
                       &g.brush(QColorGroup::Button));
        qDrawShadeRect(p, x+1, y+6, w-2, 6, g, false, 1, 0,
                       &g.brush(QColorGroup::Mid));
        qDrawShadeRect(p, x+1, y+13, w-2, 4, g, false, 1, 0,
                       &g.brush(QColorGroup::Button));
    }

}

void ColdIonsStyle::drawArrow(QPainter *p, Qt::ArrowType type, bool on, int x,
                            int y, int w, int h, const QColorGroup &g,
                            bool enabled, const QBrush *)
{
    static QBitmap up(8, 8, up_bits, true);
    static QBitmap down(8, 8, down_bits, true);
    static QBitmap left(8, 8, left_bits, true);
    static QBitmap right(8, 8, right_bits, true);

    if(!up.mask()){
        up.setMask(up);
        down.setMask(down);
        left.setMask(left);
        right.setMask(right);
    }
    
    p->setPen(enabled ? on ? g.light() : Qt::black : g.mid());
    if(w > 8){
        x = x + (w-8)/2;
        y = y + (h-8)/2;
    }
    switch(type){
    case Qt::UpArrow:
        p->drawPixmap(x, y, up);
        break;
    case Qt::DownArrow:
        p->drawPixmap(x, y, down);
        break;
    case Qt::LeftArrow:
        p->drawPixmap(x, y, left);
        break;
    default:
        p->drawPixmap(x, y, right);
        break;
    }
}

void ColdIonsStyle::drawPopupMenuItem( QPainter* p, bool checkable, int maxpmw,
                                     int tab, QMenuItem* mi,
                                     const QPalette& pal, bool act,
                                     bool enabled, int x, int y, int w, int h)
{
static const int motifItemFrame         = 2;
static const int motifItemHMargin       = 3;
static const int motifItemVMargin       = 2;
static const int motifArrowHMargin      = 6;
static const int windowsRightBorder     = 12;

    if(act){
        bool dis = !enabled;
        QColorGroup itemg = dis ? pal.disabled() : pal.active();

        if (checkable)
            maxpmw = QMAX( maxpmw, 12 );
        int checkcol = maxpmw;

        int x2 = x+w-1;
        int y2 = y+h-1;
        p->setPen(itemg.light());
        p->drawLine(x, y, x2, y);
        p->drawLine(x, y, x, y2);
        p->setPen(itemg.dark());
        p->drawLine(x2, y, x2, y2);
        p->drawLine(x, y2, x2, y2);
        p->fillRect(x+1, y+1, w-2, h-2, itemg.midlight());
        if ( mi->iconSet() ) {
            QIconSet::Mode mode = dis? QIconSet::Disabled : QIconSet::Normal;
            if (!dis)
                mode = QIconSet::Active;
            QPixmap pixmap = mi->iconSet()->pixmap(QIconSet::Small, mode);
            int pixw = pixmap.width();
            int pixh = pixmap.height();
            QRect cr(x, y, checkcol, h);
            QRect pmr(0, 0, pixw, pixh);
            pmr.moveCenter( cr.center() );
            p->setPen(itemg.text());
            p->drawPixmap(pmr.topLeft(), pixmap );

        }
        else if(checkable) {
            int mw = checkcol + motifItemFrame;
            int mh = h - 2*motifItemFrame;
            if (mi->isChecked()){
                drawCheckMark( p, x + motifItemFrame,
                               y+motifItemFrame, mw, mh, itemg, act, dis );
            }
        }
        p->setPen(itemg.dark());
        QColor discol;
        if (dis) {
            discol = itemg.text();
            p->setPen(discol);
        }
        int xm = motifItemFrame + checkcol + motifItemHMargin;
        QString s = mi->text();
        if (!s.isNull()) {
            int t = s.find( '\t' );
            int m = motifItemVMargin;
            const int text_flags = AlignVCenter|ShowPrefix | DontClip | SingleLine;
            if (t >= 0) {
                p->drawText(x+w-tab-windowsRightBorder-motifItemHMargin-motifItemFrame,
                            y+m, tab, h-2*m, text_flags, s.mid( t+1 ));
            }
            p->drawText(x+xm, y+m, w-xm-tab+1, h-2*m, text_flags, s, t);
        } else if (mi->pixmap()) {
            QPixmap *pixmap = mi->pixmap();
            if (pixmap->depth() == 1)
                p->setBackgroundMode(OpaqueMode);
            p->drawPixmap( x+xm, y+motifItemFrame, *pixmap);
            if (pixmap->depth() == 1)
                p->setBackgroundMode(TransparentMode);
        }
        if (mi->popup()) {
            int dim = (h-2*motifItemFrame) / 2;
            if (!dis)
                discol = itemg.text();
            QColorGroup g2(discol, itemg.highlight(),
                           white, white,
                           dis ? discol : white,
                           discol, white);
            drawArrow(p, RightArrow, true,
                      x+w - motifArrowHMargin - motifItemFrame - dim,  y+h/2-dim/2,
                      dim, dim, itemg, TRUE);
        }
    }
    else
        KStyle::drawPopupMenuItem(p, checkable, maxpmw, tab, mi, pal, act,
                                  enabled, x, y, w, h);
}

