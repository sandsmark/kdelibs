/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *               1999 Waldo Bastian (bastian@kde.org)
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

//#define CSS_DEBUG

#include "css_stylesheetimpl.h"

#include "css_stylesheet.h"
#include "css_rule.h"
#include "css_ruleimpl.h"
#include "css_valueimpl.h"
#include "csshelper.h"

#include "dom_string.h"
#include "xml/dom_nodeimpl.h"
#include "html/html_documentimpl.h"
#include "dom_exception.h"
using namespace DOM;

#include <kdebug.h>

#include "htmlhashes.h"
#include "misc/helper.h"

#include "cssproperties.h"
#include "cssvalues.h"


//
// The following file defines the function
//     const struct props *findProp(const char *word, int len)
//
// with 'props->id' a CSS property in the range from CSS_PROP_MIN to
// (and including) CSS_PROP_TOTAL-1
#include "cssproperties.c"
#include "cssvalues.c"

int DOM::getPropertyID(const char *tagStr, int len)
{
    const struct props *propsPtr = findProp(tagStr, len);
    if (!propsPtr)
        return 0;

    return propsPtr->id;
}

// ------------------------------------------------------------------------------------------------------

bool StyleBaseImpl::deleteMe()
{
    if(!m_parent && _ref <= 0) return true;
    return false;
}

void StyleBaseImpl::setParent(StyleBaseImpl *parent)
{
    m_parent = parent;
}

void StyleBaseImpl::checkLoaded()
{
    if(m_parent) m_parent->checkLoaded();
}

DOMString StyleBaseImpl::baseUrl()
{
    // try to find the style sheet. If found look for it's url.
    // If it has none, look for the parentsheet, or the parentNode and
    // try to find out about their url
    StyleBaseImpl *b = this;
    while(b && !b->isStyleSheet())
        b = b->parent();

    if(!b) return 0;

    StyleSheetImpl *sheet = static_cast<StyleSheetImpl *>(b);
    if(!sheet->href().isNull())
        return sheet->href();

    // find parent
    if(sheet->parent()) return sheet->parent()->baseUrl();

    if(!sheet->ownerNode()) return 0;

    DocumentImpl *doc = static_cast<DocumentImpl*>(sheet->ownerNode()->nodeType() == Node::DOCUMENT_NODE ? sheet->ownerNode() : sheet->ownerNode()->ownerDocument());

    if(!doc->isHTMLDocument()) return 0;

    HTMLDocumentImpl *htmldoc = static_cast<HTMLDocumentImpl *>(doc);
    return htmldoc->baseURL();
}

/*
 * parsing functions for stylesheets
 */

bool isspace(const QChar &c)
{
     return (c == ' ' || c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == QChar(0xa0));
}

const QChar *
StyleBaseImpl::parseSpace(const QChar *curP, const QChar *endP)
{
  bool sc = false;     // possible start comment?
  bool ec = false;     // possible end comment?
  bool ic = false;     // in comment?

  while (curP < endP)
  {
      if (ic)
      {
          if (ec && (*curP == '/'))
              ic = false;
          else if (*curP == '*')
              ec = true;
          else
              ec = false;
      }
      else if (sc && (*curP == '*'))
      {
          ic = true;
      }
      else if (*curP == '/')
      {
          sc = true;
      }
      else if (!isspace(*curP))
      {
          return(curP);
      }
      else
      {
          sc = false;
      }
      curP++;
  }

  return(0);
}

/*
 * ParseToChar
 *
 * Search for an expected character.  Deals with escaped characters,
 * quoted strings, and pairs of braces/parens/brackets.
 */
const QChar *
StyleBaseImpl::parseToChar(const QChar *curP, const QChar *endP, QChar c, bool chkws, bool endAtBlock)
{
    //kdDebug( 6080 ) << "parsetochar: \"" << QString(curP, endP-curP) << "\" searching " << c << " ws=" << chkws << endl;

    bool sq = false; /* in single quote? */
    bool dq = false; /* in double quote? */
    bool esc = false; /* escape mode? */

    while (curP < endP)
    {
        if (esc)
            esc = false;
        else if (*curP == '\\')
            esc = true;
        else if (!sq && (*curP == '"'))
            dq = !dq;
        else if (!dq && (*curP == '\''))
            sq = !sq;
        else if (!sq && !dq && *curP == c)
            return(curP);
        else if (!sq && !dq && chkws && isspace(*curP))
            return(curP);
        else if(!sq && !dq ) {
            if (*curP == '{') {
                if(endAtBlock)
                    return curP;
                curP = parseToChar(curP + 1, endP, '}', false);
                if (!curP)
                    return(0);
            } else if (*curP == '(') {
                curP = parseToChar(curP + 1, endP, ')', false);
                if (!curP)
                    return(0);
            } else if (*curP == '[') {
                curP = parseToChar(curP + 1, endP, ']', false);
                if (!curP)
                    return(0);
            }
        }
        curP++;
    }

    return(0);
}

CSSRuleImpl *
StyleBaseImpl::parseAtRule(const QChar *&curP, const QChar *endP)
{
    curP++;
    const QChar *startP = curP;
    while( *curP != ' ' && *curP != '{' && *curP != '\'')
        curP++;

    QString rule(startP, curP-startP);
    rule = rule.lower();

    //kdDebug( 6080 ) << "rule = '" << rule << "'" << endl;

    if(rule == "import")
    {
        // ### load stylesheet and pass it over
        curP = parseSpace(curP, endP);
        if(!curP) return 0;
        startP = curP++;
        curP = parseToChar(startP, endP, ';', true);
        if(!curP) return 0;
        DOMString url = khtml::parseURL(DOMString(startP, curP - startP));
        startP = curP;
        if(*curP != ';')
            curP = parseToChar(startP, endP, ';', false, true);
        if(!curP) return 0;
        QString media(startP, curP - startP);
        // ### check if at the beginning of the stylesheet (no style rule
        //     before the import rule)
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "at rule: url = " << url.string()
                        << " media = " << media << endl;
#endif
        // ignore block following @import rule
        if( *curP == '{' ) {
            curP++;
            curP = parseToChar(curP, endP, '}', false);
            if(curP)
                curP++;
        }
        // ### only media="", "screen and "all" are imported for the moment...
        if( !media.isEmpty() && !(media.contains("all") || media.contains("screen")) )
            return 0;
        if(!this->isCSSStyleSheet()) return 0;
        return new CSSImportRuleImpl(this, url, 0);
    }
    else if(rule == "charset")
    {
        // ### invoke decoder
        startP = curP++;
        curP = parseToChar(startP, endP, ';', false);
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "charset = " << QString(startP, curP - startP) << endl;
#endif
    }
    else if(rule == "font-face")
    {
        startP = curP++;
        curP = parseToChar(startP, endP, '}', false);
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "font rule = " << QString(startP, curP - startP) << endl;
#endif
    }
    else if(rule == "media")
    {
        startP = curP++;
        curP = parseToChar(startP, endP, '}', false);
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "media rule = " << QString(startP, curP - startP) << endl;
#endif
    }
    else if(rule == "page")
    {
        startP = curP++;
        curP = parseToChar(startP, endP, '}', false);
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "page rule = " << QString(startP, curP - startP) << endl;
#endif
    }


    return 0;
}

