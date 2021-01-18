#include "HasMultipleEdges.h"

extern "C" {
DynamicCore * create() {
    return new HasMultipleEdges_DynamicCore;
}
}


bool HasMultipleEdges_Witness::is_equal(const Witness &rhs)const {
    if (HasMultipleEdges_Witness const *e = dynamic_cast<HasMultipleEdges_Witness const *>(&rhs)) {
        shared_ptr<HasMultipleEdges_Witness const> p = e->shared_from_this();
        return this->edgeSet == p->edgeSet and this->found == p->found;
    }else{
        cerr<<"ERROR: in HasMultipleEdges_Witness::is_equal cast error"<<endl;
        exit(20);
    }

}

bool HasMultipleEdges_Witness::is_less(const Witness &rhs )const {
    if (HasMultipleEdges_Witness  const *e = dynamic_cast<HasMultipleEdges_Witness const *>(&rhs)) {
        shared_ptr<HasMultipleEdges_Witness const>  p = e->shared_from_this();
        if(this->found < p->found){
            return true;
        } else if (this->found == p->found){
            if(this->edgeSet < p->edgeSet){
                return true;
            }
        }
        return false;
    }else{
        cerr<<"ERROR: in HasMultipleEdges_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &HasMultipleEdges_Witness::set_equal(Witness &rhs) {
    if(HasMultipleEdges_Witness  *e = dynamic_cast<HasMultipleEdges_Witness *>(&rhs)) {
        shared_ptr<HasMultipleEdges_Witness> p = e->shared_from_this();
        this->found = p->found;
        this->edgeSet = p->edgeSet;
        return *this;
    }else{
        cerr<<"ERROR: in HasMultipleEdges_Witness::set_equal cast error"<<endl;
        exit(20);
    }
}

shared_ptr<Witness> HasMultipleEdges_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    HasMultipleEdges_WitnessPointer w(new HasMultipleEdges_Witness);
    if(this->found ){
        w->found = true;
        return w;
    }else{
        set<pair<unsigned,unsigned>> newEdgeSet;
        for(auto p:this->edgeSet){
            if(relabelingMap.count(p.first) and relabelingMap.count(p.second)){
                if(relabelingMap[p.first] < relabelingMap[p.second]){
                    newEdgeSet.insert(make_pair(relabelingMap[p.first],relabelingMap[p.second]));
                }else{
                    newEdgeSet.insert(make_pair(relabelingMap[p.second],relabelingMap[p.first]));
                }
            }else{
                cout<<"Error: HasMultipleEdges_Witness::relabel "<< p.first <<" or "<< p.second << " is not in the map"<<endl;
                print();
                cout<<"\nmap"<<endl;
                for(auto item:relabelingMap){
                    cout<<item.first<<"->"<<item.second<<endl;
                }
                exit(20);
            }
        }
        w->edgeSet = newEdgeSet;
        w->found = false;
        return w;
    }
}

void HasMultipleEdges_Witness::print() {
    cout<<"{";
    for (set<pair<unsigned,unsigned>>::iterator it = this->edgeSet.begin() ; it != this->edgeSet.end() ; ++it) {
        cout<<"("<<it->first<<","<<it->second<<"),";
    }
    cout<<"} found: "<<this->found;
}

string HasMultipleEdges_Witness::witnessInformation() {
    string info;
    info = "{";
    for (set<pair<unsigned,unsigned>>::iterator it = this->edgeSet.begin() ; it != this->edgeSet.end() ; ++it) {
        info=info + "(" + to_string(it->first)+","+to_string(it->second)+"),";
    }
    info=info+"} found: "+to_string(this->found);
    return info;
}

HasMultipleEdges_DynamicCore::HasMultipleEdges_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","HasMultipleEdges");
    addAttribute("ParameterType","NoParameter");
    addAttribute("PrimaryOperator","---");
    createInitialWitnessSet();
}

