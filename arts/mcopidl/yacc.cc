
/*  A Bison parser, made from yacc.yy
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_STRUCT	257
#define	T_ENUM	258
#define	T_INTERFACE	259
#define	T_MODULE	260
#define	T_LEFT_CURLY_BRACKET	261
#define	T_RIGHT_CURLY_BRACKET	262
#define	T_LEFT_PARANTHESIS	263
#define	T_RIGHT_PARANTHESIS	264
#define	T_LESS	265
#define	T_GREATER	266
#define	T_EQUAL	267
#define	T_SEMICOLON	268
#define	T_COLON	269
#define	T_COMMA	270
#define	T_IDENTIFIER	271
#define	T_INTEGER_LITERAL	272
#define	T_UNKNOWN	273
#define	T_BOOLEAN	274
#define	T_STRING	275
#define	T_LONG	276
#define	T_BYTE	277
#define	T_OBJECT	278
#define	T_SEQUENCE	279
#define	T_AUDIO	280
#define	T_IN	281
#define	T_OUT	282
#define	T_STREAM	283
#define	T_MULTI	284
#define	T_ATTRIBUTE	285
#define	T_READONLY	286
#define	T_ASYNC	287
#define	T_ONEWAY	288

#line 22 "yacc.yy"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "core.h"

using namespace std;

extern int idl_line_no;
extern string idl_filename;

extern int yylex();
extern void mcopidlInitFlex( const char *_code );
extern void addEnumTodo( EnumDef *edef );
extern void addStructTodo( TypeDef *type );
extern void addInterfaceTodo( InterfaceDef *iface );

void yyerror( const char *s )
{
	printf( "%s:%i: %s\n", idl_filename.c_str(), idl_line_no, s );
    exit(1);
	//   theParser->parse_error( idl_lexFile, s, idl_line_no );
}


#line 49 "yacc.yy"
typedef union
{
  // generic data types
  long		_int;
  char*		_str;
  unsigned short	_char;
  double	_float;

  vector<char *> *_strs;

  // types
  vector<TypeComponent *> *_typeComponentSeq;
  TypeComponent* _typeComponent;

  // enums
  vector<EnumComponent *> *_enumComponentSeq;

  // interfaces
  InterfaceDef *_interfaceDef;

  ParamDef* _paramDef;
  vector<ParamDef *> *_paramDefSeq;

  MethodDef* _methodDef;
  vector<MethodDef *> *_methodDefSeq;

  AttributeDef* _attributeDef;
  vector<AttributeDef *> *_attributeDefSeq;
} YYSTYPE;
#ifndef YYDEBUG
#define YYDEBUG 1
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		110
#define	YYFLAG		-32768
#define	YYNTBASE	35

#define YYTRANSLATE(x) ((unsigned)(x) <= 288 ? yytranslate[x] : 60)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    30,    31,    32,    33,    34
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     7,     9,    11,    13,    15,    22,    29,
    31,    33,    35,    39,    43,    49,    57,    59,    62,    69,
    71,    74,    77,    79,    85,    87,    89,    91,    93,    99,
   101,   104,   106,   109,   112,   116,   119,   123,   131,   133,
   136,   138,   142,   145,   147,   151,   153,   158,   160,   165,
   167,   169,   171,   173,   175,   177,   179
};

static const short yyrhs[] = {    36,
     0,    59,     0,    37,    36,     0,    38,     0,    42,     0,
    44,     0,    39,     0,     3,    17,     7,    56,     8,    14,
     0,     4,    40,     7,    41,     8,    14,     0,    17,     0,
    59,     0,    17,     0,    17,    13,    18,     0,    41,    16,
    17,     0,    41,    16,    17,    13,    18,     0,     5,    17,
    43,     7,    45,     8,    14,     0,    59,     0,    15,    55,
     0,     6,    17,     7,    36,     8,    14,     0,    59,     0,
    51,    45,     0,    46,    45,     0,    49,     0,    47,    31,
    57,    55,    14,     0,    59,     0,    32,     0,    59,     0,
    34,     0,    50,    57,    29,    55,    14,     0,    27,     0,
    27,    30,     0,    28,     0,    28,    30,     0,    33,    27,
     0,    33,    27,    30,     0,    33,    28,     0,    33,    28,
    30,     0,    48,    57,    17,     9,    52,    10,    14,     0,
    59,     0,    54,    53,     0,    59,     0,    53,    16,    54,
     0,    57,    17,     0,    17,     0,    55,    16,    17,     0,
    59,     0,    57,    55,    14,    56,     0,    58,     0,    25,
    11,    58,    12,     0,    20,     0,    21,     0,    22,     0,
    23,     0,    24,     0,    26,     0,    17,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   113,   115,   115,   117,   117,   117,   117,   119,   131,   144,
   144,   146,   153,   159,   167,   174,   196,   198,   200,   208,
   212,   217,   223,   225,   239,   241,   244,   246,   249,   262,
   264,   265,   266,   267,   268,   269,   270,   274,   284,   289,
   296,   301,   310,   325,   327,   329,   333,   352,   353,   365,
   367,   368,   369,   370,   371,   372,   376
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_STRUCT",
"T_ENUM","T_INTERFACE","T_MODULE","T_LEFT_CURLY_BRACKET","T_RIGHT_CURLY_BRACKET",
"T_LEFT_PARANTHESIS","T_RIGHT_PARANTHESIS","T_LESS","T_GREATER","T_EQUAL","T_SEMICOLON",
"T_COLON","T_COMMA","T_IDENTIFIER","T_INTEGER_LITERAL","T_UNKNOWN","T_BOOLEAN",
"T_STRING","T_LONG","T_BYTE","T_OBJECT","T_SEQUENCE","T_AUDIO","T_IN","T_OUT",
"T_STREAM","T_MULTI","T_ATTRIBUTE","T_READONLY","T_ASYNC","T_ONEWAY","aidlfile",
"definitions","definition","structdef","enumdef","maybe_identifier","enumbody",
"interfacedef","inheritedinterfaces","moduledef","classbody","attributedef",
"maybereadonly","maybeoneway","streamdef","direction","methoddef","paramdefs",
"paramdefs1","paramdef","identifierlist","structbody","type","simpletype","epsilon", NULL
};
#endif

static const short yyr1[] = {     0,
    35,    36,    36,    37,    37,    37,    37,    38,    39,    40,
    40,    41,    41,    41,    41,    42,    43,    43,    44,    45,
    45,    45,    46,    46,    47,    47,    48,    48,    49,    50,
    50,    50,    50,    50,    50,    50,    50,    51,    52,    52,
    53,    53,    54,    55,    55,    56,    56,    57,    57,    58,
    58,    58,    58,    58,    58,    58,    59
};

static const short yyr2[] = {     0,
     1,     1,     2,     1,     1,     1,     1,     6,     6,     1,
     1,     1,     3,     3,     5,     7,     1,     2,     6,     1,
     2,     2,     1,     5,     1,     1,     1,     1,     5,     1,
     2,     1,     2,     2,     3,     2,     3,     7,     1,     2,
     1,     3,     2,     1,     3,     1,     4,     1,     4,     1,
     1,     1,     1,     1,     1,     1,     0
};

static const short yydefact[] = {    57,
     0,    57,     0,     0,     1,    57,     4,     7,     5,     6,
     2,     0,    10,     0,    11,    57,     0,     3,    57,     0,
     0,     0,    17,    57,    56,    50,    51,    52,    53,    54,
     0,    55,     0,     0,    48,    46,    12,     0,    44,    18,
    57,     0,     0,     0,     0,     0,     0,     0,     0,    30,
    32,    26,     0,    28,     0,    57,     0,     0,    23,     0,
    57,    27,     0,     0,     8,    57,    13,     9,    14,    45,
    31,    33,    34,    36,     0,    22,     0,     0,     0,    21,
    19,    49,    47,     0,    35,    37,    16,     0,     0,     0,
    15,     0,    57,     0,    24,     0,    57,     0,    39,    29,
     0,    40,    41,    43,    38,     0,    42,     0,     0,     0
};

static const short yydefgoto[] = {   108,
     5,     6,     7,     8,    14,    38,     9,    22,    10,    55,
    56,    57,    58,    59,    60,    61,    96,   102,    97,    40,
    33,    34,    35,    11
};

static const short yypact[] = {    40,
   -10,     5,    17,    21,-32768,    40,-32768,-32768,-32768,-32768,
-32768,    41,-32768,    45,-32768,    46,    53,-32768,     7,    48,
    49,    55,-32768,    40,-32768,-32768,-32768,-32768,-32768,-32768,
    52,-32768,    59,    49,-32768,-32768,    56,     0,-32768,    54,
     8,    60,   -11,    57,    33,    58,    61,    62,    63,    43,
    44,-32768,    -9,-32768,    64,     8,    47,     7,-32768,     7,
     8,    -5,    67,    65,-32768,     7,-32768,-32768,    69,-32768,
-32768,-32768,    66,    68,    70,-32768,     7,    71,    72,-32768,
-32768,-32768,-32768,    74,-32768,-32768,-32768,    49,    76,    49,
-32768,    37,     7,    42,-32768,    73,-32768,    77,-32768,-32768,
    75,    81,-32768,-32768,-32768,     7,-32768,    86,    87,-32768
};

static const short yypgoto[] = {-32768,
    -1,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -36,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -16,   -33,
    27,   -56,    78,    -2
};


#define	YYLAST		121


static const short yytable[] = {    15,
    45,    78,   -20,    79,    18,    25,    12,    47,    26,    27,
    28,    29,    30,    23,    32,    48,    36,    73,    74,    76,
    88,    13,    42,    25,    80,   -25,    26,    27,    28,    29,
    30,    31,    32,    16,    50,    51,    98,    17,    62,    52,
    53,    54,     1,     2,     3,     4,    66,    19,    49,    98,
    95,    20,    49,    62,    92,   100,    94,    49,    62,    24,
    21,    41,    43,    36,    37,    39,    44,    63,    46,    49,
    65,    75,    71,    72,    68,    67,    82,    77,    69,    70,
    81,    84,   101,    87,    93,   109,   110,    89,   105,   107,
    99,    91,    83,   104,   103,    85,   106,    86,     0,     0,
    90,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    64
};

static const short yycheck[] = {     2,
    34,    58,     8,    60,     6,    17,    17,     8,    20,    21,
    22,    23,    24,    16,    26,    16,    19,    27,    28,    56,
    77,    17,    24,    17,    61,    31,    20,    21,    22,    23,
    24,    25,    26,    17,    27,    28,    93,    17,    41,    32,
    33,    34,     3,     4,     5,     6,    14,     7,    16,   106,
    14,     7,    16,    56,    88,    14,    90,    16,    61,     7,
    15,     7,    11,    66,    17,    17,     8,     8,    13,    16,
    14,     8,    30,    30,    14,    18,    12,    31,    17,    17,
    14,    13,    10,    14,     9,     0,     0,    17,    14,   106,
    93,    18,    66,    17,    97,    30,    16,    30,    -1,    -1,
    29,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    43
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/local/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/local/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 8:
#line 125 "yacc.yy"
{
		addStructTodo(new TypeDef(yyvsp[-4]._str,*yyvsp[-2]._typeComponentSeq));
	    free(yyvsp[-4]._str);
	  ;
    break;}
case 9:
#line 137 "yacc.yy"
{
	  	addEnumTodo(new EnumDef(yyvsp[-4]._str,*yyvsp[-2]._enumComponentSeq));
		free(yyvsp[-4]._str);
		delete yyvsp[-2]._enumComponentSeq;
	  ;
    break;}
case 10:
#line 144 "yacc.yy"
{ yyval._str = yyvsp[0]._str; ;
    break;}
case 11:
#line 144 "yacc.yy"
{ yyval._str = strdup(""); ;
    break;}
case 12:
#line 148 "yacc.yy"
{
	  	yyval._enumComponentSeq = new vector<EnumComponent *>;
		yyval._enumComponentSeq->push_back(new EnumComponent(yyvsp[0]._str,0));
		free(yyvsp[0]._str);
	  ;
    break;}
case 13:
#line 154 "yacc.yy"
{
	  	yyval._enumComponentSeq = new vector<EnumComponent *>;
		yyval._enumComponentSeq->push_back(new EnumComponent(yyvsp[-2]._str,yyvsp[0]._int));
		free(yyvsp[-2]._str);
	  ;
    break;}
case 14:
#line 160 "yacc.yy"
{
	  	EnumComponent *last = (*yyvsp[-2]._enumComponentSeq)[yyvsp[-2]._enumComponentSeq->size()-1];

		yyval._enumComponentSeq = yyvsp[-2]._enumComponentSeq;
		yyval._enumComponentSeq->push_back(new EnumComponent(yyvsp[0]._str,last->value+1));
		free(yyvsp[0]._str);
	  ;
    break;}
case 15:
#line 168 "yacc.yy"
{
		yyval._enumComponentSeq = yyvsp[-4]._enumComponentSeq;
		yyval._enumComponentSeq->push_back(new EnumComponent(yyvsp[-2]._str,yyvsp[0]._int));
		free(yyvsp[-2]._str);
	  ;
    break;}
case 16:
#line 180 "yacc.yy"
{
	    vector<char *>::iterator ii;
		for(ii=yyvsp[-4]._strs->begin(); ii != yyvsp[-4]._strs->end(); ii++)
		{
			yyvsp[-2]._interfaceDef->inheritedInterfaces.push_back(*ii);
			free(*ii);
		}
		delete yyvsp[-4]._strs;

	  	//addInterfaceTodo(new InterfaceDef($2,*$5));
		yyvsp[-2]._interfaceDef->name = yyvsp[-5]._str;
		free(yyvsp[-5]._str);
	  	addInterfaceTodo(yyvsp[-2]._interfaceDef);
	  ;
    break;}
case 17:
#line 197 "yacc.yy"
{ yyval._strs = new vector<char *>; ;
    break;}
case 18:
#line 198 "yacc.yy"
{ yyval._strs = yyvsp[0]._strs; ;
    break;}
case 20:
#line 209 "yacc.yy"
{
	  	yyval._interfaceDef = new InterfaceDef();
	  ;
    break;}
case 21:
#line 213 "yacc.yy"
{
		yyval._interfaceDef = yyvsp[0]._interfaceDef;
		yyval._interfaceDef->methods.insert(yyval._interfaceDef->methods.begin(),yyvsp[-1]._methodDef);
	  ;
    break;}
case 22:
#line 218 "yacc.yy"
{
	    yyval._interfaceDef = yyvsp[0]._interfaceDef;
	    yyval._interfaceDef->attributes.insert(yyval._interfaceDef->attributes.begin(),yyvsp[-1]._attributeDefSeq->begin(),yyvsp[-1]._attributeDefSeq->end());
	  ;
    break;}
case 24:
#line 226 "yacc.yy"
{
	    // 16 == attribute
		vector<char *>::iterator i;
		yyval._attributeDefSeq = new vector<AttributeDef *>;
		for(i=yyvsp[-1]._strs->begin();i != yyvsp[-1]._strs->end();i++)
		{
	  	  yyval._attributeDefSeq->push_back(new AttributeDef((*i),yyvsp[-2]._str,(AttributeType)(yyvsp[-4]._int + 16)));
		  free(*i);
		}
		delete yyvsp[-1]._strs;
	  ;
    break;}
case 25:
#line 240 "yacc.yy"
{ yyval._int = 1+2; /* in&out (read & write) */ ;
    break;}
