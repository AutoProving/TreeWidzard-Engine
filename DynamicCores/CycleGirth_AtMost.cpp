// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "CycleGirth_AtMost.h"


///////////////////////////Implementation/////////////////////////////
/////////////////////////////////////////////////////////////////////
bool CycleGirth_AtMost_Witness::is_equal_implementation(const CycleGirth_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->current_size == w->current_size &&
           this->degree_0 == w->degree_0 &&
           this->degree_2 == w->degree_2 &&
           this->extremity == w->extremity &&
           this->found_cycle == w->found_cycle;
    //*****************************
    //*****************************
}
bool CycleGirth_AtMost_Witness::is_less_implementation(const CycleGirth_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    if (this->current_size != w->current_size) return this->current_size < w->current_size;
    if (this->degree_0 != w->degree_0) return this->degree_0 < w->degree_0;
    if (this->degree_2 != w->degree_2) return this->degree_2 < w->degree_2;
    if (this->extremity != w->extremity) return this->extremity < w->extremity;
    return this->found_cycle < w->found_cycle;
    //*****************************
    //*****************************
}
Witness & CycleGirth_AtMost_Witness::set_equal_implementation(CycleGirth_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    this->current_size = w->current_size;
    this->degree_0 = w->degree_0;
    this->degree_2 = w->degree_2;
    this->extremity = w->extremity;
    this->found_cycle = w->found_cycle;
    return *this;
    //*****************************
    //*****************************
}

shared_ptr<Witness> CycleGirth_AtMost_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    CycleGirth_AtMost_WitnessPointer w(new CycleGirth_AtMost_Witness);
    if(this->found_cycle){
        w->found_cycle = true;
        w->current_size = this->current_size;
        return w;
    }else{
        set<unsigned > newDegree_0;
        set<unsigned > newDegree_2;
        map<unsigned, unsigned> newExtremity;
        for(auto v: this->degree_0){
            if(relabelingMap.count(v)){
                newDegree_0.insert(relabelingMap[v]);
            }else{
                cout<<"Error: CycleGirth_AtMost_Witness::relabel "<< v << " from degree_0 is not in the map"<<endl;
                exit(20);
            }
        }
        for(auto v: this->degree_2){
            if(relabelingMap.count(v)){
                newDegree_2.insert(relabelingMap[v]);
            }else{
                cout<<"Error: CycleGirth_AtMost_Witness::relabel "<< v << "from degree_2 is not in the map"<<endl;
                exit(20);
            }
        }
        for(auto v: this->extremity){
            if(relabelingMap.count(v.first) and relabelingMap.count(v.second)){
                newExtremity.insert(make_pair(relabelingMap[v.first],relabelingMap[v.second]));
            }else{
                cout<<"Error: CycleGirth_AtMost_Witness::relabel "<< v.first <<" or "<< v.second << " from extremity is not in the map"<<endl;
                print();
                for(auto m:relabelingMap){
                    cout<<m.first  << " "<<m.second<<endl;
                }
                exit(20);
            }
        }
        w->found_cycle = false;
        w->current_size = this->current_size;
        w->degree_0 = newDegree_0;
        w->degree_2 = newDegree_2;
        w->extremity = newExtremity;
        return w;
    }
}

void CycleGirth_AtMost_Witness::print() {
    //*****************************
    //*****************************
    cout << "Current size of cycle: " << this->current_size << endl;
    cout << endl;
    cout << "Vertices of degree 0: ";
    for (unsigned v : this->degree_0) cout << v << " ";
    cout << endl
         << endl;
    cout << "Vertices of degree 2: ";
    for (unsigned v : this->degree_2) cout << v << " ";
    cout << endl
         << endl;
    cout << "Corresponding Extremity Map (key -> value):" << endl;
    for (auto &elem : this->extremity) cout << elem.first << " -> " << elem.second << endl;
    cout << endl;
    cout << "Found Cycle: " << this->found_cycle << endl;
    //*****************************
    //*****************************
}

