/*
    This file is part of the KDE libraries

    Copyright (C) 1997 Martin Jones (mjones@kde.org)
              (C) 1997 Torben Weis (weis@kde.org)
              (C) 1998 Waldo Bastian (bastian@kde.org)
              (C) 1999 Lars Knoll (knoll@kde.org)
              (C) 1999 Antti Koivisto (koivisto@kde.org)
              (C) 2001 Dirk Mueller (mueller@kde.org)

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
//----------------------------------------------------------------------------
//
// KDE HTML Widget - Tokenizers
// $Id$

//#define TOKEN_DEBUG
//#define TOKEN_PRINT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include "htmltokenizer.h"
#include "misc/loader.h"
#include "khtmlview.h"
#include "khtml_part.h"
#include "htmlparser.h"
#include "html_documentimpl.h"
#include "dtd.h"
#include "htmlhashes.h"
#include <kcharsets.h>
#include <kglobal.h>
#include <ctype.h>
#include <assert.h>
#include <kdebug.h>

#include "kjs.h"
#include "kentities.c"

using namespace khtml;

static const QChar commentStart [] = { '<','!','-','-', QChar::null };
static const QChar commentEnd [] = { '-','-','>', QChar::null };
static const QChar scriptEnd [] = { '<','/','s','c','r','i','p','t','>', QChar::null };
static const QChar styleEnd [] = { '<','/','s','t','y','l','e','>', QChar::null };
static const QChar listingEnd [] = { '<','/','l','i','s','t','i','n','g','>', QChar::null };
static const QChar textareaEnd [] = { '<','/','t','e','x','t','a','r','e','a','>', QChar::null };

#define QT_ALLOC_QCHAR_VEC( N ) (QChar*) new char[ sizeof(QChar)*( N ) ]
#define QT_DELETE_QCHAR_VEC( P ) delete[] ((char*)( P ))

// Partial support for MS Windows Latin-1 extensions
// full list http://www.bbsinc.com/iso8859.html
// There may be better equivalents
#define fixUpChar(x) \
            if (!x.row() ) { \
                switch (x.cell()) \
                { \
                case 0x82: EntityChar = ','; break; \
                case 0x84: EntityChar = '"'; break; \
                case 0x8b: EntityChar = '<'; break; \
                case 0x9b: EntityChar = '>'; break; \
                case 0x91: EntityChar = '\''; break; \
                case 0x92: EntityChar = '\''; break; \
                case 0x93: EntityChar = '"'; break; \
                case 0x94: EntityChar = '"'; break; \
                case 0x95: EntityChar = 0xb7; break; \
                case 0x96: EntityChar = '-'; break; \
                case 0x97: EntityChar = '-'; break; \
                case 0x98: EntityChar = '~'; break; \
                default: break; \
                } \
            }

// ----------------------------------------------------------------------------

HTMLTokenizer::HTMLTokenizer(DOM::HTMLDocumentImpl *_doc, KHTMLView *_view)
{
    view = _view;
    buffer = 0;
    scriptCode = 0;
    scriptCodeSize = scriptCodeMaxSize = 0;
    charsets = KGlobal::charsets();
    parser = new KHTMLParser(_view, _doc);
    cachedScript = 0;
    executingScript = false;
    onHold = false;

    reset();
}

HTMLTokenizer::HTMLTokenizer(DOM::HTMLDocumentImpl *_doc, DOM::DocumentFragmentImpl *i)
{
    view = 0;
    buffer = 0;
    scriptCode = 0;
    scriptCodeSize = scriptCodeMaxSize = 0;
    charsets = KGlobal::charsets();
    parser = new KHTMLParser( i, _doc );
    cachedScript = 0;
    executingScript = false;
    onHold = false;

    reset();
}

void HTMLTokenizer::reset()
{
    assert(executingScript == false);
    assert(onHold == false);
    if (cachedScript)
        cachedScript->deref(this);
    cachedScript = 0;

    if ( buffer )
        QT_DELETE_QCHAR_VEC(buffer);
    buffer = 0;
    size = 0;

    if ( scriptCode )
        QT_DELETE_QCHAR_VEC(scriptCode);
    scriptCode = 0;
    scriptCodeSize = scriptCodeMaxSize = 0;

    currToken.reset();
}

void HTMLTokenizer::begin()
{
    executingScript = false;
    onHold = false;
    reset();
    size = 4095;
    buffer = QT_ALLOC_QCHAR_VEC( 4096 );
    dest = buffer;
    tag = NoTag;
    pending = NonePending;
    discard = NoneDiscard;
    pre = false;
    prePos = 0;
    plaintext = 0;
    listing = false;
    processingInstruction = false;
    script = false;
    style = false;
    skipLF = false;
    select = false;
    comment = false;
    textarea = false;
    startTag = false;
    tquote = NoQuote;
    searchCount = 0;
    Entity = NoEntity;
    loadingExtScript = false;
    scriptSrc = "";
    pendingSrc = "";
    scriptOutput = "";
    noMoreData = false;
}

void HTMLTokenizer::addListing(DOMStringIt list)
{
    bool old_pre = pre;
    // This function adds the listing 'list' as
    // preformatted text-tokens to the token-collection
    // thereby converting TABs.
    if(!style) pre = true;
    prePos = 0;

#if 1
    while ( list.length() )
    {
        checkBuffer();

        if (skipLF && ( list[0] != '\n' ))
        {
            skipLF = false;
        }

        if (skipLF)
        {
            skipLF = false;
            ++list;
        }
        else if (( list[0] == '\n' ) || ( list[0] == '\r' ))
        {
            if (discard == LFDiscard)
            {
                // Ignore this LF
                discard = NoneDiscard; // We have discarded 1 LF
            }
            else
            {
                // Process this LF
                if (pending)
                    addPending();
                pending = LFPending;
            }
            /* Check for MS-DOS CRLF sequence */
            if (list[0] == '\r')
            {
                skipLF = true;
            }
            ++list;
        }
        else if (( list[0] == ' ' ) || ( list[0] == '\t'))
        {
            if (pending)
                addPending();
            if (list[0] == ' ')
                pending = SpacePending;
            else
                pending = TabPending;
            ++list;
        }
        else
        {
            discard = NoneDiscard;
            if (pending)
                addPending();

            prePos++;
            *dest++ = list[0];
            ++list;
        }

    }

    if ((pending == SpacePending) || (pending == TabPending))
    {
        addPending();
    }
    pending = NonePending;
