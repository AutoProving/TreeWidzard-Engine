%defines
%define api.prefix {input_}

%code requires {
    #include "../../Conjecture/Conjecture.h"
    #include "../../Kernel/Width.h"
    #include <iostream>
    #include <vector>
    #include <map>
    #include <utility>

}
%{
    #include <iostream>
    #include <string.h>
    #include <cmath>
    #include <vector>
    #include <map>
    #include <string.h>
    #include <utility>
    #include "../../Conjecture/Conjecture.h"
    #include "../../Kernel/Width.h"
    #include <algorithm>

    // this function will be generated
    // using flex
    extern int yylex();
    extern int input_lineno;
    extern void yyerror(Conjecture &conj, int &result,std::map<std::string,std::map<std::string,std::string>> &coreList, std::map<std::string,std::string> &varToCoreName, std::map<std::string, PropertyAssignment*> varToProperty, char const* msg);
    std::map<char*,ConjectureNode*> sub_formula_variables;

    bool check_varToProperty(std::string v,std::map<std::string, PropertyAssignment*> &varToProperty);
    bool check_sub_formula_variables(char* v);
%}
%locations
%union{
     ConjectureNode *conjectureNode;
     double number;
     char* string;
     std::vector<int> * vec;
     PropertyAssignment *property;
}

%parse-param {Conjecture &conj}
%parse-param {int &result}
%parse-param {std::map<std::string,std::map<std::string,std::string>> &coreList}
%parse-param {std::map<std::string,std::string> &varToCoreName}
%parse-param {std::map<std::string, PropertyAssignment*> &varToProperty}



%token SEPERATOR  FILEPATH LEFTP RIGHTP NAME NEWLINE AND OR IFF IMPLIES NOT TRUE FALSE COMMENT NUMBER_DOUBLE COMMA
        FORMULA_NAME EXP ATLEAST ATMOST LESS BIGGER BINARY_ARITHMETIC_OPERATOR BINARY_FUNCTION UNARY_FUNCTION
%type<string> COMPARATOR SEPERATOR FILEPATH LEFTP RIGHTP NAME NEWLINE AND OR IFF IMPLIES NOT TRUE FALSE COMMENT VARIABLE ATOMIC_PREDICATE COMMA FORMULA_NAME EXP
            ATLEAST ATMOST LESS BIGGER BINARY_ARITHMETIC_OPERATOR BINARY_FUNCTION UNARY_FUNCTION
%type<number> NUMBER_DOUBLE
%type<property> VARIABLE_CORE_ASSIGNMENT
%type<conjectureNode> FORMULA SUB_FORMULA FORMULA_TERMINAL
%type<vec> NUMBER_DOUBLES

%start START
%left IFF
%left IMPLIES
%left OR
%left AND
%left LESS
%left BIGGER
%left ATLEAST
%left ATMOST
%left BINARY_ARITHMETIC_OPERATOR
%right NOT
%left NEWLINE
%right BINARY_FUNCTION
%right UNARY_FUNCTION
%%

START            :COMMENTS VARIABLES_CORES_ASSIGNMENT FORMULA_NAME NEWLINE VARIABLES_SUBFORMULA_ASSIGNMENTS  FORMULA FORMULACOMMENTS
                    {conj.setRoot($6); result = 0;}
                 ;
VARIABLES_CORES_ASSIGNMENT  : VARIABLES_CORES_ASSIGNMENT VARIABLE_CORE_ASSIGNMENT NEWLINE COMMENTS {}
                            | %empty
                            ;
