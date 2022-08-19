%defines
%define api.prefix {ctd_}
%code requires {
    #include "../TreeAutomaton/ConcreteTreeDecomposition.h"
    #include <iostream>
    #include <vector>
    #include <memory>
    #include <set>
    #include <tuple>
}
%{
    #include <iostream>
    #include <cmath>
    #include <vector>
    #include <memory>
    #include <set>
    #include <tuple>
    #include "../TreeAutomaton/ConcreteTreeDecomposition.h"
    #include <algorithm>

    // using flex
    extern int yylex();
    extern int ctd_lineno;
    extern void yyerror(ConcreteTreeDecomposition &ctd, int &result, char const* msg);

	// First coordinate is for node, second coordinate is for labels of the children of the node, third coordinate is for label of the node
	// forth coordinate is for integers inside the symbol.
	//Example 4 IntroVertex_2 (3) => 4 is the third coordinate, 2 is the forth, {3} is the second.
    vector<tuple<shared_ptr<TermNode<ConcreteNode>>,set<int>,int,set<int>>> ctdVec;
   	// It is the positions of the children of a node in ctdVec. It construts inside ctd_construct.
    vector<set<int>> ctdChildPos;
    int ctd_construct(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<TermNode<ConcreteNode>>,set<int>,int,set<int>>> &ctdVec,vector<set<int>> &ctdChildPos);
    int ctd_constructBags(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<TermNode<ConcreteNode>>,set<int>,int,set<int>>> &ctdVec,vector<set<int>> &ctdChildPos, int nodeNum);
  %}
%locations
%union{
     unsigned number;
     char* string;
}

%parse-param {ConcreteTreeDecomposition  &ctd}
%parse-param {int &result}


%token CTD_NUM CTD_COMMENT CTD_NEWLINE CTD_EMPTY CTD_INTROVERTEX CTD_INTROEDGE CTD_FORGETVERTEX CTD_JOIN CTD_LEFTP CTD_RIGHTP CTD_SEP1 CTD_SEP2
%type<string>  CTD_COMMENT CTD_NEWLINE CTD_EMPTY CTD_INTROVERTEX CTD_INTROEDGE CTD_FORGETVERTEX CTD_JOIN CTD_LEFTP CTD_RIGHTP CTD_SEP1 CTD_SEP2
%type<number> CTD_NUM

%start CTD_START


%%

CTD_START                  : CTD_NODE                            {int r = ctd_construct(ctd, ctdVec, ctdChildPos); if(r==-1){YYERROR;}; if(!ctd_constructBags(ctd,ctdVec,ctdChildPos,r)){YYERROR;}}
                           ;