#endif

    currToken.text = new DOMStringImpl( buffer, dest-buffer);
    currToken.text->ref();
    processToken();
    prePos = 0;

    pre = old_pre;
}

void HTMLTokenizer::parseListing( DOMStringIt &src)
{
    // We are inside a <script>, <style>, <textarea> . Look for the end tag
    // which is either </script>, </style> , </textarea> or -->
    // otherwise print out every received character
    if (Entity) {
        checkScriptBuffer();
        QChar *scriptCodeDest = scriptCode+scriptCodeSize;
        parseEntity(src,scriptCodeDest);
        scriptCodeSize = scriptCodeDest-scriptCode;
    }


#ifdef TOKEN_DEBUG
    kdDebug( 6036 ) << "HTMLTokenizer::parseListing()" << endl;
#endif

    bool doScriptExec = false;
    while ( src.length() )
    {
        // do we need to enlarge the buffer?
        checkBuffer();

        // Allocate memory to store the script. We will write maximal
        // 10 characers.
        checkScriptBuffer();

        char ch = src[0].latin1();
        if ( (!script || tquote == NoQuote) && ( ch == '>' ) && ( searchFor[ searchCount ] == '>'))
        {
            ++src;
            scriptCode[ scriptCodeSize ] = 0;
            scriptCode[ scriptCodeSize + 1 ] = 0;
            if (script)
            {
                if (!scriptSrc.isEmpty()) {
                    // forget what we just got; load from src url instead
                    cachedScript = parser->doc()->docLoader()->requestScript(scriptSrc, parser->doc()->baseURL());
                }
                else {
#ifdef TOKEN_DEBUG
                    kdDebug( 6036 ) << "---START SCRIPT---" << endl;
                    kdDebug( 6036 ) << QString(scriptCode, scriptCodeSize) << endl;
                    kdDebug( 6036 ) << "---END SCRIPT---" << endl;
#endif
                    // Parse scriptCode containing <script> info
                    doScriptExec = true;
                }
                processToken();
            }
            else if (style)
            {
#ifdef TOKEN_DEBUG
		kdDebug( 6036 ) << "---START STYLE---" << endl;
		kdDebug( 6036 ) << QString(scriptCode, scriptCodeSize) << endl;
		kdDebug( 6036 ) << "---END STYLE---" << endl;
#endif
                // just add it. The style element will get a DOM::TextImpl passed, which it will
                // convert into a StyleSheet.
                addListing(DOMStringIt(scriptCode, scriptCodeSize));
            }
            else
            {
                //
                // Add scriptcode to the buffer
                addListing(DOMStringIt(scriptCode, scriptCodeSize));
            }
            if(script)
                currToken.id = ID_SCRIPT + ID_CLOSE_TAG;
            else if(style)
                currToken.id = ID_STYLE + ID_CLOSE_TAG;
	    else if (textarea)
		currToken.id = ID_TEXTAREA + ID_CLOSE_TAG;
            else
                currToken.id = ID_LISTING + ID_CLOSE_TAG;
            processToken();
            if (cachedScript) {
                cachedScript->ref(this);
                if (cachedScript) { // will be 0 if script was already loaded and ref() executed it
                    loadingExtScript = true;
                    pendingSrc = QString(src.current(), src.length());
                    _src = "";
                    src = DOMStringIt();
                }
            }
            else if (view && doScriptExec && javascript && !parser->skipMode()) {
                executingScript = true;
                view->part()->executeScript(QString(scriptCode, scriptCodeSize));
                executingScript = false;
            }
            script = style = listing = textarea = false;
            scriptCodeSize = 0;

            addScriptOutput();

            return; // Finished parsing script/style/listing
        }
        // Find out wether we see an end tag without looking at
        // any other then the current character, since further characters
        // may still be on their way thru the web!
        else if ( searchCount > 0 )
        {
            const QChar& cmp = src[0];
            // broken HTML: "--->"
            // be tolerant: skip spaces before the ">", i.e "</script >"
            if (cmp.isSpace() && searchFor[searchCount].latin1() == '>')
            {
                ++src;
            }
            else if ( searchFor[searchCount] != QChar::null && cmp.lower() == searchFor[ searchCount ] )
            {
                searchBuffer[ searchCount++ ] = cmp;
                ++src;
            }
            // We were wrong => print all buffered characters and the current one;
            else
            {
                searchBuffer[ searchCount ] = 0;
		DOMStringIt pit(searchBuffer,searchCount);
		while (pit.length()) {
		    if (textarea && pit[0] == '&') {
			QChar *scriptCodeDest = scriptCode+scriptCodeSize;
			++pit;
			parseEntity(pit,scriptCodeDest,true);
			scriptCodeSize = scriptCodeDest-scriptCode;
		    }
		    else {
			scriptCode[ scriptCodeSize++ ] = pit[0];
			++pit;
		    }
		}
                searchCount = 0;
            }
        }
        // Is this perhaps the start of the </script> or </style> tag?
        else if ( ch == '<' || ch == '-' )
        {
            searchCount = 1;
            searchBuffer[ 0 ] = src[0];
            ++src;
        }
	else
        {
	    if (textarea && ch == '&') {
		QChar *scriptCodeDest = scriptCode+scriptCodeSize;
		++src;
		parseEntity(src,scriptCodeDest,true);
		scriptCodeSize = scriptCodeDest-scriptCode;
	    }
	    else {
                if(script && ch == '\"')
                    tquote = (tquote == NoQuote) ? DoubleQuote : ((tquote == SingleQuote) ? SingleQuote : NoQuote);
                else if(script && ch == '\'')
                    tquote = (tquote == NoQuote) ? SingleQuote : (tquote == DoubleQuote) ? DoubleQuote : NoQuote;
                else if (script && tquote != NoQuote && (ch == '\r' || ch == '\n'))
                    tquote = NoQuote; // HACK!!!

		scriptCode[ scriptCodeSize++ ] = src[0];
		++src;
	    }
        }
    }
}