CycleGirth_AtMost_Witness::~CycleGirth_AtMost_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

CycleGirth_AtMost_DynamicCore::CycleGirth_AtMost_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","CycleGirth");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
    createInitialWitnessSet();
}
CycleGirth_AtMost_DynamicCore::CycleGirth_AtMost_DynamicCore(unsigned parameter) {
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName","CycleGirth");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
    this->parameter = parameter;
    createInitialWitnessSet();
}
void CycleGirth_AtMost_DynamicCore::createInitialWitnessSet_implementation() {
    CycleGirth_AtMost_WitnessPointer witness = createWitness();
    this->insertIntoInitialWitnessSet(witness);
}
void CycleGirth_AtMost_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, CycleGirth_AtMost_WitnessPointer w,
                                                           CycleGirth_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************

    if(w->found_cycle){
        witnessSet->insert(w);
    }else if (w->current_size < this->parameter) {
        // In this witness we don't add v to cycle
        witnessSet->insert(w);
        // In this witness we add v to cycle only if current_size < parameter
        CycleGirth_AtMost_WitnessPointer w_with_v = createWitness();
        w_with_v->set_equal(*w);
        w_with_v->current_size++;
        w_with_v->degree_0.insert(i);
        witnessSet->insert(w_with_v);
    }
    //*****************************
    //*****************************
}
void CycleGirth_AtMost_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                           CycleGirth_AtMost_WitnessPointer w,
                                                           CycleGirth_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************

    if(w->found_cycle){
        witnessSet->insert(w);
    }else{
        // Insert witness, where edge is not introduced
        CycleGirth_AtMost_WitnessPointer w_no_edge = createWitness();
        w_no_edge->set_equal(*w);
        witnessSet->insert(w_no_edge);
        for(auto l:w_no_edge->extremity){
            if(!(w_no_edge->extremity[l.second]==l.first and w_no_edge->extremity[l.first]== l.second)){
                cout<<"Error!!! 1"<<endl;
                w_no_edge->print();
                exit(20);
            }
        }
        // Now calculate and insert witness, where edge is introduced
        // If the degree of either endpoint of the edge is already 2, then
        // we are trying to add an edge to an inner node of the cycle.
        // This is not allowed and therefore no additional witness is created.
        if (w->degree_2.count(i) || w->degree_2.count(j)) {
            // The edge {i,j} cannot be introduced.
        }else if (w->degree_0.count(i) && w->degree_0.count(j)) {
            // If both u and v have degree 0 in the input witness, then
            // by connecting these vertices, their degrees become 1.
            CycleGirth_AtMost_WitnessPointer w_connect_nodes = createWitness();
            w_connect_nodes->set_equal(*w);
            w_connect_nodes->extremity[i] = j;
            w_connect_nodes->extremity[j] = i;
            w_connect_nodes->degree_0.erase(i);
            w_connect_nodes->degree_0.erase(j);
            for(auto l:w_connect_nodes->extremity){
                if(!(w_connect_nodes->extremity[l.second]==l.first and w_connect_nodes->extremity[l.first]== l.second)){
                    cout<<"Error!!! 2"<<endl;
                    w_connect_nodes->print();
                    exit(20);
                }
            }
            witnessSet->insert(w_connect_nodes);
        }else{
            auto u_extremity_it = w->extremity.find(i);
            auto v_extremity_it = w->extremity.find(j);
            // If one endpoint of the edge has degree 0 (say, u) and the other has degree 1 (say, v),
            // then v is an extremety of a path. This path gets extended by the new edge (u,v) and
            // now u becomes the new endpoint of this path.
            if ((w->degree_0.count(i) && v_extremity_it != w->extremity.end()) ||
                (w->degree_0.count(j) && u_extremity_it != w->extremity.end())) {
                // Swap u and v, such that u has always degree 0 and v always degree 1
                if (!w->degree_0.count(i)) {
                    swap(i, j);
                    swap(u_extremity_it, v_extremity_it);
                }
                unsigned v_extremity = v_extremity_it->second;
                // extend cycle (v is always inner node: u - v - v_extremity)
                CycleGirth_AtMost_WitnessPointer w_extend = createWitness();
                w_extend->set_equal(*w);
                w_extend->degree_0.erase(i);
                w_extend->degree_2.insert(j);
                w_extend->extremity[v_extremity] = i;
                w_extend->extremity[i]=v_extremity;
                w_extend->extremity.erase(j);
//                w_extend->degree_0.erase(i);
//                w_extend->extremity[v_extremity] = j;
//                w_extend->extremity[i] = v_extremity;
//                w_extend->extremity.erase(j);
//                w_extend->degree_2.insert(j);
                for(auto l:w_extend->extremity){
                    if(!(w_extend->extremity[l.second]==l.first and w_extend->extremity[l.first]== l.second)){
                        cout<<"Error!!! 3"<<endl;
                        cout<<"i: "<<i<<" j:"<<j<<" v_extremity:"<<v_extremity<<endl;
                        w->print();
                        w_extend->print();
                        exit(20);
                    }
                }
                witnessSet->insert(w_extend);
            }else if (u_extremity_it != w->extremity.end() && v_extremity_it != w->extremity.end()) {
                //The next IF analyses the case in which both endpoints of the edge have degree 1 in the input witness
                unsigned u_extremity = u_extremity_it->second;
                unsigned v_extremity = v_extremity_it->second;
                // There are two cases to be analysed. In the first case, u and v are the
                // extremities of a path. Therefore by adding the edge (u,v) the cycle gets closed
                if (u_extremity == j && v_extremity == i) {  // found cycle
                    CycleGirth_AtMost_WitnessPointer w_cycle = createWitness();
                    w_cycle->set_equal(*w);
                    w_cycle->extremity.erase(i);
                    w_cycle->extremity.erase(j);
                    if (w_cycle->degree_0.empty() && w_cycle->extremity.empty()) {
                        w_cycle->found_cycle = true;
                        for(auto l:w_cycle->extremity){
                            if(!(w_cycle->extremity[l.second]==l.first and w_cycle->extremity[l.first]== l.second)){
                                cout<<"Error!!! 4"<<endl;
                                w_cycle->print();
                                exit(20);
                            }
                        }
                        w_cycle->degree_2.clear();
                        witnessSet->insert(w_cycle);
                    } else {
                        // nothing
                    }
                }else{
                    // In the second case, u is an extremity of a path and v is an extremity of another path
                    // In this case these paths are merged by connecting u and v.
                    CycleGirth_AtMost_WitnessPointer w_connect_parts = createWitness();
                    w_connect_parts->set_equal(*w);
                    w_connect_parts->extremity.erase(i);
                    w_connect_parts->extremity.erase(j);
                    w_connect_parts->degree_2.insert(i);
                    w_connect_parts->degree_2.insert(j);
                    w_connect_parts->extremity[u_extremity] = v_extremity;
                    w_connect_parts->extremity[v_extremity] = u_extremity;
                    for(auto l:w_connect_parts->extremity){
                        if(!(w_connect_parts->extremity[l.second]==l.first and w_connect_parts->extremity[l.first]== l.second)){
                            cout<<"Error!!! 5"<<endl;
                            w_connect_parts->print();
                            exit(20);
                        }
                    }
                    witnessSet->insert(w_connect_parts);
                }
            }
        }
    }
    //*****************************
    //*****************************
}
void CycleGirth_AtMost_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, CycleGirth_AtMost_WitnessPointer w,
                                                            CycleGirth_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    if(w->found_cycle){
        witnessSet->insert(w);
    }else if (w->degree_2.count(i)) {
        CycleGirth_AtMost_WitnessPointer w_forgotten = createWitness();
        w_forgotten->set_equal(*w);
        w_forgotten->degree_2.erase(i);
        witnessSet->insert(w_forgotten);
    }
    // If we try to forget a vertex of degree 0 or 1, then the input witness is incompatible with forgetting
    // this vertex, therefore nothing is added to witnessSet.
    //*****************************
    //*****************************
}
void CycleGirth_AtMost_DynamicCore::join_implementation(Bag &b, CycleGirth_AtMost_WitnessPointer w1,
                                                        CycleGirth_AtMost_WitnessPointer w2,
                                                        CycleGirth_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    CycleGirth_AtMost_WitnessPointer wPrime = createWitness();
    wPrime->set_equal(*w1);  // This is a copy of the first witness and will
    // be the resulting merged witness
    CycleGirth_AtMost_WitnessPointer w_join = createWitness();
    w_join->set_equal(*w2);  // This is a copy of the second witness and will
    // be merged into witness w
    wPrime->found_cycle = wPrime->found_cycle || w_join->found_cycle;
    wPrime->current_size += w_join->current_size;  // The final size of the returned
    // witness is equal to the size of both input witnesses minus the
    // number of vertices ocurring in both witnesses. This subtraction will be
    // realized below when identifying vertices that occur in both witnesses.

    // First we merge all nodes in w_join->degree_0 into witness w
    for (unsigned v : w_join->degree_0) {
        if (wPrime->degree_0.count(v) ||
            wPrime->degree_2.count(v) ||
            wPrime->extremity.find(v) != wPrime->extremity.end()) {
            // If v already exists in witness w, v will keep the degree as in
            // witness w. Because v is already in degree_0, degree_2 or the
            // extremity map, it also will stay there
            // Since v occurrs in both witnesses, we decrease current_size by one
            wPrime->current_size--;
        } else {
            // If v doesn't occur in witness w (it is not contained in degree_0,
            // degree_2 or the extremity map), we insert it into witness w
            wPrime->degree_0.insert(v);
        }
    }

    // Then we merge all nodes in w_join->degree_2 into witness w
    for (unsigned v : w_join->degree_2) {
        if (wPrime->degree_2.count(v) ||
            wPrime->extremity.find(v) != wPrime->extremity.end()) {
            // If v already has degree_2 or is contained in the extremity map in
            // witness w, we are trying to merge a node of degree 2 with a node
            // of degree 1 or 2. Since this cannot be a cycle, the join is not well
            // defined for the input witnesses
            return;
        } else if (wPrime->degree_0.count(v)) {
            // If v has degree_0 in witness w, v will get the degree_2, hence we
            // delete it from degree_0 and insert it into degree_2.
            // Since v occurrs in both witnesses, we decrease current_size by one
            wPrime->current_size--;
            wPrime->degree_0.erase(v);
            wPrime->degree_2.insert(v);
        } else {
            // If v doesn't occur in witness w (it is not contained in degree_0,
            // degree_2 or the extremity map, we insert it into witness w
            wPrime->degree_2.insert(v);
        }
    }

    // Finally we will merge the w_join->extremity map into witness w, by processing
    // each element in w_join->extremity until exhaustion
    while (!w_join->extremity.empty()) {
        // u and v will be the nodes of one element from w_join->extremity
        // Because we will processed this element after this step,
        // we will remove u and v from w_join->extremity
        pair<unsigned, unsigned> elem = *wPrime->extremity.begin();
        unsigned u = elem.first;
        unsigned v = elem.second;
        w_join->extremity.erase(u);
        w_join->extremity.erase(v);
        if (wPrime->degree_2.count(u) || wPrime->degree_2.count(v)) {
            // We are trying to merge either u or v (the extremities of this element)
            // with a node of degree 2. Since this cannot be a cycle, the join is not
            // well defined for the input witnesses
            return;
        }

        // If u, v or both have degree_0 in witness w, we will remove it from
        // degree_0 and insert it into the extremity map later on.
        // If u or v is contained in degree_0, it occurrs in both witnesses, hence we
        // also decrease current_size by one in this case
        if (wPrime->degree_0.erase(u)) {
            wPrime->current_size--;
        }
        if (wPrime->degree_0.erase(v)) {
            wPrime->current_size--;
        }

        // In the following, we will check whether there exists a path in witness w
        // which extends the current path element. Therefore we first try to find
        // an extension for u and then for v (for convenience we do the extension
        // process twice with u but will swap node u and v before the second
        // iteration of the loop)
        for (unsigned i = 0; i < 2; i++) {
            auto u_extremity_it = wPrime->extremity.find(u);
            if (u_extremity_it != wPrime->extremity.end()) {
                // If we find an extremity for u in witness w then this node occurs
                // in both witnesses, hence we reduce the current_size by one
                unsigned u_extremity = u_extremity_it->second;
                wPrime->current_size--;
                // u is now inner node of a path, hence will be removed from
                // extremity and will be inserted into degree_2
                wPrime->extremity.erase(u);
                wPrime->degree_2.insert(u);
                if (u_extremity == v) {
                    // If u_extremity is the same node as v, we found a cycle
                    // Because v ( == u_extremity) occur in both witnessses
                    // we reduce current_size by one
                    wPrime->current_size--;
                    // v is now inner node of a path, hence will be removed from
                    // extremity and will be inserted into degree_2
                    wPrime->extremity.erase(v);
                    wPrime->degree_2.insert(v);
                    if (wPrime->degree_0.empty() && wPrime->extremity.empty() &&
                        wPrime->current_size <= this->parameter) {
                        wPrime->found_cycle = true;
                        wPrime->degree_2.clear();
                        witnessSet->insert(wPrime);
                        return;
                    } else {
                        return;
                    }
                }
                // u is now the new extremity of this element
                u = u_extremity;
            }
            // u becomes now v and we try to find an extension with v in the
            // second iteration
            swap(u, v);
        }
        // Update the corresponding extremities
        wPrime->extremity[u] = v;
        wPrime->extremity[v] = u;
    }
    if (wPrime->current_size <= this->parameter) {
        witnessSet->insert(wPrime);
    }
    //*****************************
    //*****************************
}

