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

    vector<tuple<shared_ptr<CTDNodeNew>,set<int>,int,set<int>>> ctdVec;
    vector<set<int>> ctdChildPos;
    int ctd_construct(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<CTDNodeNew>,set<int>,int,set<int>>> &ctdVec,vector<set<int>> &ctdChildPos);
    int ctd_constructBags(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<CTDNodeNew>,set<int>,int,set<int>>> &ctdVec,vector<set<int>> &ctdChildPos, int nodeNum);
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
CTD_NODE                   : CTD_COMMENTS CTD_NUM CTD_EMPTY CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew); node->set_nodeType("Empty");
                                                                        set<int> elements;
                                                                        set<int> children; ctdVec.push_back(make_tuple(node,children,$2,elements));
                                                                        set<int> childPos; ctdChildPos.push_back(childPos);
                                                                        }
                           | CTD_COMMENTS CTD_NUM CTD_INTROVERTEX CTD_SEP1 CTD_NUM CTD_LEFTP CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew);
                                                                                                                            node->set_nodeType("IntroVertex_"+to_string($5));
                                                                                                                            set<int> children; children.insert($7);
                                                                                                                            set<int> elements; elements.insert($5);
                                                                                                                            ctdVec.push_back(make_tuple(node,children,$2,elements));
                                                                                                                            set<int> childPos; ctdChildPos.push_back(childPos);
                                                                                                                            }
                           | CTD_COMMENTS CTD_NUM CTD_INTROEDGE CTD_SEP1 CTD_NUM CTD_SEP1 CTD_NUM CTD_LEFTP CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew);
                                                                                                                                           node->set_nodeType("IntroEdge_"+to_string($5)+"_"+to_string($7));
                                                                                                                                           set<int> children; children.insert($9);
                                                                                                                                           set<int> elements; elements.insert($5); elements.insert($7);
                                                                                                                                           ctdVec.push_back(make_tuple(node,children,$2,elements));
                                                                                                                                           set<int> childPos; ctdChildPos.push_back(childPos);
                                                                                                                                           }
                           | CTD_COMMENTS CTD_NUM CTD_FORGETVERTEX CTD_SEP1 CTD_NUM CTD_LEFTP CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew);
                                                                                                                               node->set_nodeType("ForgetVertex_"+to_string($5));
                                                                                                                               set<int> children; children.insert($7);
                                                                                                                               set<int> elements; elements.insert($5);
                                                                                                                               ctdVec.push_back(make_tuple(node,children,$2,elements));
                                                                                                                               set<int> childPos; ctdChildPos.push_back(childPos);
                                                                                                                               }
                           | CTD_COMMENTS CTD_NUM CTD_JOIN CTD_LEFTP CTD_NUM CTD_SEP2 CTD_NUM CTD_RIGHTP CTD_NODES {shared_ptr<CTDNodeNew> node(new CTDNodeNew);
                                                                                                               node->set_nodeType("Join");
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

int ctd_construct(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<CTDNodeNew>,set<int>,int,set<int>>> &ctdVec,vector<set<int>> &ctdChildPos){
    for(int k = 0 ; k<ctdVec.size(); k++){
        /*cout<<get<2>(ctdVec[k])<< " " << get<0>(ctdVec[k])->get_nodeType()<< " ";
        for(auto e:get<1>(ctdVec[k])){
            cout<<e<<" ";
        }
        cout<<endl;*/
        vector<shared_ptr<CTDNodeNew>> children;

        for(int i = 0 ; i<ctdVec.size();i++){
            if( get<1>(ctdVec[k]).count(get<2>(ctdVec[i]))){
                get<0>(ctdVec[i])->set_parent(get<0>(ctdVec[k]));
                children.push_back(get<0>(ctdVec[i]));
                ctdChildPos[k].insert(i);
            }
        }
        if(get<0>(ctdVec[k])->get_nodeType()=="Join"){
            if(children.size()!=2){

                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[k])<< " " << get<0>(ctdVec[k])->get_nodeType()<< "' ";
                return -1;
                //yyerror(ctd, result, "Decomposition is not correct");
            }
        }else if(get<0>(ctdVec[k])->get_nodeType()=="Empty"){
            if(children.size()!=0){
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[k])<< " " << get<0>(ctdVec[k])->get_nodeType()<< "' ";
                return -1;
                //yyerror(ctd, result, "Decomposition is not correct");
          }
        }else{
            if(children.size()!=1){
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[k])<< " " << get<0>(ctdVec[k])->get_nodeType()<< "' ";
                return -1;
                //yyerror(ctd, result, "Decomposition is not correct");
            }
        }

        get<0>(ctdVec[k])->set_children(children);
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
        return -1;
    }else{
        ctd.set_root(get<0>(ctdVec[*(rootIndex.begin())]));
    }
    return *(rootIndex.begin());
}