CSSSelector *
StyleBaseImpl::parseSelector2(const QChar *curP, const QChar *endP)
{
    CSSSelector *cs = new CSSSelector();
    QString selecString( curP, endP - curP );

#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "selectString = \"" << selecString << "\"" << endl;
#endif

    if (*curP == '#' && (curP < endP && !((*(curP+1)).isDigit())))
    {
        cs->tag = -1;
        cs->attr = ATTR_ID;
        cs->match = CSSSelector::Exact;
        cs->value = DOMString( curP + 1, endP - curP -1 );
    }
    else if (*curP == '.' && (curP < endP && !((*(curP+1)).isDigit())))
    {
        cs->tag = -1;
        cs->attr = ATTR_CLASS;
        cs->match = CSSSelector::List;
        cs->value = DOMString( curP + 1, endP - curP -1 );
    }
    else if (*curP == ':')
    {
        cs->tag = -1;
        cs->value = DOMString( curP, endP - curP );
        cs->match = CSSSelector::Pseudo;
    }
    else
    {
        const QChar *startP = curP;
        QString tag;
        while (curP < endP)
        {
            if (*curP =='#' && (curP < endP && !((*(curP+1)).isDigit())))
            {
                tag = QString( startP, curP-startP );
                cs->attr = ATTR_ID;
                cs->match = CSSSelector::Exact;
                cs->value = DOMString( curP + 1, endP - curP - 1);
                break;
            }
            else if (*curP == '.' && (curP < endP && !((*(curP+1)).isDigit())))
            {
                tag = QString( startP, curP - startP );
                cs->attr = ATTR_CLASS;
                cs->match = CSSSelector::List;
                cs->value = DOMString( curP + 1, endP - curP - 1);
                break;
            }
            else if (*curP == ':')
            {
                // pseudo attributes (:link, :hover, ...)
                tag = QString( startP, curP - startP );
                cs->value = DOMString( curP, endP - curP);
                cs->match = CSSSelector::Pseudo;
                break;
            }
            else if (*curP == '[')
            {
                tag = QString( startP, curP - startP );
                curP++;
                if ( curP >= endP ) {
                    delete cs;
                    return 0;
                }
#ifdef CSS_DEBUG
                kdDebug( 6080 ) << "tag = " << tag << endl;
#endif
                const QChar *equal = parseToChar(curP, endP, '=', false);
                QString attr;
                if(!equal)
                {
                    attr = QString( curP, endP - curP - 1 );
                    attr = attr.stripWhiteSpace();
#ifdef CSS_DEBUG
                    kdDebug( 6080 ) << "attr = '" << attr << "'" << endl;
#endif
                    cs->match = CSSSelector::Set;
                }
                else
                {
                    // check relation: = / ~= / |=
                    if(*(equal-1) == '~')
                    {
                        attr = QString( curP, equal - curP - 1 );
                        cs->match = CSSSelector::List;
                    }
                    else if(*(equal-1) == '|')
                    {
                        attr = QString( curP, equal - curP - 1 );
                        cs->match = CSSSelector::Hyphen;
                    }
                    else
                    {
                        attr = QString(curP, equal - curP );
                        cs->match = CSSSelector::Exact;
                    }
                }
                attr = attr.stripWhiteSpace();
                cs->attr = khtml::getAttrID(attr.ascii(), attr.length());
                if(equal)
                {
                    equal++;
                    while(equal < endP && *equal == ' ')
                        equal++;
                    if(equal >= endP ) {
                        delete cs;
                        return 0;
                    }
                    if(*equal == '\'')
                    {
                        equal++;
                        while(*endP != '\'' && endP > equal)
                            endP--;
                    }
                    else if(*equal == '\"')
                    {
                        equal++;
                        while(*endP != '\"' && endP > equal)
                            endP--;
                    }
                    else
                        endP--;
                    cs->value = DOMString(equal, endP - equal);
                }
                break;
            }
            else
            {
                curP++;
            }
        }
        if (curP == endP)
        {
            tag = QString( startP, curP - startP );
        }
        if(tag == "*")
        {
            //kdDebug( 6080 ) << "found '*' selector" << endl;
            cs->tag = -1;
        }
        else
            cs->tag = khtml::getTagID(tag.lower().ascii(), tag.length());
   }
   if (cs->tag == 0)
   {
       delete cs;
       return(0);
   }
#ifdef CSS_DEBUG
   kdDebug( 6080 ) << "[Selector: tag=" << cs->tag << " Attribute=" << cs->attr << " relation=" << (int)cs->match << " value=" << cs->value.string() << " specificity=" << cs->specificity() << "]" << endl;
#endif
   return(cs);
}

CSSSelector *
StyleBaseImpl::parseSelector1(const QChar *curP, const QChar *endP)
{
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "selector1 is \'" << QString(curP, endP-curP) << "\'" << endl;
#endif

    CSSSelector *selecStack=0;

    curP = parseSpace(curP, endP);
    if (!curP)
        return(0);

    CSSSelector::Relation relation = CSSSelector::Descendant;

    const QChar *startP = curP;
    while (curP <= endP)
    {
        if ((curP == endP) || isspace(*curP) || *curP == '+' || *curP == '>')
        {
            CSSSelector *cs = parseSelector2(startP, curP);
            if (cs)
            {
                cs->tagHistory = selecStack;
                cs->relation = relation;
                selecStack = cs;
            }
            else
            {
                // invalid selector, delete
                delete selecStack;
                return 0;
            }

            curP = parseSpace(curP, endP);
            if (!curP)
                return(selecStack);

            relation = CSSSelector::Descendant;
            if(*curP == '+')
            {
                relation = CSSSelector::Sibling;
                curP++;
                curP = parseSpace(curP, endP);
            }
            else if(*curP == '>')
            {
#ifdef CSS_DEBUG
                kdDebug( 6080 ) << "child selector" << endl;
#endif
                relation = CSSSelector::Child;
                curP++;
                curP = parseSpace(curP, endP);
            }
            if(cs)
                cs->print();
            startP = curP;
        }
        else
        {
            curP++;
        }
    }
    return(selecStack);
}

QList<CSSSelector> *
StyleBaseImpl::parseSelector(const QChar *curP, const QChar *endP)
{
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "selector is \'" << QString(curP, endP-curP) << "\'" << endl;
#endif

    QList<CSSSelector> *slist  = 0;
    const QChar *startP;

    while (curP < endP)
    {
        startP = curP;
        curP = parseToChar(curP, endP, ',', false);
        if (!curP)
            curP = endP;

        CSSSelector *selector = parseSelector1(startP, curP);
        if (selector)
        {
            if (!slist)
            {
                slist = new QList<CSSSelector>;
                slist->setAutoDelete(true);
            }
            slist->append(selector);
        }
        else
        {
#ifdef CSS_DEBUG
            kdDebug( 6080 ) << "invalid selector" << endl;
#endif
            // invalid selector, delete
            delete slist;
            return 0;
        }
        curP++;
    }
    return slist;
}


