/**
 * This file is part of the DOM implementation for KDE.
 *
 * (C) 1999 Lars Knoll (knoll@kde.org)
 * (C) 2000 Dirk Mueller (mueller@kde.org)
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
//#define DEBUG_LAYOUT
//#define BIDI_DEBUG

#include "render_text.h"
#include "break_lines.h"
#include "render_style.h"

#include "misc/loader.h"

#include <qfontmetrics.h>
#include <qfontinfo.h>
#include <qfont.h>
#include <qpainter.h>
#include <qstring.h>
#include <qcolor.h>
#include <qrect.h>
#include <kdebug.h>

#include <assert.h>

#define QT_ALLOC_QCHAR_VEC( N ) (QChar*) new char[ sizeof(QChar)*( N ) ]
#define QT_DELETE_QCHAR_VEC( P ) delete[] ((char*)( P ))

using namespace khtml;
using namespace DOM;

TextSlave::~TextSlave()
{
    if(deleteText)
        QT_DELETE_QCHAR_VEC(m_text);
}


void TextSlave::print( QPainter *p, int _tx, int _ty)
{
    if (!m_text || len <= 0)
        return;

    QConstString s(m_text, len);
    //kdDebug( 6040 ) << "textSlave::printing(" << s.string() << ") at(" << x+_tx << "/" << y+_ty << ")" << endl;
    p->drawText(m_x + _tx, m_y + _ty + m_baseline, s.string());
}

void TextSlave::printSelection(QPainter *p, RenderStyle* style, int tx, int ty, int startPos, int endPos)
{
    if(startPos > len) return;
    if(startPos < 0) startPos = 0;

    int _len = len;
    int _width = m_width;
    if(endPos > 0 && endPos < len) {
        _len = endPos;
    }
    _len -= startPos;

    QConstString s(m_text+startPos , _len);

    if (_len != len)
        _width = p->fontMetrics().width(s.string());

    int _offset = 0;
    if ( startPos > 0 ) {
        QConstString aStr(m_text, startPos);
        _offset = p->fontMetrics().width(aStr.string());
    }

    p->save();
    QColor c = style->color();
    p->setPen(QColor(0xff-c.red(),0xff-c.green(),0xff-c.blue()));
    p->fillRect(m_x + tx + _offset, m_y + ty, _width, m_height, c);

    ty += m_baseline;

    //kdDebug( 6040 ) << "textSlave::printing(" << s.string() << ") at(" << x+_tx << "/" << y+_ty << ")" << endl;
    p->drawText(m_x + tx + _offset, m_y + ty, s.string());
    p->restore();
}

// no blink at the moment...
void TextSlave::printDecoration( QPainter *p, int _tx, int _ty, int deco)
{
    _tx += m_x;
    _ty += m_y;

    int underlineOffset = m_height/7 + m_baseline;
    if(underlineOffset == m_baseline) underlineOffset++;

    if(deco & UNDERLINE)
        p->drawLine(_tx, _ty + underlineOffset, _tx + m_width, _ty + underlineOffset );
    if(deco & OVERLINE)
        p->drawLine(_tx, _ty, _tx + m_width, _ty );
    if(deco & LINE_THROUGH)
        p->drawLine(_tx, _ty + 2*m_baseline/3, _tx + m_width, _ty + 2*m_baseline/3 );
// ### add BLINK
}

void TextSlave::printBoxDecorations(QPainter *pt, RenderStyle* style, RenderText *p, int _tx, int _ty, bool begin, bool end)
{
    _tx += m_x;
    _ty += m_y;

    bool parentInline = p->parent()->isInline();

    //kdDebug( 6040 ) << "renderBox::printDecorations()" << endl;
    if (parentInline)
        _ty -= p->paddingTop() + p->borderTop();

    int width = m_width;
    if(begin && parentInline)
        _tx -= p->paddingLeft() + p->borderLeft();

    QColor c = style->backgroundColor();
    CachedImage *i = style->backgroundImage();
    if(c.isValid() && (!i || i->tiled_pixmap(c).mask()))
        pt->fillRect(_tx, _ty, width, m_height, c);

    if(i)
    {
        // ### might need to add some correct offsets
        // ### use paddingX/Y
        pt->drawTiledPixmap(_tx + p->borderLeft(), _ty + p->borderTop(),
                            m_width + p->paddingLeft() + p->paddingRight(),
                            m_height + p->paddingTop() + p->paddingBottom(), i->tiled_pixmap(c));
    }

    if(style->hasBorder())
        p->printBorder(pt, _tx, _ty, width, m_height + p->paddingTop() + p->paddingBottom() +
                       p->borderTop() + p->borderBottom(), style, begin, end);

#ifdef BIDI_DEBUG
    int h = m_height + p->paddingTop() + p->paddingBottom() + p->borderTop() + p->borderBottom();
    c = QColor("#0000ff");
    p->drawBorder(pt, _tx, _ty, _tx, _ty + h, 1,
                  RenderObject::BSLeft, c, SOLID);
    p->drawBorder(pt, _tx + width, _ty, _tx + width, _ty + h, style->borderRightWidth(),
                  RenderObject::BSRight, c, SOLID);
#endif
}

void TextSlave::printActivation( QPainter *p, int _tx, int _ty)
{
  p->drawRect(_tx+m_x-2,_ty+m_y, m_width+3, m_height+2);
  p->drawRect(_tx+m_x-3,_ty+m_y+1, m_width+5, m_height);
}


bool TextSlave::checkPoint(int _x, int _y, int _tx, int _ty)
{
    if((_ty + m_y > _y) || (_ty + m_y + m_height < _y) ||
       (_tx + m_x > _x) || (_tx + m_x + m_width < _x))
        return false;
    return true;
}

// -----------------------------------------------------------------------------

TextSlaveArray::TextSlaveArray()
{
    setAutoDelete(true);
}

int TextSlaveArray::compareItems( Item d1, Item d2 )
{
    ASSERT(d1);
    ASSERT(d2);

    return static_cast<TextSlave*>(d1)->m_y - static_cast<TextSlave*>(d2)->m_y;
}

// remove this once QVector::bsearch is fixed
int TextSlaveArray::findFirstMatching( Item d) const
{
    int len = count();

    if ( !len )
	return -1;
    if ( !d )
	return -1;
    int n1 = 0;
    int n2 = len - 1;
    int mid = 0;
    bool found = FALSE;
    while ( n1 <= n2 ) {
	int  res;
	mid = (n1 + n2)/2;
	if ( (*this)[mid] == 0 )			// null item greater
	    res = -1;
	else
	    res = ((QGVector*)this)->compareItems( d, (*this)[mid] );
	if ( res < 0 )
	    n2 = mid - 1;
	else if ( res > 0 )
	    n1 = mid + 1;
	else {					// found it
	    found = TRUE;
	    break;
	}
    }
    if ( !found && !1)
	return -1;
    // search to first one equal or bigger
    while ( found && (mid > 0) && !((QGVector*)this)->compareItems(d, (*this)[mid-1]) )
	mid--;
    return mid;
}

// -------------------------------------------------------------------------------------

RenderText::RenderText(DOMStringImpl *_str)
    : RenderObject()
{
    // init RenderObject attributes
    m_isText = true;   // our object inherits from RenderText
    m_inline = true;   // our object is Inline

    m_minWidth = -1;
    m_maxWidth = -1;
    str = _str;
    if(str) str->ref();
    fm = 0;

    m_selectionState = SelectionNone;

#ifdef DEBUG_LAYOUT
    QConstString cstr(str->s, str->l);
    kdDebug( 6040 ) << "RenderText::setText '" << (const char *)cstr.string().utf8() << "'" << endl;
#endif
}

void RenderText::setStyle(RenderStyle *style)
{
    RenderObject::setStyle(style);
    delete fm;
    fm = new QFontMetrics(m_style->font());
    m_contentHeight = m_style->lineHeight().width(fm->height());
}

RenderText::~RenderText()
{
    deleteSlaves();
    delete fm; fm = 0;
    if(str) str->deref();
}

void RenderText::deleteSlaves()
{
    // this is a slight variant of QArray::clear().
    // We don't delete the array itself here because its
    // likely to be used in the same size later again, saves
    // us resize() calls
    unsigned int len = m_lines.size();
    for(unsigned int i=0; i < len; i++)
        m_lines.remove(i);

    ASSERT(m_lines.count() == 0);
}

bool RenderText::checkPoint(int _x, int _y, int _tx, int _ty, int &offset)
{
    TextSlave *s = m_lines.count() ? m_lines[0] : 0;
    int si = 0;
    while(s)
    {
        if( s->checkPoint(_x, _y, _tx, _ty) )
        {
            // now we need to get the exact position
            int delta = _x - _tx - s->m_x;
            int pos = 0;
            while(pos < s->len)
            {
                // ### this will produce wrong results for RTL text!!!
                int w = fm->width(*(s->m_text+pos));
                int w2 = w/2;
                w = w - w2;
                delta -= w2;
                if(delta <= 0) break;
                pos++;
                delta -= w;
            }
            offset = s->m_text - m_lines[0]->m_text + pos;
            //kdDebug( 6040 ) << " Text  --> inside at position " << offset << endl;

            return true;
        }
        // ### this might be wrong, if combining chars are used ( eg arabic )
        s = si < (int)m_lines.count()-1 ? m_lines[++si] : 0;
    }
    return false;
}

void RenderText::cursorPos(int offset, int &_x, int &_y, int &height)
{
  if (!m_lines.count()) {
    _x = _y = height = -1;
    return;
  }

  TextSlave* s = m_lines[0];
  int si = 0;
  _x = 0;
  int off = s->len;
  while(offset > off && si < (int)m_lines.count())
  {
      s = m_lines[++si];
      off = s->m_text - m_lines[0]->m_text + s->len;
  }   // we are now in the correct text slave
  int pos = (offset > off ? s->len : s->len - (off - offset ));
  _y = s->m_y;
  height = s->m_height;

  QString tekst(s->m_text, s->len);
  _x = s->m_x + (fm->boundingRect(tekst, pos)).right();
  if(pos)
      _x += fm->rightBearing( *(s->m_text + pos - 1 ) );

  int absx, absy;
  absolutePosition(absx,absy);
  if (absx == -1) {
    // we don't know out absolute position, and there is not point returning
    // just a relative one
    _x = _y = -1;
  }
  else {
    _x += absx;
    _y += absy;
  }
}

void RenderText::absolutePosition(int &xPos, int &yPos, bool)
{
    if(m_parent) {
        m_parent->absolutePosition(xPos, yPos, false);
        if ( m_lines.count() ) {
            TextSlave* s = m_lines[0];
            xPos += s->m_x;
            yPos += s->m_y;
            return;
        }
    }

    xPos = yPos = -1;
}

void RenderText::posOfChar(int chr, int &x, int &y)
{
    if (!m_parent)
    {
       x = -1;
       y = -1;
       return;
    }
    m_parent->absolutePosition( x, y, false );
    if( chr > (int) str->l )
	chr = str->l;

    int si = 0;
    TextSlave *s = m_lines[0];
    TextSlave *last = s;
    QChar *ch = str->s + chr;
    while ( s && ch >= s->m_text )
    {
        last = s;
        s = m_lines[++si];
    }

    x += last->m_x;
    y += last->m_y;
}

void RenderText::printObject( QPainter *p, int /*x*/, int y, int /*w*/, int h,
                      int tx, int ty)
{
    RenderStyle* pseudoStyle = m_style->getPseudoStyle(RenderStyle::FIRST_LINE);
    int d = m_style->textDecoration();
    TextSlave f(0, y-ty);
    int firstSi = m_lines.findFirstMatching(&f);
    int si = firstSi;
    if(si >= 0)
    {
        // something matching found, find the first one to print
        while(si > 0 && m_lines[si-1]->checkVerticalPoint(y, ty, h))
            si--;

        firstSi = si;

        int endPos, startPos;
        bool breakAtEnd = false;
        if (selectionState() != SelectionNone)
        {
            if (selectionState() == SelectionInside)
            {
                startPos = 0;
                endPos = -1;
            }
            else
            {
                selectionStartEnd(startPos, endPos);
                breakAtEnd = true;
                if(selectionState() == SelectionStart) {
                    endPos = -1;
                    breakAtEnd = false;
                }
                else if(selectionState() == SelectionEnd)
                    startPos = 0;
            }
        }

        // run until we find one that is outside the range, then we
        // know we can stop
        do {
            TextSlave* s = m_lines[si];
            RenderStyle* style = pseudoStyle && s->firstLine ? pseudoStyle : m_style;

            if(style->font() != p->font())
                p->setFont(style->font());

            if(style->color() != p->pen().color())
                p->setPen(style->color());

            if((m_printSpecial  &&
                (m_parent->isInline() || pseudoStyle)) &&
               (!pseudoStyle || s->firstLine))
                s->printBoxDecorations(p, style, this, tx, ty, si == 0, si == (int)m_lines.count());

            s->print(p, tx, ty);

            if(d != TDNONE && hasKeyboardFocus == DOM::ActivationOff)
            {
                p->setPen(m_style->textDecorationColor());
                s->printDecoration(p, tx, ty, d);
            }


            if (selectionState() != SelectionNone && endPos)
            {

                s->printSelection(p, style, tx, ty, startPos, endPos);

                int diff;
                if(si < (int)m_lines.count()-1)
                    diff = m_lines[si+1]->m_text - s->m_text;
                else
                    diff = s->len;
                endPos -= diff;
                startPos -= diff;
            }
        } while (++si < (int)m_lines.count() && m_lines[si]->checkVerticalPoint(y, ty, h));

        if (hasKeyboardFocus!=DOM::ActivationOff)
        {
            si = firstSi;
            bool clip = p->hasClipping();
            p->setClipping(false);
            p->setRasterOp(Qt::XorROP);
            if (hasKeyboardFocus==DOM::ActivationPassive)
                p->setPen(Qt::white);
            else
                p->setPen(Qt::blue);

            do {
                TextSlave* s = m_lines[si];

                if(s->checkVerticalPoint(y, ty, h))
                    s->printActivation(p, tx, ty);

                int diff;
                if(si < (int) m_lines.count()-1)
                    diff = m_lines[si+1]->m_text - s->m_text;
                else
                    diff = s->len;

            } while (++si < (int)m_lines.count() && m_lines[si]->checkVerticalPoint(y, ty, h));
            p->setRasterOp(Qt::CopyROP);
            p->setClipping(clip);
        }
    }
}