void HTMLTokenizer::parseScript(DOMStringIt &src)
{
    parseListing(src);
}
void HTMLTokenizer::parseStyle(DOMStringIt &src)
{
    parseListing(src);
}

void HTMLTokenizer::parseComment(DOMStringIt &src)
{
#ifdef TOKEN_DEBUG
    kdDebug( 6036 ) << "HTMLTokenizer::parseComment()" << endl;
#endif

    while ( src.length() )
    {
        // do we need to enlarge the buffer?
        checkBuffer();

        // Allocate memory to store the script. We will write maximal
        // 10 characers.
        checkScriptBuffer();

        char ch = src[0].latin1();
        if (ch == '>' && searchFor[ searchCount ] == '>')
        {
            ++src;
#ifdef COMMENTS_IN_DOM
            scriptCode[ scriptCodeSize ] = 0;
            scriptCode[ scriptCodeSize + 1 ] = 0;
            currToken.id = ID_COMMENT;
            addListing(DOMStringIt(scriptCode, scriptCodeSize));
            processToken();
            currToken.id = ID_COMMENT + ID_CLOSE_TAG;
            processToken();
#endif
            script = style = listing = comment = textarea = false;
            scriptCodeSize = 0;
            return; // Finished parsing comment
        }
        // Find out wether we see an end tag without looking at
        // any other then the current character, since further characters
        // may still be on their way thru the web!
        else if ( searchCount > 0 )
        {
            const QChar& cmp = src[0];
            // broken HTML: "--->"
            if (searchCount == 2 && cmp.latin1() == '-' && searchBuffer[0].latin1() != '<')
            {
                scriptCode[ scriptCodeSize++ ] = cmp;
                ++src;
            }
            // broken HTML: "--!>"
            else if (searchCount == 2 && cmp.latin1() == '!' && searchBuffer[0].latin1() != '<')
            {
                ++src;
            }
            // be tolerant: skip spaces before the ">", i.e "</script >"
            else if (cmp.isSpace() && searchFor[searchCount].latin1() == '>')
            {
                ++src;
            }
            else if ( cmp.lower() == searchFor[ searchCount ] )
            {
                searchBuffer[ searchCount++ ] = cmp;
                ++src;
            }
            // We were wrong => print all buffered characters and the current one;
            else
            {
                searchBuffer[ searchCount ] = 0;
		DOMStringIt pit(searchBuffer,searchCount);
		while (pit.length()) {
		    if (textarea && pit[0] == '&') {
			QChar *scriptCodeDest = scriptCode+scriptCodeSize;
			++pit;
			parseEntity(pit,scriptCodeDest,true);
			scriptCodeSize = scriptCodeDest-scriptCode;
		    }
		    else {
			scriptCode[ scriptCodeSize++ ] = pit[0];
			++pit;
		    }
		}
                searchCount = 0;
            }
        }
        // Is this perhaps the start of the --> (end of comment)?
        else if ( ch == '-' )
        {
            searchCount = 1;
            searchBuffer[ 0 ] = src[0];
            ++src;
        }
	else
        {
            scriptCode[ scriptCodeSize++ ] = src[0];
            ++src;
        }
    }
}

void HTMLTokenizer::parseProcessingInstruction(DOMStringIt &src)
{
    while ( src.length() )
    {
        char chbegin = src[0].latin1();
        // Look for '?>'
        if ( chbegin == '?' )
        {
            if (searchCount < 1)        // Watch out for '--->'
                searchCount++;
        }
        else if ((searchCount == 1) && (chbegin == '>'))
        {
            // We got a '?>' sequence
            processingInstruction = false;
            ++src;
            discard=LFDiscard;
            return; // Finished parsing comment!
        }
        else
        {
            searchCount = 0;
        }
        ++src;
    }
}

void HTMLTokenizer::parseText(DOMStringIt &src)
{
    while ( src.length() )
    {
        // do we need to enlarge the buffer?
        checkBuffer();

        // ascii is okay because we only do ascii comparisons
        char chbegin = src[0].latin1();

        if (skipLF && ( chbegin != '\n' ))
        {
            skipLF = false;
        }

        if (skipLF)
        {
            skipLF = false;
            ++src;
        }
        else if (( chbegin == '\n' ) || ( chbegin == '\r' ))
        {
            processToken();

            /* Check for MS-DOS CRLF sequence */
            if (chbegin == '\r')
            {
                skipLF = true;
            }
            ++src;
        }
        else
        {
            *dest++ = src[0];
            ++src;
        }
    }
}


