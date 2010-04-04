%{
/***************************************
 $Header: /home/amb/CVS/routino/src/xmlparse.l,v 1.4 2010-04-04 14:29:34 amb Exp $

 A simple generic XML parser where the structure comes from the function parameters.
 Not intended to be fully conforming to XML staandard or a validating parser but
 sufficient to parse OSM XML and simple program configuration files.

 Part of the Routino routing software.
 ******************/ /******************
 This file Copyright 2010 Andrew M. Bishop

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Parser outputs */

#define LEX_EOF                    0

#define LEX_TAG_BEGIN              1
#define LEX_XML_DECL_BEGIN         2
#define LEX_TAG_POP                3
#define LEX_TAG_PUSH               4
#define LEX_XML_DECL_FINISH        6
#define LEX_TAG_FINISH             7
#define LEX_ATTR_KEY               8
#define LEX_ATTR_VAL               9

#define LEX_ERROR                100

#define LEX_ERROR_TAG_START      101
#define LEX_ERROR_XML_DECL_START 102
#define LEX_ERROR_TAG            103
#define LEX_ERROR_XML_DECL       104
#define LEX_ERROR_ATTR           105
#define LEX_ERROR_END_TAG        106
#define LEX_ERROR_COMMENT        107
#define LEX_ERROR_CLOSE          108

#define LEX_ERROR_UNEXP_TAG      201
#define LEX_ERROR_UNBALANCED     202
#define LEX_ERROR_NO_START       203
#define LEX_ERROR_UNEXP_ATT      204
#define LEX_ERROR_UNEXP_EOF      205
#define LEX_ERROR_XML_NOT_FIRST  206


/* Lexer definitions */

#define YY_SKIP_YYWRAP 1 /* Remove error with prototype of ..._yywrap */
#ifndef yywrap
/*+ Needed in lex but does nothing. +*/
#define yywrap() 1
#endif

/*+ Reset the current string. +*/
#define reset_string \
 if(string) *string=0; \
 stringused=0;

/*+ append information to the current string. +*/
#define append_string(xx) \
 newlen=strlen(xx); \
 if((stringused+newlen)>=stringlen) \
    string=(char*)realloc((void*)string,stringlen=(stringused+newlen+16)); \
 strcpy(string+stringused,xx); \
 stringused+=newlen;

#define YY_NO_INPUT


/* Lexer functions and variables */

extern int yylex(void);

static char *yylval=NULL;

%}

%option 8bit
%option pointer
%option batch
%option yylineno

%option nodefault
%option perf-report
%option fast
%option nounput


 /* Grammar based on http://www.w3.org/TR/2004/REC-xml-20040204/ but for ASCII not Unicode. */

S               [ \t\r\n]

letter          [a-zA-Z]
digit           [0-9]
xdigit          [a-fA-F0-9]

namechar        ({letter}|{digit}|[-._:])
name            ({letter}|[_:]){namechar}*

entityref       &{name};
charref         &#({digit}+|x{xdigit}+);


%x COMMENT
%x CDATA
%x DOCTYPE
%x XML_DECL_START XML_DECL
%x TAG_START TAG
%x ATTR_KEY ATTR_VAL
%x END_TAG1 END_TAG2
%x DQUOTED SQUOTED

%%
 /* Must use static variables since the parser returns often. */
 static char *string=NULL;
 static int stringlen=0,stringused=0;
 static int after_attr=0;
 int newlen;
 int doctype_depth=0;

 /* Handle top level entities */

"<!--"                      { BEGIN(COMMENT); }
"<![CDATA["                 { BEGIN(CDATA); }
"<!DOCTYPE"                 { BEGIN(DOCTYPE); doctype_depth=0; }
"</"                        { BEGIN(END_TAG1); }
"<?"                        { BEGIN(XML_DECL_START); }
"<"                         { BEGIN(TAG_START); }
">"                         { return(LEX_ERROR_CLOSE); }
[^<>]+                      { }

 /* Comments */

<COMMENT>"--->"             { return(LEX_ERROR_COMMENT); }
<COMMENT>"-->"              { BEGIN(INITIAL); }
<COMMENT>"--"[^->]+         { }
<COMMENT>[^-]+              { }
<COMMENT>"-"                { }

 /* CDATA */

<CDATA>"]]>"                { BEGIN(INITIAL); }
<CDATA>"]"                  { }
<CDATA>[^]]+                { }

 /* CDATA */