case 26:
#line 241 "yacc.yy"
{ yyval._int = 2; /* out (readonly) */ ;
    break;}
case 27:
#line 245 "yacc.yy"
{ yyval._int = methodTwoway; ;
    break;}
case 28:
#line 246 "yacc.yy"
{ yyval._int = methodOneway; ;
    break;}
case 29:
#line 250 "yacc.yy"
{
	    // 8 == stream
		vector<char *>::iterator i;
		yyval._attributeDefSeq = new vector<AttributeDef *>;
		for(i=yyvsp[-1]._strs->begin();i != yyvsp[-1]._strs->end();i++)
		{
	  	  yyval._attributeDefSeq->push_back(new AttributeDef((*i),yyvsp[-3]._str,(AttributeType)(yyvsp[-4]._int + 8)));
		  free(*i);
		}
		delete yyvsp[-1]._strs;
	  ;
    break;}
case 30:
#line 263 "yacc.yy"
{ yyval._int = streamIn; ;
    break;}
case 31:
#line 264 "yacc.yy"
{ yyval._int = streamIn|streamMulti; ;
    break;}
case 32:
#line 265 "yacc.yy"
{ yyval._int = streamOut; ;
    break;}
case 33:
#line 266 "yacc.yy"
{ yyval._int = streamOut|streamMulti; ;
    break;}