void HTMLTokenizer::parseEntity(DOMStringIt &src, QChar *&dest, bool start)
{
    if( start )
    {
        entityPos = 0;
        Entity = SearchEntity;
    }

    while( src.length() )
    {
        char cc = src[0].latin1();
        switch(Entity) {
        case NoEntity:
            qDebug("NoEntity");
            return;

            break;
        case SearchEntity:
            if(cc == '#') {
                entityBuffer[entityPos++] = cc;
                ++src;
                Entity = NumericSearch;
            }
            else
                Entity = EntityName;

            break;

        case NumericSearch:
            if(cc == 'x' || cc == 'X') {
                entityBuffer[entityPos++] = cc;
                ++src;
                Entity = Hexadecimal;
            }
            else if(cc >= '0' && cc <= '9')
                Entity = Decimal;
            else
                Entity = SearchSemicolon;

            break;

        case Hexadecimal:
        {
            int uc = EntityChar.unicode();
            while(src.length()) {
                QChar csrc(src[0].lower());
                cc = src[0].cell();

                if(csrc.row() || !((cc >= '0' && cc <= '9') || (cc >= 'a' && cc <= 'f'))) {
                    Entity = SearchSemicolon;
                    break;
                }
                uc = uc*16 + (cc - ( cc < 'a' ? '0' : 'a' - 10));
                ++src;
            }
            EntityChar = QChar(uc);

            break;
        }
        case Decimal:
        {
            int uc = EntityChar.unicode();
            while(src.length()) {
                cc = src[0].cell();

                if(src[0].row() || !(cc >= '0' && cc <= '9')) {
                    Entity = SearchSemicolon;
                    break;
                }

                uc = uc * 10 + (cc - '0');
                ++src;
            }
            EntityChar = QChar(uc);
            break;
        }
        case EntityName:
        {
            int ll = QMIN(src.length(), 9-entityPos);
            while(ll--) {
                QChar csrc = src[0];
                cc = csrc.cell();

                if(csrc.row() || !((cc >= 'a' && cc <= 'z') ||
                                   (cc >= '0' && cc <= '9') || (cc >= 'A' && cc <= 'Z'))) {
                    Entity = SearchSemicolon;
                    break;
                }

                entityBuffer[entityPos++] = cc;
                ++src;
            }
            if(Entity == SearchSemicolon) {
                if(entityPos > 1) {
                    const entity *e = findEntity(entityBuffer, entityPos);
                    if(e)
                        EntityChar = e->code;
                }
            }
            else
                break;
        }
        case SearchSemicolon:
            //kdDebug( 6036 ) << "ENTITY " << EntityChar.unicode() << ", " << res << endl;

            if (tag && cc != ';' ) {
                // Don't translate entities in tags with a missing ';'
                EntityChar = QChar::null;
            }

            fixUpChar(EntityChar);

            if ( EntityChar != QChar::null ) {
                checkBuffer();
                // Just insert it
                *dest++ = EntityChar;
                if (pre)
                    prePos++;
                if (src[0] == ';')
                    ++src;
            } else {
#ifdef TOKEN_DEBUG
                kdDebug( 6036 ) << "unknown entity!" << endl;
#endif

                checkBuffer(10);
                // ignore the sequence, add it to the buffer as plaintext
                *dest++ = '&';
                for(unsigned int i = 0; i < entityPos; i++)
                    dest[i] = entityBuffer[i];
                dest += entityPos;
                Entity = NoEntity;
                if (pre)
                    prePos += entityPos+1;
            }

            EntityChar = QChar::null;
            Entity = NoEntity;
            return;
        };
    }
}

static inline bool isSeparator(char curchar)
{

  return curchar == '\t' || curchar == '\r' ||
         curchar == '\n' || curchar == ' ';
}