void StyleBaseImpl::parseProperty(const QChar *curP, const QChar *endP, QList<CSSProperty> *propList)
{
    bool important = false;
    const QChar *colon;
    // Get rid of space in front of the declaration

    curP = parseSpace(curP, endP);
    if (!curP)
        return;

    // Search for the required colon or white space
    colon = parseToChar(curP, endP, ':', true);
    if (!colon)
        return;

    QString propName( curP, colon - curP );
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "Property-name = \"" << propName << "\"" << endl;
#endif

    // May have only reached white space before
    if (*colon != ':')
    {
        // Search for the required colon
        colon = parseToChar(curP, endP, ':', false);
        if (!colon)
            return;
    }
    curP = colon+1;
    // remove space in front of the value
    while(curP < endP && *curP == ' ')
        curP++;
    if ( curP >= endP )
        return;

    // search for !important
    const QChar *exclam = parseToChar(curP, endP, '!', false);
    if(exclam)
    {
        //const QChar *imp = parseSpace(exclam+1, endP);
        QString s(exclam+1, endP - exclam - 1);
        s = s.stripWhiteSpace();
        s.lower();
        if(s != "important")
            return;
        important = true;
        endP = exclam;
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "important property!" << endl;
#endif
    }

    // remove space after the value;
    while (endP > curP)
    {
        if (!isspace(*(endP-1)))
            break;
        endP--;
    }


    QString propVal( curP , endP - curP );
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "Property-value = \"" << propVal.data() << "\"" << endl;
#endif

    const struct props *propPtr = findProp(propName.lower().ascii(), propName.length());
    if (!propPtr)
    {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "Unknown property" << propName << endl;
#endif
         return;
    }

    unsigned int numProps = propList->count();
    if(!parseValue(curP, endP, propPtr->id, important, propList)) {
#ifdef CSS_DEBUG
        kdDebug(6080) << "invalid property, removing added properties from propList" << endl;
#endif
        while(propList->count() > numProps)
            propList->removeLast();
    }
}

QList<CSSProperty> *StyleBaseImpl::parseProperties(const QChar *curP, const QChar *endP)
{
    QList<CSSProperty> *propList=0;
    propList = new QList<CSSProperty>;
    propList->setAutoDelete(true);

    while (curP < endP)
    {
        const QChar *startP = curP;
        curP = parseToChar(curP, endP, ';', false);
        if (!curP)
            curP = endP;

        QString propVal( startP , curP - startP );
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "Property = \"" << propVal.data() << "\"" << endl;
#endif

        parseProperty(startP, curP, propList);
        curP++;
    }
    if(!propList->isEmpty())
        return propList;

#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "empty property list" << endl;
#endif
    delete propList;
    return 0;
}

bool StyleBaseImpl::parseValue(const QChar *curP, const QChar *endP, int propId, bool important,
                                        QList<CSSProperty> *propList)
{
    QString value(curP, endP - curP);
    value = value.lower();
    const char *val = value.ascii();
    //kdDebug() << "parseValue: '" << value << "'" << endl;

    CSSValueImpl *parsedValue = 0;

    if(!strcmp(val, "inherit"))
    {
        // inherited value
        parsedValue = new CSSInheritedValueImpl();
        goto end;
    }

    switch(propId)
    {
    case CSS_PROP_AZIMUTH:
    case CSS_PROP_BACKGROUND_ATTACHMENT:
    case CSS_PROP_BACKGROUND_POSITION:
    case CSS_PROP_BACKGROUND_REPEAT:
    case CSS_PROP_BORDER_TOP_STYLE:
    case CSS_PROP_BORDER_RIGHT_STYLE:
    case CSS_PROP_BORDER_BOTTOM_STYLE:
    case CSS_PROP_BORDER_LEFT_STYLE:
    case CSS_PROP_BORDER_COLLAPSE:
    case CSS_PROP_CAPTION_SIDE:
    case CSS_PROP_CLEAR:
    case CSS_PROP_CLIP:
    case CSS_PROP_CONTENT:
    case CSS_PROP_COUNTER_INCREMENT:
    case CSS_PROP_COUNTER_RESET:
    case CSS_PROP_CUE_AFTER:
    case CSS_PROP_CUE_BEFORE:
    case CSS_PROP_CURSOR:
    case CSS_PROP_DIRECTION:
    case CSS_PROP_DISPLAY:
    case CSS_PROP_ELEVATION:
    case CSS_PROP_EMPTY_CELLS:
    case CSS_PROP_FLOAT:
    case CSS_PROP_FONT_SIZE:
    case CSS_PROP_FONT_SIZE_ADJUST:
    case CSS_PROP_FONT_STRETCH:
    case CSS_PROP_FONT_STYLE:
    case CSS_PROP_FONT_VARIANT:
    case CSS_PROP_FONT_WEIGHT:
    case CSS_PROP_LETTER_SPACING:
    case CSS_PROP_LINE_HEIGHT:
    case CSS_PROP_LIST_STYLE_POSITION:
    case CSS_PROP_LIST_STYLE_TYPE:
    case CSS_PROP_MARGIN_TOP:
    case CSS_PROP_MARGIN_RIGHT:
    case CSS_PROP_MARGIN_BOTTOM:
    case CSS_PROP_MARGIN_LEFT:
    case CSS_PROP_MAX_HEIGHT:
    case CSS_PROP_MAX_WIDTH:
    case CSS_PROP_OUTLINE_STYLE:
    case CSS_PROP_OUTLINE_WIDTH:
    case CSS_PROP_OVERFLOW:
    case CSS_PROP_PAGE:
    case CSS_PROP_PAGE_BREAK_AFTER:
    case CSS_PROP_PAGE_BREAK_BEFORE:
    case CSS_PROP_PAGE_BREAK_INSIDE:
    case CSS_PROP_PITCH:
    case CSS_PROP_PLAY_DURING:
    case CSS_PROP_POSITION:
    case CSS_PROP_QUOTES:
    case CSS_PROP_SIZE:
    case CSS_PROP_SPEAK:
    case CSS_PROP_SPEAK_HEADER:
    case CSS_PROP_SPEAK_NUMERAL:
    case CSS_PROP_SPEAK_PUNCTUATION:
    case CSS_PROP_SPEECH_RATE:
    case CSS_PROP_TABLE_LAYOUT:
    case CSS_PROP_TEXT_ALIGN:
        //case CSS_PROP_TEXT_DECORATION:
    case CSS_PROP_TEXT_TRANSFORM:
    case CSS_PROP_UNICODE_BIDI:
    case CSS_PROP_VERTICAL_ALIGN:
    case CSS_PROP_VISIBILITY:
    case CSS_PROP_VOICE_FAMILY:
    case CSS_PROP_VOLUME:
    case CSS_PROP_WHITE_SPACE:
    case CSS_PROP_WORD_SPACING:
    case CSS_PROP_Z_INDEX:
    case CSS_PROP_WIDTH:
    case CSS_PROP_LEFT:
    case CSS_PROP_RIGHT:
    case CSS_PROP_TOP:
    case CSS_PROP_BOTTOM:
    {
        //kdDebug( 6080 ) << "parseValue: value = " << val << endl;
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval)
        {
            //kdDebug( 6080 ) << "got value " << cssval->id << endl;
            parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            goto end;
            // ### FIXME: should check if the identifier makes sense with the property
        }
    }
    default:
        break;
    }

    // we don't have an identifier.

    switch(propId)
    {
// ident only properties
    case CSS_PROP_BACKGROUND_ATTACHMENT:
    case CSS_PROP_BACKGROUND_REPEAT:
    case CSS_PROP_BORDER_COLLAPSE:
    case CSS_PROP_BORDER_TOP_STYLE:
    case CSS_PROP_BORDER_RIGHT_STYLE:
    case CSS_PROP_BORDER_BOTTOM_STYLE:
    case CSS_PROP_BORDER_LEFT_STYLE:
    case CSS_PROP_CAPTION_SIDE:
    case CSS_PROP_CLEAR:
    case CSS_PROP_DIRECTION:
    case CSS_PROP_DISPLAY:
    case CSS_PROP_EMPTY_CELLS:
    case CSS_PROP_FLOAT:
    case CSS_PROP_FONT_STRETCH:
    case CSS_PROP_FONT_STYLE:
    case CSS_PROP_FONT_VARIANT:
    case CSS_PROP_FONT_WEIGHT:
    case CSS_PROP_LIST_STYLE_POSITION:
    case CSS_PROP_LIST_STYLE_TYPE:
    case CSS_PROP_OUTLINE_STYLE:
    case CSS_PROP_OVERFLOW:
    case CSS_PROP_PAGE:
    case CSS_PROP_PAGE_BREAK_AFTER:
    case CSS_PROP_PAGE_BREAK_BEFORE:
    case CSS_PROP_PAGE_BREAK_INSIDE:
    case CSS_PROP_PAUSE_AFTER:
    case CSS_PROP_PAUSE_BEFORE:
    case CSS_PROP_POSITION:
    case CSS_PROP_SPEAK:
    case CSS_PROP_SPEAK_HEADER:
    case CSS_PROP_SPEAK_NUMERAL:
    case CSS_PROP_SPEAK_PUNCTUATION:
    case CSS_PROP_TABLE_LAYOUT:
    case CSS_PROP_TEXT_TRANSFORM:
    case CSS_PROP_UNICODE_BIDI:
    case CSS_PROP_VISIBILITY:
    case CSS_PROP_WHITE_SPACE:
        break;

// special properties (css_extensions)
    case CSS_PROP_AZIMUTH:
        // CSS2Azimuth
    case CSS_PROP_BACKGROUND_POSITION:
        // CSS2BackgroundPosition
    case CSS_PROP_CURSOR:
        // CSS2Cursor
        // ### should also support URI, but let's ignore that for now.
        break;
    case CSS_PROP_PLAY_DURING:
        // CSS2PlayDuring
    case CSS_PROP_TEXT_SHADOW:
        // list of CSS2TextShadow
        break;
    case CSS_PROP_BORDER_SPACING: // should be able to have two values
        parsedValue = parseUnit(curP, endP, LENGTH);
        break;


// colors || inherit
    case CSS_PROP_BACKGROUND_COLOR:
    case CSS_PROP_BORDER_TOP_COLOR:
    case CSS_PROP_BORDER_RIGHT_COLOR:
    case CSS_PROP_BORDER_BOTTOM_COLOR:
    case CSS_PROP_BORDER_LEFT_COLOR:
    case CSS_PROP_COLOR:
    case CSS_PROP_TEXT_DECORATION_COLOR:
    case CSS_PROP_OUTLINE_COLOR:
    {
        QString value(curP, endP - curP);
        value = value.stripWhiteSpace();
        //kdDebug(6080) << "parsing color " << value << endl;
        QColor c;
        khtml::setNamedColor(c, value);
        if(!c.isValid() && (value != "transparent" ) && !value.isEmpty() ) return false;
        //kdDebug( 6080 ) << "color is: " << c.red() << ", " << c.green() << ", " << c.blue() << endl;
        parsedValue = new CSSPrimitiveValueImpl(c);
        break;
    }

// uri || inherit
    case CSS_PROP_BACKGROUND_IMAGE:
    case CSS_PROP_LIST_STYLE_IMAGE:
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval && cssval->id == CSS_VAL_NONE)
        {
            parsedValue = new CSSImageValueImpl();
#ifdef CSS_DEBUG
            kdDebug( 6080 ) << "empty image " << static_cast<CSSImageValueImpl *>(parsedValue)->image() << endl;
#endif
            break;
        }
        else
        {
            DOMString value(curP, endP - curP);
            value = khtml::parseURL(value);
#ifdef CSS_DEBUG
            kdDebug( 6080 ) << "image, url=" << value.string() << " base=" << baseUrl().string() << endl;
#endif
            parsedValue = new CSSImageValueImpl(value, baseUrl(), this);
            break;
        }
    }
    case CSS_PROP_CUE_AFTER:
    case CSS_PROP_CUE_BEFORE:
    {
        DOMString value(curP, endP - curP);
        value = khtml::parseURL(value);
        parsedValue = new CSSPrimitiveValueImpl(value, CSSPrimitiveValue::CSS_URI);
        break;
    }

