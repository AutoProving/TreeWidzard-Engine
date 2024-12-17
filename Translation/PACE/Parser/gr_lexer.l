%option noyywrap
%option prefix="gr_"
%{
    #include <cstdlib>
    #include <vector>
    #include "../../../Multigraph/MultiGraph.h"
    #include "gr_parser.hpp"
    #define yylval gr_lval
    extern int yyparse(MultiGraph &multigraph, int &result);

%}

%option yylineno

%%

p                           yylval.string = "p"; return GR_P;
tw                          yylval.string = "tw"; return GR_TW;
[0-9]+                      yylval.number = atoi(yytext); return GR_NUM;
c.*\n                       yylval.string = strdup(yytext); return GR_COMMENT;
\n                          yylval.string= strdup(yytext);  return GR_NEWLINE;
[\r \t]+                    ;
%%