<DOCTYPE>"<"                { doctype_depth++; }
<DOCTYPE>">"                { if(doctype_depth==0) BEGIN(INITIAL); else doctype_depth--; }
<DOCTYPE>[^<>]+             { }

 /* XML Declaration start */

<XML_DECL_START>{name}      { BEGIN(XML_DECL); yylval=yytext; return(LEX_XML_DECL_BEGIN); }
<XML_DECL_START>.|\n        { return(LEX_ERROR_XML_DECL_START); }

 /* Tag middle */

<XML_DECL>"?>"              { BEGIN(INITIAL); return(LEX_XML_DECL_FINISH); }
<XML_DECL>{S}+              { }
<XML_DECL>{name}            { after_attr=XML_DECL; BEGIN(ATTR_KEY); yylval=yytext; return(LEX_ATTR_KEY); }
<XML_DECL>.|\n              { return(LEX_ERROR_XML_DECL); }

 /* Any tag start */

<TAG_START>{name}           { BEGIN(TAG); yylval=yytext; return(LEX_TAG_BEGIN); }
<TAG_START>.|\n             { return(LEX_ERROR_TAG_START); }

 /* End-tag start */

<END_TAG1>{name}            { BEGIN(END_TAG2); yylval=yytext; return(LEX_TAG_POP); }
<END_TAG1>.|\n              { return(LEX_ERROR_END_TAG); }

<END_TAG2>">"               { BEGIN(INITIAL); }
<END_TAG2>.|\n              { return(LEX_ERROR_END_TAG); }

 /* Any tag middle */

<TAG>"/>"                   { BEGIN(INITIAL); return(LEX_TAG_FINISH); }
<TAG>">"                    { BEGIN(INITIAL); return(LEX_TAG_PUSH); }
<TAG>{S}+                   { }
<TAG>{name}                 { after_attr=TAG; BEGIN(ATTR_KEY); yylval=yytext; return(LEX_ATTR_KEY); }
<TAG>.|\n                   { return(LEX_ERROR_TAG); }

 /* Attributes */

<ATTR_KEY>=                 { BEGIN(ATTR_VAL); }
<ATTR_KEY>.|\n              { return(LEX_ERROR_ATTR); }

<ATTR_VAL>\"                { BEGIN(DQUOTED); reset_string; }
<ATTR_VAL>\'                { BEGIN(SQUOTED); reset_string; }
<ATTR_VAL>.|\n              { return(LEX_ERROR_ATTR); }

 /* Quoted strings */

<DQUOTED>\"                 { BEGIN(after_attr); yylval=string; return(LEX_ATTR_VAL); }
<DQUOTED>{entityref}        { append_string(yytext); }
<DQUOTED>{charref}          { append_string(yytext); }
<DQUOTED>[<>&]              { return(LEX_ERROR_TAG); }
<DQUOTED>[^<>&\"]+          { append_string(yytext); }

<SQUOTED>\'                 { BEGIN(after_attr); yylval=string; return(LEX_ATTR_VAL); }
<SQUOTED>{entityref}        { append_string(yytext); }
<SQUOTED>{charref}          { append_string(yytext); }
<SQUOTED>[<>&]              { return(LEX_ERROR_TAG); }
<SQUOTED>[^<>&\']+          { append_string(yytext); }

 /* End of file */

<<EOF>>                     { free(string); string=NULL; BEGIN(INITIAL); return(LEX_EOF); }

%%

#include "xmlparse.h"


/*++++++++++++++++++++++++++++++++++++++
  A function to call the callback function with the parameters needed.

  char *name The name of the tag.

  void (*callback)() The callback function.

  int type The type of tag (start and/or end).

  int nattributes The number of attributes collected.

  char *attributes[XMLPARSE_MAX_ATTRS] The list of attributes.
  ++++++++++++++++++++++++++++++++++++++*/

static inline void call_callback(char *name,void (*callback)(),int type,int nattributes,char *attributes[XMLPARSE_MAX_ATTRS])
{
 switch(nattributes)
   {
   case  0: (*callback)(type); break;
   case  1: (*callback)(type,attributes[0]); break;
   case  2: (*callback)(type,attributes[0],attributes[1]); break;
   case  3: (*callback)(type,attributes[0],attributes[1],attributes[2]); break;
   case  4: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3]); break;
   case  5: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4]); break;
   case  6: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5]); break;
   case  7: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6]); break;
   case  8: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6],attributes[7]); break;
   case  9: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6],attributes[7],attributes[8]); break;
   case 10: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6],attributes[7],attributes[8],attributes[9]); break;
   case 11: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6],attributes[7],attributes[8],attributes[9],attributes[10]); break;
   case 12: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6],attributes[7],attributes[8],attributes[9],attributes[10],attributes[11]); break;
   case 13: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6],attributes[7],attributes[8],attributes[9],attributes[10],attributes[11],attributes[12]); break;
   case 14: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6],attributes[7],attributes[8],attributes[9],attributes[10],attributes[11],attributes[12],attributes[13]); break;
   case 15: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6],attributes[7],attributes[8],attributes[9],attributes[10],attributes[11],attributes[12],attributes[13],attributes[14]); break;
   case 16: (*callback)(type,attributes[0],attributes[1],attributes[2],attributes[3],attributes[4],attributes[5],attributes[6],attributes[7],attributes[8],attributes[9],attributes[10],attributes[11],attributes[12],attributes[13],attributes[14],attributes[15]); break;

   default:
    fprintf(stderr,"XML Parser: Error on line %d: too many attributes for tag '%s'.\n",yylineno,name);
    exit(1);
   }
}


