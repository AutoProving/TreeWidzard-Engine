%option noyywrap
%option prefix="ctd_"
%{
    #include <cstdlib>
    #include <vector>
    #include "../TreeAutomaton/ConcreteTreeDecomposition.h"
    #include "ctd_parser.hpp"
    #define yylval ctd_lval
    extern int yyparse(ConcreteTreeDecomposition &ctd, int &result);

%}

%option yylineno

%%

[0-9]+                      yylval.number = atoi(yytext); return CTD_NUM;
Empty                       yylval.string = strdup(yytext); return CTD_EMPTY;
IntroVertex                 yylval.string = strdup(yytext); return CTD_INTROVERTEX;
IntroEdge                   yylval.string = strdup(yytext); return CTD_INTROEDGE;
ForgetVertex                yylval.string = strdup(yytext); return CTD_FORGETVERTEX;
Join                        yylval.string = strdup(yytext); return CTD_JOIN;
"("                         yylval.string = strdup(yytext); return CTD_LEFTP;
")"                         yylval.string = strdup(yytext); return CTD_RIGHTP;
_                           yylval.string = strdup(yytext); return CTD_SEP1;
,                           yylval.string = strdup(yytext); return CTD_SEP2;
"//".*                      yylval.string = strdup(yytext); return CTD_COMMENT;
\n                          yylval.string= strdup(yytext);  return CTD_NEWLINE;
[\r \t]+                    ;
%%

