//Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
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
    extern void yyerror(Conjecture &conj, int &result,map<string,map<string,string>> &coreList, map<string,string> &varToCoreName, map<string, PropertyAssignment*> varToProperty, char const* msg);
    map<char*,ConjectureNode*> sub_formula_variables;
    
    bool check_varToProperty(string v,map<string, PropertyAssignment*> &varToProperty);
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
%parse-param {map<string,map<string,string>> &coreList}
%parse-param {map<string,string> &varToCoreName}
%parse-param {map<string, PropertyAssignment*> &varToProperty}



%token SEPERATOR  FILEPATH LEFTP RIGHTP NAME NEWLINE AND OR IFF IMPLIES NOT TRUE FALSE COMMENT NUMBER_DOUBLE COMMA
        FORMULA_NAME EXP ATLEAST ATMOST LESS BIGGER
%type<string> COMPARATOR SEPERATOR FILEPATH LEFTP RIGHTP NAME NEWLINE AND OR IFF IMPLIES NOT TRUE FALSE COMMENT VARIABLE ATOMIC_PREDICATE COMMA FORMULA_NAME EXP
            ATLEAST ATMOST LESS BIGGER
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
%right NOT
%left NEWLINE
%%

START            :COMMENTS VARIABLES_CORES_ASSIGNMENT FORMULA_NAME NEWLINE VARIABLES_SUBFORMULA_ASSIGNMENTS  FORMULA FORMULACOMMENTS
                    {conj.setRoot($6); result = 0;}
                 ;
VARIABLES_CORES_ASSIGNMENT  : VARIABLES_CORES_ASSIGNMENT VARIABLE_CORE_ASSIGNMENT NEWLINE COMMENTS {}
                            | %empty
                            ;
VARIABLE_CORE_ASSIGNMENT    : VARIABLE SEPERATOR ATOMIC_PREDICATE COMPARATOR NUMBER_DOUBLE
                                {if(check_varToProperty($1,varToProperty)){ cout<<" variable " << $1 << " is written at least two times" <<endl; YYERROR; exit(20);}
                                if(!coreList.count($3)){cout<<"core name \" "<<$3<<"\"is not exist"<<endl; YYERROR;}
                                if(!coreList[$3].count("CoreType")){cout<<"CoreType of "<<$3<<" couldn't find, chech the core properities"<<endl; YYERROR;}
                                if(coreList[$3]["CoreType"]!="Bool"){cout<<"CoreType of "<<$3<< " isn't Bool, check the core properties"<<endl; YYERROR;}
                                if(!coreList[$3].count("ParameterType")){cout<<"Parameter of "<<$3<<" couldn't find, chech the core properities"<<endl; YYERROR;}
                                if(coreList[$3]["ParameterType"]!="UnsignedInt"){cout<<"Parameter of "<<$3<< " isn't UnsignedInt, check the core properties"<<endl; YYERROR;}
                                if(!coreList[$3].count("PrimaryOperator")){cout<<"PrimaryOperator for "<<$3<<" isn't set, check the core properties"<<endl;YYERROR;}
                                if(coreList[$3]["PrimaryOperator"]=="AtLeast" and (strcmp($4,"<")==0 or strcmp($4,"<=")==0)   ){
                                   cout<<"PrimaryOperator for "<<$3<<" is " <<coreList[$3]["PrimaryOperator"]<<" and the written operator is " <<$4<<" , check the core properties"<<endl;YYERROR;}
                                if( coreList[$3]["PrimaryOperator"]=="AtMost" and (strcmp($4,">")==0 or strcmp($4,">=")==0)  )
                                {cout<<"PrimaryOperator for "<<$3<<" is " <<coreList[$3]["PrimaryOperator"]<<" and the written operator is " <<$4<<" , check the core properties"<<endl;YYERROR;}
                                $$ = new PropertyAssignment(); $$->setParameterType("UnsignedInt");
                                $$->setName($3); $$->setOp($4);
                                $$->setParameter($5);  varToProperty[$1] = $$; varToCoreName[$1] = $3;}
                                                                                                                    
                        | VARIABLE SEPERATOR ATOMIC_PREDICATE LEFTP  FILEPATH  RIGHTP{}
                        | VARIABLE SEPERATOR ATOMIC_PREDICATE                        
                        {if(check_varToProperty($1,varToProperty)){ cout<<" variable " << $1 << " is written at least two times" <<endl; YYERROR; exit(20);}
                                if(!coreList.count($3)){cout<<"core name \" "<<$3<<"\"is not exist"<<endl; YYERROR;}
                                if(!coreList[$3].count("CoreType")){cout<<"CoreType of "<<$3<<" couldn't find, chech the core properities"<<endl; YYERROR;}
                                if(!(coreList[$3]["CoreType"]=="Bool" or coreList[$3]["CoreType"]=="Min" or coreList[$3]["CoreType"]=="Max"))
                                {cout<<"CoreType of "<<$3<< " isn't correct, check the core properties, the given CoreType is "<<coreList[$3]["CoreType"] <<endl; YYERROR;}
                                if(!coreList[$3].count("ParameterType")){cout<<"Parameter of "<<$3<<" couldn't find, chech the core properities"<<endl; YYERROR;}
                                if(coreList[$3]["ParameterType"]!="ParameterLess"){cout<<"Parameter of "<<$3 <<" is " << coreList[$3]["ParameterType"]<< " and it should be ParameterLess, check the core properties"<<endl; YYERROR;}
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
                                                                    cout<< "variable " << $2 << " declared at least two times" <<endl; YYERROR;}
                                                                  if(check_sub_formula_variables($2)){
                                                                    cout<< "variable " << $2 << " declared at least two times" <<endl; YYERROR;
                                                                  }
                                                                  sub_formula_variables.insert(make_pair($2, $4));
                                                                  }
				  | %empty
				  ;