// length
    case CSS_PROP_BORDER_TOP_WIDTH:
    case CSS_PROP_BORDER_RIGHT_WIDTH:
    case CSS_PROP_BORDER_BOTTOM_WIDTH:
    case CSS_PROP_BORDER_LEFT_WIDTH:
    case CSS_PROP_MARKER_OFFSET:
    case CSS_PROP_LETTER_SPACING:
    case CSS_PROP_OUTLINE_WIDTH:
    case CSS_PROP_WORD_SPACING:
        {
            const struct css_value *cssval = findValue(val, value.length());
            if (cssval) {
                if(cssval->id == CSS_VAL_THIN || cssval->id == CSS_VAL_MEDIUM ||
                   cssval->id == CSS_VAL_THICK )
                    //kdDebug( 6080 ) << "got value " << cssval->id << endl;
                    parsedValue = new CSSPrimitiveValueImpl(cssval->id);
                break;
            }
            parsedValue = parseUnit(curP, endP, LENGTH);
            break;
        }

// length, percent
    case CSS_PROP_PADDING_TOP:
    case CSS_PROP_PADDING_RIGHT:
    case CSS_PROP_PADDING_BOTTOM:
    case CSS_PROP_PADDING_LEFT:
    case CSS_PROP_TEXT_INDENT:
    case CSS_PROP_BOTTOM:
    case CSS_PROP_FONT_SIZE:
    case CSS_PROP_HEIGHT:
    case CSS_PROP_LEFT:
    case CSS_PROP_MARGIN_TOP:
    case CSS_PROP_MARGIN_RIGHT:
    case CSS_PROP_MARGIN_BOTTOM:
    case CSS_PROP_MARGIN_LEFT:
    case CSS_PROP_MAX_HEIGHT:
    case CSS_PROP_MAX_WIDTH:
    case CSS_PROP_MIN_HEIGHT:
    case CSS_PROP_MIN_WIDTH:
    case CSS_PROP_RIGHT:
    case CSS_PROP_TOP:
    case CSS_PROP_VERTICAL_ALIGN:
    case CSS_PROP_WIDTH:
        parsedValue = parseUnit(curP, endP, LENGTH | PERCENT );
        break;

// angle
    case CSS_PROP_ELEVATION:
        parsedValue = parseUnit(curP, endP, ANGLE);
        break;

// number
    case CSS_PROP_FONT_SIZE_ADJUST:
    case CSS_PROP_ORPHANS:
    case CSS_PROP_PITCH_RANGE:
    case CSS_PROP_RICHNESS:
    case CSS_PROP_SPEECH_RATE:
    case CSS_PROP_STRESS:
    case CSS_PROP_WIDOWS:
    case CSS_PROP_Z_INDEX:
        parsedValue = parseUnit(curP, endP, NUMBER);
        break;

