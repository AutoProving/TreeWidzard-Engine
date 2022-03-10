#include "WitnessTreePACE.h"
void WitnessTreePACENode::print() {
    cout<< node_type <<endl;
    for(auto it : witnesses){
        it->print();
        cout<<endl;
    }
    for(auto it:children){
        it->print();
    }
}

shared_ptr<WitnessTreePACENode> WitnessTreePACE::generateNode(shared_ptr<StateTreeNode> snode, shared_ptr<WitnessTreePACENode> parent, unsigned joinno, DynamicKernel &kernel) {
    State::ptr q = snode->get_S();
    vector<shared_ptr<Witness> > witnesses;
    witnesses.resize(parent->witnesses.size());
    for(size_t i=0; i < parent->witnesses.size(); i++){
        if(parent->witnesses[i]){
            if(strstr(parent->node_type.c_str(),"IntroVertex_")){
                set<unsigned> setDiff;
                set<unsigned> p= snode->get_parent()->get_S()->get_bag().get_elements();
                set<unsigned> n= q->get_bag().get_elements();
                set_difference(p.begin(), p.end(), n.begin(), n.end(), inserter(setDiff, setDiff.begin()));
                if(setDiff.size()!=1){
                    cout<<"ERROR: in WitnessTreePACEN::generateNode intro vertex is not correct";
                    exit(20);
                }
                bool flag=false;
                for(auto itr= q->getWitnessSet(i)->begin(); itr != q->getWitnessSet(i)->end() ; itr++){
                    if(flag){
                        break;
                    }else{
                        Bag b = q->get_bag();
                        shared_ptr<WitnessSet> w = kernel.pointerToCoreNumber(i)->intro_v(*setDiff.begin(),b ,**itr);
                        for(auto t = w->begin(); t!=w->end();t++){
                            if( **t == *parent->witnesses[i]){
                                witnesses[i]=*itr;
                                flag=true;
                                break;
                            }
                        }
                    }
                }
            }else if(strstr(parent->node_type.c_str(),"ForgetVertex_")){
                set<unsigned> setDiff;
                set<unsigned> p= snode->get_parent()->get_S()->get_bag().get_elements();
                set<unsigned> n= q->get_bag().get_elements();
                set_difference(n.begin(), n.end(), p.begin(), p.end(), inserter(setDiff, setDiff.begin()));
                if(setDiff.size()!=1){
                    cout<<"ERROR: in WitnessTreePACEN::generateNode forget vertex is not correct";
                    exit(20);
                }
                bool flag=false;
                for(auto itr=q->getWitnessSet(i)->begin();itr!=q->getWitnessSet(i)->end();itr++){
                    if(flag){
                        break;
                    }else{
                        Bag b = q->get_bag();
                        shared_ptr<WitnessSet> w = kernel.pointerToCoreNumber(i)->forget_v(*setDiff.begin(),b ,**itr);
                        for(auto t = w->begin(); t!=w->end();t++){
                            if( **t == *parent->witnesses[i]){
                                witnesses[i]=*itr;
                                flag=true;
                                break;
                            }
                        }
                    }
                }

            }else if(strstr(parent->node_type.c_str(),"IntroEdge_")){
                pair<unsigned,unsigned> e = snode->get_parent()->get_S()->get_bag().get_edge();
                if(e.first == 0 or e.second==0){
                    cout<<"ERROR: in WitnessTreePACEN::generateNode intro edge is not correct";
                    exit(20);
                }
                bool flag=false;
                for(auto itr=q->getWitnessSet(i)->begin();itr!=q->getWitnessSet(i)->end();itr++){
                    if(flag){
                        break;
                    }else{
                        Bag b = q->get_bag();
                        shared_ptr<WitnessSet> w = kernel.pointerToCoreNumber(i)->intro_e(e.first, e.second,b ,**itr);
                        for(auto t = w->begin(); t!=w->end();t++){
                            if( **t == *parent->witnesses[i]){
                                witnesses[i]=*itr;
                                flag=true;
                                break;
                            }
                        }
                    }
                }
            }else if(strstr(parent->node_type.c_str(),"Join")){
                State::ptr nq = snode->get_parent()->get_children()[1-joinno]->get_S();
                bool flag=false;
                for(auto itr=q->getWitnessSet(i)->begin();itr!=q->getWitnessSet(i)->end();itr++){
                    if(flag){
                        break;
                    }else{
                        Bag b = q->get_bag();
                        shared_ptr<WitnessSet> nqw = nq->getWitnessSet(i);
                        for(auto it=nqw->begin(); it!= nqw->end();it++){
                            if(flag){
                                break;
                            }else{
                                shared_ptr<WitnessSet> w = kernel.pointerToCoreNumber(i)->join(b ,**itr,**it);
                                for(auto t = w->begin(); t!=w->end();t++){
                                    if( **t == *parent->witnesses[i]){
                                        witnesses[i]=*itr;
                                        flag=true;
                                        break;
                                    }
                                }
                            }
                        }

                    }
                }
            }else{
                cout<<"ERROR: in WitnessTreePACEN::generateNode node's type is not correct"<<endl;
                exit(20);
            }
        }
    }
    shared_ptr<WitnessTreePACENode> node(new WitnessTreePACENode);
    node->node_type = snode->get_nodeType();
    node->witnesses = witnesses;
    node->parent=parent;
    for(size_t c =0; c<snode->get_children().size(); c++ ){
        node->children.push_back(generateNode(snode->get_children()[c],node, c, kernel));
    }
    return node;
}

bool WitnessTreePACE::stateTreeToWitnessTreePACE(StateTree &stateTree, DynamicKernel &kernel) {
    shared_ptr<WitnessTreePACENode> node (new WitnessTreePACENode);
    State::ptr q = stateTree.root->get_S();
    //vector<shared_ptr<WitnessSet> > witnessSet = q->get_witnessSetVector();
    vector<shared_ptr<Witness>> witnesses;
    witnesses.resize(q->numberOfComponents());
    for(size_t i = 0; i< q->numberOfComponents();i++){
        for(auto it=q->getWitnessSet(i)->begin(); it!= q->getWitnessSet(i)->end();it++){
            if(kernel.pointerToCoreNumber(i)->is_final_witness(**it)){
                witnesses[i]=*it;
                break;
            }
        }
        if(witnesses[i]==NULL){
            cout<<"The root node doesn't have a final witness"<<endl;
            return false;
        }
    }
    node->node_type = stateTree.root->get_nodeType();
    node->witnesses = witnesses;
    for(size_t c =0; c< stateTree.root->get_children().size(); c++ ){
        node->children.push_back(generateNode(stateTree.root->get_children()[c],node, c, kernel));
    }
    this->root = node;
    return true;
}
void WitnessTreePACE::print() {
    root->print();
}
