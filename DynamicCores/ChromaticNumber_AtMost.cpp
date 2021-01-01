// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "ChromaticNumber_AtMost.h"

extern "C" {
DynamicCore * create() {
    return new ChromaticNumber_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new ChromaticNumber_AtMost_DynamicCore(param);
}
}
//
//shared_ptr<Witness> ChromaticNumber_AtMost_Witness::relabel(map<unsigned,unsigned> relabelingMap){
//	shared_ptr<ChromaticNumber_AtMost_Witness> relabeledWitness(new ChromaticNumber_AtMost_Witness);
//	for (auto it = this->coloring.begin(); it!=this->coloring.end();it++){
//		pair<unsigned,unsigned> relabeledPair;
//		relabeledPair.first = relabelingMap(*it.first);
//		relabeledPair.second = *it.second.
//		relabeledWitnesscoloring.insert(relabeledPair);
//	}
//	return relabeledWitness;
//}


bool ChromaticNumber_AtMost_Witness::is_equal(const Witness &rhs)const{
    if (ChromaticNumber_AtMost_Witness const *e = dynamic_cast<ChromaticNumber_AtMost_Witness const *>(&rhs)) {
        shared_ptr<const ChromaticNumber_AtMost_Witness> p = e->shared_from_this();
        if(coloring==p->coloring){
            return true;
        }else{
            return false;
        }
    } else {
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool ChromaticNumber_AtMost_Witness::is_less(const Witness &rhs)const {
    if (ChromaticNumber_AtMost_Witness const *e = dynamic_cast<ChromaticNumber_AtMost_Witness const *>(&rhs)) {
        shared_ptr<const ChromaticNumber_AtMost_Witness> p = e->shared_from_this();

        return coloring < p->coloring;
    } else {
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &ChromaticNumber_AtMost_Witness::set_equal(Witness &rhs) {
    if (ChromaticNumber_AtMost_Witness const *e = dynamic_cast<ChromaticNumber_AtMost_Witness const *>(&rhs)) {
        shared_ptr<const ChromaticNumber_AtMost_Witness> p = e->shared_from_this();

        ChromaticNumber_AtMost_Witness::coloring = p->coloring;
        return *this;
    } else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_Witness::set_equal cast error"<<endl;
        exit(20);
    }
}

void ChromaticNumber_AtMost_Witness::print() {
        cout<<"{";
        for (map<unsigned,unsigned>::iterator it = coloring.begin(); it != coloring.end() ; ++it) {
            cout<< it->first <<" -> "<< it->second << ", ";
        }
        cout<<"}";
}

ChromaticNumber_AtMost_DynamicCore::ChromaticNumber_AtMost_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","ChromaticNumber");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

ChromaticNumber_AtMost_DynamicCore::ChromaticNumber_AtMost_DynamicCore(unsigned k){
    this->k = k;
    createInitialWitnessSet();
    // Initializing attributes
    addAttribute("CoreName","ChromaticNumber");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

void ChromaticNumber_AtMost_DynamicCore::createInitialWitnessSet() {
    shared_ptr<ChromaticNumber_AtMost_Witness> witness(new ChromaticNumber_AtMost_Witness);
    shared_ptr<ChromaticNumber_AtMost_WitnessSet>  witnessSet(new ChromaticNumber_AtMost_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    this->insertIntoInitialWitnessSet(witness);
}
WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (ChromaticNumber_AtMost_Witness  *e = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness)) {
        shared_ptr<ChromaticNumber_AtMost_WitnessSet> witnessSet (new ChromaticNumber_AtMost_WitnessSet);
        //shared_ptr<ChromaticNumber_AtMost_Witness> p = make_shared<ChromaticNumber_AtMost_Witness>(*e);
        shared_ptr<ChromaticNumber_AtMost_Witness> p = e->shared_from_this();
        for (unsigned j = 1; j < k+1 ; j++) {
            shared_ptr<ChromaticNumber_AtMost_Witness> w(new ChromaticNumber_AtMost_Witness);
            w->coloring = p->coloring;
            w->coloring.insert(make_pair(i,j));
            witnessSet->insert(w);
        }
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}
WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (ChromaticNumber_AtMost_Witness *e = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness)) {
        shared_ptr<ChromaticNumber_AtMost_Witness> p = e->shared_from_this();
        shared_ptr<ChromaticNumber_AtMost_WitnessSet> witnessSet (new ChromaticNumber_AtMost_WitnessSet);
        if(p->coloring.find(i)->second != p->coloring.find(j)->second){
            witnessSet->insert(p);
        }
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_DynamicCore::intro_e cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (ChromaticNumber_AtMost_Witness *e = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness)) {
        shared_ptr<ChromaticNumber_AtMost_Witness> p = e->shared_from_this();
        shared_ptr<ChromaticNumber_AtMost_WitnessSet> witnessSet (new ChromaticNumber_AtMost_WitnessSet);
        shared_ptr<ChromaticNumber_AtMost_Witness> w(new ChromaticNumber_AtMost_Witness);
        w->coloring = p->coloring;
        w->coloring.erase(i);
        witnessSet->insert(w);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}
WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (ChromaticNumber_AtMost_Witness *e1 = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness1)) {
        if (ChromaticNumber_AtMost_Witness *e2 = dynamic_cast<ChromaticNumber_AtMost_Witness *>(&witness2)) {
            shared_ptr<ChromaticNumber_AtMost_Witness> p1 = e1->shared_from_this();
            shared_ptr<ChromaticNumber_AtMost_Witness> p2 = e2->shared_from_this();
            shared_ptr<ChromaticNumber_AtMost_WitnessSet> witnessSet (new ChromaticNumber_AtMost_WitnessSet);
            if(p1->coloring == p2->coloring){
                witnessSet->insert(p1);
            }
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in ChromaticNumber_AtMost_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    } else{
        cerr<<"ERROR: in ChromaticNumber_AtMost_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

bool ChromaticNumber_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    return true;
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::clean(WitnessSetPointer witnessSet) {
    return witnessSet;
}