void RenderText::print( QPainter *p, int x, int y, int w, int h,
                      int tx, int ty)
{
    if ( !m_visible )
        return;

    printObject(p, x, y, w, h, tx, ty);
}

void RenderText::calcMinMaxWidth()
{
    //kdDebug( 6040 ) << "Text::calcMinMaxWidth(): known=" << minMaxKnown() << endl;
    if(minMaxKnown()) return;

    // ### calc Min and Max width...
    m_minWidth = 0;
    m_maxWidth = 0;

    int currMinWidth = 0;
    int currMaxWidth = 0;

    int len = str->l;
    for(int i = 0; i < len; i++)
    {
        int wordlen = 0;
        char c;
        do {
            wordlen++;
        } while( i+wordlen < len && !(isBreakable( str->s, i+wordlen, str->l )) ); // && c != '-'
        if(i+wordlen < len) wordlen--;
        if (wordlen)
        {
            int w = fm->width(QConstString(str->s+i, wordlen).string());
            currMinWidth += w;
            currMaxWidth += w;
        }
        if(i+wordlen < len)
        {
            if ( c == '\n' )
            {
                assert(c == '\n');
                if(currMinWidth > m_minWidth) m_minWidth = currMinWidth;
                currMinWidth = 0;
                if(currMaxWidth > m_maxWidth) m_maxWidth = currMaxWidth;
                currMaxWidth = 0;
            }
            else
            {
                if(currMinWidth > m_minWidth) m_minWidth = currMinWidth;
                currMinWidth = 0;
                currMaxWidth += fm->width( *(str->s+i+wordlen) );
            }
            /* else if( c == '-')
            {
                currMinWidth += minus_width;
                if(currMinWidth > m_minWidth) m_minWidth = currMinWidth;
                currMinWidth = 0;
                currMaxWidth += minus_width;
            }*/
        }
        i += wordlen;
    }
    if(currMinWidth > m_minWidth) m_minWidth = currMinWidth;
    currMinWidth = 0;
    if(currMaxWidth > m_maxWidth) m_maxWidth = currMaxWidth;
    currMaxWidth = 0;
    setMinMaxKnown();
}

