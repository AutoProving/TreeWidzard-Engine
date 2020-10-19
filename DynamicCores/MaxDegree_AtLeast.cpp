// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "MaxDegree_AtLeast.h"
extern "C" {
    DynamicCore * create() {
        return new MaxDegree_AtLeast_DynamicCore;
    }
    DynamicCore * create_int(unsigned param) {
        return new MaxDegree_AtLeast_DynamicCore(param);
    }
}

MaxDegree_AtLeast_Witness::MaxDegree_AtLeast_Witness() {
    found =false;
}

void MaxDegree_AtLeast_Witness::print() {
    cout<<"{";

    for(map<unsigned,unsigned>::iterator it = degreeCounter.begin(); it != degreeCounter.end() ; it++){
        auto tempIt = it;
        cout<<it->first << " -> "<< it->second;
        if(++tempIt != degreeCounter.end()){
            cout<<", ";
        }
    }
    cout<<"} "<<"found : "<<found;
}

bool MaxDegree_AtLeast_Witness::is_equal(const Witness &rhs)const {
    if(MaxDegree_AtLeast_Witness const *e = dynamic_cast<MaxDegree_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<MaxDegree_AtLeast_Witness const> p = e->shared_from_this();
        if(found == p->found and (degreeCounter==p->degreeCounter)){
            return  true;
        } else{
            return false;
        }
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool MaxDegree_AtLeast_Witness::is_less(const  Witness &rhs)const {
    if (MaxDegree_AtLeast_Witness const *e = dynamic_cast<MaxDegree_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<MaxDegree_AtLeast_Witness const> p = e->shared_from_this();
        if(this->found < p->found or (this->found == p->found and this->degreeCounter < p->degreeCounter)){
            return true;
        }else{
            return false;
        }
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &MaxDegree_AtLeast_Witness::set_equal(Witness &witness) {
    if (MaxDegree_AtLeast_Witness  *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
        shared_ptr<MaxDegree_AtLeast_Witness> p = e->shared_from_this();
        found = p->found;
        degreeCounter = p->degreeCounter;
        return *this;
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

MaxDegree_AtLeast_Witness::~MaxDegree_AtLeast_Witness() {

}

MaxDegree_AtLeast_DynamicCore::MaxDegree_AtLeast_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","MaxDegree");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtLeast");
}

MaxDegree_AtLeast_DynamicCore::MaxDegree_AtLeast_DynamicCore(unsigned maxDegree){
    this->maxDegree = maxDegree;
    createInitialWitnessSet();
    // Initializing attributes
    addAttribute("CoreName","MaxDegree");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtLeast");
}

void MaxDegree_AtLeast_DynamicCore::createInitialWitnessSet() {
    shared_ptr<MaxDegree_AtLeast_Witness> w(new MaxDegree_AtLeast_Witness);
    w->found=false;
    shared_ptr<MaxDegree_AtLeast_WitnessSet> witnessSet(new MaxDegree_AtLeast_WitnessSet);
    setInitialWitnessSet(witnessSet);
    this->insertIntoInitialWitnessSet(w);

}

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (MaxDegree_AtLeast_Witness *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
       // shared_ptr<MaxDegree_AtLeast_Witness> p = make_shared<MaxDegree_AtLeast_Witness>(*e);
        shared_ptr<MaxDegree_AtLeast_Witness> p = e->shared_from_this();

        shared_ptr<MaxDegree_AtLeast_WitnessSet> witnessSet(new MaxDegree_AtLeast_WitnessSet);
        if(p->found==true){
            witnessSet->insert(p);
            return clean(witnessSet);
        }else{
            shared_ptr<MaxDegree_AtLeast_Witness> w(new MaxDegree_AtLeast_Witness);
            w->found=false;
            w->degreeCounter = p->degreeCounter;
            w->degreeCounter.insert(make_pair(i,0));
            witnessSet->insert(w);
            return clean(witnessSet);
        }
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::intro_e(unsigned i , unsigned j, Bag &b, Witness &witness) {
    if (MaxDegree_AtLeast_Witness *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
        shared_ptr<MaxDegree_AtLeast_Witness> p = e->shared_from_this();
        shared_ptr<MaxDegree_AtLeast_WitnessSet> witnessSet(new MaxDegree_AtLeast_WitnessSet);
        if(p->found == true){
            witnessSet->insert(p);
            return clean(witnessSet);
        }else if( i!=j and  p->degreeCounter.find(i)->second < this->maxDegree-1 and p->degreeCounter.find(j)->second < this->maxDegree-1 ){
            shared_ptr<MaxDegree_AtLeast_Witness> w(new MaxDegree_AtLeast_Witness);
            w->degreeCounter = p->degreeCounter;
            w->degreeCounter.erase(i);
            w->degreeCounter.erase(j);
            w->degreeCounter.insert(make_pair(i,p->degreeCounter.find(i)->second+1));
            w->degreeCounter.insert(make_pair(j,p->degreeCounter.find(j)->second+1));
            w->found = false;
            witnessSet->insert(w);
            return clean(witnessSet);
        }else if(i==j and p->degreeCounter.find(i)->second < this->maxDegree-2){
            shared_ptr<MaxDegree_AtLeast_Witness> w(new MaxDegree_AtLeast_Witness);
            w->degreeCounter = p->degreeCounter;
            w->degreeCounter.erase(i);
            w->degreeCounter.insert(make_pair(i,p->degreeCounter.find(i)->second+2));
            witnessSet->insert(w);
            return clean(witnessSet);
        }else{
            shared_ptr<MaxDegree_AtLeast_Witness> w(new MaxDegree_AtLeast_Witness);
            w->found = true;
            witnessSet->insert(w);
            return clean(witnessSet);
        }
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (MaxDegree_AtLeast_Witness *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
        shared_ptr<MaxDegree_AtLeast_Witness> p = e->shared_from_this();
        shared_ptr<MaxDegree_AtLeast_WitnessSet> witnessSet(new MaxDegree_AtLeast_WitnessSet);
        if(p->found == true){
            witnessSet->insert(p);
            return clean(witnessSet);
        }else{
            shared_ptr<MaxDegree_AtLeast_Witness> w(new MaxDegree_AtLeast_Witness);
            w->found = false;
            w->degreeCounter = p->degreeCounter;
            w->degreeCounter.erase(i);
            witnessSet->insert(w);
            return clean(witnessSet);
        }
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (MaxDegree_AtLeast_Witness  *e1 = dynamic_cast<MaxDegree_AtLeast_Witness  *>(&witness1)){
        if(MaxDegree_AtLeast_Witness  *e2 = dynamic_cast<MaxDegree_AtLeast_Witness  *>(&witness2)) {
            shared_ptr<MaxDegree_AtLeast_Witness> p1 = e1->shared_from_this();
            shared_ptr<MaxDegree_AtLeast_Witness> p2 = e2->shared_from_this();
            shared_ptr<MaxDegree_AtLeast_WitnessSet> witnessSet(new MaxDegree_AtLeast_WitnessSet);
            shared_ptr<MaxDegree_AtLeast_Witness> w(new MaxDegree_AtLeast_Witness);
            if (p1->found == true ){
                witnessSet->insert(p1);
                return clean(witnessSet);
            } else if( p2->found == true){
                witnessSet->insert(p2);
                return clean(witnessSet);
            }else {
                map<unsigned, unsigned> temp1 = p1->degreeCounter;
                map<unsigned, unsigned> temp2 = p2->degreeCounter;
                map<unsigned, unsigned> temp;
                bool found = false;
                map<unsigned, unsigned>::iterator it1 = temp1.begin();
                for (map<unsigned, unsigned>::iterator it2 = temp2.begin(); it2 != temp2.end(); ++it2) {
                    if (it1->second + it2->second < this->maxDegree) {
                        temp.insert(make_pair(it1->first, it1->second + it2->second));
                        it1++;
                    } else {
                        found = true;
                        break;
                    }
                }
                if (found == false) {
                    w->found = false;
                    w->degreeCounter = temp;
                    witnessSet->insert(w);
                    return clean(witnessSet);
                } else {
                    w->found = true;
                    witnessSet->insert(w);
                    return clean(witnessSet);
                }
            }
        }else{
            cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::join cast error"<<endl;
        exit(20);
    }
}

bool MaxDegree_AtLeast_DynamicCore::is_final_witness(Witness &witness) {
    if (MaxDegree_AtLeast_Witness *e = dynamic_cast<MaxDegree_AtLeast_Witness *>(&witness)) {
        shared_ptr<MaxDegree_AtLeast_Witness> p = e->shared_from_this();
        return p->found;
    }else{
        cerr<<"ERROR: in MaxDegree_GreaterThan_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer MaxDegree_AtLeast_DynamicCore::clean(WitnessSetPointer witnessSet) {
    return witnessSet;
}

