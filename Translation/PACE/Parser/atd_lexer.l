%option noyywrap
%option prefix="atd_"
%{
    #include <cstdlib>
    #include <vector>
    #include "../../../TreeAutomaton/InstructiveTreeDecomposition.h"
    #include "atd_parser.hpp"
    #define yylval atd_lval
    extern int yyparse(InstructiveTreeDecomposition &atd, int &result);

%}

%option yylineno

%%

[0-9]+                      yylval.number = atoi(yytext);   return atd_NUM;
Leaf                        yylval.string = strdup(yytext); return atd_LEAF;
IntroVertex                 yylval.string = strdup(yytext); return atd_INTROVERTEX;
IntroEdge                   yylval.string = strdup(yytext); return atd_INTROEDGE;
ForgetVertex                yylval.string = strdup(yytext); return atd_FORGETVERTEX;
Join                        yylval.string = strdup(yytext); return atd_JOIN;
"("                         yylval.string = strdup(yytext); return atd_LEFTP;
")"                         yylval.string = strdup(yytext); return atd_RIGHTP;
_                           yylval.string = strdup(yytext); return atd_SEP1;
,                           yylval.string = strdup(yytext); return atd_SEP2;
"//".*                      yylval.string = strdup(yytext); return atd_COMMENT;
\n                          yylval.string= strdup(yytext);  return atd_NEWLINE;
[\r \t]+                    ;
%%

