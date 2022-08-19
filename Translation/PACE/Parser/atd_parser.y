%defines
%define api.prefix {atd_}
%code requires {
    #include "../../../TreeAutomaton/AbstractTreeDecomposition.h"
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
    #include "../../../TreeAutomaton/AbstractTreeDecomposition.h"
    #include <algorithm>

    // using flex
    extern int yylex();
    extern int atd_lineno;
    extern void yyerror(AbstractTreeDecomposition &atd, int &result, char const* msg);

	// First coordinate is for node, second coordinate is for labels of the children of the node, third coordinate is for label of the node
	// forth coordinate is for integers inside the symbol.
	//Example 4 IntroVertex_2 (3) => 4 is the third coordinate, 2 is the forth, {3} is the second.
   // vector<tuple<shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>>,int,set<int>>> atdVec;
   	// It is the positions of the children of a node in atdVec. It construts inside atd_construct.
    map<int,shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>>> labelToNode;
    map<int,int> labelToParentLabel;
    int atd_construct(AbstractTreeDecomposition &atd, map<int,shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>>> &labelToNode, map<int,int> &labelToParentLabel);
  %}
%locations
%union{
     unsigned number;
     char* string;
}

%parse-param {AbstractTreeDecomposition  &atd}
%parse-param {int &result}


%token atd_NUM atd_COMMENT atd_NEWLINE atd_LEAF atd_INTROVERTEX atd_INTROEDGE atd_FORGETVERTEX atd_JOIN atd_LEFTP atd_RIGHTP atd_SEP1 atd_SEP2
%type<string>  atd_COMMENT atd_NEWLINE atd_LEAF atd_INTROVERTEX atd_INTROEDGE atd_FORGETVERTEX atd_JOIN atd_LEFTP atd_RIGHTP atd_SEP1 atd_SEP2
%type<number> atd_NUM

%start atd_START


%%

atd_START                  : atd_NODE                            {if(!atd_construct(atd,labelToNode,labelToParentLabel)){YYERROR;}; cout<<"DONE!"<<endl;}
                           ;
atd_NODE                   : atd_COMMENTS atd_NUM atd_LEAF atd_NODES {shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> node(new TermNode<AbstractTreeDecompositionNodeContent>);
 									AbstractTreeDecompositionNodeContent atd_node;
 									atd_node.setSymbol("Leaf");
 									node->setNodeContent(atd_node);
 									if(labelToNode.count($2)){cout<< "label "<<$2<< " is repeated"<<endl; YYERROR;}
 									labelToNode[$2]=node;

                                    }
                           | atd_COMMENTS atd_NUM atd_INTROVERTEX atd_SEP1 atd_NUM atd_LEFTP atd_NUM atd_RIGHTP atd_NODES {
                                    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> node(new TermNode<AbstractTreeDecompositionNodeContent>);
 									AbstractTreeDecompositionNodeContent atd_node;
									atd_node.setSymbol("IntroVertex_"+to_string($5));
									node->setNodeContent(atd_node);
 									if(labelToNode.count($2)){cout<< "label "<<$2<< " is repeated"<<endl; YYERROR;}
 									labelToNode[$2]=node;
 									if(labelToParentLabel.count($7)){cout<< "node "<<$7<<" has two parents" << labelToParentLabel[$7] << " and "<<$2<<endl; YYERROR;}
									labelToParentLabel[$7]=$2;

									}
                           | atd_COMMENTS atd_NUM atd_INTROEDGE atd_SEP1 atd_NUM atd_SEP1 atd_NUM atd_LEFTP atd_NUM atd_RIGHTP atd_NODES {
									shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> node(new TermNode<AbstractTreeDecompositionNodeContent>);
 									AbstractTreeDecompositionNodeContent atd_node;
									atd_node.setSymbol("IntroEdge_"+to_string($5)+"_"+to_string($7));
									node->setNodeContent(atd_node);
                            		if(labelToNode.count($2)){cout<< "label "<<$2<< " is repeated"<<endl; YYERROR;}
 									labelToNode[$2]=node;
 									if(labelToParentLabel.count($9)){cout<< "node "<<$9<<" has two parents" << labelToParentLabel[$9] << " and "<<$2<<endl; YYERROR;}
									labelToParentLabel[$9]=$2;
                                    
									}
                           | atd_COMMENTS atd_NUM atd_FORGETVERTEX atd_SEP1 atd_NUM atd_LEFTP atd_NUM atd_RIGHTP atd_NODES {
                                    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> node(new TermNode<AbstractTreeDecompositionNodeContent>);
 									AbstractTreeDecompositionNodeContent atd_node;
									atd_node.setSymbol("ForgetVertex_"+to_string($5));
									node->setNodeContent(atd_node);
									if(labelToNode.count($2)){cout<< "label "<<$2<< " is repeated"<<endl; YYERROR;}
 									labelToNode[$2]=node;
 									if(labelToParentLabel.count($7)){cout<< "node "<<$7<<" has two parents" << labelToParentLabel[$7] << " and "<<$2<<endl; YYERROR;}
									labelToParentLabel[$7]=$2;
								    }
                           | atd_COMMENTS atd_NUM atd_JOIN atd_LEFTP atd_NUM atd_SEP2 atd_NUM atd_RIGHTP atd_NODES {
                                    shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> node(new TermNode<AbstractTreeDecompositionNodeContent>);
 									AbstractTreeDecompositionNodeContent atd_node;
									atd_node.setSymbol("Join");
									node->setNodeContent(atd_node);
                            		if(labelToNode.count($2)){cout<< "label "<<$2<< " is repeated"<<endl; YYERROR;}
 									labelToNode[$2]=node;
 									if(labelToParentLabel.count($5)){cout<< "node "<<$5<<" has two parents" << labelToParentLabel[$5] << " and "<<$2<<endl; YYERROR;}
									labelToParentLabel[$5]=$2;
                                	if(labelToParentLabel.count($7)){cout<< "node "<<$7<<" has two parents" << labelToParentLabel[$7] << " and "<<$2<<endl; YYERROR;}
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

void yyerror(AbstractTreeDecomposition &atd, int &result, char const* msg){
  std::cerr<<"Syntax Error: "<< msg << " on line " <<atd_lineno << std::endl;
  // error printing  disabled, it is handeled in main.cpp
}

int atd_construct(AbstractTreeDecomposition &atd,  map<int,shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>>> &labelToNode, map<int,int> &labelToParentLabel){
   set<int> rootCandidate;
   for(auto node:labelToNode){
        if(!labelToParentLabel.count(node.first)){
            if(rootCandidate.size()!=0){
                cout<< " there are more than two root candidate"<<endl;
                for(auto item:rootCandidate){
                    cout<<item<<",";
                }
                cout<<endl;
                return 0;
                //YYERROR;
            }else{
                rootCandidate.insert(node.first);
            }
        }else{

            shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> parent_node = labelToNode[labelToParentLabel[node.first]];
            vector<shared_ptr<TermNode<AbstractTreeDecompositionNodeContent>> > children = parent_node->getChildren();
            children.push_back(node.second);
            node.second->setParent(parent_node);
            parent_node->setChildren(children);
            
        }
    }
    if(rootCandidate.size()!=1){
        cout<< "Candidate nodes for root are not valid"<<endl;
        for(auto item:rootCandidate){
            cout<<item<<",";
        }
        cout<<endl;
        return 0;
    
    }
    atd.setRoot(labelToNode[*rootCandidate.begin()]);
    return 1;
}