void HTMLTokenizer::parseTag(DOMStringIt &src)
{
    if (Entity)
        parseEntity(src,dest);

    while ( src.length() )
    {
        checkBuffer();
        char curchar = src[0].latin1();

//         if(curchar == '>') {
//             tag = NoTag;
//             break;
//         }
//         else
//             src++;
#if 1
        if (( discard == AllDiscard &&
              ( curchar == ' ' || curchar == '\t' || curchar == '\n' || curchar == '\r' ) ) ||
            ( discard == SpaceDiscard && ( curchar == ' ' || curchar == '\t') ) ||
            ( discard == LFDiscard && ( curchar == '\n' || curchar == '\r' ) ))
        {
            pending = (discard == LFDiscard ? LFPending : SpacePending);
            ++src;
        }
        else
        {
//              int l = 0;
//              while(l < src.length() && (*(src.current()+l)).latin1() != '>')
//                  l++;
//              qDebug("src is now: *%s*, pending: %d, discard: %d, tquote: %d",
//                     QConstString((QChar*)src.current(), l).string().latin1(), pending, discard, tquote);
            switch(tag) {
            case NoTag:
            {
                return;
            }
            case TagName:
            {
                if (searchCount > 0)
                {
                    if (src[0] == commentStart[searchCount])
                    {
                        searchCount++;
                        if (searchCount == 4)
                        {
#ifdef TOKEN_DEBUG
                            kdDebug( 6036 ) << "Found comment" << endl;
#endif
                            // Found '<!--' sequence
                            ++src;
                            dest = buffer; // ignore the previous part of this tag
                            comment = true;
                            searchCount = 0;
                            searchFor = commentEnd;
                            tag = NoTag;
                            parseComment(src);

                            return; // Finished parsing tag!
                        }
                        *dest = src[0].lower();
                        dest++;
                        ++src;
                        break;
                    }
                    else
                    {
                        searchCount = 0; // Stop looking for '<!--' sequence
                    }
                }
                if( !isSeparator(curchar) && curchar != '>')
                {
                    // this is faster than QChar::lower()
                    if((curchar >= 'A') && (curchar <= 'Z'))
                        *dest = curchar + 'a' - 'A';
                    else
                        *dest = src[0];
                    dest++;
                    ++src;
                }
                else
                {
                    int len = dest - buffer;
                    bool beginTag;
                    QChar *ptr = buffer;
                    if ((len > 0) && (*ptr == '/'))
                    {
                        // End Tag
                        beginTag = false;
                        ptr++;
                        len--;
                    }
                    else
                    {
                        // Start Tag
                        beginTag = true;
                        // Ignore CR/LF's and spaces after a start tag
                        discard = AllDiscard;
                    }
                    // limited xhtml support. Accept empty xml tags like <br/>
                    if((len > 1) && (*(dest-1) == '/')) len--;

                    QConstString tmp(ptr, len);
                    uint tagID = khtml::getTagID(tmp.string().latin1(), len);
                    if (!tagID) {
#ifdef TOKEN_DEBUG
                        kdDebug( 6036 ) << "Unknown tag: \"" << tmp.string() << "\"" << endl;
#endif
                        dest = buffer;
                        discard = NoneDiscard;
                    }
                    else
                    {
#ifdef TOKEN_DEBUG
                        kdDebug( 6036 ) << "found tag id=" << tagID << ": " << tmp.string() << endl;
#endif
                        currToken.id = beginTag ? tagID : tagID + ID_CLOSE_TAG;
                        dest = buffer;
                    }
                    tag = SearchAttribute;
                    discard = AllDiscard;
                }
                break;
            }
            case SearchAttribute:
            {
                if( curchar == '>' )
                    tag = SearchEnd; // we reached the end
                else if(pending != NonePending && (curchar == '\'' || curchar == '"'))
                {
                    // something went wrong, lets assume it
                    // is a value because only there we handle quotes
                    tag = SearchValue;
                    discard = NoneDiscard;
                    pending = NonePending;
                    *dest++ = 0;
                    attrName = QString::null;
                }
                else
                {
                    tag = AttributeName;
                    discard = NoneDiscard;
                    pending = NonePending;
                }
                break;
            }
            case AttributeName:
            {
                discard = NoneDiscard;
                if(!isSeparator(curchar) && curchar != '=' && curchar != '>')
                {
                    if((curchar >= 'A') && (curchar <= 'Z'))
                        *dest = curchar + 'a' - 'A';
                    else
                        *dest = src[0];

                    dest++;
                    ++src;
                }
                else
                {
                    // beginning of name
                    QChar *ptr = buffer;
                    attrName = QString(ptr, dest-buffer);
                    unsigned int a = 0;
                    // ignore attributes with leading '!'
                    if(dest-buffer && attrName[0].latin1() != '!')
                        a = khtml::getAttrID(attrName.latin1(), dest-buffer);
                    dest = buffer;
                    *dest++ = a;

#ifdef TOKEN_DEBUG
                    if (!a || (attrName.length() && attrName[0].latin1() == '!')) {
                       kdDebug( 6036 ) << "Unknown attribute: " << attrName << endl;
                    } else
                    {
                       kdDebug( 6036 ) << "Known attribute: " << attrName << endl;
                    }
#endif
                    tag = SearchEqual;
                    discard = AllDiscard; // discard whitespaces before '='
                }
                break;
            }
            case SearchEqual:
            {
                if( curchar == '=' )
                {
#ifdef TOKEN_DEBUG
                    kdDebug(6036) << "found equal" << endl;
#endif
                    tag = SearchValue;
                    pending = NonePending; // ignore spaces before '='
                    discard = AllDiscard; // discard whitespaces after '='
                    ++src;
                }
                else // other chars indicate a new attribute or '>'
                {
                    AttrImpl* a;
                    if(*buffer)
                        a = new AttrImpl(parser->doc(), (int)*buffer);
                    else
                        a = new AttrImpl(parser->doc(), attrName);

                    a->setValue("");
                    currToken.insertAttr(a);

                    dest = buffer;
                    tag = SearchAttribute;
                    discard = AllDiscard;
                }
                break;
            }
            case SearchValue:
            {
                if(curchar == '\'' || curchar == '\"') {
                    tquote = curchar == '\"' ? DoubleQuote : SingleQuote;
                    tag = QuotedValue;
                    ++src;
                    discard = NoneDiscard;
                } else {
                    tag = Value;
                    discard = AllDiscard;
                }
                pending = NonePending;
                break;
            }
            case QuotedValue:
            {
                // ### attributes like '&{blaa....};' are supposed to be treated as jscript.
                if ( curchar == '&' )
                {
                    ++src;
                    discard = NoneDiscard;
                    parseEntity(src, dest, true);
                    break;
                }
                else if ( (tquote == SingleQuote && curchar == '\'') ||
                          (tquote == DoubleQuote && curchar == '\"') )
                {
                    // end of attribute
                    AttrImpl* a;

                    if(*buffer)
                        a = new AttrImpl(parser->doc(), (int)*buffer);
                    else
                        a = new AttrImpl(parser->doc(), DOMString(attrName));

                    if(a->attrId || !attrName.isNull())
                    {
                        // some <input type=hidden> rely on trailing spaces. argh
                        while(dest > buffer+1 && (*(dest-1) == '\n' || *(dest-1) == '\r'))
                            dest--; // remove trailing newlines
                        a->setValue(DOMString(buffer+1, dest-buffer-1));
                        currToken.insertAttr(a);
                    }

                    dest = buffer;
                    tag = SearchAttribute;
                    tquote = NoQuote;
                    discard = AllDiscard;
                    pending = NonePending;
                    ++src;
                    break;
                }
                discard = NoneDiscard;

                *dest++ = src[0];
                ++src;
                break;
            }
            case Value:
            {
                // parse Entities
                if ( curchar == '&' )
                {
                    ++src;
                    parseEntity(src, dest, true);
                    break;
                }
                if ( pending || curchar == '>')
                {
                    // no quotes. Every space means end of value
                    AttrImpl* a;
                    if(*buffer)
                        a = new AttrImpl(parser->doc(), (int)*buffer);
                    else
                        a = new AttrImpl(parser->doc(), DOMString(attrName));

                    a->setValue(DOMString(buffer+1, dest-buffer-1));
                    currToken.insertAttr(a);

                    dest = buffer;
                    tag = SearchAttribute;
                    discard = AllDiscard;
                    pending = NonePending;
                    break;
                }
                *dest++ = src[0];
                ++src;
                break;
            }
            case SearchEnd:
            {
                if ( curchar != '>')
                {
                    // discard everything, until we found the end
                    ++src;
                    break;
                }

                searchCount = 0; // Stop looking for '<!--' sequence
                tag = NoTag;
                tquote = NoQuote;
                pending = NonePending; // Ignore pending spaces
                ++src;

                if ( currToken.id == 0 ) //stop if tag is unknown
                {
                    discard = NoneDiscard;
                    return;
                }
                uint tagID = currToken.id;
#ifdef TOKEN_DEBUG
                kdDebug( 6036 ) << "appending Tag: " << tagID << endl;
#endif
                bool beginTag = tagID < ID_CLOSE_TAG;
                if(!(pre || textarea || tagID == ID_PRE)) {
                    // Ignore Space/LF's after a start tag
                    // Don't ignore CR/LF's after a close tag
                    discard = beginTag ? LFDiscard : NoneDiscard;
                }
                else
                    discard = NoneDiscard;

                if(!beginTag)
                    tagID -= ID_CLOSE_TAG;

                if ( beginTag && tagID == ID_SCRIPT ) {
                    AttrImpl* a = 0;
                    if(currToken.attrs) {
                        a = currToken.attrs->getIdItem(ATTR_SRC);
                        scriptSrc = a ? a->value().string() : QString("");
                        a = currToken.attrs->getIdItem(ATTR_LANGUAGE);
                    }
                    javascript = true;
                    if( a ) {
                        QString lang = a->value().string();
                        lang = lang.lower();
                        if( !lang.contains("javascript") &&
                            !lang.contains("ecmascript") &&
                            !lang.contains("jscript") )
                           javascript = false;
                    } else {
                        if( currToken.attrs )
                            a = currToken.attrs->getIdItem(ATTR_TYPE);
                        if( a ) {
                            QString lang = a->value().string();
                            lang = lang.lower();
                            if( !lang.contains("javascript") &&
                                !lang.contains("ecmascript") &&
                                !lang.contains("jscript") )
                                javascript = false;
                        }
                    }
                }

                processToken();

                // we have to take care to close the pre block in
                // case we encounter an unallowed element....
                if(pre && beginTag && !DOM::checkChild(ID_PRE, tagID)) {
                    kdDebug(6036) << " not allowed in <pre> " << (int)tagID << endl;
                    pre = false;
                }

                if ( tagID == ID_PRE )
                {
                    prePos = 0;
                    pre = beginTag;
                }
                else if ( tagID == ID_TEXTAREA )
                {
                    if(beginTag) {
                        listing = true;
			textarea = true;
                        searchCount = 0;
                        searchFor = textareaEnd;
                        parseListing(src);
                    }
                }
                else if ( tagID == ID_SCRIPT )
                {
                    if (beginTag)
                    {
#ifdef TOKEN_DEBUG
                        kdDebug( 6036 ) << "start of script, token->id = " << currToken.id << endl;
#endif
                        script = true;
                        searchCount = 0;
                        searchFor = scriptEnd;
                        tquote = NoQuote;
                        parseScript(src);
#ifdef TOKEN_DEBUG
                        kdDebug( 6036 ) << "end of script" << endl;
#endif
                    }
                }
                else if ( tagID == ID_STYLE )
                {
                    if (beginTag)
                    {
                        style = true;
                        searchCount = 0;
                        searchFor = styleEnd;
                        parseStyle(src);
                    }
                }
                else if ( tagID == ID_LISTING )
                {
                    if (beginTag)
                    {
                        listing = true;
                        searchCount = 0;
                        searchFor = listingEnd;
                        parseListing(src);
                    }
                }
                else if ( tagID == ID_SELECT )
                {
                    select = beginTag;
                }
                return; // Finished parsing tag!
            }
            default:
            {
#ifdef TOKEN_DEBUG
                kdDebug( 6036 ) << "error in parseTag! " << __LINE__ << endl;
#endif
                return;
            }

            } // end switch
        }
#endif
    }
    return;
}