CTD_NODE                   : CTD_COMMENTS CTD_NUM CTD_EMPTY CTD_NODES {shared_ptr<TermNode<ConcreteNode>> node(new TermNode<ConcreteNode>);
 									ConcreteNode concrete;
 									concrete.setSymbol("Leaf");
 									node->setNodeContent(concrete);
                                                                        set<int> elements;
                                                                        set<int> children; ctdVec.push_back(make_tuple(node,children,$2,elements));
                                                                        set<int> childPos; ctdChildPos.push_back(childPos);
                                                                        }
                           | CTD_COMMENTS CTD_NUM CTD_INTROVERTEX CTD_SEP1 CTD_NUM CTD_LEFTP CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<TermNode<ConcreteNode>> node(new TermNode<ConcreteNode>);
															ConcreteNode concrete;
															concrete.setSymbol("IntroVertex_"+to_string($5));
															node->setNodeContent(concrete);
														        set<int> children; children.insert($7);
														        set<int> elements; elements.insert($5);
														        ctdVec.push_back(make_tuple(node,children,$2,elements));
														        set<int> childPos; ctdChildPos.push_back(childPos);
														        }
                           | CTD_COMMENTS CTD_NUM CTD_INTROEDGE CTD_SEP1 CTD_NUM CTD_SEP1 CTD_NUM CTD_LEFTP CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<TermNode<ConcreteNode>> node(new TermNode<ConcreteNode>);
																	ConcreteNode concrete;
																	concrete.setSymbol("IntroEdge_"+to_string($5)+"_"+to_string($7));
																	node->setNodeContent(concrete);
																 	set<int> children; children.insert($9);
																   	set<int> elements; elements.insert($5); elements.insert($7);
																   	ctdVec.push_back(make_tuple(node,children,$2,elements));
																   	set<int> childPos; ctdChildPos.push_back(childPos);
																   	}
                           | CTD_COMMENTS CTD_NUM CTD_FORGETVERTEX CTD_SEP1 CTD_NUM CTD_LEFTP CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<TermNode<ConcreteNode>> node(new TermNode<ConcreteNode>);
															ConcreteNode concrete;
															concrete.setSymbol("ForgetVertex_"+to_string($5));
															node->setNodeContent(concrete);
														       	set<int> children; children.insert($7);
														       	set<int> elements; elements.insert($5);
														       	ctdVec.push_back(make_tuple(node,children,$2,elements));
														       	set<int> childPos; ctdChildPos.push_back(childPos);
														       }
                           | CTD_COMMENTS CTD_NUM CTD_JOIN CTD_LEFTP CTD_NUM CTD_SEP2 CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<TermNode<ConcreteNode>> node(new TermNode<ConcreteNode>);
														ConcreteNode concrete;
														concrete.setSymbol("Join");
														node->setNodeContent(concrete);
                                                                                                               	set<int> children; children.insert($5); children.insert($7);
                                                                                                               	set<int> elements;
                                                                                                               	ctdVec.push_back(make_tuple(node,children,$2,elements));
                                                                                                               	set<int> childPos; ctdChildPos.push_back(childPos);
                                                                                                               	}
                           ;
CTD_NODES                  : CTD_NEWLINE CTD_NODE
                           | CTD_NEWLINE
                           |
                           ;
CTD_COMMENTS               : CTD_COMMENT CTD_NEWLINE CTD_COMMENTS                                              {}
                           |
                           ;


%%

void yyerror(ConcreteTreeDecomposition &ctd, int &result, char const* msg){
  std::cerr<<"Syntax Error: "<< msg << " on line " <<ctd_lineno << std::endl;
  // error printing  disabled, it is handeled in main.cpp
}

int ctd_construct(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<TermNode<ConcreteNode>>,set<int>,int,set<int>>> &ctdVec,vector<set<int>> &ctdChildPos){
    for(int k = 0 ; k < ctdVec.size(); k++){
        /*cout<<get<2>(ctdVec[k])<< " " << get<0>(ctdVec[k])->get_nodeType()<< " ";
        for(auto e:get<1>(ctdVec[k])){
            cout<<e<<" ";
        }
        cout<<endl;*/
        vector<shared_ptr<TermNode<ConcreteNode>>> children;
        for(int i = 0 ; i < ctdVec.size();i++){
            if( get<1>(ctdVec[k]).count(get<2>(ctdVec[i]))){
                get<0>(ctdVec[i])->setParent(get<0>(ctdVec[k]));
                children.push_back(get<0>(ctdVec[i]));
                ctdChildPos[k].insert(i);
            }
        }
        // Check that the number of the childre is correct. For example, Join should has 2 children.
        if(get<0>(ctdVec[k])->getNodeContent().getSymbol()=="Join"){
            if(children.size()!=2){
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[k])<< " " << get<0>(ctdVec[k])->getNodeContent().getSymbol()<< "' ";
                return -1;
                //yyerror(ctd, result, "Decomposition is not correct");
            }
        }else if(get<0>(ctdVec[k])->getNodeContent().getSymbol()=="Leaf"){
            if(children.size()!=0){
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[k])<< " " << get<0>(ctdVec[k])->getNodeContent().getSymbol()<< "' ";
                return -1;
                //yyerror(ctd, result, "Decomposition is not correct");
          }
        }else{
            if(children.size()!=1){
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[k])<< " " << get<0>(ctdVec[k])->getNodeContent().getSymbol()<< "' ";
                return -1;
                //yyerror(ctd, result, "Decomposition is not correct");
            }
        }

        get<0>(ctdVec[k])->setChildren(children);
    }
    // Find root
    set<int> rootIndex;
    for(int i=0; i< ctdVec.size();i++){
        if(get<0>(ctdVec[i])->getParent()==nullptr){
            rootIndex.insert(i);
        }
    }
    if(rootIndex.size()!=1){
        cerr<<"Error: Decomposition is not correct. " ;
        if(rootIndex.size()==0){
            cerr<<"There is not root."<<endl;
        }else{
            cerr<<"There are several root."<<endl;
        }
        return -1;
    }else{
        ctd.setRoot(get<0>(ctdVec[*(rootIndex.begin())]));
    }
    return *(rootIndex.begin());
}

