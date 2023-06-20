%defines
%define api.prefix {atd_}
%code requires {
    #include "../../../TreeAutomaton/InstructiveTreeDecomposition.h"
    #include <iostream>
    #include <vector>
    #include <memory>
    #include <map>
    #include <set>
    #include <tuple>
}
%{
    #include <iostream>
    #include <cmath>
    #include <vector>
    #include <memory>
    #include <map>
    #include <set>
    #include <tuple>
    #include "../../../TreeAutomaton/InstructiveTreeDecomposition.h"
    #include <algorithm>

    // using flex
    extern int yylex();
    extern int atd_lineno;
    extern void yyerror(InstructiveTreeDecomposition &atd, int &result, char const* msg);

	// First coordinate is for node, second coordinate is for labels of the children of the node, third coordinate is for label of the node
	// forth coordinate is for integers inside the symbol.
	//Example 4 IntroVertex_2 (3) => 4 is the third coordinate, 2 is the forth, {3} is the second.
   // std::vector<std::tuple<std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>>,int,std::set<int>>> atdVec;
   	// It is the positions of the children of a node in atdVec. It construts inside atd_construct.
    std::map<int,std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>>> labelToNode;
    std::map<int,int> labelToParentLabel;
    int atd_construct(InstructiveTreeDecomposition &atd, std::map<int,std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>>> &labelToNode, std::map<int,int> &labelToParentLabel);
  %}
%locations
%union{
     unsigned number;
     char* string;
}

%parse-param {InstructiveTreeDecomposition  &atd}
%parse-param {int &result}


%token atd_NUM atd_COMMENT atd_NEWLINE atd_LEAF atd_INTROVERTEX atd_INTROEDGE atd_FORGETVERTEX atd_JOIN atd_LEFTP atd_RIGHTP atd_SEP1 atd_SEP2
%type<string>  atd_COMMENT atd_NEWLINE atd_LEAF atd_INTROVERTEX atd_INTROEDGE atd_FORGETVERTEX atd_JOIN atd_LEFTP atd_RIGHTP atd_SEP1 atd_SEP2
%type<number> atd_NUM

%start atd_START


%%

atd_START                  : atd_NODE                            {if(!atd_construct(atd,labelToNode,labelToParentLabel)){YYERROR;}; std::cout<<"DONE!"<<std::endl;}
                           ;