/*++++++++++++++++++++++++++++++++++++++
  Parse the XML and call the functions for each tag as seen.

  int ParseXML Returns 0 if OK or something else in case of an error.

  FILE *file The file to parse.

  xmltag **tags The array of pointers to tags for the top level.

  int ignore_unknown_attributes If set to 0 then exit if unknown attribute is seen, if sete to 1 then warn, if set to 2 then ignore.
  ++++++++++++++++++++++++++++++++++++++*/

int ParseXML(FILE *file,xmltag **tags,int ignore_unknown_attributes)
{
 int yychar,i;

 int nattributes=0;
 char *attributes[XMLPARSE_MAX_ATTRS];
 int attribute=0;

 int stackdepth=0,stackused=0;
 xmltag ***tags_stack=NULL;
 xmltag **tag_stack=NULL;
 xmltag *tag=NULL;

 static int first=1;

 /* Parser (re)-initialisation */

 yyin=file;

 if(!first)
    yyrestart(NULL);

 first=0;

 /* The actual parser. */

 do
   {
    yychar=yylex();

    switch(yychar)
      {
       /* The start of a tag for an XML declaration */

      case LEX_XML_DECL_BEGIN:

       if(tag_stack)
         {
          fprintf(stderr,"XML Parser: Error on line %d: XML declaration not before all other tags.\n",yylineno);
          yychar=LEX_ERROR_XML_NOT_FIRST;
          break;
         }

       /* The start of a tag for an element */

      case LEX_TAG_BEGIN:

       tag=NULL;

       for(i=0;tags[i];i++)
          if(!strcasecmp(yylval,tags[i]->name))
            {
             tag=tags[i];

             for(i=0;i<nattributes;i++)
                free(attributes[i]);

             for(i=0;i<XMLPARSE_MAX_ATTRS;i++)
                if(!tag->attributes[i])
                   break;

             nattributes=i;

             for(i=0;i<nattributes;i++)
                attributes[i]=NULL;

             break;
            }

       if(tag==NULL)
         {
          fprintf(stderr,"XML Parser: Error on line %d: unexpected tag '%s'.\n",yylineno,yylval);
          yychar=LEX_ERROR_UNEXP_TAG;
         }

       break;

       /* The end of the start-tag for an element */

      case LEX_TAG_PUSH:

       if(stackused==stackdepth)
         {
          tag_stack =(xmltag**) realloc((void*)tag_stack ,(stackdepth+=8)*sizeof(xmltag*));
          tags_stack=(xmltag***)realloc((void*)tags_stack,(stackdepth+=8)*sizeof(xmltag**));
         }

       tag_stack [stackused]=tag;
       tags_stack[stackused]=tags;
       stackused++;

       if(tag->callback)
          call_callback(tag->name,tag->callback,XMLPARSE_TAG_START,nattributes,attributes);

       tags=tag->subtags;

       break;

       /* The end of the empty-element-tag for an XML declaration */

      case LEX_XML_DECL_FINISH:

       /* The end of the empty-element-tag for an element */

      case LEX_TAG_FINISH:

       if(tag->callback)
          call_callback(tag->name,tag->callback,XMLPARSE_TAG_START|XMLPARSE_TAG_END,nattributes,attributes);

       if(stackused>0)
          tag=tag_stack[stackused-1];
       else
          tag=NULL;

       break;

       /* The end of the end-tag for an element */

      case LEX_TAG_POP:

       stackused--;
       tags=tags_stack[stackused];
       tag =tag_stack [stackused];

       if(strcmp(tag->name,yylval))
         {
          fprintf(stderr,"XML Parser: Error on line %d: end tag '</%s>' doesn't match start tag '<%s ...>'.\n",yylineno,yylval,tag->name);
          yychar=LEX_ERROR_UNBALANCED;
         }

       if(stackused<0)
         {
          fprintf(stderr,"XML Parser: Error on line %d: end tag '</%s>' seen but there was no start tag '<%s ...>'.\n",yylineno,yylval,yylval);
          yychar=LEX_ERROR_NO_START;
         }

       if(tag->callback)
          call_callback(tag->name,tag->callback,XMLPARSE_TAG_END,nattributes,attributes);

       if(stackused>0)
          tag=tag_stack[stackused-1];
       else
          tag=NULL;

       break;

       /* An attribute key */

      case LEX_ATTR_KEY:

       attribute=-1;

       for(i=0;i<nattributes;i++)
          if(!strcasecmp(yylval,tag->attributes[i]))
            {
             attribute=i;

             break;
            }

       if(attribute==-1)
         {
          if(ignore_unknown_attributes==0)
            {
             fprintf(stderr,"XML Parser: Error on line %d: unexpected attribute '%s' for tag '%s'.\n",yylineno,yylval,tag->name);
             yychar=LEX_ERROR_UNEXP_ATT;
            }
          else if(ignore_unknown_attributes==1)
             fprintf(stderr,"XML Parser: Warning on line %d: unexpected attribute '%s' for tag '%s'.\n",yylineno,yylval,tag->name);
         }

       break;

       /* An attribute value */

      case LEX_ATTR_VAL:

       if(yylval && attribute!=-1)
          attributes[attribute]=strcpy(malloc(strlen(yylval)+1),yylval);

       break;

       /* End of file */

      case LEX_EOF:

       if(tag)
         {
          fprintf(stderr,"XML Parser: Error on line %d: end of file seen without end tag '</%s>'.\n",yylineno,tag->name);
          yychar=LEX_ERROR_UNEXP_EOF;
         }

       break;

      case LEX_ERROR_TAG_START:
       fprintf(stderr,"XML Parser: Error on line %d: character '<' seen not at start of tag.\n",yylineno);
       break;

      case LEX_ERROR_XML_DECL_START:
       fprintf(stderr,"XML Parser: Error on line %d: characters '<?' seen not at start of XML declaration.\n",yylineno);
       break;

      case LEX_ERROR_TAG:
       fprintf(stderr,"XML Parser: Error on line %d: invalid character seen inside tag '<%s...>'.\n",yylineno,tag->name);
       break;

      case LEX_ERROR_XML_DECL:
       fprintf(stderr,"XML Parser: Error on line %d: invalid character seen inside XML declaration '<?%s...>'.\n",yylineno,tag->name);
       break;

      case LEX_ERROR_ATTR:
       fprintf(stderr,"XML Parser: Error on line %d: invalid attribute definition seen in tag.\n",yylineno);
       break;

      case LEX_ERROR_END_TAG:
       fprintf(stderr,"XML Parser: Error on line %d: invalid character seen in end-tag.\n",yylineno);
       break;

      case LEX_ERROR_COMMENT:
       fprintf(stderr,"XML Parser: Error on line %d: invalid comment seen.\n",yylineno);
       break;

      case LEX_ERROR_CLOSE:
       fprintf(stderr,"XML Parser: Error on line %d: character '>' seen not at end of tag.\n",yylineno);
       break;
      }
   }
 while(yychar>LEX_EOF && yychar<LEX_ERROR);

 /* Delete the tagdata */

 for(i=0;i<nattributes;i++)
    if(attributes[i])
       free(attributes[i]);

 if(stackdepth)
    free(tags_stack);

 return(yychar);
}


/*++++++++++++++++++++++++++++++++++++++
  Return the current parser line number.

  int ParseXML_LineNumber Returns the line number.
  ++++++++++++++++++++++++++++++++++++++*/

int ParseXML_LineNumber(void)
{
 return(yylineno);
}