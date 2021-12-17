%option noyywrap
%option prefix="td_"
%{
    #include <cstdlib>
    #include <vector>
    #include "../TreeDecompositionPACE.h"
    #include "td_parser.hpp"
    #define yylval td_lval

    extern int yyparse(TreeDecompositionPACE &td, int &result);

%}

%option yylineno

%%

s                           yylval.string = "s" ; return TD_S;
td                          yylval.string = strdup(yytext); return TD_TD;
b                           yylval.string = "b" ; return TD_B;
[0-9]+                      yylval.number = atoi(yytext);   return TD_NUM;
c.\r.*\n                       yylval.string = strdup(yytext); return TD_COMMENT;
\n                          yylval.string= strdup(yytext);  return TD_NEWLINE;
[\r \t]+                    ;
%%