CycleGirth_AtMost_WitnessSetPointer CycleGirth_AtMost_DynamicCore::clean_implementation(
        CycleGirth_AtMost_WitnessSetPointer witnessSet) {
    return witnessSet;
}

bool CycleGirth_AtMost_DynamicCore::is_final_witness_implementation(CycleGirth_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    return w->current_size <= this->parameter &&
           w->degree_0.empty() &&
           w->extremity.empty() &&
           w->found_cycle;
    //*****************************
    //*****************************
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
bool CycleGirth_AtMost_Witness::is_equal(const Witness &rhs) const {
    if (CycleGirth_AtMost_Witness const *e = dynamic_cast<CycleGirth_AtMost_Witness const *>(&rhs)) {
        CycleGirth_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    } else {
        cerr << "ERROR: in CycleGirth_AtMost_Witness::is_equal cast error" << endl;
        exit(20);
    }
}

bool CycleGirth_AtMost_Witness::is_less(const Witness &rhs) const {
    if (CycleGirth_AtMost_Witness const *e = dynamic_cast<CycleGirth_AtMost_Witness const *>(&rhs)) {
        CycleGirth_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    } else {
        cerr << "ERROR: in CycleGirth_AtMost_Witness::is_less cast error" << endl;
        exit(20);
    }
}

Witness &CycleGirth_AtMost_Witness::set_equal(Witness &witness) {
    if (CycleGirth_AtMost_Witness *e = dynamic_cast<CycleGirth_AtMost_Witness *>(&witness)) {
        CycleGirth_AtMost_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    } else {
        cerr << "ERROR: in CycleGirth_AtMost_Witness::operator= cast error" << endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> CycleGirth_AtMost_WitnessSet::createEmptyWitnessSet() {
    CycleGirth_AtMost_WitnessSetPointer witnessSet(new CycleGirth_AtMost_WitnessSet);
    return witnessSet;
}

void CycleGirth_AtMost_DynamicCore::createInitialWitnessSet() {
    CycleGirth_AtMost_WitnessSetPointer witnessSet(new CycleGirth_AtMost_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

CycleGirth_AtMost_WitnessPointer CycleGirth_AtMost_DynamicCore::createWitness(){
    CycleGirth_AtMost_WitnessPointer w(new CycleGirth_AtMost_Witness);
    return w;
}

void CycleGirth_AtMost_DynamicCore::copyWitness(CycleGirth_AtMost_WitnessPointer w_input,
                                                CycleGirth_AtMost_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

WitnessSetPointer CycleGirth_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (CycleGirth_AtMost_Witness *e = dynamic_cast<CycleGirth_AtMost_Witness *>(&witness)) {
        CycleGirth_AtMost_WitnessPointer w = e->shared_from_this();
        CycleGirth_AtMost_WitnessSetPointer witnessSet(new CycleGirth_AtMost_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    } else {
        cerr << "ERROR: in CycleGirth_AtMost_DynamicCore::intro_v cast error" << endl;
        exit(20);
    }
}
WitnessSetPointer CycleGirth_AtMost_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (CycleGirth_AtMost_Witness *e = dynamic_cast<CycleGirth_AtMost_Witness *>(&witness)) {
        CycleGirth_AtMost_WitnessPointer w = e->shared_from_this();
        CycleGirth_AtMost_WitnessSetPointer witnessSet(new CycleGirth_AtMost_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    } else {
        cerr << "ERROR: in CycleGirth_AtMost_DynamicCore::intro_e cast error" << endl;
        exit(20);
    }
}
WitnessSetPointer CycleGirth_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (CycleGirth_AtMost_Witness *e = dynamic_cast<CycleGirth_AtMost_Witness *>(&witness)) {
        CycleGirth_AtMost_WitnessPointer w = e->shared_from_this();
        CycleGirth_AtMost_WitnessSetPointer witnessSet(new CycleGirth_AtMost_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    } else {
        cerr << "ERROR: in CycleGirth_AtMost_DynamicCore::forget_v cast error" << endl;
        exit(20);
    }
}
WitnessSetPointer CycleGirth_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (CycleGirth_AtMost_Witness *e1 = dynamic_cast<CycleGirth_AtMost_Witness *>(&witness1)) {
        if (CycleGirth_AtMost_Witness *e2 = dynamic_cast<CycleGirth_AtMost_Witness *>(&witness2)) {
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            CycleGirth_AtMost_WitnessPointer w1 = e1->shared_from_this();
            CycleGirth_AtMost_WitnessPointer w2 = e2->shared_from_this();
            CycleGirth_AtMost_WitnessSetPointer witnessSet(new CycleGirth_AtMost_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        } else {
            cerr << "ERROR: in CycleGirth_AtMost_DynamicCore::join cast error" << endl;
            exit(20);
        }
    } else {
        cerr << "ERROR: in CycleGirth_AtMost_DynamicCore::join cast error" << endl;
        exit(20);
    }
}

WitnessSetPointer CycleGirth_AtMost_DynamicCore::clean(WitnessSetPointer witnessSet) {
    if (CycleGirth_AtMost_WitnessSetPointer e = dynamic_pointer_cast<CycleGirth_AtMost_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in CycleGirth_AtMost_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool CycleGirth_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    if (CycleGirth_AtMost_Witness *e = dynamic_cast<CycleGirth_AtMost_Witness *>(&witness)) {
        CycleGirth_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    } else {
        cerr << "ERROR: in CycleGirth_AtMost_DynamicCore::is_final_witness cast error" << endl;
        exit(20);
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new CycleGirth_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new CycleGirth_AtMost_DynamicCore(param);
}
}