
/*  A Bison parser, made from yacc.yy
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	T_CHARACTER_LITERAL	257
#define	T_DOUBLE_LITERAL	258
#define	T_IDENTIFIER	259
#define	T_INTEGER_LITERAL	260
#define	T_STRING_LITERAL	261
#define	T_INCLUDE	262
#define	T_CLASS	263
#define	T_STRUCT	264
#define	T_LEFT_CURLY_BRACKET	265
#define	T_LEFT_PARANTHESIS	266
#define	T_RIGHT_CURLY_BRACKET	267
#define	T_RIGHT_PARANTHESIS	268
#define	T_COLON	269
#define	T_SEMICOLON	270
#define	T_PUBLIC	271
#define	T_PROTECTED	272
#define	T_TRIPE_DOT	273
#define	T_PRIVATE	274
#define	T_VIRTUAL	275
#define	T_CONST	276
#define	T_INLINE	277
#define	T_FRIEND	278
#define	T_RETURN	279
#define	T_SIGNAL	280
#define	T_SLOT	281
#define	T_TYPEDEF	282
#define	T_PLUS	283
#define	T_MINUS	284
#define	T_COMMA	285
#define	T_ASTERISK	286
#define	T_TILDE	287
#define	T_LESS	288
#define	T_GREATER	289
#define	T_AMPERSAND	290
#define	T_EXTERN	291
#define	T_EXTERN_C	292
#define	T_ACCESS	293
#define	T_ENUM	294
#define	T_NAMESPACE	295
#define	T_USING	296
#define	T_UNKNOWN	297
#define	T_TRIPLE_DOT	298
#define	T_TRUE	299
#define	T_FALSE	300
#define	T_STATIC	301
#define	T_EQUAL	302
#define	T_SCOPE	303
#define	T_NULL	304
#define	T_INT	305
#define	T_ARRAY_OPEN	306
#define	T_ARRAY_CLOSE	307
#define	T_CHAR	308
#define	T_DCOP	309
#define	T_DCOP_AREA	310
#define	T_SIGNED	311
#define	T_UNSIGNED	312
#define	T_LONG	313
#define	T_SHORT	314
#define	T_FUNOPERATOR	315
#define	T_MISCOPERATOR	316

#line 1 "yacc.yy"

/*****************************************************************
Copyright (c) 1999 Torben Weis <weis@kde.org>
Copyright (c) 2000 Matthias Ettrich <ettrich@kde.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <config.h>
#include <qstring.h>

#define AMP_ENTITY "&amp;"
#define YYERROR_VERBOSE

extern int yylex();

// extern QString idl_lexFile;
extern int idl_line_no;
extern int function_mode;

static int dcop_area = 0;

static QString in_namespace( "" );

void dcopidlInitFlex( const char *_code );

void yyerror( const char *s )
{
	qDebug( "In line %i : %s", idl_line_no, s );
        exit(1);
	//   theParser->parse_error( idl_lexFile, s, idl_line_no );
}


#line 57 "yacc.yy"
typedef union
{
  long   _int;
  QString        *_str;
  unsigned short          _char;
  double _float;
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



#define	YYFINAL		329
#define	YYFLAG		-32768
#define	YYNTBASE	63

#define YYTRANSLATE(x) ((unsigned)(x) <= 316 ? yytranslate[x] : 114)

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
    27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
    47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
    57,    58,    59,    60,    61,    62
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     4,     5,     8,    13,    14,    16,    17,    24,    28,
    32,    39,    40,    48,    53,    59,    62,    67,    75,    84,
    87,    89,    91,    93,    96,    97,    99,   101,   103,   105,
   107,   109,   111,   112,   116,   119,   122,   124,   128,   130,
   134,   136,   139,   143,   146,   148,   149,   151,   153,   156,
   159,   162,   165,   168,   171,   177,   182,   187,   192,   197,
   204,   212,   219,   226,   232,   236,   238,   242,   244,   246,
   248,   251,   253,   255,   257,   261,   269,   279,   286,   287,
   289,   291,   293,   296,   299,   303,   306,   309,   313,   316,
   318,   320,   322,   324,   327,   330,   333,   335,   336,   338,
   342,   344,   346,   349,   352,   357,   364,   368,   370,   373,
   375,   379,   383,   386,   389,   391,   394,   398,   400,   404,
   407,   409,   410,   413,   419,   421,   423,   425,   427,   432,
   433,   435,   437,   439,   441,   443,   450,   458,   460,   462,
   466,   471,   473,   477,   480,   486,   490,   496,   504,   511,
   515,   517,   519,   523,   528,   531,   532,   534,   537,   538,
   541,   545,   551
};

static const short yyrhs[] = {    64,
    66,    63,     0,     0,     8,    64,     0,    38,    11,    63,
    13,     0,     0,    55,     0,     0,     9,    74,    78,    65,
    80,    16,     0,     9,    74,    16,     0,    10,    74,    16,
     0,    10,    74,    11,    63,    13,    16,     0,     0,    41,
     5,    11,    67,    63,    13,    79,     0,    42,    41,     5,
    16,     0,    42,     5,    49,     5,    16,     0,    37,    16,
     0,    28,    94,    74,    16,     0,    28,    10,    11,    68,
    13,    74,    16,     0,    28,    10,    74,    11,    68,    13,
    74,    16,     0,    23,   106,     0,   106,     0,   113,     0,
    81,     0,   113,    68,     0,     0,    45,     0,    46,     0,
    20,     0,    18,     0,    17,     0,    26,     0,    27,     0,
     0,    70,    71,    15,     0,    71,    15,     0,    56,    15,
     0,     5,     0,     5,    49,    74,     0,    74,     0,    99,
    17,    75,     0,    75,     0,    76,    11,     0,    76,    31,
    77,     0,    15,    77,     0,    11,     0,     0,    16,     0,
    13,     0,    86,    80,     0,   106,    80,     0,    81,    80,
     0,    73,    80,     0,    72,    80,     0,   113,    80,     0,
    24,     9,    74,    16,    80,     0,    24,    74,    16,    80,
     0,    24,   101,    16,    80,     0,     9,    74,    16,    80,
     0,    10,    74,    16,    80,     0,    42,     5,    49,     5,
    16,    80,     0,    40,     5,    11,    82,    13,     5,    16,
     0,    40,     5,    11,    82,    13,    16,     0,    40,    11,
    82,    13,     5,    16,     0,    40,    11,    82,    13,    16,
     0,    83,    31,    82,     0,    83,     0,     5,    48,    85,
     0,     5,     0,     3,     0,     6,     0,    30,     6,     0,
    50,     0,    74,     0,    84,     0,    84,    29,    84,     0,
    28,    74,    34,    95,    35,    74,    16,     0,    28,    74,
    34,    95,    35,    49,     5,    74,    16,     0,    10,    74,
    11,    63,    13,    16,     0,     0,    22,     0,    57,     0,
    58,     0,    57,    60,     0,    57,    59,     0,    57,    60,
    51,     0,    58,    60,     0,    58,    59,     0,    58,    60,
    51,     0,    58,    51,     0,    51,     0,    59,     0,    60,
     0,    54,     0,    57,    54,     0,    58,    54,     0,    32,
    89,     0,    32,     0,     0,    96,     0,    90,    31,    96,
     0,    88,     0,    74,     0,    10,    74,     0,     9,    74,
     0,    74,    34,    92,    35,     0,    74,    34,    92,    35,
    49,    74,     0,    93,    31,    92,     0,    93,     0,    91,
    89,     0,    91,     0,    22,    91,    89,     0,    22,    91,
    36,     0,    22,    91,     0,    91,    36,     0,    91,     0,
    91,    89,     0,    94,    31,    95,     0,    94,     0,    94,
    74,    97,     0,    94,    97,     0,    44,     0,     0,    48,
    98,     0,    48,    12,    94,    14,    98,     0,     7,     0,
    85,     0,     4,     0,    69,     0,    74,    12,    90,    14,
     0,     0,    21,     0,    62,     0,    35,     0,    34,     0,
    48,     0,    94,    74,    12,    90,    14,    87,     0,    94,
    61,   100,    12,    90,    14,    87,     0,    84,     0,   102,
     0,   102,    31,   103,     0,     5,    12,   103,    14,     0,
   104,     0,   104,    31,   105,     0,   101,   108,     0,    21,
   101,    48,    50,   108,     0,    21,   101,   108,     0,    74,
    12,    90,    14,   108,     0,    74,    12,    90,    14,    15,
   105,   108,     0,    99,    33,    74,    12,    14,   108,     0,
    47,   101,   108,     0,    11,     0,    16,     0,   107,   109,
    13,     0,   107,   109,    13,    16,     0,   110,   109,     0,
     0,    16,     0,    31,   112,     0,     0,     5,   111,     0,
    94,   112,    16,     0,    47,    94,     5,    97,    16,     0,
    94,     5,    52,    85,    53,    16,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   151,   154,   157,   161,   164,   170,   171,   174,   179,   182,
   185,   188,   192,   200,   203,   206,   209,   212,   215,   218,
   221,   224,   227,   232,   233,   235,   235,   237,   237,   237,
   239,   239,   239,   242,   246,   252,   258,   261,   268,   276,
   280,   287,   291,   299,   303,   310,   313,   317,   321,   325,
   329,   333,   337,   341,   345,   349,   353,   357,   361,   365,
   371,   372,   373,   374,   377,   378,   381,   382,   385,   386,
   387,   388,   389,   392,   393,   396,   406,   411,   416,   420,
   427,   428,   429,   430,   431,   432,   433,   434,   435,   436,
   437,   438,   439,   440,   441,   444,   445,   448,   452,   453,
   461,   462,   463,   464,   465,   471,   481,   485,   492,   497,
   505,   510,   517,   522,   527,   532,   540,   544,   550,   559,
   567,   575,   578,   581,   587,   590,   593,   596,   599,   605,
   606,   609,   609,   609,   609,   612,   637,   644,   647,   648,
   651,   654,   655,   658,   662,   666,   670,   676,   682,   688,
   695,   701,   702,   703,   706,   707,   710,   713,   714,   716,
   719,   720,   721
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","T_CHARACTER_LITERAL",
"T_DOUBLE_LITERAL","T_IDENTIFIER","T_INTEGER_LITERAL","T_STRING_LITERAL","T_INCLUDE",
"T_CLASS","T_STRUCT","T_LEFT_CURLY_BRACKET","T_LEFT_PARANTHESIS","T_RIGHT_CURLY_BRACKET",
"T_RIGHT_PARANTHESIS","T_COLON","T_SEMICOLON","T_PUBLIC","T_PROTECTED","T_TRIPE_DOT",
"T_PRIVATE","T_VIRTUAL","T_CONST","T_INLINE","T_FRIEND","T_RETURN","T_SIGNAL",
"T_SLOT","T_TYPEDEF","T_PLUS","T_MINUS","T_COMMA","T_ASTERISK","T_TILDE","T_LESS",
"T_GREATER","T_AMPERSAND","T_EXTERN","T_EXTERN_C","T_ACCESS","T_ENUM","T_NAMESPACE",
"T_USING","T_UNKNOWN","T_TRIPLE_DOT","T_TRUE","T_FALSE","T_STATIC","T_EQUAL",
"T_SCOPE","T_NULL","T_INT","T_ARRAY_OPEN","T_ARRAY_CLOSE","T_CHAR","T_DCOP",
"T_DCOP_AREA","T_SIGNED","T_UNSIGNED","T_LONG","T_SHORT","T_FUNOPERATOR","T_MISCOPERATOR",
"main","includes","dcoptag","declaration","@1","member_list","bool_value","nodcop_area",
"sigslot","nodcop_area_begin","dcop_area_begin","Identifier","super_class_name",
"super_class","super_classes","class_header","opt_semicolon","body","enum","enum_list",
"enum_item","number","int_expression","typedef","const_qualifier","int_type",
"asterisks","params","type_name","templ_type_list","templ_type","type","type_list",
"param","default","default_value","virtual_qualifier","operator","function_header",
"argument","arguments","init_item","init_list","function","function_begin","function_body",
"function_lines","function_line","Identifier_list_rest","Identifier_list","member", NULL
};
#endif

static const short yyr1[] = {     0,
    63,    63,    64,    64,    64,    65,    65,    66,    66,    66,
    66,    67,    66,    66,    66,    66,    66,    66,    66,    66,
    66,    66,    66,    68,    68,    69,    69,    70,    70,    70,
    71,    71,    71,    72,    72,    73,    74,    74,    75,    76,
    76,    77,    77,    78,    78,    79,    79,    80,    80,    80,
    80,    80,    80,    80,    80,    80,    80,    80,    80,    80,
    81,    81,    81,    81,    82,    82,    83,    83,    84,    84,
    84,    84,    84,    85,    85,    86,    86,    86,    87,    87,
    88,    88,    88,    88,    88,    88,    88,    88,    88,    88,
    88,    88,    88,    88,    88,    89,    89,    90,    90,    90,
    91,    91,    91,    91,    91,    91,    92,    92,    93,    93,
    94,    94,    94,    94,    94,    94,    95,    95,    96,    96,
    96,    97,    97,    97,    98,    98,    98,    98,    98,    99,
    99,   100,   100,   100,   100,   101,   101,   102,   103,   103,
   104,   105,   105,   106,   106,   106,   106,   106,   106,   106,
   107,   108,   108,   108,   109,   109,   110,   111,   111,   112,
   113,   113,   113
};

static const short yyr2[] = {     0,
     3,     0,     2,     4,     0,     1,     0,     6,     3,     3,
     6,     0,     7,     4,     5,     2,     4,     7,     8,     2,
     1,     1,     1,     2,     0,     1,     1,     1,     1,     1,
     1,     1,     0,     3,     2,     2,     1,     3,     1,     3,
     1,     2,     3,     2,     1,     0,     1,     1,     2,     2,
     2,     2,     2,     2,     5,     4,     4,     4,     4,     6,
     7,     6,     6,     5,     3,     1,     3,     1,     1,     1,
     2,     1,     1,     1,     3,     7,     9,     6,     0,     1,
     1,     1,     2,     2,     3,     2,     2,     3,     2,     1,
     1,     1,     1,     2,     2,     2,     1,     0,     1,     3,
     1,     1,     2,     2,     4,     6,     3,     1,     2,     1,
     3,     3,     2,     2,     1,     2,     3,     1,     3,     2,
     1,     0,     2,     5,     1,     1,     1,     1,     4,     0,
     1,     1,     1,     1,     1,     6,     7,     1,     1,     3,
     4,     1,     3,     2,     5,     3,     5,     7,     6,     3,
     1,     1,     3,     4,     2,     0,     1,     2,     0,     2,
     3,     5,     6
};

static const short yydefact[] = {     5,
     5,     0,   130,     3,     5,    37,     0,     0,   131,     0,
   130,     0,     0,     0,     0,     0,     0,    90,    93,    81,
    82,    91,    92,     5,   102,    23,   101,   115,     0,     0,
     0,    21,    22,     0,     0,   104,   103,     0,     0,   102,
     0,     0,   113,     0,    20,     0,     0,    16,     0,     0,
     0,     0,     0,     0,     0,    94,    84,    83,    89,    95,
    87,    86,     1,    98,     0,    97,   114,   116,    37,     0,
     0,     0,     0,   151,   152,   156,   144,     4,    38,    45,
   130,     9,     7,     5,    10,   104,   103,     0,   146,   112,
   111,    25,   103,     0,     0,    68,     0,    66,    12,     0,
     0,    37,   150,    85,    88,   121,     0,   122,    99,   110,
     0,   108,    96,     0,     0,   160,   134,   133,   135,   132,
     0,    98,   161,     0,   157,     0,   156,   131,    39,    41,
     0,    44,     0,     6,    33,     0,     0,     0,     0,     0,
    25,    25,    17,     0,     0,     0,     0,     5,     0,    14,
     0,     0,     0,     0,   122,   120,   109,   105,     0,   159,
   158,    69,    70,     0,    72,    73,    74,     0,    98,     0,
     0,   153,   155,    42,   130,     0,     0,     0,    48,    30,
    29,    28,     0,    31,    32,     0,     0,     0,    33,     0,
    33,    33,     0,    33,    33,    33,    33,     0,   145,     0,
     0,   159,    24,     0,     0,    67,     0,    64,    65,     0,
    15,   127,   125,     0,    26,    27,   128,    73,   126,   123,
   162,     0,   147,   100,   119,     0,   107,    71,     0,     0,
     0,    79,     0,   154,    43,    40,   104,   103,     0,   102,
     0,     0,     0,    36,     0,    35,    53,    52,     8,    51,
    49,    50,    54,    11,   122,     0,     0,     0,    62,    63,
    46,     0,    98,     0,   142,     0,   106,    75,   163,    79,
    80,   136,   149,    33,     5,    33,   104,    33,    33,     0,
     0,    34,    18,     0,    61,    47,    13,     0,     0,     0,
     0,   148,   137,    58,     0,    59,    33,    56,    57,   118,
     0,     0,    19,   124,   129,   138,   139,     0,   143,     0,
    55,     0,     0,    33,     0,   141,    78,   117,     0,     0,
    60,   140,     0,    76,     0,    77,     0,     0,     0
};

static const short yydefgoto[] = {    34,
     3,   135,    24,   148,   139,   217,   189,   190,   191,   192,
    40,   130,   131,   132,    83,   287,   193,   194,    97,    98,
   167,   219,   195,   272,    27,    68,   107,    28,   111,   112,
    29,   301,   109,   152,   220,    30,   121,    31,   307,   308,
   265,   266,   196,    76,    77,   126,   127,   116,    72,   197
};

static const short yypact[] = {    99,
    90,    49,   209,-32768,    21,    -2,    97,    97,   375,    59,
   286,   405,   110,   179,   131,    52,   375,-32768,-32768,   175,
   161,-32768,-32768,   114,    89,-32768,-32768,   221,    10,   176,
    34,-32768,-32768,   144,    97,   154,   123,    97,    97,   145,
    12,    38,   223,   375,-32768,   202,    97,-32768,   197,   219,
   214,   178,   233,    13,    34,-32768,-32768,   188,-32768,-32768,
-32768,   189,-32768,   347,    59,   210,-32768,-32768,     9,   133,
   229,   231,    97,-32768,-32768,   236,-32768,-32768,-32768,-32768,
   157,-32768,   190,    21,-32768,-32768,-32768,   208,-32768,-32768,
-32768,   365,   250,   246,   219,   216,   257,   241,-32768,   269,
   261,   105,-32768,-32768,-32768,-32768,   124,    19,-32768,   210,
   244,   249,-32768,   276,    25,-32768,-32768,-32768,-32768,-32768,
   271,   347,-32768,   272,-32768,   273,   236,-32768,-32768,-32768,
    31,-32768,   275,-32768,   308,   277,    34,   375,   280,   283,
   365,   365,-32768,   284,    25,    36,   219,    21,   282,-32768,
    85,   285,   195,   347,   251,-32768,-32768,   240,    59,   274,
-32768,-32768,-32768,   294,-32768,-32768,   287,   253,   347,   127,
   288,   293,-32768,-32768,   157,    97,    97,    97,-32768,-32768,
-32768,-32768,   431,-32768,-32768,    97,   298,   289,   160,   300,
   308,   308,   303,   308,   308,   308,   308,   306,-32768,   318,
    97,    77,-32768,   311,    95,-32768,   315,-32768,-32768,   314,
-32768,-32768,-32768,   375,-32768,-32768,-32768,   326,-32768,-32768,
-32768,   334,-32768,-32768,-32768,    97,-32768,-32768,    25,   331,
   135,   320,    34,-32768,-32768,-32768,   333,   206,    97,    69,
   335,   319,   305,-32768,   343,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,   251,   344,    97,   345,-32768,-32768,
   355,   349,   347,   360,   342,    34,-32768,-32768,-32768,   320,
-32768,-32768,-32768,   308,    21,   308,   361,   308,   308,   375,
   371,-32768,-32768,   362,-32768,-32768,-32768,   198,   146,    25,
   334,-32768,-32768,-32768,   366,-32768,   308,-32768,-32768,   350,
   348,   370,-32768,-32768,-32768,-32768,   351,   374,-32768,   373,
-32768,   375,    16,   308,    25,-32768,-32768,-32768,   385,   376,
-32768,-32768,    97,-32768,   377,-32768,   394,   395,-32768
};

static const short yypgoto[] = {     3,
   398,-32768,-32768,-32768,    55,-32768,-32768,   207,-32768,-32768,
    -3,   224,-32768,   227,-32768,-32768,  -115,   400,   -75,-32768,
  -206,   -82,-32768,   138,-32768,   -27,  -116,    -9,   252,-32768,
     2,   101,   255,   -60,   129,   -69,-32768,    -7,-32768,   103,
-32768,   130,   122,-32768,   -33,   301,-32768,-32768,   316,     4
};


#define	YYLAST		491


static const short yytable[] = {    25,
    43,    42,   327,    36,    37,   170,    33,    25,    89,    55,
    41,   133,    41,    47,    69,    91,     6,   102,    54,   144,
     6,   103,   268,     6,  -159,    71,    63,   162,     1,     6,
   163,    79,   168,    -2,    86,    87,    55,    71,   113,   114,
   207,   174,    93,    94,    74,    41,    35,   156,    74,    75,
    71,   208,   231,    75,   164,   110,    52,    35,     2,     5,
   115,   175,   206,     6,   319,   108,   151,    38,    39,   124,
    70,   209,    70,    70,   165,   247,   248,   129,   250,   251,
   252,   253,   157,   306,   278,    88,   136,   162,   212,     6,
   163,   213,    53,   140,   225,   141,   214,     1,    -2,   258,
    64,     6,    65,   199,   155,   133,     1,   114,   306,    18,
   259,   166,    19,    -2,   164,    20,    21,    22,    23,   223,
  -122,     1,    65,   108,    32,    48,    -2,     2,   115,   215,
   216,    25,    45,    84,   165,    51,     2,   153,    85,   200,
   232,   166,   140,   140,   141,   141,   289,   218,   270,   110,
   210,     2,   151,    35,   154,   108,    78,   154,   294,   305,
   296,     6,   298,   299,    80,   154,   117,   118,    81,    82,
   108,   129,   129,   237,   238,   241,   154,   128,    65,   240,
   119,   311,   242,    49,    41,   184,   185,    25,    25,    50,
    25,    25,    25,    25,   120,   203,   204,   256,   321,   273,
   162,   212,     6,   163,   213,    74,     6,    95,    73,   222,
    75,    59,    92,     6,    60,   262,   275,     7,     8,    61,
    62,   276,   267,    96,    99,   166,   100,   164,    56,     9,
    10,    11,   292,    57,    58,   277,    12,   101,   104,   105,
   122,    66,   215,   216,   134,    13,   123,   165,    14,    15,
    16,   125,    66,   284,    66,    17,    67,   137,    90,    18,
   142,   143,    19,   145,   108,    20,    21,    22,    23,   146,
    25,   147,    25,   149,    25,    25,   150,   295,   158,   159,
   160,   300,   169,   171,   218,   172,   166,   202,   226,   198,
     6,   176,   201,    25,    38,    39,   205,   211,   151,   228,
   221,   233,   243,   244,   114,   230,     9,    10,   234,   320,
    25,   166,     6,   300,   246,   229,   177,   178,   249,   325,
   179,   254,   255,   257,   180,   181,   261,   182,     9,    10,
   260,   183,    44,   184,   185,   186,    18,   263,   264,    19,
  -130,   271,    20,    21,    22,    23,   269,    14,   274,   187,
   279,     6,   280,   281,    17,    38,    39,   282,    18,   283,
   285,    19,   288,   188,    20,    21,    22,    23,    10,     6,
   286,   290,   291,    38,    39,   302,   297,   303,   310,     6,
   312,   315,   313,    38,    39,   314,    10,   316,   317,   323,
   106,   324,   326,   328,   329,   245,    10,    18,     4,   236,
    19,   235,    26,    20,    21,    22,    23,   293,   224,     6,
   227,   138,   318,    38,    46,    18,   304,   322,    19,     0,
   309,    20,    21,    22,    23,    18,    10,   173,    19,   161,
     0,    20,    21,    22,    23,     6,     0,     0,     0,   239,
    39,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,    10,     0,     0,    18,     0,     0,    19,     0,
     0,    20,    21,    22,    23,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    18,     0,     0,    19,     0,     0,    20,    21,    22,
    23
};

static const short yycheck[] = {     3,
    10,     9,     0,     7,     8,   122,     3,    11,    42,    17,
     9,    81,    11,    12,     5,    43,     5,     5,    17,    95,
     5,    55,   229,     5,    16,    29,    24,     3,     8,     5,
     6,    35,   115,    13,    38,    39,    44,    41,    66,    31,
     5,    11,    46,    47,    11,    44,    49,   108,    11,    16,
    54,    16,   169,    16,    30,    65,     5,    49,    38,    11,
    52,    31,   145,     5,    49,    64,    48,     9,    10,    73,
    61,   147,    61,    61,    50,   191,   192,    81,   194,   195,
   196,   197,   110,   290,    16,    48,    84,     3,     4,     5,
     6,     7,    41,    92,   155,    92,    12,     8,     0,     5,
    12,     5,    34,   137,   108,   175,     8,    31,   315,    51,
    16,   115,    54,     0,    30,    57,    58,    59,    60,   153,
    16,     8,    34,   122,     3,    16,    13,    38,    52,    45,
    46,   135,    11,    11,    50,     5,    38,    14,    16,   138,
    14,   145,   141,   142,   141,   142,   263,   151,    14,   159,
   148,    38,    48,    49,    31,   154,    13,    31,   274,    14,
   276,     5,   278,   279,    11,    31,    34,    35,    15,    16,
   169,   175,   176,   177,   178,   183,    31,    21,    34,   183,
    48,   297,   186,     5,   183,    26,    27,   191,   192,    11,
   194,   195,   196,   197,    62,   141,   142,   201,   314,   233,
     3,     4,     5,     6,     7,    11,     5,    11,    33,    15,
    16,    51,    11,     5,    54,   214,    11,     9,    10,    59,
    60,    16,   226,     5,    11,   229,    49,    30,    54,    21,
    22,    23,   266,    59,    60,   239,    28,     5,    51,    51,
    12,    32,    45,    46,    55,    37,    16,    50,    40,    41,
    42,    16,    32,   257,    32,    47,    36,    50,    36,    51,
    11,    16,    54,    48,   263,    57,    58,    59,    60,    13,
   274,    31,   276,     5,   278,   279,    16,   275,    35,    31,
     5,   280,    12,    12,   288,    13,   290,     5,    49,    13,
     5,    17,    13,   297,     9,    10,    13,    16,    48,     6,
    16,    14,     5,    15,    31,    53,    21,    22,    16,   313,
   314,   315,     5,   312,    15,    29,     9,    10,    16,   323,
    13,    16,     5,    13,    17,    18,    13,    20,    21,    22,
    16,    24,    47,    26,    27,    28,    51,    12,     5,    54,
    33,    22,    57,    58,    59,    60,    16,    40,    16,    42,
    16,     5,    34,    49,    47,     9,    10,    15,    51,    16,
    16,    54,    14,    56,    57,    58,    59,    60,    22,     5,
    16,    12,    31,     9,    10,     5,    16,    16,    13,     5,
    31,    31,    35,     9,    10,    16,    22,    14,    16,     5,
    44,    16,    16,     0,     0,   189,    22,    51,     1,   176,
    54,   175,     3,    57,    58,    59,    60,   270,   154,     5,
   159,    47,   312,     9,    10,    51,   288,   315,    54,    -1,
   291,    57,    58,    59,    60,    51,    22,   127,    54,   114,
    -1,    57,    58,    59,    60,     5,    -1,    -1,    -1,     9,
    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    22,    -1,    -1,    51,    -1,    -1,    54,    -1,
    -1,    57,    58,    59,    60,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    51,    -1,    -1,    54,    -1,    -1,    57,    58,    59,
    60
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "//usr/lib/bison.simple"
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

#line 217 "//usr/lib/bison.simple"

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

case 1:
#line 152 "yacc.yy"
{
	  ;
    break;}
case 3:
#line 158 "yacc.yy"
{
		printf("<INCLUDE>%s</INCLUDE>\n", yyvsp[-1]._str->latin1() );
	  ;
    break;}
case 4:
#line 162 "yacc.yy"
{
	  ;
    break;}
case 5:
#line 165 "yacc.yy"
{
          ;
    break;}
case 6:
#line 170 "yacc.yy"
{ yyval._int = 1; ;
    break;}
case 7:
#line 171 "yacc.yy"
{ yyval._int = 0; ;
    break;}
case 8:
#line 175 "yacc.yy"
{
	 	if (yyvsp[-2]._int)
			  printf("<CLASS>\n    <NAME>%s</NAME>\n%s%s</CLASS>\n", ( in_namespace + *yyvsp[-4]._str ).latin1(), yyvsp[-3]._str->latin1(), yyvsp[-1]._str->latin1() );
	  ;
    break;}
case 9:
#line 180 "yacc.yy"
{
	  ;
    break;}
case 10:
#line 183 "yacc.yy"
{
	  ;
    break;}
case 11:
#line 186 "yacc.yy"
{
	  ;
    break;}
case 12:
#line 189 "yacc.yy"
{
                      in_namespace += *yyvsp[-1]._str; in_namespace += "::";
                  ;
    break;}
case 13:
#line 193 "yacc.yy"
{
                      int pos = in_namespace.findRev( "::", -3 );
                      if( pos >= 0 )
                          in_namespace = in_namespace.left( pos + 2 );
                      else
                          in_namespace = "";
                  ;
    break;}
case 14:
#line 201 "yacc.yy"
{
          ;
    break;}
case 15:
#line 204 "yacc.yy"
{
          ;
    break;}
case 16:
#line 207 "yacc.yy"
{
	  ;
    break;}
case 17:
#line 210 "yacc.yy"
{
	  ;
    break;}
case 18:
#line 213 "yacc.yy"
{
	  ;
    break;}
case 19:
#line 216 "yacc.yy"
{
	  ;
    break;}
case 20:
#line 219 "yacc.yy"
{
	  ;
    break;}
case 21:
#line 222 "yacc.yy"
{
	  ;
    break;}
case 22:
#line 225 "yacc.yy"
{
	  ;
    break;}
case 23:
#line 228 "yacc.yy"
{
	  ;
    break;}
case 34:
#line 243 "yacc.yy"
{
	  dcop_area = 0;
	;
    break;}
case 35:
#line 247 "yacc.yy"
{
	  dcop_area = 0;
	;
    break;}
case 36:
#line 253 "yacc.yy"
{
	  dcop_area = 1;
	;
    break;}
case 37:
#line 258 "yacc.yy"
{
	  yyval._str = yyvsp[0]._str;
	;
    break;}
case 38:
#line 261 "yacc.yy"
{
	   QString* tmp = new QString( "%1::%2" );
           *tmp = tmp->arg(*(yyvsp[-2]._str)).arg(*(yyvsp[0]._str));
           yyval._str = tmp;
	;
    break;}
case 39:
#line 269 "yacc.yy"
{
		QString* tmp = new QString( "    <SUPER>%1</SUPER>\n" );
		*tmp = tmp->arg( *(yyvsp[0]._str) );
		yyval._str = tmp;
	  ;
    break;}
case 40:
#line 277 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 41:
#line 281 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 42:
#line 288 "yacc.yy"
{
		yyval._str = yyvsp[-1]._str;
	  ;
    break;}
case 43:
#line 292 "yacc.yy"
{
		/* $$ = $1; */
		yyval._str = new QString( *(yyvsp[-2]._str) + *(yyvsp[0]._str) );
	  ;
    break;}
