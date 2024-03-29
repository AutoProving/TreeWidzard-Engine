//Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
%defines
%define api.prefix {input_}

%code requires {
    #include "../Kernel/PropertyAssignment.h"
    #include "../Kernel/Conjecture.h"
    #include "../Kernel/Width.h"
    #include <iostream>
    #include <vector>
}
%{
    #include <iostream>
    #include <string.h>
    #include <cmath>
    #include <vector>
    #include "../Kernel/PropertyAssignment.h"
    #include "../Kernel/Conjecture.h"
    #include "../Kernel/Width.h"
    #include <algorithm>
   
    // this function will be generated
    // using flex
    extern int yylex();
    extern int input_lineno;
    extern void yyerror(std::vector<PropertyAssignment*> &pl, Conjecture &conj, Width &w, int &result, char const* msg);
    vector<char*> variables;
    bool check_variables(char* v);
    int index_variable(char* v);
%}
%locations
%union{
     PropertyAssignment *property;
     ConjectureNode *conjecture;
     int number;
     char* string;
     std::vector<int> * vec;
}

%parse-param {std::vector<PropertyAssignment*> &pl}
%parse-param {Conjecture &conj}
%parse-param {Width &w}
%parse-param {int &result}


%token  WIDTHPARAM SEPERATOR COMPARATOR FILEPATH LEFTP RIGHTP NAME NEWLINE AND OR IFF IMPLIES NOT TRUE FALSE COMMENT INTEGER COMMA
%type<string> WIDTHPARAM SEPERATOR COMPARATOR FILEPATH LEFTP RIGHTP NAME NEWLINE AND OR IFF IMPLIES NOT TRUE FALSE COMMENT VARIABLE ATOMIC_PREDICATE COMMA
%type<number> INTEGER
%type<property> VARIABLE_ASSIGNMENT
%type<conjecture> FORMULA
%type<vec> INTEGERS


%start START
%left IFF 
%left IMPLIES
%left OR
%left AND
%right NOT

%%

START                   : COMMENTS WIDTH_ASSIGNMENT NEWLINE COMMENTS VARIABLES_ASSIGNMENT FORMULA FORMULACOMMENTS       { conj.setRoot($6); result = 0; }
                        ;
WIDTH_ASSIGNMENT        : WIDTHPARAM INTEGER                                                                            {w.set_name($1); w.set_value($2); } 
                        ;
VARIABLES_ASSIGNMENT    : VARIABLES_ASSIGNMENT  VARIABLE_ASSIGNMENT NEWLINE COMMENTS                                    {}
                        |
                        ;
VARIABLE_ASSIGNMENT     : VARIABLE SEPERATOR ATOMIC_PREDICATE COMPARATOR INTEGER                                       { if(!check_variables($1)) exit(20);
                                                                                                                        $$ = new PropertyAssignment();$$->set_type("UnsignedInt");
                                                                                                                        $$->set_label($1); $$->set_name($3); $$->set_operatorSign($4);
                                                                                                                        $$->set_value($5); pl.push_back($$); variables.push_back($1); }
                                                                                                                    
                        | VARIABLE SEPERATOR ATOMIC_PREDICATE LEFTP  FILEPATH  RIGHTP                                  {if(!check_variables($1)) exit(20);
                                                                                                                         $$ = new PropertyAssignment(); $$->set_type("InputFile"); $$->set_label($1); $$->set_name($3); $$->set_filePath($5);
                                                                                                                         pl.push_back($$); variables.push_back($1); }
                        | VARIABLE SEPERATOR ATOMIC_PREDICATE                                                          {if(!check_variables($1)) exit(20);
                                                                                                                         $$ = new PropertyAssignment(); $$->set_type("NoParameter"); $$->set_label($1); $$->set_name($3);
                                                                                                                         pl.push_back($$); variables.push_back($1); }
                        | VARIABLE SEPERATOR ATOMIC_PREDICATE  LEFTP INTEGERS RIGHTP 					{if(!check_variables($1)) exit(20);
														  	$$ = new PropertyAssignment(); $$->set_type("MultiParameter"); $$->set_label($1); $$->set_name($3);
														  	$$->set_parameters(*$5);
														  	pl.push_back($$); variables.push_back($1);}
                        ;
INTEGERS		: INTEGERS COMMA INTEGER {$$ = new std::vector<int>; $$ = $1; $$->push_back($3);}
			| INTEGER { $$ = new std::vector<int>;  $$->push_back($1);}
ATOMIC_PREDICATE        : NAME
                        ;
VARIABLE                : NAME                                  
                        ;
FORMULA                 : NOT FORMULA                               {$$ = new ConjectureNode(); $$->setLogicalOperator($1); $$->addChild($2); } 
                        | FORMULA AND FORMULA                       {$$ = new ConjectureNode(); $$->setLogicalOperator($2); $$->addChild($1); $$->addChild($3);} 
                        | FORMULA OR  FORMULA                       {$$ = new ConjectureNode(); $$->setLogicalOperator($2); $$->addChild($1); $$->addChild($3);}
                        | FORMULA IMPLIES FORMULA                   {$$ = new ConjectureNode(); $$->setLogicalOperator($2); $$->addChild($1); $$->addChild($3);}
                        | FORMULA IFF FORMULA                       {$$ = new ConjectureNode(); $$->setLogicalOperator($2); $$->addChild($1); $$->addChild($3);}

                        | LEFTP FORMULA RIGHTP                      {$$ = $2;}
                        | TRUE                                      {$$ = new ConjectureNode(); $$->setLogicalOperator($1); $$->setPropertyIndex(0); }
                        | FALSE                                     {$$ = new ConjectureNode(); $$->setLogicalOperator($1); $$->setPropertyIndex(0); }
                        | VARIABLE                                  {$$ = new ConjectureNode() ; $$->setPropertyIndex(index_variable($1)); }
                        ;
COMMENTS                : COMMENT COMMENTS                          {}
                        |
                        ;
FORMULACOMMENTS         : NEWLINE COMMENTS FORMULACOMMENTS
                        | 
                        ;
                                  
%%

void yyerror(std::vector<PropertyAssignment*> &pl, Conjecture &conj, Width &w, int &result, char const* msg){
  std::cout<<"Syntax Error: "<< msg << " " <<input_lineno << std::endl;
  // error printing  disabled, it is handeled in main.cpp 
}
bool check_variables(char* v){
    if(variables.size()>0){
        for(int i = 0 ; i < variables.size() ; i++){
            int result = strcmp(variables[i], v);
            if(result == 0){
                std::cout<<"duplicate variable name"<<endl;
                return false;
            }
        }
        return true;
    }else{
        return true;
    }

}

int index_variable(char* v){

    if(variables.size()>0){
        for(int i = 0 ; i < variables.size() ; i++){
            int result = strcmp(variables[i], v);
            if(result == 0){
                return i+1;
            }
        }
        std::cout<< "grammar error!"<<std::endl;

    }else{
        std::cout << "Input file variable error "<<std::endl;

    }


}