void HTMLTokenizer::addPending()
{
    if ( tag || select)
    {
        *dest++ = ' ';
    }
    else if ( textarea )
    {
        if (pending == LFPending)
            *dest++ = '\n';
        else
            *dest++ = ' ';
    }
    else if ( pre )
    {
        int p;

        switch (pending)
        {
        case SpacePending:
            // Insert a non-breaking space
            *dest++ = QChar(' ');
            prePos++;
            break;

        case LFPending:
            *dest = '\n';
            dest++;
            prePos = 0;
            break;

        case TabPending:
            p = TAB_SIZE - ( prePos % TAB_SIZE );
            for ( int x = 0; x < p; x++ )
            {
                *dest = QChar(' ');
                dest++;
            }
            prePos += p;
            break;

        default:
#ifdef TOKEN_DEBUG
            kdDebug( 6036 ) << "Assertion failed: pending = " << (int) pending << endl;
#endif
            break;
        }
    }
    else
    {
        *dest++ = ' ';
    }

    pending = NonePending;
}

void HTMLTokenizer::setPlainText()
{
    if (!plaintext)
    {
       // Do this only once!
       plaintext = true;
       currToken.id = ID_PLAIN;
       processToken();
       dest = buffer;
    }
}

void HTMLTokenizer::write( const QString &str )
{
    // we have to make this function reentrant. This is needed, because some
    // script code could call document.write(), which would add something here.
#ifdef TOKEN_DEBUG
    kdDebug( 6036 ) << "Tokenizer::write(\"" << str << "\")" << endl;
#endif

    if ( str.isEmpty() || buffer == 0L )
        return;

    // reentrant...
    // we just insert the code at the tokenizers current position. Parsing will continue once
    // we return from the script stuff
    // (this won't happen if we're in the middle of loading an external script)
    if(executingScript || onHold) {
#ifdef TOKEN_DEBUG
        kdDebug( 6036 ) << "adding to scriptOutput" << endl;
#endif
        scriptOutput += str;
        return;
    }

    if (loadingExtScript) {
        // don't parse; we will do this later
        pendingSrc += str;
        return;
    }

    _src = str;
    src = DOMStringIt(_src);

    if (plaintext)
        parseText(src);
    else if (comment)
        parseComment(src);
    else if (script)
        parseScript(src);
    else if (style)
        parseStyle(src);
    else if (listing)
        parseListing(src);
    else if (processingInstruction)
        parseProcessingInstruction(src);
    else if (tag)
        parseTag(src);
    else if (Entity)
        parseEntity(src, dest);

    while ( src.length() )
    {
        // do we need to enlarge the buffer?
        checkBuffer();

        // doesn't hurt because we only do ASCII comparisons
        // use chbegin here instead of comparing equality with "src[0]"
        // because this is slow (two function calls)
        char chbegin = src[0].latin1();

        if (skipLF && (chbegin != '\n'))
            skipLF = false;

        if (skipLF)
        {
            skipLF = false;
            ++src;
        }
        else if ( startTag )
        {
            startTag = false;

            switch(chbegin) {
            case '/':
            {
                // Start of an End-Tag
                if(!(pre || textarea) && pending == LFPending)
                    pending = NonePending; // Ignore leading Spaces/LFs

                break;
            }

            case '!':
            {
                // <!-- comment -->
                searchCount = 1; // Look for '<!--' sequence to start comment

                break;
            }

            case '?':
            {
                // xml processing instruction
                processingInstruction = true;
                parseProcessingInstruction(src);
                continue;

                break;
            }

            default:
            {

                if( ((chbegin >= 'a') && (chbegin <= 'z')) || ((chbegin >= 'A') && (chbegin <= 'Z')))
                {
                    // Start of a Start-Tag
                }
                else
                {
                    // Invalid tag
                    // Add as is
                    if (pending)
                        addPending();
                    *dest = '<';
                    dest++;
                    *dest++ = src[0];
                    ++src;
                    continue;
                }
            }
            }; // end case

            if(pending) addPending();

            processToken();

            tag = TagName;
            parseTag(src);
        }
        else if ( chbegin == '&' )
        {
            ++src;

            discard = NoneDiscard;
            if (pending)
                addPending();
            parseEntity(src, dest, true);
        }
        else if ( chbegin == '<')
        {
            ++src;
            startTag = true;
            discard = NoneDiscard;
        }
        else if (( chbegin == '\n' ) || ( chbegin == '\r' ))
        {
            if ( pre || textarea)
            {
                if (discard == LFDiscard || discard == AllDiscard)
                {
                    // Ignore this LF
                    discard = NoneDiscard; // We have discarded 1 LF
                }
                else
                {
                    // Process this LF
                    if (pending)
                        addPending();
                    pending = LFPending;
                }
            }
            else
            {
                if (discard == LFDiscard)
                {
                    // Ignore this LF
                    discard = NoneDiscard; // We have discarded 1 LF
                }
                else if(discard == AllDiscard)
                {
                }
                else
                {
                    // Process this LF
                    if (pending == NonePending)
                        pending = LFPending;
                }
            }
            /* Check for MS-DOS CRLF sequence */
            if (chbegin == '\r')
            {
                skipLF = true;
            }
            ++src;
        }
        else if (( chbegin == ' ' ) || ( chbegin == '\t' ))
        {
            if ( pre || textarea)
            {
                if (pending)
                    addPending();
                if (chbegin == ' ')
                    pending = SpacePending;
                else
                    pending = TabPending;
            }
            else
            {
                if(discard == SpaceDiscard)
                    discard = NoneDiscard;
                else if(discard == AllDiscard)
                { }
                else
                    pending = SpacePending;
            }
            ++src;
        }
        else
        {
            if (pending)
                addPending();

            discard = NoneDiscard;
            if ( pre )
            {
                prePos++;
            }
            unsigned char row = src[0].row();
            if ( row > 0x05 && row < 0x10 || row > 0xfd )
                    currToken.complexText = true;
            *dest++ = src[0];
            ++src;
        }
    }

    _src = QString();
    if (noMoreData && !cachedScript)
        end(); // this actually causes us to be deleted
}