int RenderText::xPos() const
{
    if (m_lines.count())
        return m_lines[0]->m_x;
    else
        return 0;
}

int RenderText::yPos() const
{
    if (m_lines.count())
        return m_lines[0]->m_y;
    else
        return 0;
}

const QFont &RenderText::font()
{
    return parent()->style()->font();
}

void RenderText::setText(DOMStringImpl *text)
{
    if(str) str->deref();
    str = text;
    if(str) str->ref();

    setLayouted(false);
    if (containingBlock()!=this)
    {
        containingBlock()->setLayouted(false);
        containingBlock()->layout();
    }
#ifdef DEBUG_LAYOUT
    QConstString cstr(str->s, str->l);
    kdDebug( 6040 ) << "RenderText::setText '" << (const char *)cstr.string().utf8() << "'" << endl;
#endif
}

int RenderText::height() const
{
    return m_contentHeight
        + m_style->borderTopWidth() + m_style->borderBottomWidth();
   // ### padding is relative to the _width_ of the containing block
    //+ m_style->paddingTop() + m_style->paddingBottom()
 }

int RenderText::bidiHeight() const
{
    return m_contentHeight;
}

short RenderText::baselineOffset() const
{
    return (m_contentHeight - fm->height())/2 + fm->ascent();
}

short RenderText::verticalPositionHint() const
{
    return (m_contentHeight - fm->height())/2 + fm->ascent();
}

