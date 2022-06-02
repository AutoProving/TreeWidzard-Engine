%defines
%define api.prefix {term_}
%code requires {
    #include "../../TreeAutomaton/TreeAutomaton.h"
    #include <iostream>
    #include <vector>
}
%{
    #include <iostream>
    #include <string.h>
    #include <cmath>
    #include <vector>
    #include "../../TreeAutomaton/TreeAutomaton.h"
    #include <algorithm>
   
    // this function will be generated
    // using flex
    extern int yylex();
    extern int term_lineno;
    template<class S>
    extern void yyerror(Term<S> &term, int &result, char const* msg);
  %}
%locations
%union{
     unsigned number;
     char* string;

}
%parse-param {Term<std::string> &term}
%parse-param {int &result}

%token TR_NODE_NUM TR_COMMENT TR_NEWLINE TR_NODE_SYMBOL TR_SEPARATOR
%type<string> TR_NODE_SYMBOL TR_COMMENT TR_SEPARATOR
%type<number> TR_NODE_NUM

%start TR_START


%%

TR_START                   : TR_NODES_INFO1                            {std::cout<<"hello"<<std::endl;}
                           ;
TR_NODES_INFO1             : TR_NODE_NUM TR_SEPARATOR TR_NODE_SYMBOL TR_NODE_CHILDREN TR_NODES_INFO2
                           | TR_COMMENTS TR_NODES_INFO1
                           |
                           ;
TR_NODES_INFO2             : TR_NEWLINE TR_NODE_NUM TR_SEPARATOR TR_NODE_SYMBOL TR_NODE_CHILDREN  TR_NODES_INFO2
                           | TR_NEWLINE TR_NODES_INFO2
                           |
                           ;

TR_NODE_CHILDREN           : TR_NODE_NUM TR_NODE_NUM
                           | TR_NODE_NUM
                           |
                           ;
TR_COMMENTS                : TR_COMMENT TR_COMMENTS
                           |
                           ;
%%
template<class S>
void yyerror(Term<S> &term, int &result, char const* msg){
  std::cout<<"Syntax Error: "<< msg << " " <<term_lineno << std::endl;
  // error printing  disabled, it is handeled in main.cpp 
}