// length, percent, number
    case CSS_PROP_LINE_HEIGHT:
        //kdDebug(0) << "line-height: '" << QString(curP, endP-curP) << "'" << endl;
        parsedValue = parseUnit(curP, endP, LENGTH | PERCENT | NUMBER);
        break;

// number, percent
    case CSS_PROP_VOLUME:
        parsedValue = parseUnit(curP, endP, PERCENT | NUMBER);
        break;

// frequency
    case CSS_PROP_PITCH:
        parsedValue = parseUnit(curP, endP, FREQUENCY);
        break;

// string
    case CSS_PROP_TEXT_ALIGN:
        // only for table columns.
        break;

// rect
    case CSS_PROP_CLIP:
        // rect, ident
        break;

// lists
    case CSS_PROP_CONTENT:
        // list of string, uri, counter, attr, i
    case CSS_PROP_COUNTER_INCREMENT:
        // list of CSS2CounterIncrement
    case CSS_PROP_COUNTER_RESET:
        // list of CSS2CounterReset
    case CSS_PROP_FONT_FAMILY:
        // list of strings and ids
    {
        CSSValueListImpl *list = new CSSValueListImpl;
        QString str(curP, endP-curP);
        //kdDebug( 6080 ) << "faces: '" << str << "'" << endl;
        int pos=0, pos2;
        while( 1 )
        {
            pos2 = str.find(',', pos);
            QString face = str.mid(pos, pos2-pos);
            face = face.stripWhiteSpace();
            if(face.length() == 0) break;
            if(face[0] == '\"') face.remove(0, 1);
            if(face[face.length()-1] == '\"') face = face.left(face.length()-1);
            //kdDebug( 6080 ) << "found face '" << face << "'" << endl;
            list->append(new CSSPrimitiveValueImpl(DOMString(face), CSSPrimitiveValue::CSS_STRING));
            pos = pos2 + 1;
            if(pos2 == -1) break;
        }
        //kdDebug( 6080 ) << "got " << list->length() << " faces" << endl;
        if(list->length())
            parsedValue = list;
        else
            delete list;
        break;
    }
    case CSS_PROP_QUOTES:
        // list of strings or i
    case CSS_PROP_SIZE:
        // ### look up
        break;
    case CSS_PROP_TEXT_DECORATION:
        // list of ident
    {
        CSSValueListImpl *list = new CSSValueListImpl;
        QString str(curP, endP-curP);
        str.simplifyWhiteSpace();
        //kdDebug( 6080 ) << "text-decoration: '" << str << "'" << endl;
        int pos=0, pos2;
        while( 1 )
        {
            pos2 = str.find(' ', pos);
            QString decoration = str.mid(pos, pos2-pos);
            decoration = decoration.stripWhiteSpace();
            //kdDebug( 6080 ) << "found decoration '" << decoration << "'" << endl;
            const struct css_value *cssval = findValue(decoration.lower().ascii(),
                                                       decoration.length());
            if (cssval)
                list->append(new CSSPrimitiveValueImpl(cssval->id));

            pos = pos2 + 1;
            if(pos2 == -1) break;
        }
        //kdDebug( 6080 ) << "got " << list->length() << "d decorations" << endl;
        if(list->length())
            parsedValue = list;
        else
            delete list;
        break;
    }

    case CSS_PROP_VOICE_FAMILY:
        // list of strings and i
        break;

// shorthand properties
    case CSS_PROP_BACKGROUND:
    {
        // add all shorthand properties to the list...
        //kdDebug( 6080 ) << "parsing '" << QString(curP, endP - curP) << "'" << endl;
        bool last = false;
        while(!last)
        {
            bool ignoreSpace=false;
            const QChar *nextP = curP;
            while((*nextP != ' ' || ignoreSpace) && *nextP != ';')
            {
                if(nextP >= endP) {
                    last = true;
                    break;
                }
                if (*nextP == '(')
                   ignoreSpace=true;
                if (*nextP == ')')
                   ignoreSpace=false;
                nextP++;
            }
            bool found;
            //kdDebug( 6080 ) << "parsing '" << QString(curP, nextP - curP) << "'" << endl;
            found = parseValue(curP, nextP, CSS_PROP_BACKGROUND_COLOR, important, propList);
            if(!found) found = parseValue(curP, nextP, CSS_PROP_BACKGROUND_IMAGE, important, propList);
            if(!found) found = parseValue(curP, nextP, CSS_PROP_BACKGROUND_POSITION, important, propList);
            if(!found) found = parseValue(curP, nextP, CSS_PROP_BACKGROUND_REPEAT, important, propList);
            if(!found) found = parseValue(curP, nextP, CSS_PROP_BACKGROUND_ATTACHMENT, important, propList);
            if(!found) found = parseValue(curP, nextP, CSS_PROP_BACKGROUND_POSITION, important, propList);
            curP = nextP+1;
            if(curP >= endP) break;
        }
        return true;
    }