void RenderText::position(int x, int y, int from, int len, int width, bool reverse, bool firstLine)
{
    // ### should not be needed!!!
    if(len == 0) return;

    QChar *ch;
    bool deleteChar = false;
    if ( reverse && !m_style->visuallyOrdered() ) {
        deleteChar = true;
        // reverse String
        QString aStr = QConstString(str->s+from, len).string();
        //kdDebug( 6040 ) << "reversing '" << (const char *)aStr.utf8() << "' len=" << aStr.length() << " oldlen=" << len << endl;
        len = aStr.length();
        ch = QT_ALLOC_QCHAR_VEC(len);
        int half =  len/2;
        const QChar *s = aStr.unicode();
        for(int i = 0; i <= half; i++)
        {
            ch[len-1-i] = s[i];
            ch[i] = s[len-1-i];
            if(ch[i].mirrored() && !m_style->visuallyOrdered())
                ch[i] = ch[i].mirroredChar();
            if(ch[len-1-i].mirrored() && !m_style->visuallyOrdered() && i != len-1-i)
                ch[len-1-i] = ch[len-1-i].mirroredChar();
        }
    }
    else
        ch = str->s+from;

    // ### margins and RTL
    if(from == 0 && m_parent->isInline() && m_parent->firstChild()==this)
    {
        x += paddingLeft() + borderLeft() + marginLeft();
        width -= marginLeft();
    }

    if(from + len == int(str->l) && m_parent->isInline() && m_parent->lastChild()==this)
        width -= marginRight();

#ifdef DEBUG_LAYOUT
    QConstString cstr(ch, len);
    kdDebug( 6040 ) << "setting slave text to '" << (const char *)cstr.string().utf8() << "' len=" << len << " width=" << width << " at (" << x << "/" << y << ")" << " height=" << bidiHeight() << " fontHeight=" << fm->height() << " ascent =" << fm->ascent() << endl;
#endif

    TextSlave *s = new TextSlave(x, y, ch, len,
                                 bidiHeight(), baselineOffset(), width, deleteChar, firstLine);

    if(m_lines.count() == m_lines.size())
        m_lines.resize(m_lines.size()*2+1);

    m_lines.insert(m_lines.count(), s);
}