int ctd_constructBags(ConcreteTreeDecomposition &ctd, vector<tuple<shared_ptr<CTDNodeNew>,set<int>,int,set<int>>> &ctdVec, vector<set<int>> &ctdChildPos, int nodeNum){
    if(get<0>(ctdVec[nodeNum])->get_nodeType()=="Empty"){
        return 1;
    }else{
        for(auto n:ctdChildPos[nodeNum])
            if(!ctd_constructBags(ctd,ctdVec,ctdChildPos,n)) return 0;
        if(strstr(get<0>(ctdVec[nodeNum])->get_nodeType().c_str(),"IntroVertex")){
            int i = *(get<3>(ctdVec[nodeNum]).begin());
            int n = *(ctdChildPos[nodeNum].begin());
            if(get<0>(ctdVec[n])->get_B().vertex_introducible(i)){
                get<0>(ctdVec[nodeNum])->set_B(get<0>(ctdVec[n])->get_B().intro_v(i));
            }else{
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[nodeNum])<< " " << get<0>(ctdVec[nodeNum])->get_nodeType()<< "( "<<n<<" )" <<"' ";
                return 0;
            }
        }else if(strstr(get<0>(ctdVec[nodeNum])->get_nodeType().c_str(),"ForgetVertex")){
              int i = *(get<3>(ctdVec[nodeNum]).begin());
              int n = *(ctdChildPos[nodeNum].begin());
              if(get<0>(ctdVec[n])->get_B().vertex_forgettable(i)){
                  get<0>(ctdVec[nodeNum])->set_B(get<0>(ctdVec[n])->get_B().forget_v(i));
              }else{
                   cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[nodeNum])<< " " << get<0>(ctdVec[nodeNum])->get_nodeType()<< "( "<<n<<" )" <<"' ";
                   return 0;
              }
         }else if(strstr(get<0>(ctdVec[nodeNum])->get_nodeType().c_str(),"IntroEdge")){
            int i = *(get<3>(ctdVec[nodeNum]).begin());
            int j = *(++get<3>(ctdVec[nodeNum]).begin());
            int n = *(ctdChildPos[nodeNum].begin());
            if(get<0>(ctdVec[n])->get_B().edge_introducible(i,j)){
                 get<0>(ctdVec[nodeNum])->set_B(get<0>(ctdVec[n])->get_B().intro_e(i,j));
            }else{
                cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[nodeNum])<< " " << get<0>(ctdVec[nodeNum])->get_nodeType()<< "( "<<n<<" )" <<"' ";
                return 0;
           }
        } else if(strstr(get<0>(ctdVec[nodeNum])->get_nodeType().c_str(),"Join")){

            int child1 = *(ctdChildPos[nodeNum].begin());
            int child2 = *(++ctdChildPos[nodeNum].begin());
          if(get<0>(ctdVec[child1])->get_B().joinable(get<0>(ctdVec[child2])->get_B())){
               get<0>(ctdVec[nodeNum])->set_B(get<0>(ctdVec[child1])->get_B());
          }else{
            cerr<<"Error: Decomposition is not correct on node '" <<get<2>(ctdVec[nodeNum])<< " " << get<0>(ctdVec[nodeNum])->get_nodeType()<< "( "<<child1<<", "<<child2<<" )" <<"' ";
            return 0;
          }
        }else{
         return 0;
        }
    }
    return 1;
}


