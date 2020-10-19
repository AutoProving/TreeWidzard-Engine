%defines
%define api.prefix {gr_}
%code requires {
    #include "../../../Multigraph/MultiGraph.h"
    #include <iostream>
    #include <vector>
}
%{
    #include <iostream>
    #include <string.h>
    #include <cmath>
    #include <vector>
    #include "../../../Multigraph/MultiGraph.h"
    #include <algorithm>
   
    // this function will be generated
    // using flex
    extern int yylex();
    extern int td_lineno;
    extern void yyerror(MultiGraph &multigraph, int &result, char const* msg);
  %}
%locations
%union{
     unsigned number;
     char* string;
}


%parse-param {MultiGraph &graph}
%parse-param {int &result}


%token GR_P GR_TW GR_NUM GR_COMMENT GR_NEWLINE
%type<string> GR_P GR_TW GR_COMMENT GR_NEWLINE
%type<number> GR_NUM

%start GR_START


%%

GR_START                   : GR_COMMENTS GR_GRAPHINFO  GR_COMMENTS GR_EDGES                             {}
                           ;
GR_GRAPHINFO               : GR_P GR_TW GR_NUM GR_NUM GR_NEWLINE                                           {}
GR_EDGES                   : GR_EDGE GR_NEWLINE GR_COMMENTS GR_EDGES                                    {}                
                           |
                           ;
GR_EDGE                    : GR_NUM GR_NUM                                                        {graph.addEdgeEndPoints($1,$2);}                                          
                           ;
GR_COMMENTS                : GR_COMMENT  GR_COMMENTS                                              {}
                           | GR_NEWLINE
                           |
                           ;
                        
                                  
%%

void yyerror(MultiGraph &multigraph, int &result, char const* msg){
  std::cout<<"Syntax Error: "<< msg << " " <<td_lineno << std::endl;
  // error printing  disabled, it is handeled in main.cpp 
}