void HTMLTokenizer::end()
{
    if ( buffer == 0 ) {
        emit finishedParsing();
        return;
    }

    processToken();

    if(buffer)
        QT_DELETE_QCHAR_VEC(buffer);

    if(scriptCode)
        QT_DELETE_QCHAR_VEC(scriptCode);

    scriptCode = 0;
    scriptCodeSize = scriptCodeMaxSize = 0;
    buffer = 0;
    emit finishedParsing();
}

void HTMLTokenizer::finish()
{
    // do this as long as we don't find matching comment ends
    while(comment && scriptCode && scriptCodeSize)
    {
        // we've found an unmatched comment start
        scriptCode[ scriptCodeSize ] = 0;
        scriptCode[ scriptCodeSize + 1 ] = 0;
        int pos = QConstString(scriptCode, scriptCodeSize).string().find('>');
        QString food;
        food.setUnicode(scriptCode+pos+1, scriptCodeSize-pos-1); // deep copy
        QT_DELETE_QCHAR_VEC(scriptCode);
        scriptCode = 0;
        scriptCodeSize = scriptCodeMaxSize = 0;
        script = style = listing = comment = textarea = false;
        scriptCodeSize = 0;
        write(food);
    }
    // this indicates we will not recieve any more data... but if we are waiting on
    // an external script to load, we can't finish parsing until that is done
    noMoreData = true;
    if (!loadingExtScript && !executingScript && !onHold)
        end(); // this actually causes us to be deleted
}