void HasMultipleEdges_DynamicCore::createInitialWitnessSet() {
    shared_ptr<HasMultipleEdges_Witness> witness(new HasMultipleEdges_Witness);
    witness->found = false;
    shared_ptr<HasMultipleEdges_WitnessSet> witnessSet(new HasMultipleEdges_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    insertIntoInitialWitnessSet(witness);
}

WitnessSetPointer HasMultipleEdges_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if(HasMultipleEdges_Witness *e = dynamic_cast<HasMultipleEdges_Witness *>(&witness)) {
        shared_ptr<HasMultipleEdges_Witness> p = e->shared_from_this();
        shared_ptr<HasMultipleEdges_WitnessSet> witnessSet (new HasMultipleEdges_WitnessSet);
        witnessSet->insert(p);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer HasMultipleEdges_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if(HasMultipleEdges_Witness *e = dynamic_cast<HasMultipleEdges_Witness *>(&witness)) {
        shared_ptr<HasMultipleEdges_Witness> p = e->shared_from_this();
        shared_ptr<HasMultipleEdges_WitnessSet> witnessSet (new HasMultipleEdges_WitnessSet);
        shared_ptr<HasMultipleEdges_Witness> w(new HasMultipleEdges_Witness);
        if(i!=j and !p->found){
            pair<int,int> newEdge;
            if(i<j){
                newEdge = make_pair(i,j);
            }else{
                newEdge = make_pair(j,i);
            }
            if(p->edgeSet.find(newEdge)!= p->edgeSet.end()){
                w->found = true;
                witnessSet->insert(w);
                return clean(witnessSet);
            }else{
                w->found =false;
                w->edgeSet = p->edgeSet;
                w->edgeSet.insert(newEdge);
                witnessSet->insert(w);
                return clean(witnessSet);
            }

        }else{
            witnessSet->insert(p);
            return clean(witnessSet);
        }
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::intro_e cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer HasMultipleEdges_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if(HasMultipleEdges_Witness *e = dynamic_cast<HasMultipleEdges_Witness *>(&witness)) {
        shared_ptr<HasMultipleEdges_Witness> p = e->shared_from_this();
        shared_ptr<HasMultipleEdges_WitnessSet> witnessSet (new HasMultipleEdges_WitnessSet);
        shared_ptr<HasMultipleEdges_Witness> w(new HasMultipleEdges_Witness);
        if(p->found){
            witnessSet->insert(p);
            return clean(witnessSet);
        }else{
            w->found = p->found;
            w->edgeSet = p->edgeSet;
            set<unsigned> tempBag = b.get_elements();
            for(set<unsigned>::iterator it = tempBag.begin(); it != tempBag.end() ; it++){
                pair<unsigned,unsigned> e = make_pair(i,*it);
                w->edgeSet.erase(e);
                e = make_pair(*it,i);
                w->edgeSet.erase(e);
            }
            witnessSet->insert(w);
            return clean(witnessSet);
        }
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer HasMultipleEdges_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(HasMultipleEdges_Witness *e1 = dynamic_cast<HasMultipleEdges_Witness *>(&witness1)) {
        if(HasMultipleEdges_Witness *e2 = dynamic_cast<HasMultipleEdges_Witness *>(&witness2)) {
            shared_ptr<HasMultipleEdges_Witness> p1 = e1->shared_from_this();
            shared_ptr<HasMultipleEdges_Witness> p2 = e2->shared_from_this();
            shared_ptr<HasMultipleEdges_WitnessSet> witnessSet (new HasMultipleEdges_WitnessSet);
            shared_ptr<HasMultipleEdges_Witness> w(new HasMultipleEdges_Witness);
            if(p1->found ){
                witnessSet->insert(p1);
                return clean(witnessSet);
            }else if(p2->found){
                witnessSet->insert(p2);
                return clean(witnessSet);
            }else{
                set<pair<unsigned,unsigned>> intersection_set;
                set_intersection(p1->edgeSet.begin(),p1->edgeSet.end(),p2->edgeSet.begin(),p2->edgeSet.end(), std::inserter(intersection_set,intersection_set.begin()));
                if(intersection_set.size()>0){
                    w->found = true;
                    witnessSet->insert(w);
                    return clean(witnessSet);
                }else{
                    set<pair<unsigned,unsigned>> union_set;
                    set_union(p1->edgeSet.begin(),p1->edgeSet.end(),p2->edgeSet.begin(),p2->edgeSet.end(),std::inserter(union_set,union_set.begin()));
                    w->found = false;
                    w->edgeSet = union_set;
                    witnessSet->insert(w);
                    return clean(witnessSet);
                }
            }
        }else{
            cerr<<"ERROR: in HasMultipleEdges_DynamicCore::join cast error"<<endl;
            exit(20);}
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::join cast error"<<endl;
        exit(20);}
}

bool HasMultipleEdges_DynamicCore::is_final_witness(Witness &witness) {
    if(HasMultipleEdges_Witness *e = dynamic_cast<HasMultipleEdges_Witness *>(&witness)) {
        shared_ptr<HasMultipleEdges_Witness> p = e->shared_from_this();
        return p->found;
    }else{
        cerr<<"ERROR: in HasMultipleEdges_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer HasMultipleEdges_DynamicCore::clean(WitnessSetPointer witnessSet) {
    return witnessSet;
}