// add all shorthand properties to the list...
    case CSS_PROP_BORDER:
    case CSS_PROP_BORDER_TOP:
    case CSS_PROP_BORDER_RIGHT:
    case CSS_PROP_BORDER_BOTTOM:
    case CSS_PROP_BORDER_LEFT:
    {
#ifdef CSS_DEBUG
        kdDebug(6080) << "parsing border property" << endl;
#endif
        const int *properties;
        const int properties_border[3] = {
            CSS_PROP_BORDER_WIDTH, CSS_PROP_BORDER_STYLE, CSS_PROP_BORDER_COLOR };
        const int properties_border_top[3] = {
            CSS_PROP_BORDER_TOP_WIDTH, CSS_PROP_BORDER_TOP_STYLE, CSS_PROP_BORDER_TOP_COLOR };
        const int properties_border_bottom[3] = {
            CSS_PROP_BORDER_BOTTOM_WIDTH, CSS_PROP_BORDER_BOTTOM_STYLE, CSS_PROP_BORDER_BOTTOM_COLOR };
        const int properties_border_left[3] = {
            CSS_PROP_BORDER_LEFT_WIDTH, CSS_PROP_BORDER_LEFT_STYLE, CSS_PROP_BORDER_LEFT_COLOR };
        const int properties_border_right[3] = {
            CSS_PROP_BORDER_RIGHT_WIDTH, CSS_PROP_BORDER_RIGHT_STYLE, CSS_PROP_BORDER_RIGHT_COLOR };

        if(propId == CSS_PROP_BORDER)
            properties = properties_border;
        else if(propId == CSS_PROP_BORDER_TOP)
            properties = properties_border_top;
        else if(propId == CSS_PROP_BORDER_BOTTOM)
            properties = properties_border_bottom;
        else if(propId == CSS_PROP_BORDER_LEFT)
            properties = properties_border_left;
        else if(propId == CSS_PROP_BORDER_RIGHT)
            properties = properties_border_right;
        else return false;

        bool last = false;
        while(!last)
        {
            const QChar *nextP = curP;
            while(*nextP != ' ' && *nextP != ';')
            {
                nextP++;
                if(nextP >= endP) {
                    last = true;
                    break;
                }
            }
            bool found;
#ifdef CSS_DEBUG
            kdDebug(6080) << "parsing \"" << QString(curP, nextP - curP) << "\"" << endl;
#endif
            found = parseValue(curP, nextP, properties[0], important, propList);
            if(!found) found = parseValue(curP, nextP, properties[1], important, propList);
            if(!found) found = parseValue(curP, nextP, properties[2], important, propList);
            if(!found) {
#ifdef CSS_DEBUG
                kdDebug(6080) << "invalid border prop" << endl;
#endif
                return false;
            }
            curP = nextP+1;
            if(curP >= endP) break;
        }
        return true;
    }

    case CSS_PROP_BORDER_COLOR:
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval && cssval->id == CSS_VAL_TRANSPARENT)
        {
            // set border colors to invalid
            parsedValue = new CSSPrimitiveValueImpl(CSS_VAL_TRANSPARENT);
            break;
        }
        const int properties[4] = {
            CSS_PROP_BORDER_TOP_COLOR,
            CSS_PROP_BORDER_RIGHT_COLOR,
            CSS_PROP_BORDER_BOTTOM_COLOR,
            CSS_PROP_BORDER_LEFT_COLOR
            };
        return parse4Values(curP, endP, properties, important, propList);
    }
    case CSS_PROP_BORDER_WIDTH:
    {
        const int properties[4] = {
            CSS_PROP_BORDER_TOP_WIDTH,
            CSS_PROP_BORDER_RIGHT_WIDTH,
            CSS_PROP_BORDER_BOTTOM_WIDTH,
            CSS_PROP_BORDER_LEFT_WIDTH
            };
        return parse4Values(curP, endP, properties, important, propList);
    }
    case CSS_PROP_BORDER_STYLE:
    {
        const int properties[4] = {
            CSS_PROP_BORDER_TOP_STYLE,
            CSS_PROP_BORDER_RIGHT_STYLE,
            CSS_PROP_BORDER_BOTTOM_STYLE,
            CSS_PROP_BORDER_LEFT_STYLE
            };
        return parse4Values(curP, endP, properties, important, propList);
    }
    case CSS_PROP_MARGIN:
    {
        const int properties[4] = {
            CSS_PROP_MARGIN_TOP,
            CSS_PROP_MARGIN_RIGHT,
            CSS_PROP_MARGIN_BOTTOM,
            CSS_PROP_MARGIN_LEFT
            };
        return parse4Values(curP, endP, properties, important, propList);
    }
    case CSS_PROP_PADDING:
    {
        const int properties[4] = {
            CSS_PROP_PADDING_TOP,
            CSS_PROP_PADDING_RIGHT,
            CSS_PROP_PADDING_BOTTOM,
            CSS_PROP_PADDING_LEFT
            };
        return parse4Values(curP, endP, properties, important, propList);
    }

    case CSS_PROP_CUE:
	break;
    case CSS_PROP_FONT:
    {
	QString fontStr = QString( curP, endP - curP );
	return parseFont( curP, endP, important, propList );
    }
    case CSS_PROP_LIST_STYLE:
    case CSS_PROP_OUTLINE:
    case CSS_PROP_PAUSE:
        break;
#ifdef CSS_DEBUG
    default:
        kdDebug( 6080 ) << "illegal property!" << endl;
#endif
    }

 end:

    if(!parsedValue) return false;

    QListIterator<CSSProperty> propIt(*propList);
    propIt.toLast(); // just remove the top one - not sure what should happen if we have multiple instances of the property
    while (propIt.current() && propIt.current()->m_id != propId)
        --propIt;
    if (propIt.current())
        propList->removeRef(propIt.current());

    CSSProperty *prop = new CSSProperty();
    prop->m_id = propId;
    prop->setValue(parsedValue);
    prop->m_bImportant = important;

    propList->append(prop);
    //kdDebug( 6080 ) << "added property " << propId << endl;

    return true;
}

// used for shorthand properties xxx{1,4}
bool StyleBaseImpl::parse4Values(const QChar *curP, const QChar *endP, const int *properties,
                                 bool important, QList<CSSProperty> *propList)
{
    bool last = false;
    QList<QChar> list;
    while(!last)
    {
        const QChar *nextP = curP;
        while(*nextP != ' ' && *nextP != ';')
        {
            nextP++;
            if(nextP >= endP) {
                last = true;
                break;
            }
        }
        list.append(curP);
        list.append(nextP);
        curP = nextP+1;
        if(curP >= endP) break;
    }
    switch(list.count())
    {
    case 2:
        // this is needed to get a correct reply for the border shorthand
        // property
        if(!parseValue(list.at(0), list.at(1), properties[0],
                   important, propList)) return false;
        parseValue(list.at(0), list.at(1), properties[1],
                   important, propList);
        parseValue(list.at(0), list.at(1), properties[2],
                   important, propList);
        parseValue(list.at(0), list.at(1), properties[3],
                   important, propList);
        return true;
    case 4:
        if(!parseValue(list.at(0), list.at(1), properties[0],
                   important, propList)) return false;
        if(!parseValue(list.at(2), list.at(3), properties[1],
                   important, propList)) return false;
        parseValue(list.at(0), list.at(1), properties[2],
                   important, propList);
        parseValue(list.at(2), list.at(3), properties[3],
                   important, propList);
        return true;
    case 6:
        if(!parseValue(list.at(0), list.at(1), properties[0],
                   important, propList)) return false;
        if(!parseValue(list.at(2), list.at(3), properties[1],
                   important, propList)) return false;
        if(!parseValue(list.at(4), list.at(5), properties[2],
                   important, propList)) return false;
        parseValue(list.at(2), list.at(3), properties[3],
                   important, propList);
        return true;
    case 8:
        if(!parseValue(list.at(0), list.at(1), properties[0],
                   important, propList)) return false;
        if(!parseValue(list.at(2), list.at(3), properties[1],
                   important, propList)) return false;
        if(!parseValue(list.at(4), list.at(5), properties[2],
                   important, propList)) return false;
        if(!parseValue(list.at(6), list.at(7), properties[3],
                   important, propList)) return false;
        return true;
    default:
        return false;
    }
}