SUB_FORMULA : SUB_FORMULA AND SUB_FORMULA {$$= new ConjectureNode(OPERATOR,"and");    
                vector<ConjectureNode*> children; children.push_back($1); children.push_back($3);
                $$->setChildren(children);
                $1->setParent($$); $3->setParent($$);
                }
            
            | NOT SUB_FORMULA  {$$= new ConjectureNode(OPERATOR,"not");
                vector<ConjectureNode*> children; children.push_back($2);
                $$->setChildren(children);
                $2->setParent($$);
                }

            | LEFTP SUB_FORMULA RIGHTP {$$ = $2;}
           
            
            | VARIABLE {if(check_varToProperty($1,varToProperty) and !check_sub_formula_variables($1) ){
                    $$ = new ConjectureNode(CORE_VARIABLE, $1);
                }else if(check_sub_formula_variables($1) and !check_varToProperty($1,varToProperty) ) {
                    $$ = sub_formula_variables[$1];    
                }else{
                    cout<<" variable "<< $1 << " is not valid"<< endl; YYERROR;
                }
            }
            
            | TRUE {$$ = new ConjectureNode(NUMBER,1);} 
            
            | FALSE {$$ = new ConjectureNode(NUMBER,0);}
            
            | NUMBER_DOUBLE {$$= new ConjectureNode(NUMBER,$1);}
            ;
FORMULA     : FORMULA AND FORMULA {$$ = new ConjectureNode(OPERATOR,"and");
                    vector<ConjectureNode*> children;
                    children.push_back($1); children.push_back($3);	$$->setChildren(children);
                    $1->setParent($$); $3->setParent($$);
            }
            | FORMULA OR FORMULA {$$ = new ConjectureNode(OPERATOR,"or");
                                  vector<ConjectureNode*> children;
                                  children.push_back($1); children.push_back($3);	$$->setChildren(children);
                                  $1->setParent($$); $3->setParent($$);
                          }
	        | FORMULA IMPLIES FORMULA {$$ = new ConjectureNode(OPERATOR,"implies");
					    vector<ConjectureNode*> children;
					    children.push_back($1); children.push_back($3);	$$->setChildren(children);
					    $1->setParent($$); $3->setParent($$);
				    }
	       | FORMULA IFF FORMULA {$$ = new ConjectureNode(OPERATOR,"iff");
						    vector<ConjectureNode*> children;
						    children.push_back($1); children.push_back($3);	$$->setChildren(children);
						    $1->setParent($$); $3->setParent($$);
					    }
            | FORMULA ATLEAST FORMULA {$$ = new ConjectureNode(OPERATOR,">=");
						    vector<ConjectureNode*> children;
						    children.push_back($1); children.push_back($3);	$$->setChildren(children);
						    $1->setParent($$); $3->setParent($$);
					    }
          | FORMULA ATMOST FORMULA {$$ = new ConjectureNode(OPERATOR,"<=");
						    vector<ConjectureNode*> children;
						    children.push_back($1); children.push_back($3);	$$->setChildren(children);
						    $1->setParent($$); $3->setParent($$);
					    }
          | FORMULA LESS FORMULA {$$ = new ConjectureNode(OPERATOR,"<");
						    vector<ConjectureNode*> children;
						    children.push_back($1); children.push_back($3);	$$->setChildren(children);
						    $1->setParent($$); $3->setParent($$);
					    }
   | FORMULA BIGGER  FORMULA {$$ = new ConjectureNode(OPERATOR,">");
						    vector<ConjectureNode*> children;
						    children.push_back($1); children.push_back($3);	$$->setChildren(children);
						    $1->setParent($$); $3->setParent($$);
					    }

            | NOT FORMULA  {$$ = new ConjectureNode(OPERATOR,"not");
                    vector<ConjectureNode*> children;
                    children.push_back($2);	$$->setChildren(children); $2->setParent($$);
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
                                    cout<<" variable "<< $1 << " is not valid"<< endl; YYERROR;
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

void yyerror(Conjecture &conj, int &result,map<string,map<string,string>> &coreList, map<string,string> &varToCoreName,map<string, PropertyAssignment*> varToProperty, char const* msg){
  std::cout<<"Syntax Error: "<< msg << " line " <<input_lineno << std::endl;
  // error printing  disabled, it is handeled in main.cpp 
}

bool check_varToProperty(string v,map<string, PropertyAssignment*> &varToProperty ){
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