case 34:
#line 267 "yacc.yy"
{ yyval._int = streamAsync|streamIn; ;
    break;}
case 35:
#line 268 "yacc.yy"
{ yyval._int =streamAsync|streamIn|streamMulti  ;
    break;}
case 36:
#line 269 "yacc.yy"
{ yyval._int = streamAsync|streamOut; ;
    break;}
case 37:
#line 270 "yacc.yy"
{ yyval._int = streamAsync|streamOut|streamMulti; ;
    break;}
case 38:
#line 277 "yacc.yy"
{
	  	yyval._methodDef = new MethodDef(yyvsp[-4]._str,yyvsp[-5]._str,(MethodType)yyvsp[-6]._int,*yyvsp[-2]._paramDefSeq);
		free(yyvsp[-4]._str);
		free(yyvsp[-5]._str);
	  ;
    break;}
case 39:
#line 286 "yacc.yy"
{
	  	yyval._paramDefSeq = new vector<ParamDef *>;
	  ;
    break;}
case 40:
#line 290 "yacc.yy"
{
	  	yyval._paramDefSeq = yyvsp[0]._paramDefSeq;
		yyval._paramDefSeq->insert(yyval._paramDefSeq->begin(),yyvsp[-1]._paramDef);
	  ;
    break;}
case 41:
#line 298 "yacc.yy"
{
	  	yyval._paramDefSeq = new vector<ParamDef *>;
	  ;
    break;}
