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

#define QT_ALLOC_QCHAR_VEC( N ) (QChar*) new char[ sizeof(QChar)*( N ) ]
#define QT_DELETE_QCHAR_VEC( P ) delete[] ((char*)( P ))

using namespace khtml;
using namespace DOM;

TextSlave::~TextSlave()
{
    if(m_reversed)
        QT_DELETE_QCHAR_VEC(m_text);
}


void TextSlave::print( QPainter *p, int _tx, int _ty)
{
    if (!m_text || m_len <= 0)
        return;

    QConstString s(m_text, m_len);
    //kdDebug( 6040 ) << "textSlave::printing(" << s.string() << ") at(" << x+_tx << "/" << y+_ty << ")" << endl;
    p->drawText(m_x + _tx, m_y + _ty + m_baseline, s.string());
}

void TextSlave::printSelection(QPainter *p, RenderStyle* style, int tx, int ty, int startPos, int endPos)
{
    if(startPos > m_len) return;
    if(startPos < 0) startPos = 0;

    int _len = m_len;
    int _width = m_width;
    if(endPos > 0 && endPos < m_len) {
        _len = endPos;
    }
    _len -= startPos;

    //kdDebug(6040) << "TextSlave::printSelection startPos (relative)=" << startPos << " len (of selection)=" << _len << "  (m_len=" << m_len << ")" << endl;
    QConstString s(m_text+startPos , _len);

    if (_len != m_len)
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

// Return -2 = before, -1 = after (offset set to max), 0 = inside the text, at @p offset
int TextSlave::checkSelectionPoint(int _x, int _y, int _tx, int _ty, QFontMetrics * fm, int & offset)
{
    kdDebug(6040) << "TextSlave::checkSelectionPoint " << this << " _x=" << _x << " _y=" << _y
                  << " _tx+m_x=" << _tx+m_x << " _ty+m_y=" << _ty+m_y << endl;
    offset = 0;

    if ( _y < _ty + m_y )
        return -2; // above -> before

    if ( _y > _ty + m_y + m_height || _x > _tx + m_x + m_width )
    {
        // below or on the right -> after
        // Set the offset to the max
        offset = m_len;
        return -1;
    }

    // The Y matches, check if we're on the left
    if ( _x < _tx + m_x )
        return -2; // on the left (and not below) -> before

    if ( m_reversed )
        return -2; // Abort if RTL (TODO)

    int delta = _x - (_tx + m_x);
    kdDebug(6040) << "TextSlave::checkSelectionPoint delta=" << delta << endl;
    int pos = 0;
    while(pos < m_len)
    {
        // ### this will produce wrong results for RTL text!!!
        int w = fm->width(*(m_text+pos));
        int w2 = w/2;
        w -= w2;
        delta -= w2;
        if(delta <= 0) break;
        pos++;
        delta -= w;
    }
    //kdDebug( 6040 ) << " Text  --> inside at position " << pos << endl;
    offset = pos;
    return 0;
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
int TextSlaveArray::findFirstMatching(Item d) const
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
    /* if ( !found )
	return -1; */
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
    setRenderText();   // our object inherits from RenderText
    setInline(true);   // our object is Inline

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

void RenderText::setStyle(RenderStyle *_style)
{
    RenderObject::setStyle(_style);
    delete fm;
    fm = new QFontMetrics(style()->font());
    m_contentHeight = style()->lineHeight().width(fm->height());
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

TextSlave * RenderText::findTextSlave( int offset, int &pos )
{
    // The text slaves point to parts of the rendertext's str string
    // (they don't include '\n')
    // Find the text slave that includes the character at @p offset
    // and return pos, which is the position of the char in the slave.

    if ( m_lines.isEmpty() )
        return 0L;

    TextSlave* s = m_lines[0];
    uint si = 0;
    int off = s->m_len;
    while(offset > off && si < m_lines.count())
    {
        s = m_lines[++si];
        if ( s->m_reversed )
            return 0L; // Abort if RTL (TODO)
        // ### only for visuallyOrdered !
        off = s->m_text - str->s + s->m_len;
    }
    // we are now in the correct text slave
    pos = (offset > off ? s->m_len : s->m_len - (off - offset) );
    return s;
}

bool RenderText::checkPoint(int _x, int _y, int _tx, int _ty, int &offset)
{
    // #### TODO get rid of offset, we won't need it anymore now that
    // selection is handled separately.

    TextSlave *s = m_lines.count() ? m_lines[0] : 0;
    int si = 0;
    while(s)
    {
        if( s->checkPoint(_x, _y, _tx, _ty) )
        {
// Disabled, we don't need this anymore, offset isn't used (we have checkSelectionPoint instead)
#if 0
            // now we need to get the exact position
            int delta = _x - _tx - s->m_x;
            int pos = 0;
            while(pos < s->m_len)
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
            if ( s->m_reversed )
                return false; // Abort if RTL (TODO)
            // ### only for visuallyOrdered !
            offset = s->m_text - str->s + pos;
            //kdDebug( 6040 ) << " Text  --> inside at position " << offset << endl;
#endif
            return true;
        }
        // ### this might be wrong, if combining chars are used ( eg arabic )
        s = si < (int)m_lines.count()-1 ? m_lines[++si] : 0;
    }
    return false;
}

int RenderText::checkSelectionPoint(int _x, int _y, int _tx, int _ty, int &offset)
{
    for(unsigned int si = 0; si < m_lines.count(); si++)
    {
        TextSlave* s = m_lines[si];
        if ( s->m_reversed )
            return -2; // abort if RTL (TODO)
        int result = s->checkSelectionPoint(_x, _y, _tx, _ty, fm, offset);
        kdDebug(6040) << "RenderText::checkSelectionPoint line " << si << " result=" << result << endl;
        if ( result == 0 ) // x,y is inside the textslave
        {
            // ### only for visuallyOrdered !
            offset += s->m_text - str->s; // add the offset from the previous lines
            kdDebug(6040) << "RenderText::checkSelectionPoint inside -> " << offset << endl;
            return 0;
        } else if ( result == -2 )
        {
            // x,y is before the textslave -> stop here
            if ( si > 0 )
            {
                // ### only for visuallyOrdered !
                offset = s->m_text - str->s - 1;
                kdDebug(6040) << "RenderText::checkSelectionPoint before -> " << offset << endl;
                return 0;
            } else
            {
                offset = 0;
                kdDebug(6040) << "RenderText::checkSelectionPoint before us -> returning -2" << endl;
                return -2;
            }
        }
    }

    // set offset to max
    offset = str->l;
    return -1; // after
}

void RenderText::cursorPos(int offset, int &_x, int &_y, int &height)
{
  if (!m_lines.count()) {
    _x = _y = height = -1;
    return;
  }

  int pos;
  TextSlave * s = findTextSlave( offset, pos );
  _y = s->m_y;
  height = s->m_height;

  QString tekst(s->m_text, s->m_len);
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
    if(parent()) {
        parent()->absolutePosition(xPos, yPos, false);
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
    if (!parent())
    {
       x = -1;
       y = -1;
       return;
    }
    parent()->absolutePosition( x, y, false );

    //if( chr > (int) str->l )
    //chr = str->l;

    int pos;
    TextSlave * s = findTextSlave( chr, pos );

    if ( s )
    {
        // s is the line containing the character
        x += s->m_x; // this is the x of the beginning of the line, but it's good enough for now
        y += s->m_y;
    }
}

void RenderText::printObject( QPainter *p, int /*x*/, int y, int /*w*/, int h,
                      int tx, int ty)
{
    RenderStyle* pseudoStyle = style()->getPseudoStyle(RenderStyle::FIRST_LINE);
    int d = style()->textDecoration();
    TextSlave f(0, y-ty);
    int si = m_lines.findFirstMatching(&f);
    // something matching found, find the first one to print
    if(si >= 0)
    {
        // Move up until out of area to be printed
        while(si > 0 && m_lines[si-1]->checkVerticalPoint(y, ty, h))
            si--;

        //kdDebug(6040) << endl;
        //kdDebug(6040) << this << " RenderText::printObject y=" << y << " ty=" << ty << " h=" << h << " first line is " << si << endl;
        int firstSi = si;

        // Now calculate startPos and endPos, for printing selection.
        // We print selection while endPos > 0
        int endPos, startPos;
        if (selectionState() != SelectionNone)
        {
            if (selectionState() == SelectionInside)
            {
                //kdDebug(6040) << this << " SelectionInside -> 0 to end" << endl;
                startPos = 0;
                endPos = str->l;
            }
            else
            {
                selectionStartEnd(startPos, endPos);
                if(selectionState() == SelectionStart)
                    endPos = str->l;
                else if(selectionState() == SelectionEnd)
                    startPos = 0;
            }
            //kdDebug(6040) << this << " Selection from " << startPos << " to " << endPos << endl;

            // Eat the lines we don't print (startPos and endPos are from line 0!)
            // Note that we do this even if si==0, because we may have \n as the first char,
            // and this takes care of it too (David)
            if ( m_lines[si]->m_reversed )
                endPos = -1; // No selection if RTL (TODO)
            else
            {
                // ## Only valid for visuallyOrdered
                int len = m_lines[si]->m_text - str->s;
                //kdDebug(6040) << this << " RenderText::printObject adjustement si=" << si << " len=" << len << endl;
                startPos -= len;
                endPos -= len;
            }
        }


        // run until we find one that is outside the range, then we
        // know we can stop
        do {
            TextSlave* s = m_lines[si];
            RenderStyle* _style = pseudoStyle && s->m_firstLine ? pseudoStyle : style();

            if(_style->font() != p->font())
                p->setFont(_style->font());

            if(_style->color() != p->pen().color())
                p->setPen(_style->color());

            if((hasSpecialObjects()  &&
                (parent()->isInline() || pseudoStyle)) &&
               (!pseudoStyle || s->m_firstLine))
                s->printBoxDecorations(p, _style, this, tx, ty, si == 0, si == (int)m_lines.count());

            s->print(p, tx, ty);

            if(d != TDNONE && hasKeyboardFocus == DOM::ActivationOff)
            {
                p->setPen(_style->textDecorationColor());
                s->printDecoration(p, tx, ty, d);
            }


            if (selectionState() != SelectionNone && endPos > 0)
            {
                //kdDebug(6040) << this << " printSelection with startPos=" << startPos << " endPos=" << endPos << endl;
                s->printSelection(p, _style, tx, ty, startPos, endPos);

                int diff;
                if(si < (int)m_lines.count()-1)
                    // ### only for visuallyOrdered ! (we disabled endPos for RTL, so we can't go here in RTL mode)
                    diff = m_lines[si+1]->m_text - s->m_text;
                else
                    diff = s->m_len;
                //kdDebug(6040) << this << " RenderText::printSelection eating the line si=" << si << " length=" << diff << endl;
                endPos -= diff;
                startPos -= diff;
                //kdDebug(6040) << this << " startPos now " << startPos << ", endPos now " << endPos << endl;
            }
        } while (++si < (int)m_lines.count() && m_lines[si]->checkVerticalPoint(y, ty, h));

        if (hasKeyboardFocus!=DOM::ActivationOff)
        {
            si = firstSi;
            bool clip = p->hasClipping();
            p->setClipping(false);
            p->setRasterOp(Qt::CopyROP);
	    p->setBrush(Qt::NoBrush);
            if (hasKeyboardFocus==DOM::ActivationPassive)
                p->setPen(Qt::green);
            else
                p->setPen(Qt::blue);

            do {
                TextSlave* s = m_lines[si];

                if(s->checkVerticalPoint(y, ty, h))
                    s->printActivation(p, tx, ty);

                int diff;
                if(si < (int) m_lines.count()-1 && !m_lines[si+1]->m_reversed) // ### no RTL
                    diff = m_lines[si+1]->m_text - s->m_text;
                else
                    diff = s->m_len;

            } while (++si < (int)m_lines.count() && m_lines[si]->checkVerticalPoint(y, ty, h));
        }
    }
}

void RenderText::print( QPainter *p, int x, int y, int w, int h,
                      int tx, int ty)
{
    if ( !isVisible() )
        return;

    int s = m_lines.count() - 1;
    if ( s < 0 ) return;
    if ( ty + m_lines[0]->m_y > y + h ) return;
    if ( ty + m_lines[s]->m_y + m_lines[s]->m_height < y ) return;

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
        do {
            wordlen++;
        } while( i+wordlen < len && !(isBreakable( str->s, i+wordlen, str->l )) );
        if (wordlen)
        {
            int w = fm->width(QConstString(str->s+i, wordlen).string());
            currMinWidth += w;
            currMaxWidth += w;
        }
        if(i+wordlen < len)
        {
            if ( (*(str->s+i+wordlen)).latin1() == '\n' )
            {
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
    return m_contentHeight * m_lines.count()
        + style()->borderTopWidth() + style()->borderBottomWidth();
    // ### padding is relative to the _width_ of the containing block
    //+ style()->paddingTop() + style()->paddingBottom()
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
    reverse = reverse && !style()->visuallyOrdered();
    if ( reverse ) {
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
            if(ch[i].mirrored() && !style()->visuallyOrdered())
                ch[i] = ch[i].mirroredChar();
            if(ch[len-1-i].mirrored() && !style()->visuallyOrdered() && i != len-1-i)
                ch[len-1-i] = ch[len-1-i].mirroredChar();
        }
    }
    else
        ch = str->s+from;

    // ### margins and RTL
    if(from == 0 && parent()->isInline() && parent()->firstChild()==this)
    {
        x += paddingLeft() + borderLeft() + marginLeft();
        width -= marginLeft();
    }

    if(from + len == int(str->l) && parent()->isInline() && parent()->lastChild()==this)
        width -= marginRight();

#ifdef DEBUG_LAYOUT
    QConstString cstr(ch, len);
    kdDebug( 6040 ) << "setting slave text to '" << (const char *)cstr.string().utf8() << "' len=" << len << " width=" << width << " at (" << x << "/" << y << ")" << " height=" << bidiHeight() << " fontHeight=" << fm->height() << " ascent =" << fm->ascent() << endl;
#endif

    TextSlave *s = new TextSlave(x, y, ch, len,
                                 bidiHeight(), baselineOffset(),
                                 width, reverse, firstLine);

    if(m_lines.count() == m_lines.size())
        m_lines.resize(m_lines.size()*2+1);

    m_lines.insert(m_lines.count(), s);
}

unsigned int RenderText::width(unsigned int from, unsigned int len, bool firstLine) const
{
    if(!str->s || from > str->l ) return 0;

    if ( from + len > str->l ) len = str->l - from;

    QFontMetrics metrics = *fm;
    RenderStyle *pseudoStyle;
    if ( firstLine && (pseudoStyle = style()->getPseudoStyle(RenderStyle::FIRST_LINE) ) )
	metrics = QFontMetrics ( pseudoStyle->font() );

    int w;
    if( len == 1)
        w = metrics.width( *(str->s+from) );
    else
        w = metrics.width(QConstString(str->s+from, len).string());

    // ### add margins and support for RTL

    if(parent()->isInline())
    {
        if(from == 0 && parent()->firstChild() == static_cast<const RenderObject*>(this))
            w += borderLeft() + paddingLeft() + marginLeft();
        if(from + len == str->l &&
           parent()->lastChild() == static_cast<const RenderObject*>(this))
            w += borderRight() + paddingRight() +marginRight();
    }

    //kdDebug( 6040 ) << "RenderText::width(" << from << ", " << len << ") = " << w << endl;
    return w;
}

short RenderText::width() const
{
    int w;
    int minx = 100000000;
    int maxx = 0;
    // slooow
    for(unsigned int si = 0; si < m_lines.count(); si++) {
        TextSlave* s = m_lines[si];
        if(s->m_x < minx)
            minx = s->m_x;
        if(s->m_x + s->m_width > maxx)
            maxx = s->m_x + s->m_width;
    }

    w = QMAX(0, maxx-minx);

    if(parent()->isInline())
    {
        if(parent()->firstChild() == static_cast<const RenderObject*>(this))
            w += borderLeft() + paddingLeft();
        if(parent()->lastChild() == static_cast<const RenderObject*>(this))
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
    return QFontInfo(style()->font()).fixedPitch();
}

#undef BIDI_DEBUG
