%option noyywrap
%option prefix="term_"
%{
    #include <cstdlib>
    #include <vector>
    #include "../../TreeAutomaton/TreeAutomaton.h"
    #include "term_parser.hpp"
    #define yylval term_lval
    template<class S>
    extern int yyparse(Term<S> &term, int &result);
%}

%option yylineno

%%
:                           yylval.string = strdup(yytext); return TR_SEPARATOR;
[0-9]+                      yylval.number = atoi(yytext); return TR_NODE_NUM;
C.*\n                       yylval.string = strdup(yytext); return TR_COMMENT;
[a-zA-Z]+                   yylval.string = strdup(yytext); return TR_NODE_SYMBOL;
\n                          yylval.string= strdup(yytext);  return TR_NEWLINE;
[\r \t]+                    ;
%%