case 42:
#line 302 "yacc.yy"
{
	  	yyval._paramDefSeq = yyvsp[-2]._paramDefSeq;
		yyval._paramDefSeq->push_back(yyvsp[0]._paramDef);
		//$$->insert($$->begin(),$3);
	  ;
    break;}
case 43:
#line 311 "yacc.yy"
{
	  	yyval._paramDef = new ParamDef(string(yyvsp[-1]._str),string(yyvsp[0]._str));
		free(yyvsp[-1]._str);
		free(yyvsp[0]._str);
	  ;
    break;}
case 44:
#line 326 "yacc.yy"
{ yyval._strs = new vector<char *>; yyval._strs->push_back(yyvsp[0]._str); ;
    break;}
case 45:
#line 327 "yacc.yy"
{ yyval._strs = yyvsp[-2]._strs; yyval._strs->push_back(yyvsp[0]._str); ;
    break;}
case 46:
#line 329 "yacc.yy"
{
		// is empty by default
		yyval._typeComponentSeq = new vector<TypeComponent *>;
	  ;
    break;}
case 47:
#line 333 "yacc.yy"
{
	    yyval._typeComponentSeq = yyvsp[0]._typeComponentSeq;
		vector<char *>::reverse_iterator i;
		for(i = yyvsp[-2]._strs->rbegin();i != yyvsp[-2]._strs->rend();i++)
		{
		  char *identifier = *i;

		  yyval._typeComponentSeq->insert(yyval._typeComponentSeq->begin(),new TypeComponent(yyvsp[-3]._str,identifier));
		  free(identifier);
		}
		delete yyvsp[-2]._strs;
	  ;
    break;}
case 49:
#line 354 "yacc.yy"
{
	  	// a sequence<long> is for instance coded as *long

	    char *result = (char *)malloc(strlen(yyvsp[-1]._str)+1);
		result[0] = '*';
		strcpy(&result[1],yyvsp[-1]._str);
		free(yyvsp[-1]._str);

	  	yyval._str = result;
	  ;
    break;}
case 50:
#line 366 "yacc.yy"
{ yyval._str = strdup("boolean"); ;
    break;}
case 51:
#line 367 "yacc.yy"
{ yyval._str = strdup("string"); ;
    break;}
case 52:
#line 368 "yacc.yy"
{ yyval._str = strdup("long"); ;
    break;}
case 53:
#line 369 "yacc.yy"
{ yyval._str = strdup("byte"); ;
    break;}
case 54:
#line 370 "yacc.yy"
{ yyval._str = strdup("object"); ;
    break;}
case 55:
#line 371 "yacc.yy"
{ yyval._str = strdup("float"); ;
    break;}
case 56:
#line 372 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/local/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 377 "yacc.yy"


void mcopidlParse( const char *_code )
{
    mcopidlInitFlex( _code );
    yyparse();
}
