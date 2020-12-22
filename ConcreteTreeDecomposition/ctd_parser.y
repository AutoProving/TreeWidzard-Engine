%defines
%define api.prefix {ctd_}
%code requires {
    #include "../Kernel/ConcreteTreeDecomposition.h"
    #include <iostream>
    #include <vector>
    #include <memory>
    #include <set>
    #include <tuple>
}
%{
    #include <iostream>
    #include <string.h>
    #include <cmath>
    #include <vector>
    #include <memory>
    #include <set>
    #include <tuple>
    #include "../Kernel/ConcreteTreeDecomposition.h"
    #include <algorithm>
   
    // this function will be generated
    // using flex
    extern int yylex();
    extern int ctd_lineno;
    extern void yyerror(ConcreteTreeDecomposition &ctd, int &result, char const* msg);
    vector<tuple<shared_ptr<CTDNodeNew>,set<int>,int>> ctdVec;
    int ctd_construct(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<CTDNodeNew>,set<int>,int>> &ctdVec, int &result);
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

CTD_START                  : CTD_NODE                            {if(ctd_construct(ctd, ctdVec,result)){YYERROR;};}
                           ;
CTD_NODE                   : CTD_COMMENTS CTD_NUM CTD_EMPTY CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew); node->set_nodeType("Empty");
                                                                        set<int> children; ctdVec.push_back(make_tuple(node,children,$2));}
                           | CTD_COMMENTS CTD_NUM CTD_INTROVERTEX CTD_SEP1 CTD_NUM CTD_LEFTP CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew);
                                                                                                                            node->set_nodeType("IntroVertex_"+to_string($5));
                                                                                                                            set<int> children; children.insert($7);
                                                                                                                            ctdVec.push_back(make_tuple(node,children,$2));}
                           | CTD_COMMENTS CTD_NUM CTD_INTROEDGE CTD_SEP1 CTD_NUM CTD_SEP1 CTD_NUM CTD_LEFTP CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew);
                                                                                                                                           node->set_nodeType("IntroEdge_"+to_string($5)+"_"+to_string($7));
                                                                                                                                           set<int> children; children.insert($9);
                                                                                                                                           ctdVec.push_back(make_tuple(node,children,$2));}
                           | CTD_COMMENTS CTD_NUM CTD_FORGETVERTEX CTD_SEP1 CTD_NUM CTD_LEFTP CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew);
                                                                                                                               node->set_nodeType("ForgetVertex_"+to_string($5));
                                                                                                                               set<int> children; children.insert($7);
                                                                                                                               ctdVec.push_back(make_tuple(node,children,$2));}
                           | CTD_COMMENTS CTD_NUM CTD_JOIN CTD_LEFTP CTD_NUM CTD_SEP2 CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew);
                                                                                                               node->set_nodeType("Join");
                                                                                                               set<int> children; children.insert($5); children.insert($7);
                                                                                                               ctdVec.push_back(make_tuple(node,children,$2));}
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

int ctd_construct(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<CTDNodeNew>,set<int>,int>> &ctdVec, int &result){
    for(auto& node1:ctdVec){
        /*cout<<get<2>(node1)<< " " << get<0>(node1)->get_nodeType()<< " ";
        for(auto e:get<1>(node1)){
            cout<<e<<" ";
        }
        cout<<endl;*/
        vector<shared_ptr<CTDNodeNew>> children;
        for(auto& node2:ctdVec){
            if( get<1>(node1).count(get<2>(node2))){
                get<0>(node2)->set_parent(get<0>(node1));
                children.push_back(get<0>(node2));
            }
        }
        if(get<0>(node1)->get_nodeType()=="Join"){
            if(children.size()!=2){

                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(node1)<< " " << get<0>(node1)->get_nodeType()<< "' ";
                result = 1;
                return result;
                //yyerror(ctd, result, "Decomposition is not correct");
            }
        }else if(get<0>(node1)->get_nodeType()=="Empty"){
            if(children.size()!=0){
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(node1)<< " " << get<0>(node1)->get_nodeType()<< "' ";
                result = 1;
                return result;
                //yyerror(ctd, result, "Decomposition is not correct");
          }
        }else{
            if(children.size()!=1){
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(node1)<< " " << get<0>(node1)->get_nodeType()<< "' ";
                result = 1;
                return result;
                //yyerror(ctd, result, "Decomposition is not correct");
            }
        }

        get<0>(node1)->set_children(children);
    }
    // Find root
    set<int> rootIndex;

    for(int i=0; i< ctdVec.size();i++){
        if(get<0>(ctdVec[i])->get_parent()==nullptr){
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
        result = 1;
        return result;
    }else{
        ctd.set_root(get<0>(ctdVec[*(rootIndex.begin())]));
    }

}


