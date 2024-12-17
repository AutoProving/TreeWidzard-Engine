/*Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.*/
%option noyywrap
%option prefix="input_"
%{
    #include <cstdlib>
    #include <vector>
    #include <map>
    #include "../../Conjecture/PropertyAssignment.h"
    #include "../../Conjecture/Conjecture.h"
    #include "../../Kernel/Width.h"
    #include "input_parser.hpp"
    #define yylval input_lval

    extern int yyparse(Conjecture &conj, int &result,std::map<std::string,std::map<std::string,std::string>> &coreList, std::map<std::string,std::string> &varToCoreName, std::map<std::string, PropertyAssignment*> varToProperty, char const* msg);

%}

%option yylineno

%x str

%%
        std::string string_builder;

EXP                                                  yylval.string = strdup(yytext); return EXP;
Formula|FORMULA                                      yylval.string = strdup(yytext); return FORMULA_NAME;
FALSE|False|false                                    yylval.string = strdup(yytext); return FALSE;
TRUE|True|true                                       yylval.string = strdup(yytext); return TRUE;
IFF                                                  yylval.string = strdup(yytext); return IFF;
IMPLIES                                              yylval.string = strdup(yytext); return IMPLIES;
AND|\&\&                                             yylval.string = strdup(yytext); return AND;
OR|\|\                                               yylval.string = strdup(yytext); return OR;
NOT|\!                                               yylval.string = strdup(yytext); return NOT;
INV                                                  yylval.string = strdup(yytext); return INV_;
\<                                                   yylval.string = strdup(yytext); return LESS;
\>                                                   yylval.string = strdup(yytext); return BIGGER;
\>\=                                                  yylval.string = strdup(yytext); return ATLEAST;
\<\=                                                  yylval.string = strdup(yytext); return ATMOST;
\=\=                                                 yylval.string = strdup(yytext); return EQUAL;
\+|\-|\*|\/                                          yylval.string = strdup(yytext); return BINARY_ARITHMETIC_OPERATOR;
max|min|pow                                          yylval.string = strdup(yytext); return BINARY_FUNCTION;
abs|acos|asin|atan|cos|exp|floor|ln|log|sin|sqrt|tan yylval.string = strdup(yytext); return UNARY_FUNCTION;
[0-9]+                                               yylval.number = std::atoi(yytext); return NUMBER_DOUBLE;
[a-zA-Z0-9]+                                         yylval.string = strdup(yytext); return NAME;
\:\=                                                 yylval.string = strdup(yytext); return SEPERATOR;
\(                                                   yylval.string = strdup(yytext); return LEFTP;
\)                                                   yylval.string = strdup(yytext); return RIGHTP;
\n                                                   yylval.string = strdup(yytext); return NEWLINE;
\/\/[a-zA-Z0-9\t ]*\n                                yylval.string = strdup(yytext); return COMMENT;
,                                                    yylval.string = strdup(yytext); return COMMA;
[ \t]+                      ;

\"      string_builder.clear(); BEGIN(str);

<str>\"        { /* saw closing quote - all done */
        BEGIN(INITIAL);
        yylval.string = strdup(string_builder.c_str()); return STRING;
        }

<str>\n        {
            throw std::runtime_error("newline in string literal (should be escaped like \"\\n\")");
        }

<str>\\[0-7]{1,3} {
        /* octal escape sequence */
        unsigned result;

        (void) sscanf( yytext + 1, "%o", &result );

        if ( result > 0xff ) {
                /* error, constant is out-of-bounds */
                throw std::runtime_error("octal escape out of range in string literal");
        }

        string_builder += static_cast<char>(result);
        }

<str>\\[0-9]+ {
        /* generate error - bad escape sequence; something
         * like '\48' or '\0777777'
         */
            throw std::runtime_error("bad escape sequence in string literal");
        }

<str>\\n  string_builder += '\n';
<str>\\t  string_builder += '\t';
<str>\\r  string_builder += '\r';
<str>\\b  string_builder += '\b';
<str>\\f  string_builder += '\f';

<str>\\(.|\n)  string_builder += yytext[1];

<str>[^\\\n\"]+        {
        char *yptr = yytext;

        while ( *yptr )
                string_builder += *yptr++;
        }
