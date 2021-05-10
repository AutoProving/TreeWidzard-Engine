/*Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.*/
%option noyywrap
%option prefix="input_"
%{
    #include <cstdlib>
    #include <vector>
    #include <map>
    #include "../../Conjecture/PropertyAssignment.h"
    #include "../../Conjecture/Conjecture.h"
    #include "../Kernel/Width.h"
    #include "input_parser.hpp"
    #define yylval input_lval

    extern int yyparse(Conjecture &conj, int &result,map<string,map<string,string>> &coreList, map<string,string> &varToCoreName, map<string, PropertyAssignment*> varToProperty, char const* msg);

%}

%option yylineno

%%
EXP                         yylval.string = strdup(yytext); return EXP;
Formula|FORMULA             yylval.string = strdup(yytext); return FORMULA_NAME;
FALSE                       yylval.string = strdup(yytext); return FALSE;
TRUE                        yylval.string = strdup(yytext); return TRUE;
IFF                         yylval.string = strdup(yytext); return IFF;
IMPLIES                     yylval.string = strdup(yytext); return IMPLIES;
AND                         yylval.string = strdup(yytext); return AND;
OR                          yylval.string = strdup(yytext); return OR;
NOT                         yylval.string = strdup(yytext); return NOT;
\>                          yylval.string = strdup(yytext); return LESS;
\<                          yylval.string = strdup(yytext); return BIGGER;
\>=                         yylval.string = strdup(yytext); return ATLEAST;
\<=                         yylval.string = strdup(yytext); return ATMOST;
[0-9]+                      yylval.number = atoi(yytext);   return NUMBER_DOUBLE;
[a-zA-Z0-9]+                yylval.string = strdup(yytext); return NAME;
\:\=                        yylval.string = strdup(yytext); return SEPERATOR;
\(                          yylval.string = strdup(yytext); return LEFTP;
\)                          yylval.string = strdup(yytext); return RIGHTP;
\".+\"                      yylval.string = strdup(yytext); return FILEPATH;
\n                          yylval.string = strdup(yytext); return NEWLINE;
\/\/[a-zA-Z0-9\t ]*\n       yylval.string = strdup(yytext); return COMMENT;
,                           yylval.string = strdup(yytext); return COMMA;
[ \t]+                      ;
%%

