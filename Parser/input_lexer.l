/*Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.*/
%option noyywrap
%option prefix="input_"
%{
    #include <cstdlib>
    #include <vector>
    #include "../Kernel/PropertyAssignment.h"
    #include "../Kernel/Conjecture.h"
    #include "../Kernel/Width.h"
    #include "input_parser.hpp"
    #define yylval input_lval

    extern int yyparse(std::vector<PropertyAssignment*> &pl, Conjecture &conj, Width &w, int &result);

%}

%option yylineno

%%

tree_width                  yylval.string = "tree_width";   return WIDTHPARAM;
path_width                  yylval.string = "path_width";   return WIDTHPARAM;
FALSE                       yylval.string = strdup(yytext); return FALSE;
TRUE                        yylval.string= strdup(yytext);  return TRUE;
IFF                         yylval.string= strdup(yytext);  return IFF;
IMPLIES                     yylval.string= strdup(yytext);  return IMPLIES;
AND                         yylval.string= strdup(yytext);  return AND;
OR                          yylval.string= strdup(yytext);  return OR;
NOT                         yylval.string= strdup(yytext);  return NOT;
[0-9]+                      yylval.number= atoi(yytext);    return INTEGER;
[a-zA-Z0-9]+                yylval.string= strdup(yytext);  return NAME;
\:\=                        yylval.string= strdup(yytext);  return SEPERATOR;
\<|\>|\=\>|\=|\<\=          yylval.string= strdup(yytext);  return COMPARATOR;
\(                          yylval.string= strdup(yytext);  return LEFTP;
\)                          yylval.string= strdup(yytext);  return RIGHTP;
\".+\"                      yylval.string= strdup(yytext);  return FILEPATH;
\n                          yylval.string= strdup(yytext);  return NEWLINE;
\/\/[a-zA-Z0-9\t ]*\n       yylval.string= strdup(yytext);  return COMMENT;
[ \t]+                      ;
%%