case 44:
#line 300 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 45:
#line 304 "yacc.yy"
{
		yyval._str = new QString( "" );
	  ;
    break;}
case 46:
#line 311 "yacc.yy"
{
          ;
    break;}
case 48:
#line 318 "yacc.yy"
{
		yyval._str = new QString( "" );
	  ;
    break;}
case 49:
#line 322 "yacc.yy"
{
		yyval._str = new QString( *(yyvsp[-1]._str) + *(yyvsp[0]._str) );
	  ;
    break;}
case 50:
#line 326 "yacc.yy"
{
		yyval._str = new QString( *(yyvsp[-1]._str) + *(yyvsp[0]._str) );
	  ;
    break;}
case 51:
#line 330 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 52:
#line 334 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 53:
#line 338 "yacc.yy"
{	
	        yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 54:
#line 342 "yacc.yy"
{
 	        yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 55:
#line 346 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 56:
#line 350 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 57:
#line 354 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 58:
#line 358 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 59:
#line 362 "yacc.yy"
{
		yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 60:
#line 366 "yacc.yy"
{
                yyval._str = yyvsp[0]._str;
          ;
    break;}
case 67:
#line 381 "yacc.yy"
{;
    break;}
case 68:
#line 382 "yacc.yy"
{;
    break;}
case 69:
#line 385 "yacc.yy"
{;
    break;}
case 70:
#line 386 "yacc.yy"
{;
    break;}
case 71:
#line 387 "yacc.yy"
{;
    break;}
case 72:
#line 388 "yacc.yy"
{;
    break;}
case 73:
#line 389 "yacc.yy"
{;
    break;}
case 74:
#line 392 "yacc.yy"
{;
    break;}
case 75:
#line 393 "yacc.yy"
{;
    break;}
case 76:
#line 397 "yacc.yy"
{
		if (dcop_area) {
 		  QString* tmp = new QString("<TYPEDEF name=\"%1\" template=\"%2\"><PARAM %3</TYPEDEF>\n");
		  *tmp = tmp->arg( *(yyvsp[-1]._str) ).arg( *(yyvsp[-5]._str) ).arg( *(yyvsp[-3]._str) );
		  yyval._str = tmp;
		} else {
		  yyval._str = new QString("");
		}
	  ;
    break;}
case 77:
#line 407 "yacc.yy"
{
		if (dcop_area)
		  yyerror("scoped template typedefs are not supported in dcop areas!");
	  ;
    break;}
case 78:
#line 412 "yacc.yy"
{
	  ;
    break;}
case 79:
#line 417 "yacc.yy"
{
		yyval._int = 0;
	  ;
    break;}
case 80:
#line 421 "yacc.yy"
{
		yyval._int = 1;
	  ;
    break;}
case 81:
#line 427 "yacc.yy"
{ yyval._str = new QString("signed int"); ;
    break;}
case 82:
#line 428 "yacc.yy"
{ yyval._str = new QString("unsigned int"); ;
    break;}
case 83:
#line 429 "yacc.yy"
{ yyval._str = new QString("signed short int"); ;
    break;}
case 84:
#line 430 "yacc.yy"
{ yyval._str = new QString("signed long int"); ;
    break;}
case 85:
#line 431 "yacc.yy"
{ yyval._str = new QString("signed short int"); ;
    break;}
case 86:
#line 432 "yacc.yy"
{ yyval._str = new QString("unsigned short int"); ;
    break;}
case 87:
#line 433 "yacc.yy"
{ yyval._str = new QString("unsigned long int"); ;
    break;}
case 88:
#line 434 "yacc.yy"
{ yyval._str = new QString("unsigned short int"); ;
    break;}
case 89:
#line 435 "yacc.yy"
{ yyval._str = new QString("unsigned int"); ;
    break;}
case 90:
#line 436 "yacc.yy"
{ yyval._str = new QString("int"); ;
    break;}
case 91:
#line 437 "yacc.yy"
{ yyval._str = new QString("long int"); ;
    break;}
case 92:
#line 438 "yacc.yy"
{ yyval._str = new QString("short int"); ;
    break;}
case 93:
#line 439 "yacc.yy"
{ yyval._str = new QString("char"); ;
    break;}
case 94:
#line 440 "yacc.yy"
{ yyval._str = new QString("signed char"); ;
    break;}
case 95:
#line 441 "yacc.yy"
{ yyval._str = new QString("unsigned char"); ;
    break;}
case 98:
#line 449 "yacc.yy"
{
		yyval._str = new QString( "" );
	  ;
    break;}
case 100:
#line 454 "yacc.yy"
{
		yyval._str = new QString( *(yyvsp[-2]._str) + *(yyvsp[0]._str) );
	  ;
    break;}
case 102:
#line 462 "yacc.yy"
{ yyval._str = yyvsp[0]._str; ;
    break;}
case 103:
#line 463 "yacc.yy"
{ yyval._str = yyvsp[0]._str; ;
    break;}
case 104:
#line 464 "yacc.yy"
{ yyval._str = yyvsp[0]._str; ;
    break;}
case 105:
#line 465 "yacc.yy"
{
		QString *tmp = new QString("%1&lt;%2&gt;");
		*tmp = tmp->arg(*(yyvsp[-3]._str));
		*tmp = tmp->arg(*(yyvsp[-1]._str));
		yyval._str = tmp;
	 ;
    break;}
case 106:
#line 471 "yacc.yy"
{
		QString *tmp = new QString("%1&lt;%2&gt;::%3");
		*tmp = tmp->arg(*(yyvsp[-5]._str));
		*tmp = tmp->arg(*(yyvsp[-3]._str));
		*tmp = tmp->arg(*(yyvsp[0]._str));
		yyval._str = tmp;
	 ;
    break;}
case 107:
#line 482 "yacc.yy"
{
	    yyval._str = new QString(*(yyvsp[-2]._str) + "," + *(yyvsp[0]._str));
	  ;
    break;}
case 108:
#line 486 "yacc.yy"
{
 	    yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 109:
#line 493 "yacc.yy"
{
	    if (dcop_area)
	      yyerror("in dcop areas are no pointers allowed");
	  ;
    break;}
case 110:
#line 498 "yacc.yy"
{
 	    yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 111:
#line 506 "yacc.yy"
{
	    if (dcop_area)
	      yyerror("in dcop areas are no pointers allowed");
	  ;
    break;}
case 112:
#line 510 "yacc.yy"
{
	     if (dcop_area) {
	  	QString* tmp = new QString("<TYPE  qleft=\"const\" qright=\"" AMP_ENTITY "\">%1</TYPE>");
		*tmp = tmp->arg( *(yyvsp[-1]._str) );
		yyval._str = tmp;
	     }
	  ;
    break;}
case 113:
#line 517 "yacc.yy"
{
		QString* tmp = new QString("<TYPE>%1</TYPE>");
		*tmp = tmp->arg( *(yyvsp[0]._str) );
		yyval._str = tmp;
	;
    break;}
case 114:
#line 522 "yacc.yy"
{
	     if (dcop_area)
		yyerror("in dcop areas are only const references allowed!");
	  ;
    break;}
case 115:
#line 527 "yacc.yy"
{
		QString* tmp = new QString("<TYPE>%1</TYPE>");
		*tmp = tmp->arg( *(yyvsp[0]._str) );
		yyval._str = tmp;
	;
    break;}
case 116:
#line 533 "yacc.yy"
{
	    if (dcop_area)
	      yyerror("in dcop areas are no pointers allowed");
	  ;
    break;}
case 117:
#line 541 "yacc.yy"
{
	    yyval._str = new QString(*(yyvsp[-2]._str) + "," + *(yyvsp[0]._str));
	  ;
    break;}
case 118:
#line 545 "yacc.yy"
{
 	    yyval._str = yyvsp[0]._str;
	  ;
    break;}
case 119:
#line 551 "yacc.yy"
{
		if (dcop_area) {
		   QString* tmp = new QString("\n        <ARG>%1<NAME>%2</NAME></ARG>");
  		   *tmp = tmp->arg( *(yyvsp[-2]._str) );
  		   *tmp = tmp->arg( *(yyvsp[-1]._str) );
		   yyval._str = tmp;		
		} else yyval._str = new QString();
	  ;
    break;}
case 120:
#line 560 "yacc.yy"
{
		if (dcop_area) {
		   QString* tmp = new QString("\n        <ARG>%1</ARG>");
  		   *tmp = tmp->arg( *(yyvsp[-1]._str) );
		   yyval._str = tmp;		
		} else yyval._str = new QString();
	  ;
    break;}
case 121:
#line 568 "yacc.yy"
{
		if (dcop_area)
			yyerror("variable arguments not supported in dcop area.");
		yyval._str = new QString("");
	  ;
    break;}
case 122:
#line 576 "yacc.yy"
{
	  ;
    break;}
case 123:
#line 579 "yacc.yy"
{
	  ;
    break;}
case 124:
#line 582 "yacc.yy"
{
	  ;
    break;}
case 125:
#line 588 "yacc.yy"
{
          ;
    break;}
case 126:
#line 591 "yacc.yy"
{
          ;
    break;}
case 127:
#line 594 "yacc.yy"
{
          ;
    break;}
case 128:
#line 597 "yacc.yy"
{
          ;
    break;}
case 129:
#line 600 "yacc.yy"
{
          ;
    break;}
case 130:
#line 605 "yacc.yy"
{ yyval._int = 0; ;
    break;}
case 131:
#line 606 "yacc.yy"
{ yyval._int = 1; ;
    break;}
case 136:
#line 613 "yacc.yy"
{
	     if (dcop_area) {
		QString* tmp = 0;
		if ( yyvsp[0]._int )
			tmp = new QString(
				"    <FUNC qual=\"const\">\n"
				"        %2\n"
				"        <NAME>%1</NAME>"
				"%3\n"
				"     </FUNC>\n");
		else
			tmp = new QString(
				"    <FUNC>\n"
				"        %2\n"
				"        <NAME>%1</NAME>"
				"%3\n"
				"     </FUNC>\n");
		*tmp = tmp->arg( *(yyvsp[-4]._str) );
		*tmp = tmp->arg( *(yyvsp[-5]._str) );
		*tmp = tmp->arg( *(yyvsp[-2]._str) );
		yyval._str = tmp;
   	     } else
	        yyval._str = new QString("");
	  ;
    break;}
case 137:
#line 638 "yacc.yy"
{
	     if (dcop_area)
		yyerror("operators aren't allowed in dcop areas!");
	     yyval._str = new QString("");
	  ;
    break;}
case 138:
#line 644 "yacc.yy"
{;
    break;}
case 139:
#line 647 "yacc.yy"
{;
    break;}
case 140:
#line 648 "yacc.yy"
{;
    break;}
case 141:
#line 651 "yacc.yy"
{;
    break;}
case 142:
#line 654 "yacc.yy"
{;
    break;}
case 143:
#line 655 "yacc.yy"
{;
    break;}
case 144:
#line 659 "yacc.yy"
{
	        yyval._str = yyvsp[-1]._str;
	  ;
    break;}
case 145:
#line 663 "yacc.yy"
{
		yyval._str = yyvsp[-3]._str;
	  ;
    break;}
case 146:
#line 667 "yacc.yy"
{
		yyval._str = yyvsp[-1]._str;
	  ;
    break;}
case 147:
#line 671 "yacc.yy"
{
	      /* The constructor */
	      assert(!dcop_area);
              yyval._str = new QString("");
	  ;
    break;}
case 148:
#line 677 "yacc.yy"
{
	      /* The constructor */
	      assert(!dcop_area);
              yyval._str = new QString("");
	  ;
    break;}
case 149:
#line 683 "yacc.yy"
{
	      /* The destructor */
  	      assert(!dcop_area);
              yyval._str = new QString("");
	  ;
    break;}
case 150:
#line 689 "yacc.yy"
{
		if (dcop_area)
		  yyerror("static is not allowed in dcop area!");
		yyval._str = new QString();
	  ;
    break;}
case 151:
#line 696 "yacc.yy"
{
		function_mode = 1;
	;
    break;}
case 155:
#line 706 "yacc.yy"
{;
    break;}
case 156:
#line 707 "yacc.yy"
{;
    break;}
case 160:
#line 716 "yacc.yy"
{;
    break;}
case 161:
#line 719 "yacc.yy"
{;
    break;}
case 162:
#line 720 "yacc.yy"
{;
    break;}
case 163:
#line 721 "yacc.yy"
{;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "//usr/lib/bison.simple"

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
#line 723 "yacc.yy"


void dcopidlParse( const char *_code )
{
    dcopidlInitFlex( _code );
    yyparse();
}