VARIABLE_CORE_ASSIGNMENT    : VARIABLE SEPERATOR ATOMIC_PREDICATE COMPARATOR NUMBER_DOUBLE
                                {if(check_varToProperty($1,varToProperty)){ std::cout<<" variable " << $1 << " is written at least two times" <<std::endl; YYERROR; exit(20);}
                                if(!coreList.count($3)){std::cout<<"core name \" "<<$3<<"\"is not exist"<<std::endl; YYERROR;}
                                if(!coreList[$3].count("CoreType")){std::cout<<"CoreType of "<<$3<<" couldn't find, chech the core properities"<<std::endl; YYERROR;}
                                if(coreList[$3]["CoreType"]!="Bool"){std::cout<<"CoreType of "<<$3<< " isn't Bool, check the core properties"<<std::endl; YYERROR;}
                                if(!coreList[$3].count("ParameterType")){std::cout<<"Parameter of "<<$3<<" couldn't find, chech the core properities"<<std::endl; YYERROR;}
                                if(coreList[$3]["ParameterType"]!="UnsignedInt"){std::cout<<"Parameter of "<<$3<< " isn't UnsignedInt, check the core properties"<<std::endl; YYERROR;}
                                if(!coreList[$3].count("PrimaryOperator")){std::cout<<"PrimaryOperator for "<<$3<<" isn't set, check the core properties"<<std::endl;YYERROR;}
                                if(coreList[$3]["PrimaryOperator"]=="AtLeast" and (strcmp($4,"<")==0 or strcmp($4,"<=")==0)   ){
                                   std::cout<<"PrimaryOperator for "<<$3<<" is " <<coreList[$3]["PrimaryOperator"]<<" and the written operator is " <<$4<<" , check the core properties"<<std::endl;YYERROR;}
                                if( coreList[$3]["PrimaryOperator"]=="AtMost" and (strcmp($4,">")==0 or strcmp($4,">=")==0)  )
                                {std::cout<<"PrimaryOperator for "<<$3<<" is " <<coreList[$3]["PrimaryOperator"]<<" and the written operator is " <<$4<<" , check the core properties"<<std::endl;YYERROR;}
                                $$ = new PropertyAssignment(); $$->setParameterType("UnsignedInt");
                                $$->setName($3); $$->setOp($4);
                                $$->setParameter($5);  varToProperty[$1] = $$; varToCoreName[$1] = $3;}

                        | VARIABLE SEPERATOR ATOMIC_PREDICATE LEFTP  FILEPATH  RIGHTP{}
                        | VARIABLE SEPERATOR ATOMIC_PREDICATE
                        {if(check_varToProperty($1,varToProperty)){ std::cout<<" variable " << $1 << " is written at least two times" <<std::endl; YYERROR; exit(20);}
                                if(!coreList.count($3)){std::cout<<"core name \" "<<$3<<"\"is not exist"<<std::endl; YYERROR;}
                                if(!coreList[$3].count("CoreType")){std::cout<<"CoreType of "<<$3<<" couldn't find, chech the core properities"<<std::endl; YYERROR;}
                                if(!(coreList[$3]["CoreType"]=="Bool" or coreList[$3]["CoreType"]=="Min" or coreList[$3]["CoreType"]=="Max"))
                                {std::cout<<"CoreType of "<<$3<< " isn't correct, check the core properties, the given CoreType is "<<coreList[$3]["CoreType"] <<std::endl; YYERROR;}
                                if(!coreList[$3].count("ParameterType")){std::cout<<"Parameter of "<<$3<<" couldn't find, chech the core properities"<<std::endl; YYERROR;}
                                if(coreList[$3]["ParameterType"]!="ParameterLess"){std::cout<<"Parameter of "<<$3 <<" is " << coreList[$3]["ParameterType"]<< " and it should be ParameterLess, check the core properties"<<std::endl; YYERROR;}
                                $$ = new PropertyAssignment(); $$->setParameterType("ParameterLess");
                                $$->setName($3);
                                varToProperty[$1] = $$; varToCoreName[$1] = $3;}
                        | VARIABLE SEPERATOR ATOMIC_PREDICATE  LEFTP NUMBER_DOUBLES RIGHTP {}
                        ;
NUMBER_DOUBLES		: NUMBER_DOUBLES COMMA NUMBER_DOUBLE {$$ = new std::vector<int>; $$ = $1; $$->push_back($3);}
                    | NUMBER_DOUBLE { $$ = new std::vector<int>;  $$->push_back($1);}