int ctd_constructBags(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<TermNode<ConcreteNode>>,set<int>,int,set<int>>> &ctdVec, vector<set<int>> &ctdChildPos, int nodeNum){
    if(get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol()=="Leaf"){
        return 1;
    }else{
        for(auto n:ctdChildPos[nodeNum])
            if(!ctd_constructBags(ctd,ctdVec,ctdChildPos,n)) return 0;
	    if(strstr(get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol().c_str(),"IntroVertex")){
            int i = *(get<3>(ctdVec[nodeNum]).begin());
            int n = *(ctdChildPos[nodeNum].begin());
            if(get<0>(ctdVec[n])->getNodeContent().getBag().vertex_introducible(i)){
                ConcreteNode concrete;
                concrete.setSymbol(get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol());
                Bag bag = get<0>(ctdVec[n])->getNodeContent().getBag();
                concrete.setBag(bag.intro_v(i));
                get<0>(ctdVec[nodeNum])->setNodeContent(concrete);
            }else{
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[nodeNum])<< " " << get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol()<< "( "<<n<<" )" <<"' ";
                return 0;
            }
        }else if(strstr(get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol().c_str(),"ForgetVertex")){
              int i = *(get<3>(ctdVec[nodeNum]).begin());
              int n = *(ctdChildPos[nodeNum].begin());
              if(get<0>(ctdVec[n])->getNodeContent().getBag().vertex_forgettable(i)){
              	ConcreteNode concrete;
	      	concrete.setSymbol(get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol());
		Bag bag = get<0>(ctdVec[n])->getNodeContent().getBag();
		concrete.setBag(bag.forget_v(i));
		get<0>(ctdVec[nodeNum])->setNodeContent(concrete);
              }else{
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[nodeNum])<< " " << get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol()<< "( "<<n<<" )" <<"' ";
		return 0;
              }
         }else if(strstr(get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol().c_str(),"IntroEdge")){
            int i = *(get<3>(ctdVec[nodeNum]).begin());
            int j = *(++get<3>(ctdVec[nodeNum]).begin());
            int n = *(ctdChildPos[nodeNum].begin());
            if(get<0>(ctdVec[n])->getNodeContent().getBag().edge_introducible(i,j)){
                 ConcreteNode concrete;
		 concrete.setSymbol(get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol());
		 Bag bag = get<0>(ctdVec[n])->getNodeContent().getBag();
		 concrete.setBag(bag.intro_e(i,j));
		 get<0>(ctdVec[nodeNum])->setNodeContent(concrete);
            }else{
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[nodeNum])<< " " << get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol()<< "( "<<n<<" )" <<"' ";
                return 0;
           }
        } else if(strstr(get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol().c_str(),"Join")){

	  int child1 = *(ctdChildPos[nodeNum].begin());
	  int child2 = *(++ctdChildPos[nodeNum].begin());
	  Bag bag1 = get<0>(ctdVec[child1])->getNodeContent().getBag();
	  Bag bag2 = get<0>(ctdVec[child2])->getNodeContent().getBag();
          if(bag1.joinable(bag2)){
              	ConcreteNode concrete;
	 	concrete.setSymbol(get<0>(ctdVec[child1])->getNodeContent().getSymbol());
	 	concrete.setBag(bag1);
	 	get<0>(ctdVec[nodeNum])->setNodeContent(concrete);
          }else{
            cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[nodeNum])<< " " << get<0>(ctdVec[nodeNum])->getNodeContent().getSymbol()<< "( "<<child1<<", "<<child2<<" )" <<"' ";
            return 0;
          }
        }else{
         return 0;
        }
    }
    return 1;
}


