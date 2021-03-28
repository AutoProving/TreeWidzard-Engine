%option noyywrap
%option prefix="command_"
%{
    #include <cstdlib>
    #include <vector>
    #include "command_parser.hpp"
    #define yylval command_lval
    extern int yyparse(int &result);

%}

%option yylineno

%%
-search                     yylval.string = strdup(yytext); return command_search_signature;
-parse                      yylval.string = strdup(yytext); return command_parse_signature;
-ps                         yylval.string = strdup(yytext); return command_print_state_flag;
-pl                         yylval.string = strdup(yytext); return command_print_loop_flag;
-st                         yylval.string = strdup(yytext); return command_print_state_tree;
--help                      yylval.string = strdup(yytext); return command_help;
-term                       yylval.string = strdup(yytext); return command_term_signature;
-random                     yylval.string = strdup(yytext); return command_random_signature;
PACE                        yylval.string = strdup(yytext); return command_parse_pace;
ABSTRACT                    yylval.string = strdup(yytext); return command_parse_abstract;
;                           yylval.string = strdup(yytext); return command_end;
[0-9\.]+                    yylval.number = stof(yytext);   return command_number;
.+                          yylval.string = strdup(yytext); return command_string;
[\n \t \r]                  ;
%%