atd_NODE                   : atd_COMMENTS atd_NUM atd_LEAF atd_NODES {std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> node(new TermNode<InstructiveTreeDecompositionNodeContent>);
 									InstructiveTreeDecompositionNodeContent atd_node;
 									atd_node.setSymbol("Leaf");
 									node->setNodeContent(atd_node);
 									if(labelToNode.count($2)){std::cout<< "label "<<$2<< " is repeated"<<std::endl; YYERROR;}
 									labelToNode[$2]=node;

                                    }
                           | atd_COMMENTS atd_NUM atd_INTROVERTEX atd_SEP1 atd_NUM atd_LEFTP atd_NUM atd_RIGHTP atd_NODES {
                                    std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> node(new TermNode<InstructiveTreeDecompositionNodeContent>);
 									InstructiveTreeDecompositionNodeContent atd_node;
									atd_node.setSymbol("IntroVertex_"+std::to_string($5));
									node->setNodeContent(atd_node);
 									if(labelToNode.count($2)){std::cout<< "label "<<$2<< " is repeated"<<std::endl; YYERROR;}
 									labelToNode[$2]=node;
 									if(labelToParentLabel.count($7)){std::cout<< "node "<<$7<<" has two parents" << labelToParentLabel[$7] << " and "<<$2<<std::endl; YYERROR;}
									labelToParentLabel[$7]=$2;

									}
                           | atd_COMMENTS atd_NUM atd_INTROEDGE atd_SEP1 atd_NUM atd_SEP1 atd_NUM atd_LEFTP atd_NUM atd_RIGHTP atd_NODES {
									std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> node(new TermNode<InstructiveTreeDecompositionNodeContent>);
 									InstructiveTreeDecompositionNodeContent atd_node;
									atd_node.setSymbol("IntroEdge_"+std::to_string($5)+"_"+std::to_string($7));
									node->setNodeContent(atd_node);
                            		if(labelToNode.count($2)){std::cout<< "label "<<$2<< " is repeated"<<std::endl; YYERROR;}
 									labelToNode[$2]=node;
 									if(labelToParentLabel.count($9)){std::cout<< "node "<<$9<<" has two parents" << labelToParentLabel[$9] << " and "<<$2<<std::endl; YYERROR;}
									labelToParentLabel[$9]=$2;
                                    
									}
                           | atd_COMMENTS atd_NUM atd_FORGETVERTEX atd_SEP1 atd_NUM atd_LEFTP atd_NUM atd_RIGHTP atd_NODES {
                                    std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> node(new TermNode<InstructiveTreeDecompositionNodeContent>);
 									InstructiveTreeDecompositionNodeContent atd_node;
									atd_node.setSymbol("ForgetVertex_"+std::to_string($5));
									node->setNodeContent(atd_node);
									if(labelToNode.count($2)){std::cout<< "label "<<$2<< " is repeated"<<std::endl; YYERROR;}
 									labelToNode[$2]=node;
 									if(labelToParentLabel.count($7)){std::cout<< "node "<<$7<<" has two parents" << labelToParentLabel[$7] << " and "<<$2<<std::endl; YYERROR;}
									labelToParentLabel[$7]=$2;
								    }
                           | atd_COMMENTS atd_NUM atd_JOIN atd_LEFTP atd_NUM atd_SEP2 atd_NUM atd_RIGHTP atd_NODES {
                                    std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> node(new TermNode<InstructiveTreeDecompositionNodeContent>);
 									InstructiveTreeDecompositionNodeContent atd_node;
									atd_node.setSymbol("Join");
									node->setNodeContent(atd_node);
                            		if(labelToNode.count($2)){std::cout<< "label "<<$2<< " is repeated"<<std::endl; YYERROR;}
 									labelToNode[$2]=node;
 									if(labelToParentLabel.count($5)){std::cout<< "node "<<$5<<" has two parents" << labelToParentLabel[$5] << " and "<<$2<<std::endl; YYERROR;}
									labelToParentLabel[$5]=$2;
                                	if(labelToParentLabel.count($7)){std::cout<< "node "<<$7<<" has two parents" << labelToParentLabel[$7] << " and "<<$2<<std::endl; YYERROR;}
									labelToParentLabel[$7]=$2;

                                    }
                           ;
atd_NODES                  : atd_NEWLINE atd_NODE
                           | atd_NEWLINE
                           |
                           ;
atd_COMMENTS               : atd_COMMENT atd_NEWLINE atd_COMMENTS                                              {}
                           |
                           ;


%%

void yyerror(InstructiveTreeDecomposition &/*atd*/, int &/*result*/, char const* msg){
  std::cerr<<"Syntax Error: "<< msg << " on line " <<atd_lineno << std::endl;
  // error printing  disabled, it is handeled in main.cpp
}

int atd_construct(InstructiveTreeDecomposition &atd,  std::map<int,std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>>> &labelToNode, std::map<int,int> &labelToParentLabel){
   std::set<int> rootCandidate;
   for(auto node:labelToNode){
        if(!labelToParentLabel.count(node.first)){
            if(rootCandidate.size()!=0){
                std::cout<< " there are more than two root candidate"<<std::endl;
                for(auto item:rootCandidate){
                    std::cout<<item<<",";
                }
                std::cout<<std::endl;
                return 0;
                //YYERROR;
            }else{
                rootCandidate.insert(node.first);
            }
        }else{

            std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> parent_node = labelToNode[labelToParentLabel[node.first]];
            std::vector<std::shared_ptr<TermNode<InstructiveTreeDecompositionNodeContent>> > children = parent_node->getChildren();
            children.push_back(node.second);
            node.second->setParent(parent_node);
            parent_node->setChildren(children);
            
        }
    }
    if(rootCandidate.size()!=1){
        std::cout<< "Candidate nodes for root are not valid"<<std::endl;
        for(auto item:rootCandidate){
            std::cout<<item<<",";
        }
        std::cout<<std::endl;
        return 0;
    
    }
    atd.setRoot(labelToNode[*rootCandidate.begin()]);
    return 1;
}