ATOMIC_PREDICATE    : NAME
                    ;
COMPARATOR  : BIGGER{$$=$1;}
            | ATLEAST{$$=$1;}
            | ATMOST{$$=$1;}
            | LESS {$$=$1;}
VARIABLES_SUBFORMULA_ASSIGNMENTS : EXP VARIABLE SEPERATOR SUB_FORMULA NEWLINE  VARIABLES_SUBFORMULA_ASSIGNMENTS
								{if(check_varToProperty($2,varToProperty)){
                                                                    std::cout<< "variable " << $2 << " declared at least two times" <<std::endl; YYERROR;}
                                                                  if(check_sub_formula_variables($2)){
                                                                    std::cout<< "variable " << $2 << " declared at least two times" <<std::endl; YYERROR;
                                                                  }
                                                                  sub_formula_variables.insert(std::make_pair($2, $4));
                                                                  }
				  | %empty
				  ;

SUB_FORMULA : SUB_FORMULA AND SUB_FORMULA {$$= new ConjectureNode(OPERATOR,"and");
                std::vector<ConjectureNode*> children; children.push_back($1); children.push_back($3);
                $$->setChildren(children);
                $1->setParent($$); $3->setParent($$);
                }

            | NOT SUB_FORMULA  {$$= new ConjectureNode(OPERATOR,"not");
                std::vector<ConjectureNode*> children; children.push_back($2);
                $$->setChildren(children);
                $2->setParent($$);
                }

            | LEFTP SUB_FORMULA RIGHTP {$$ = $2;}


            | VARIABLE {if(check_varToProperty($1,varToProperty) and !check_sub_formula_variables($1) ){
                    $$ = new ConjectureNode(CORE_VARIABLE, $1);
                }else if(check_sub_formula_variables($1) and !check_varToProperty($1,varToProperty) ) {
                    $$ = sub_formula_variables[$1];
                }else{
                    std::cout<<" variable "<< $1 << " is not valid"<< std::endl; YYERROR;
                }
            }

            | TRUE {$$ = new ConjectureNode(NUMBER,1);}

            | FALSE {$$ = new ConjectureNode(NUMBER,0);}

            | NUMBER_DOUBLE {$$= new ConjectureNode(NUMBER,$1);}
            ;