CSSValueImpl *
StyleBaseImpl::parseUnit(const QChar * curP, const QChar *endP, int allowedUnits)
{
    endP--;
    while(*endP == ' ' && endP > curP) endP--;
    const QChar *split = endP;
    // splt up number and unit
    while( (*split < '0' || *split > '9') && *split != '.' && split > curP)
        split--;
    split++;

    QString s(curP, split-curP);

    bool isInt = false;
    if(s.find('.') == -1) isInt = true;

    bool ok;
    float value = s.toFloat(&ok);
    if(!ok) return 0;

    if(split > endP) // no unit
    {
        if(allowedUnits & NUMBER)
            return new CSSPrimitiveValueImpl(value, CSSPrimitiveValue::CSS_NUMBER);

        if(allowedUnits & INTEGER && isInt) // ### DOM CSS doesn't seem to define something for integer
            return new CSSPrimitiveValueImpl(value, CSSPrimitiveValue::CSS_NUMBER);

        if(allowedUnits & LENGTH)
            return new CSSPrimitiveValueImpl(value, CSSPrimitiveValue::CSS_PX);

        return 0;
    }

    CSSPrimitiveValue::UnitTypes type = CSSPrimitiveValue::CSS_UNKNOWN;
    int unit = 0;

    switch(split->lower().latin1())
    {
    case '%':
        type = CSSPrimitiveValue::CSS_PERCENTAGE;
        unit = PERCENT;
        break;
    case 'e':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'm' || split->latin1() == 'M')
        {
            type = CSSPrimitiveValue::CSS_EMS;
            unit = LENGTH;
        }
        else if(split->latin1() == 'x' || split->latin1() == 'X')
        {
            type = CSSPrimitiveValue::CSS_EXS;
            unit = LENGTH;
        }
        break;
    case 'p':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'x' || split->latin1() == 'X')
        {
            type = CSSPrimitiveValue::CSS_PX;
            unit = LENGTH;
        }
        else if(split->latin1() == 't' || split->latin1() == 'T')
        {
            type = CSSPrimitiveValue::CSS_PT;
            unit = LENGTH;
        }
        else if(split->latin1() == 'c' || split->latin1() == 'C')
        {
            type = CSSPrimitiveValue::CSS_PC;
            unit = LENGTH;
        }
        break;
    case 'c':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'm' || split->latin1() == 'M')
        {
            type = CSSPrimitiveValue::CSS_CM;
            unit = LENGTH;
        }
        break;
    case 'm':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'm' || split->latin1() == 'M')
        {
            type = CSSPrimitiveValue::CSS_MM;
            unit = LENGTH;
        }
        else if(split->latin1() == 's' || split->latin1() == 'S')
        {
            type = CSSPrimitiveValue::CSS_MS;
            unit = TIME;
        }
        break;
    case 'i':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'n' || split->latin1() == 'N')
        {
            type = CSSPrimitiveValue::CSS_IN;
            unit = LENGTH;
        }
        break;
    case 'd':
        type = CSSPrimitiveValue::CSS_DEG;
        unit = ANGLE;
        break;
    case 'r':
        type = CSSPrimitiveValue::CSS_RAD;
        unit = ANGLE;
        break;
    case 'g':
        type = CSSPrimitiveValue::CSS_GRAD;
        unit = ANGLE;
        break;
    case 's':
        type = CSSPrimitiveValue::CSS_S;
        unit = TIME;
        break;
    case 'h':
        type = CSSPrimitiveValue::CSS_HZ;
        unit = FREQUENCY;
        break;
    case 'k':
        type = CSSPrimitiveValue::CSS_KHZ;
        unit = FREQUENCY;
        break;
    }

    if(unit & allowedUnits)
    {
        //kdDebug( 6080 ) << "found allowed number " << value << ", unit " << type << endl;
        return new CSSPrimitiveValueImpl(value, type);
    }

    return 0;
}


// ------------------- begin font property ---------------------
/*
  Parser for the font property of CSS.  See
  http://www.w3.org/TR/REC-CSS2/fonts.html#propdef-font for details.

  Written by Jasmin Blanchette (jasmin@trolltech.com) on 2000-08-16.
*/

#include <qstring.h>
#include <qstringlist.h>

enum { Tok_Eoi, Tok_Slash, Tok_Comma, Tok_String, Tok_Symbol };

static QChar EOS = '\0';
static QChar yyCh;
static QString yyIn;
static int yyPos;

int getChar()
{
    return ( yyPos == yyIn.length() ) ? EOS : yyIn[yyPos++];
}

void startTokenizer( const QString& str )
{
    yyIn = str.simplifyWhiteSpace();
    yyPos = 0;
    yyCh = getChar();
}

static QString yyStr;

int getToken()
{
    static QString nonword = QString::fromLatin1( " /," );

    yyStr = QString::null;

    if ( yyCh == EOS )
	return Tok_Eoi;
    if ( yyCh == QChar(' ') )
	yyCh = getChar();

    if ( yyCh == QChar('/') ) {
	yyCh = getChar();
	return Tok_Slash;
    } else if ( yyCh == QChar(',') ) {
	yyCh = getChar();
	return Tok_Comma;
    } else if ( yyCh == QChar('"') ) {
	yyCh = getChar();
	while ( yyCh != QChar('"') && yyCh != EOS ) {
	    yyStr += yyCh;
	    yyCh = getChar();
	}
	yyCh = getChar();
	return Tok_String;
    } else {
	while ( nonword.find(yyCh) == -1 && yyCh != EOS ) {
	    yyStr += yyCh;
	    yyCh = getChar();
	}
	return Tok_Symbol;
    }
}

static int yyTok;

bool match( int tok )
{
    bool matched = ( yyTok == tok );
    if ( matched )
	yyTok = getToken();
    return matched;
}

bool matchFontStyle( QString *fstyle )
{
    bool matched = ( yyTok == Tok_Symbol &&
		     (yyStr == "normal" || yyStr == "italic" ||
		      yyStr == "oblique" || yyStr == "inherit") );
    if ( matched ) {
	*fstyle = yyStr;
	yyTok = getToken();
    }
    return matched;
}

bool matchFontVariant( QString *fvariant )
{
    bool matched = ( yyTok == Tok_Symbol &&
		     (yyStr == "normal" || yyStr == "small-caps"
		      || yyStr == "inherit") );
    if ( matched ) {
	*fvariant = yyStr;
	yyTok = getToken();
    }
    return matched;
}

bool matchFontWeight( QString *fweight )
{
    bool matched = ( yyTok == Tok_Symbol );
    if ( matched ) {
	if ( yyStr.length() == 3 ) {
	    matched = ( yyStr[0].unicode() >= '1' &&
			yyStr[0].unicode() <= '9' &&
			yyStr.right(2) == QString::fromLatin1("00") );
	} else {
	    matched = ( yyStr == "normal" || yyStr == "bold" ||
			yyStr == "bolder" || yyStr == "lighter" ||
			yyStr == "inherit" );
	}
    }
    if ( matched ) {
	*fweight = yyStr;
	yyTok = getToken();
    }
    return matched;
}

bool matchFontSize( QString *fsize )
{
    bool matched = ( yyTok == Tok_Symbol );
    if ( matched ) {
	*fsize = yyStr;
	yyTok = getToken();
    }
    return matched;
}

bool matchLineHeight( QString *lheight )
{
    bool matched = ( yyTok == Tok_Symbol );
    if ( matched ) {
	*lheight = yyStr;
	yyTok = getToken();
    }
    return matched;
}

bool matchNameFamily( QString *ffamily )
{
    bool matched = ( yyTok == Tok_String || yyTok == Tok_Symbol );
    if ( matched ) {
	*ffamily = yyStr;
	yyTok = getToken();
    }
    return matched;
}

bool matchFontFamily( QString *ffamily )
{
    QStringList t;

#if 0 
    // ###
    if ( yyTok == Tok_String && yyStr == "inherit" ) {
	t.clear();
	yyTok = getToken();
	return TRUE;
    }
#endif
    
    QString name;
    do {
	if ( !matchNameFamily(&name) )
	    return FALSE;
	t.append( name );
    } while ( match(Tok_Comma) );

    *ffamily = t.join(", ");
    return TRUE;
}

bool matchRealFont( QString *fstyle, QString *fvariant, QString *fweight,
		    QString *fsize, QString *lheight, QString *ffamily )
{
    bool metFstyle = matchFontStyle( fstyle );
    bool metFvariant = matchFontVariant( fvariant );
    matchFontWeight( fweight );
    if ( !metFstyle )
	metFstyle = matchFontStyle( fstyle );
    if ( !metFvariant )
	matchFontVariant( fvariant );
    if ( !metFstyle )
	matchFontStyle( fstyle );

    if ( !matchFontSize(fsize) )
	return FALSE;
    if ( match(Tok_Slash) ) {
	if ( !matchLineHeight(lheight) )
	    return FALSE;
    }
    if ( !matchFontFamily(ffamily) )
	return FALSE;
    return true;
}

