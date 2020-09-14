// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "DynamicKernel.h"


void DynamicKernel::addCore(DynamicCore &core) {
    cores.push_back(&core);
}

shared_ptr<State> DynamicKernel::initialState(){
    shared_ptr<State> initialState(new State);
    Bag emptyBag; //Empty
    vector<shared_ptr<WitnessSet>> vectorInitialWitnessSets;
    for (size_t k = 0; k < cores.size() ; ++k){
        vectorInitialWitnessSets.push_back(cores[k]->get_initialSet());
    }
    initialState->set_bag(emptyBag);
    initialState->set_witnessSetVector(vectorInitialWitnessSets);
    return initialState ;
}

shared_ptr<State> DynamicKernel::intro_v(shared_ptr<State> q, unsigned i) {
    if(q->get_bag().vertex_introducible(i)){
        shared_ptr<State> aux(new State);
        Bag b = q->get_bag();
        vector<shared_ptr<WitnessSet>> vectorWitnessSet;
        vector<shared_ptr<WitnessSet>> vectorWitnessSetTemp = q->get_witnessSetVector();
        for(size_t r = 0; r < cores.size(); r++){
            vectorWitnessSet.push_back(cores[r]->intro_v(i,b,*(vectorWitnessSetTemp[r])));
        }
        aux->set_bag(b.intro_v(i));
        aux->set_witnessSetVector(vectorWitnessSet);
        return aux;
    }else{
        cerr << "Error in function DynamicKernel::intro_v. The bag is not i introducible" << endl;
        exit(10);
    }
}

shared_ptr<State> DynamicKernel::intro_e(const shared_ptr<State> q, const unsigned i, const unsigned j) {
     if(q->get_bag().edge_introducible(i,j)){
        shared_ptr<State> aux(new State);
        Bag b = q->get_bag();
        vector<shared_ptr<WitnessSet>> vectorWitnessSet;
        vector<shared_ptr<WitnessSet>> vectorWitnessSetTemp = q->get_witnessSetVector();
        for(size_t r = 0; r < cores.size(); r++){
            vectorWitnessSet.push_back(cores[r]->intro_e(i,j, b,*vectorWitnessSetTemp[r]));
        }
         aux->set_bag(b.intro_e(i,j));
         aux->set_witnessSetVector(vectorWitnessSet);
        return aux;
    }else{
        cerr << "Error in function DynamicKernel::intro_e. The bag is not i j introducible" << endl;
        exit(10);
    }
}

shared_ptr<State> DynamicKernel::forget_v(shared_ptr<State> q, unsigned i) {
    if(q->get_bag().vertex_forgettable(i)){
        shared_ptr<State> aux(new State);
        Bag b = q->get_bag();
        vector<shared_ptr<WitnessSet>> vectorWitnessSet;
        vector<shared_ptr<WitnessSet>> vectorWitnessSetTemp = q->get_witnessSetVector();
        for(size_t r = 0; r < cores.size(); r++){
            vectorWitnessSet.push_back(cores[r]->forget_v(i,b,*vectorWitnessSetTemp[r]));
        }
        aux->set_bag(b.forget_v(i));
        aux->set_witnessSetVector(vectorWitnessSet);
        return aux;
    }else{
        cerr << "Error in function DynamicKernel::forget_v. The bag is not i forgettable" << endl;
        exit(10);
    }
}

shared_ptr<State> DynamicKernel::join(shared_ptr<State> q1, shared_ptr<State> q2) {
    if(q1->get_bag().joinable(q2->get_bag()))
    {
        shared_ptr<State> aux(new State);
        set<unsigned> elements = q1->get_bag().get_elements();
        Bag b;
        b.set_elements(elements);
        // in join the new state should not have an edge
        vector<shared_ptr<WitnessSet>> vectorWitnessSet;
        vector<shared_ptr<WitnessSet>> vectorWitnessSet1 = q1->get_witnessSetVector();
        vector<shared_ptr<WitnessSet>> vectorWitnessSet2 = q2->get_witnessSetVector();
        for(size_t r=0; r < cores.size(); r++){
               vectorWitnessSet.push_back(cores[r]->join(b, *vectorWitnessSet1[r],*vectorWitnessSet2[r]));
        }
        aux->set_bag(b);
        aux->set_witnessSetVector(vectorWitnessSet);
        return aux;
    } else{
        cerr << "Error in function DynamicKernel::join. The bags are not joinable" << endl;
        exit(10);
    }
}

// Change Name of this function from core_index to pointerToCoreNumber
DynamicCore* DynamicKernel::pointerToCoreNumber(unsigned i) {
    size_t j = i;
    if (cores.size() - 1 < j){
        cerr<<"ERROR: In DynamicKernel::pointerToCoreNumber, asked index bigger than the size of cores"<<endl;
        exit(20);
    } else{
        return cores[i];
    }
}

Width& DynamicKernel::get_width(){
    return width;
}

void DynamicKernel::set_width(Width &width) {
    DynamicKernel::width = width;
}

void DynamicKernel::set_properties(vector<PropertyAssignment> &vectorProperties) {
    properties = vectorProperties;
}

const vector<PropertyAssignment> &DynamicKernel::get_properties() const {
    return properties;
}