void HTMLTokenizer::processToken()
{
    if ( dest > buffer )
    {
#ifdef TOKEN_DEBUG
        if(currToken.id && currToken.id != ID_COMMENT)
            kdDebug( 6036 ) << "Error in processToken!!!" << endl;
#endif
        if ( currToken.complexText ) {
            // ### we do too much QString copying here, but better here than in RenderText...
            // anyway have to find a better solution in the long run (lars)
            QString s = QConstString(buffer, dest-buffer).string();
            s.compose();
            currToken.text = new DOMStringImpl( s.unicode(), s.length() );
            currToken.text->ref();
        } else {
            currToken.text = new DOMStringImpl( buffer, dest - buffer );
            currToken.text->ref();
        }
        if (currToken.id != ID_COMMENT)
            currToken.id = ID_TEXT;
    }
    else if(!currToken.id)
        return;

    dest = buffer;

#ifdef TOKEN_PRINT
    QString name = getTagName(currToken.id).string();
    QString text = currToken.text.string();

    kdDebug( 6036 ) << "Token --> " << name << "   id = " << currToken.id << endl;
    if(currToken.text != 0)
        kdDebug( 6036 ) << "text: \"" << text << "\"" << endl;
#else
#ifdef TOKEN_DEBUG
    QString name = getTagName(currToken.id).string();
    QString text;
    if(currToken.text)
        text = QConstString(currToken.text->s, currToken.text->l).string();

    kdDebug( 6036 ) << "Token --> " << name << "   id = " << currToken.id << endl;
    if(!text.isNull())
        kdDebug( 6036 ) << "text: \"" << text << "\"" << endl;
    int l = currToken.attrs ? currToken.attrs->length() : 0;
    if(l>0)
    {
        int i = 0;
        kdDebug( 6036 ) << "Attributes: " << l << endl;
        while(i<l)
        {
            AttrImpl* c = static_cast<AttrImpl*>(currToken.attrs->item(i));
            kdDebug( 6036 ) << "    " << c->attrId << " " << c->name().string()
                            << "=\"" << c->value().string() << "\"" << endl;
            i++;
        }
    }
    kdDebug( 6036 ) << endl;
#endif
#endif
    // pass the token over to the parser, the parser DOES NOT delete the token
    parser->parseToken(&currToken);

    currToken.reset();
}


HTMLTokenizer::~HTMLTokenizer()
{
    reset();
    delete parser;
}


void HTMLTokenizer::enlargeBuffer()
{
    QChar *newbuf = QT_ALLOC_QCHAR_VEC( size*2 );
    memcpy( newbuf, buffer, (dest - buffer + 1)*sizeof(QChar) );
    dest = newbuf + ( dest - buffer );
    QT_DELETE_QCHAR_VEC(buffer);
    buffer = newbuf;
    size *= 2;
}

void HTMLTokenizer::enlargeScriptBuffer()
{
    int newsize = QMAX(scriptCodeMaxSize*2, scriptCodeMaxSize+1024);
    QChar *newbuf = QT_ALLOC_QCHAR_VEC( newsize );
    if(scriptCodeSize)
        memcpy( newbuf, scriptCode, scriptCodeSize*sizeof(QChar) );
    // delete [] is unsafe!
    if(scriptCode)
        QT_DELETE_QCHAR_VEC(scriptCode);
    scriptCode = newbuf;
    scriptCodeMaxSize = newsize;
}

void HTMLTokenizer::notifyFinished(CachedObject *finishedObj)
{
    if (view && finishedObj == cachedScript) {
#ifdef TOKEN_DEBUG
        kdDebug( 6036 ) << "Finished loading an external script" << endl;
#endif
        loadingExtScript = false;
        DOMString scriptSource = cachedScript->script();
#ifdef TOKEN_DEBUG
        kdDebug( 6036 ) << "External script is:" << endl << scriptSource.string() << endl;
#endif
        cachedScript->deref(this);
        cachedScript = 0;
        executingScript = true;
        view->part()->executeScript(scriptSource.string());
        executingScript = false;

        // 'script' is true when we are called synchronously from
        // parseScript(). In that case parseScript() will take care
        // of 'scriptOutput'.
        if (!script)
        {
           QString rest = scriptOutput+pendingSrc;
           scriptOutput = pendingSrc = "";
           write(rest);
        }
    }
}

void HTMLTokenizer::addScriptOutput()
{
    if ( !scriptOutput.isEmpty() ) {
//      kdDebug( 6036 ) << "adding scriptOutput to parsed string" << endl;
        QString newStr = scriptOutput;
        newStr += QString(src.current(), src.length());
        _src = newStr;
        src = DOMStringIt(_src);
        scriptOutput = "";
    }
}

void HTMLTokenizer::setOnHold(bool _onHold)
{
    if (onHold == _onHold) return;
    onHold = _onHold;
    if (!onHold) {
	QString rest = scriptOutput+pendingSrc;
	scriptOutput = pendingSrc = "";
	write(rest);
    }
}

#include "htmltokenizer.moc"