FORMULA     : FORMULA AND FORMULA {$$ = new ConjectureNode(OPERATOR,"and");
            std::vector<ConjectureNode*> children;
            children.push_back($1); children.push_back($3);	$$->setChildren(children);
            $1->setParent($$); $3->setParent($$);
            }
            | FORMULA OR FORMULA {$$ = new ConjectureNode(OPERATOR,"or");
                    std::vector<ConjectureNode*> children;
                    children.push_back($1); children.push_back($3);	$$->setChildren(children);
                    $1->setParent($$); $3->setParent($$);
            }
            | FORMULA IMPLIES FORMULA {$$ = new ConjectureNode(OPERATOR,"implies");
            std::vector<ConjectureNode*> children;
            children.push_back($1); children.push_back($3);	$$->setChildren(children);
            $1->setParent($$); $3->setParent($$);
            }
            | FORMULA IFF FORMULA {$$ = new ConjectureNode(OPERATOR,"iff");
            std::vector<ConjectureNode*> children;
            children.push_back($1); children.push_back($3);	$$->setChildren(children);
            $1->setParent($$); $3->setParent($$);
            }
            | FORMULA ATLEAST FORMULA {$$ = new ConjectureNode(OPERATOR,">=");
            std::vector<ConjectureNode*> children;
            children.push_back($1); children.push_back($3);	$$->setChildren(children);
            $1->setParent($$); $3->setParent($$);
            }
            | FORMULA ATMOST FORMULA {$$ = new ConjectureNode(OPERATOR,"<=");
            std::vector<ConjectureNode*> children;
            children.push_back($1); children.push_back($3);	$$->setChildren(children);
            $1->setParent($$); $3->setParent($$);
            }
            | FORMULA LESS FORMULA {$$ = new ConjectureNode(OPERATOR,"<");
            std::vector<ConjectureNode*> children;
            children.push_back($1); children.push_back($3);	$$->setChildren(children);
            $1->setParent($$); $3->setParent($$);
            }
            | FORMULA BIGGER  FORMULA {$$ = new ConjectureNode(OPERATOR,">");
            std::vector<ConjectureNode*> children;
            children.push_back($1); children.push_back($3);	$$->setChildren(children);
            $1->setParent($$); $3->setParent($$);
            }

            | NOT FORMULA  {$$ = new ConjectureNode(OPERATOR,"not");
            std::vector<ConjectureNode*> children;
            children.push_back($2);	$$->setChildren(children); $2->setParent($$);
            }
            | FORMULA BINARY_ARITHMETIC_OPERATOR  FORMULA {$$ = new ConjectureNode(OPERATOR,$2);
            std::vector<ConjectureNode*> children;
            children.push_back($1); children.push_back($3);	$$->setChildren(children);
            $1->setParent($$); $3->setParent($$);
            }
            |BINARY_FUNCTION LEFTP  FORMULA COMMA FORMULA RIGHTP {$$ = new ConjectureNode(FUNCTION_BINARY,$1);
            std::vector<ConjectureNode*> children;
            children.push_back($3); children.push_back($5);	$$->setChildren(children);
            $3->setParent($$); $5->setParent($$);
            }
            |UNARY_FUNCTION LEFTP  FORMULA RIGHTP {$$ = new ConjectureNode(FUNCTION_UNARY,$1);
            std::vector<ConjectureNode*> children;
            children.push_back($3);	$$->setChildren(children);
            $3->setParent($$);
            }
            | LEFTP FORMULA RIGHTP      {$$ = $2;}
            | FORMULA_TERMINAL
            ;
FORMULA_TERMINAL : TRUE         {$$ = new ConjectureNode(NUMBER,1); }
                 | FALSE        {$$ = new ConjectureNode(NUMBER,0); }
                 | VARIABLE     { if(check_varToProperty($1,varToProperty) and !check_sub_formula_variables($1) ){
                                   $$ = new ConjectureNode(CORE_VARIABLE, $1);
                                  }else if(check_sub_formula_variables($1) and !check_varToProperty($1,varToProperty) ) {
                                    $$ = sub_formula_variables[$1];
                                  }else{
                                    std::cout<<" variable "<< $1 << " is not valid"<< std::endl; YYERROR;
                                  }
                                }
                 | NUMBER_DOUBLE {$$= new ConjectureNode(NUMBER,$1);}
                 ;
//FORMULA_OP_BINARY     : AND {$$ = new ConjectureNode(OPERATOR,"and");}
//                      | OR {$$ = new ConjectureNode(OPERATOR,"or");}
//                      ;
//FORMULA_OP_UNARY      : NOT {$$ = new ConjectureNode(OPERATOR,"not");}
//                      ;

VARIABLE                : NAME
                        ;
COMMENTS                :COMMENT COMMENTS                          {}
                        |%empty
                        ;
FORMULACOMMENTS         :NEWLINE COMMENTS FORMULACOMMENTS
                        |%empty
                        ;
%%

void yyerror(Conjecture &conj, int &result,std::map<std::string,std::map<std::string,std::string>> &coreList, std::map<std::string,std::string> &varToCoreName, std::map<std::string, PropertyAssignment*> varToProperty, char const* msg){
  std::cout<<"Syntax Error: "<< msg << " line " <<input_lineno << std::endl;
  // error printing  disabled, it is handeled in main.cpp
}

bool check_varToProperty(std::string v,std::map<std::string, PropertyAssignment*> &varToProperty ){
    if(varToProperty.count(v)){
        return true;
    }else{
        return false;
    }
}

bool check_sub_formula_variables(char* v){
    if(sub_formula_variables.count(v)){

        return true;
    }else{
        return false;
    }
}