bool StyleBaseImpl::parseFont(const QChar *curP, const QChar *endP,
			      bool important, QList<CSSProperty> *propList)
{
    QString str( curP, endP - curP );
    QString fstyle;
    QString fvariant;
    QString fweight;
    QString fsize;
    QString lheight;
    QString ffamily;

    startTokenizer( str );

    qDebug( "%s", str.latin1() );

    if ( yyIn == "caption" || yyIn == "icon" || yyIn == "menu" ||
	 yyIn == "message-box" || yyIn == "small-caption" ||
	 yyIn == "status-bar" || yyIn == "inherit" ) {
	kdDebug() << "system font requested..." << endl;
    } else {
	yyTok = getToken();
	if ( matchRealFont(&fstyle, &fvariant, &fweight, &fsize, &lheight,
			   &ffamily) ) {
	    qDebug( "  %s %s %s %s / %s", fstyle.latin1(),
		    fvariant.latin1(), fweight.latin1(), fsize.latin1(),
		    lheight.latin1() );
	    if(!fstyle.isNull())
		parseValue(fstyle.unicode(), fstyle.unicode()+fstyle.length(), 
			   CSS_PROP_FONT_STYLE,
			   important, propList);
	    if(!fvariant.isNull())
		parseValue(fvariant.unicode(), fvariant.unicode()+fvariant.length(), 
			   CSS_PROP_FONT_VARIANT,
			   important, propList);
	    if(!fweight.isNull())
		parseValue(fweight.unicode(), fweight.unicode()+fweight.length(), 
			   CSS_PROP_FONT_WEIGHT,
			   important, propList);
	    if(!fsize.isNull())
		parseValue(fsize.unicode(), fsize.unicode()+fsize.length(), 
			   CSS_PROP_FONT_SIZE,
			   important, propList);
	    if(!lheight.isNull())
		parseValue(lheight.unicode(), lheight.unicode()+lheight.length(), 
			   CSS_PROP_LINE_HEIGHT,
			   important, propList);
	    if(!ffamily.isNull())
		parseValue(ffamily.unicode(), ffamily.unicode()+ffamily.length(), 
			   CSS_PROP_FONT_FAMILY,
			   important, propList);
	    
	    return true;
	}
    }
    return false;
}

// ---------------- end font property --------------------------

CSSStyleRuleImpl *
StyleBaseImpl::parseStyleRule(const QChar *&curP, const QChar *endP)
{
    //kdDebug( 6080 ) << "style rule is \'" << QString(curP, endP-curP) << "\'" << endl;

    const QChar *startP;
    QList<CSSSelector> *slist;
    QList<CSSProperty> *plist;

    startP = curP;
    curP = parseToChar(startP, endP, '{', false);
    if (!curP)
        return(0);
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "selector is \'" << QString(startP, curP-startP) << "\'" << endl;
#endif

    slist = parseSelector(startP, curP );

    curP++; // need to get past the '{' from above

    startP = curP;
    curP = parseToChar(startP, endP, '}', false);
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "rules are \'" << QString(startP, curP-startP) << "\'" << endl;
#endif
    if (!curP)
    {
        delete slist;
        return(0);
    }

    plist = parseProperties(startP, curP );

    curP++; // need to get past the '}' from above

    if (!plist || !slist)
    {
        // Useless rule
        delete slist;
        delete plist;
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "bad style rule" << endl;
#endif
        return 0;
    }

    // return the newly created rule
    CSSStyleRuleImpl *rule = new CSSStyleRuleImpl(this);
    CSSStyleDeclarationImpl *decl = new CSSStyleDeclarationImpl(rule, plist);

    rule->setSelector(slist);
    rule->setDeclaration(decl);
    // ### set selector and value
    return rule;
}

CSSRuleImpl *
StyleBaseImpl::parseRule(const QChar *&curP, const QChar *endP)
{
    curP = parseSpace(curP, endP);
    CSSRuleImpl *rule = 0;

    if(!curP) return 0;
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "parse rule: current = " << curP->latin1() << endl;
#endif

    if (*curP == '@')
        rule = parseAtRule(curP, endP);
    else
        rule = parseStyleRule(curP, endP);

    if(curP) curP++;
    return rule;
}

// remove comments, replace character escapes and simplify spacing
QString StyleBaseImpl::preprocess(const QString &str)
{
    QString processed;

    bool sq = false;
    bool dq = false;
    bool comment = false;
    bool firstChar = false;

    const QChar *ch = str.unicode();
    const QChar *last = str.unicode()+str.length();
    while(ch < last) {
        if ( !comment && !sq && *ch == '"' ) {
            dq = !dq;
            processed += *ch;
        } else if ( !comment && !dq && *ch == '\'' ) {
            dq = !dq;
            processed += *ch;
        } else if ( comment ) {
            if ( firstChar && *ch == '/' ) {
                comment = false;
                firstChar = false;
            } else if ( *ch == '*' )
                firstChar = true;
            else
                firstChar = false;
        } else if ( !sq && !dq ) {
            // check for comment
            if ( firstChar ) {
                if ( *ch == '*' ) {
                    comment = true;
                } else {
                    processed += '/';
                    processed += *ch;
                }
                firstChar = false;
            } else if ( *ch == '/' )
                firstChar = true;
            else
                processed += *ch;
        }
        else
            processed += *ch;
        ++ch;
    }

    return processed;
}

// ------------------------------------------------------------------------------

StyleListImpl::~StyleListImpl()
{
    StyleBaseImpl *n;

    if(!m_lstChildren) return;

    for( n = m_lstChildren->first(); n != 0; n = m_lstChildren->next() )
    {
        n->setParent(0);
        if(n->deleteMe()) delete n;
    }
    delete m_lstChildren;
}

// --------------------------------------------------------------------------------

CSSSelector::CSSSelector(void)
: tag(0), tagHistory(0)
{
    attr = 0;
    match = None;
    relation = Descendant;
    nonCSSHint = false;
}

CSSSelector::~CSSSelector(void)
{
    if (tagHistory)
    {
        delete tagHistory;
    }
}

void CSSSelector::print(void)
{
//    kdDebug( 6080 ) << "[Selector: tag = " << //      tag << ", attr = \"" << attr << "\", value = \"" << value.string().data() << "\" relation = " << (int)relation << endl;
}

int CSSSelector::specificity()
{
    if ( nonCSSHint )
        return 0;

    int s = 0;
    if(tag != -1) s = 1;
    switch(match)
    {
    case Exact:
        if(attr == ATTR_ID)
        {
            s += 100;
            break;
        }
    case Set:
    case List:
    case Hyphen:
    case Pseudo:
        s += 10;
    case None:
        break;
    }
    if(tagHistory)
        s += tagHistory->specificity();
    return s;
}
// ----------------------------------------------------------------------------

CSSProperty::~CSSProperty()
{
    if(m_value) m_value->deref();
}

void CSSProperty::setValue(CSSValueImpl *val)
{
    if(m_value) m_value->deref();
    m_value = val;
    if(m_value) m_value->ref();
}

CSSValueImpl *CSSProperty::value()
{
    return m_value;
}