unsigned int RenderText::width( int from, int len) const
{
    if(!str->s || from > str->l ) return 0;

    if ( from + len > str->l ) len = str->l - from;

    int w;
    if( len == 1)
        w = fm->width( *(str->s+from) );
    else
        w = fm->width(QConstString(str->s+from, len).string());

    // ### add margins and support for RTL

    if(m_parent->isInline())
    {
        if(from == 0 && m_parent->firstChild() == static_cast<const RenderObject*>(this))
            w += borderLeft() + paddingLeft() + marginLeft();
        if(from + len == int(str->l) &&
           m_parent->lastChild() == static_cast<const RenderObject*>(this))
            w += borderRight() + paddingRight() +marginRight();
    }

    //kdDebug( 6040 ) << "RenderText::width(" << from << ", " << len << ") = " << w << endl;
    return w;
}

short RenderText::width() const
{
    int w = 0;
    // as TextSlaves are supposed to be sorted from up to bottom, left to right..
    if(m_lines.count())
        w = m_lines[m_lines.count()-1]->m_x + m_lines[m_lines.count()-1]->m_width;

    if(m_parent->isInline())
    {
        if(m_parent->firstChild() == static_cast<const RenderObject*>(this))
            w += borderLeft() + paddingLeft();
        if(m_parent->lastChild() == static_cast<const RenderObject*>(this))
            w += borderRight() + paddingRight();
    }

    return w;
}

void RenderText::repaint()
{
    RenderObject *cb = containingBlock();
    if(cb != this)
        cb->repaint();
}

bool RenderText::isFixedWidthFont() const
{
    return QFontInfo(m_style->font()).fixedPitch();
}

#undef BIDI_DEBUG
