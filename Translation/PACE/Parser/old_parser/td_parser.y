%defines
%define api.prefix {td_}
%code requires {
    #include "../TreeDecompositionPACE.h"
    #include <iostream>
    #include <vector>
    #include <set>

}
%{
    #include <iostream>
    #include <string.h>
    #include <cmath>
    #include <vector>
    #include <set>
    #include "../TreeDecompositionPACE.h"
    #include <algorithm>
    
    // this function will be generated
    // using flex
    extern int yylex();
    extern int td_lineno;// for tracking an error line 
    extern void yyerror(TreeDecompositionPACE &td, int &result, char const* msg);
  %}
%locations
%union{
     int number;
     char* string;
     std::set<unsigned>* set_unsigned;
}

// external parameters which will be passed by the program to the Parser.
%parse-param {TreeDecompositionPACE &td}
%parse-param {int &result}

%token TD_S TD_TD TD_B TD_NUM TD_COMMENT TD_NEWLINE
%type<string> TD_S TD_TD TD_B TD_COMMENT TD_NEWLINE
%type<number> TD_NUM
%type<set_unsigned> TD_NUMS

%start TD_START


%%

TD_START                   : TD_COMMENTS TD_TDINFO TD_COMMENTS TD_BAGS TD_EDGES     {}
                           ;
TD_TDINFO                  : TD_S TD_TD TD_NUM TD_NUM TD_NUM TD_NEWLINE   {td.setWidthType("tree_width"); td.setNum_vertices($3); td.setWidth($4-1);
                                                                           td.setNum_graph_vertices($5); }
                           ;
TD_BAGS                    : TD_BAG TD_NEWLINE TD_COMMENTS TD_BAGS
                           |
                           ;
TD_BAG                     : TD_B TD_NUM TD_NUMS                           {if(!td.setABag(*$3,$2)) {yyerror(td,result, "Number of bags is incorrect! The number of bags should be started from 1, and the bag with number 1 is the root.");YYERROR;}}
                           ;
TD_NUMS                    : TD_NUM TD_NUMS                                {$$= new std::set<unsigned>(*$2);  $$->insert($1);}
                           |                                               {$$=new std::set<unsigned>;}
                           ;
TD_EDGES                   : TD_EDGE TD_NEWLINE TD_COMMENTS TD_EDGES                                    {}                
                           |
                           ;
TD_EDGE                    : TD_NUM TD_NUM      {if(!td.addEdge($1,$2)) {yyerror(td, result, "Number of bags is incorrect! The number of bags should be started from 1, and the bag with number 1 is the root."); YYERROR;}}
                           ;

TD_COMMENTS                : TD_COMMENT  TD_COMMENTS                                       {}
                           | TD_NEWLINE
                           |
                           ;
                        
                                  
%%

void yyerror(TreeDecompositionPACE &td, int &result, char const* msg){
 std::cerr<< "\033[1;31mERORR:\033[0m" << std::endl;
 std::cerr<<"\033[1;31mError in Tree Decomposition's File line " <<td_lineno << "\033[0m" << std::endl;
 std::cerr << "\033[1;31m"<<msg <<"\033[0m"<< std::endl;

  // error printing  disabled, it is handeled in main.cpp 
}
