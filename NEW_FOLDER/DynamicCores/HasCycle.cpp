// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "HasCycle.h"

extern "C" {
DynamicCore *create() {
    return new HasCycle_DynamicCore;
}

}

///////////////////////////// IMPLEMENTATION OF CORES ////////////////////////////////////

bool HasCycle_Witness::is_equal_implementation(const HasCycle_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->degree_0 == w->degree_0 &&
           this->degree_2 == w->degree_2 &&
           this->extremity == w->extremity &&
           this->found_cycle == w->found_cycle;
    //*****************************
    //*****************************
}

bool HasCycle_Witness::is_less_implementation(const HasCycle_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    if (this->found_cycle != w->found_cycle ) return this->found_cycle > w->found_cycle;
    if (this->degree_0 != w->degree_0) return this->degree_0 < w->degree_0;
    if (this->degree_2 != w->degree_2) return this->degree_2 < w->degree_2;
    return this->extremity < w->extremity;
    //*****************************
    //*****************************
}

Witness &HasCycle_Witness::set_equal_implementation(HasCycle_WitnessPointer w) {
    //*****************************
    //*****************************
    this->degree_0 = w->degree_0;
    this->degree_2 = w->degree_2;
    this->extremity = w->extremity;
    this->found_cycle = w->found_cycle;
    return *this;
    //*****************************
    //*****************************
}

void HasCycle_Witness::print() {
    //*****************************
    //*****************************
    cout << "Vertices of degree 0: ";
    for (unsigned i : this->degree_0) cout << i << " ";
    cout << endl
         << endl;
    cout << "Vertices of degree 2: ";
    for (unsigned i : this->degree_2) cout << i << " ";
    cout << endl
         << endl;
    cout << "Corresponding Extremity Map (key -> value):" << endl;
    for (auto &elem : this->extremity) cout << elem.first << " -> " << elem.second << endl;
    cout << endl;
    cout << "Found Cycle: " << this->found_cycle << endl;
    //*****************************
    //*****************************
}

HasCycle_Witness::~HasCycle_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

HasCycle_DynamicCore::HasCycle_DynamicCore() {
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName", "CycleGirth");
    addAttribute("CoreType","Bool");
    addAttribute("ParameterType","ParameterLess");
    addAttribute("PrimaryOperator","---");
    createInitialWitnessSet();
    //*****************************
    //*****************************
}

void HasCycle_DynamicCore::createInitialWitnessSet_implementation() {
    HasCycle_WitnessPointer witness = createWitness();
    this->insertIntoInitialWitnessSet(witness);
}

void HasCycle_DynamicCore::intro_v_implementation(unsigned int i, Bag &b, HasCycle_WitnessPointer w, HasCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    if(w->found_cycle){
        HasCycle_WitnessPointer witness = createWitness();
        witness->found_cycle = true;
        witnessSet->insert(witness);
    }else{
        // In this witness we don't add i to cycle
        HasCycle_WitnessPointer w_without_i = createWitness();
        w_without_i->set_equal(*w);
        witnessSet->insert(w_without_i);

        // In this witness we add i to cycle
        HasCycle_WitnessPointer w_with_i = createWitness();
        w_with_i->set_equal(*w);
        w_with_i->degree_0.insert(i);
        witnessSet->insert(w_with_i);
    }
    //*****************************
    //*****************************
}

void HasCycle_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b, HasCycle_WitnessPointer w, HasCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    if(w->found_cycle){
        HasCycle_WitnessPointer witness = createWitness();
        witness->found_cycle = true;
        witnessSet->insert(witness);
    }else{
        // Insert witness, where edge is not introduced
        HasCycle_WitnessPointer w_no_edge = createWitness();
        w_no_edge->set_equal(*w);
        witnessSet->insert(w_no_edge);

        // Now calculate and insert witness, where edge is introduced
        // If the degree of either endpoint of the edge is already 2, then
        // we are trying to add an edge to an inner node of the cycle.
        // This is not allowed and therefore no additional witness is created.
        if (w->degree_2.count(i) || w->degree_2.count(j)) {
            // nothing
        } else if (w->degree_0.count(i) && w->degree_0.count(j)) {
            // If both i and j have degree 0 in the input witness, then
            // by connecting these nodes, their degree becomes 1.
            HasCycle_WitnessPointer w_connect_nodes = createWitness();
            w_connect_nodes->set_equal(*w);
            w_connect_nodes->extremity[i] = j;
            w_connect_nodes->extremity[j] = i;
            w_connect_nodes->degree_0.erase(i);
            w_connect_nodes->degree_0.erase(j);
            witnessSet->insert(w_connect_nodes);
        } else {
            auto i_extremity_it = w->extremity.find(i);
            auto j_extremity_it = w->extremity.find(j);
            // If one endpoint of the edge has degree 0 (say, i) and the other has degree 1 (say, j),
            // then i is an extremety of a path. This path gets extended by the new edge (i,j) and
            // now j becomes the new endpoint of this path.
            if ((w->degree_0.count(i) && j_extremity_it != w->extremity.end()) ||
                (w->degree_0.count(j) && i_extremity_it != w->extremity.end())) {
                // Swap i and j, such that j has always degree 0 and i always degree 1
                if (!w->degree_0.count(i)) {
                    swap(i, j);
                    swap(i_extremity_it, j_extremity_it);
                }
                unsigned i_extremity = j_extremity_it->second;
                // extend cycle (i is always inner node: j - i - i_extremity)
                HasCycle_WitnessPointer w_extend = createWitness();
                w_extend->set_equal(*w);
                w_extend->degree_0.erase(i);
                ///////BUG//////////
                //w_extend->extremity[i_extremity] = j;
                w_extend->extremity[i_extremity] = i;
                ////////////////////
                w_extend->extremity[i] = i_extremity;
                w_extend->extremity.erase(j);
                w_extend->degree_2.insert(j);
                witnessSet->insert(w_extend);
            } else if (i_extremity_it != w->extremity.end() && j_extremity_it != w->extremity.end()) {
                // The next IF analyses the case in which both endpoints of the edge have degree 1 in the input witness
                unsigned i_extremity = j_extremity_it->second;
                unsigned j_extremity = i_extremity_it->second;
                // There are two cases to be analysed. In the first case, i and j are the
                // extremities of a path. Therefore by adding the edge (i,j) the cycle gets closed
                if (i_extremity == i && j_extremity == j) {  // found cycle
                    HasCycle_WitnessPointer w_cycle = createWitness();
                    w_cycle->set_equal(*w);
                    w_cycle->extremity.erase(i);
                    w_cycle->extremity.erase(j);
                    w_cycle->degree_2.insert(i);
                    w_cycle->degree_2.insert(j);
                    /////ISSUE: IS it necessary to check w_cycle->degree_0.empty() && w_cycle->extremity.empty()//////
                    if (w_cycle->degree_0.empty() && w_cycle->extremity.empty()) {
                        w_cycle->found_cycle = true;
                        witnessSet->insert(w_cycle);
                    } else {
                        // nothing
                    }
                }else {
                    // In the second case, i is an extremity of a path and j is an extremity of another path
                    // In this case these paths are merged by connecting i and j.
                    HasCycle_WitnessPointer w_connect_parts = createWitness();
                    w_connect_parts->set_equal(*w);
                    w_connect_parts->extremity.erase(i);
                    w_connect_parts->extremity.erase(j);
                    w_connect_parts->degree_2.insert(i);
                    w_connect_parts->degree_2.insert(j);
                    w_connect_parts->extremity[j_extremity] = i_extremity;
                    w_connect_parts->extremity[i_extremity] = j_extremity;
                    witnessSet->insert(w_connect_parts);
                }
            }
        }
    }

    //*****************************
    //*****************************
}

void HasCycle_DynamicCore::forget_v_implementation(unsigned int i, Bag &b, HasCycle_WitnessPointer w, HasCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    if(w->found_cycle){
        HasCycle_WitnessPointer witness = createWitness();
        witness->found_cycle = true;
        witnessSet->insert(witness);
    }else if (w->degree_2.count(i)) {
        HasCycle_WitnessPointer w_forgot = createWitness();
        w_forgot->set_equal(*w);
        w_forgot->degree_2.erase(i);
        witnessSet->insert(w_forgot);
    }
    //*****************************
    //*****************************
}

void HasCycle_DynamicCore::join_implementation(Bag &b, HasCycle_WitnessPointer w1, HasCycle_WitnessPointer w2, HasCycle_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    if(w1->found_cycle or w2->found_cycle){
        HasCycle_WitnessPointer witness = createWitness();
        witnessSet->insert(witness);
        return;
    }
    HasCycle_WitnessPointer w_prime = createWitness();
    w_prime->set_equal(*w1);  // This is a copy of the first witness and will
    // be the resulting merged witness
    HasCycle_WitnessPointer w_join = createWitness();
    w_join->set_equal(*w2);  // This is a copy of the second witness and will
    // be merged into witness w
    w_prime->found_cycle = w_prime->found_cycle || w_join->found_cycle;

    // First we merge all nodes in w_join->degree_0 into witness w
    for (unsigned i : w_join->degree_0) {
        if (w_prime->degree_0.count(i) ||
            w_prime->degree_2.count(i) ||
            w_prime->extremity.find(i) != w_prime->extremity.end()) {
            // If i already exists in witness w, i will keep the degree as in
            // witness w. Because i is already in degree_0, degree_2 or the
            // extremity map, it also will stay there
        } else {
            // If i doesn't occur in witness w (it is not contained in degree_0,
            // degree_2 or the extremity map), we insert it into witness w
            w_prime->degree_0.insert(i);
        }
    }

    // Then we merge all nodes in w_join->degree_2 into witness w
    for (unsigned i : w_join->degree_2) {
        if (w_prime->degree_2.count(i) ||
            w_prime->extremity.find(i) != w_prime->extremity.end()) {
            // If i already has degree_2 or is contained in the extremity map in
            // witness w, we are trying to merge a node of degree 2 with a node
            // of degree 1 or 2. Since this cannot be a cycle, the join is not well
            // defined for the input witnesses
            return;
        } else if (w_prime->degree_0.count(i)) {
            // If i has degree_0 in witness w, i will get the degree_2, hence we
            // delete it from degree_0 and insert it into degree_2.
            // Since i occurrs in both witnesses, we flip is_odd
            ////w_prime->is_odd = !w_prime->is_odd;
            w_prime->degree_0.erase(i);
            w_prime->degree_2.insert(i);
        } else {
            // If i doesn't occur in witness w (it is not contained in degree_0,
            // degree_2 or the extremity map, we insert it into witness w
            w_prime->degree_2.insert(i);
        }
    }

    // Finally we will merge the w_join->extremity map into witness w, by processing
    // each element in w_join->extremity until exhaustion
    while (!w_join->extremity.empty()) {
        // i and j will be the nodes of one element from w_join->extremity
        // Because we will processed this element after this step,
        // we will remove i and j from w_join->extremity
        pair<unsigned, unsigned> elem = *w_prime->extremity.begin();
        unsigned i = elem.first;
        unsigned j = elem.second;
        w_join->extremity.erase(i);
        w_join->extremity.erase(j);
        if (w_prime->degree_2.count(i) || w_prime->degree_2.count(j)) {
            // We are trying to merge either i or j (the extremities of this element)
            // with a node of degree 2. Since this cannot be a cycle, the join is not
            // well defined for the input witnesses
            return;
        }

        // If i, j or both have degree_0 in witness w, we will remove it from
        // degree_0 and insert it into the extremity map later on.
        // If i or j is contained in degree_0, it occurrs in both witnesses, hence we
        // also flip is_odd in this case
        if (w_prime->degree_0.erase(i)) {
            w_prime->is_odd = !w_prime->is_odd;
        }
        if (w_prime->degree_0.erase(j)) {
            w_prime->is_odd = !w_prime->is_odd;
        }

        // In the following, we will check whether there exists a path in witness w
        // which extends the current path element. Therefore we first try to find
        // an extension for i and then for j (for convenience we do the extension
        // process twice with i but will swap node i and j before the second
        // iteration of the loop)
        for (unsigned k = 0; k < 2; k++) {
            auto i_extremity_it = w_prime->extremity.find(i);
            if (i_extremity_it != w_prime->extremity.end()) {
                // If we find an extremity for i in witness w then this node occurs
                // in both witnesses, hence we flip is_odd
                unsigned i_extremity = i_extremity_it->second;
                w_prime->is_odd = !w_prime->is_odd;
                // i is now inner node of a path, hence will be removed from
                // extremity and will be inserted into degree_2
                w_prime->extremity.erase(i);
                w_prime->degree_2.insert(i);
                if (i_extremity == i) {
                    // If i_extremity is the same node as i, we found a cycle
                    // Because i ( == i_extremity) occur in both witnessses
                    // we flip is_odd
                    w_prime->is_odd = !w_prime->is_odd;
                    // i is now inner node of a path, hence will be removed from
                    // extremity and will be inserted into degree_2
                    w_prime->extremity.erase(i);
                    w_prime->degree_2.insert(i);
                    if (w_prime->degree_0.empty() && w_prime->extremity.empty() && w_prime->is_odd) {
                        w_prime->found_cycle = true;
                        witnessSet->insert(w_prime);
                        return;
                    } else {
                        return;
                    }
                }
                // i is now the new extremity of this element
                i = i_extremity;
            }
            // i becomes now j and we try to find an extension with j in the
            // second iteration
            swap(i, j);
        }
        // Update the corresponding extremities
        w_prime->extremity[j] = i;
        w_prime->extremity[i] = j;
    }
    if (w_prime->is_odd) {
        witnessSet->insert(w_prime);
    }
    //*****************************
    //*****************************
}

WitnessSetPointer HasCycle_DynamicCore::clean(WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    // In most cases, you will not need to change this function.
    //*****************************
    //*****************************
    return witnessSet;
}

bool HasCycle_DynamicCore::is_final_witness_implementation(HasCycle_WitnessPointer w) {
    //*****************************
    //*****************************
    return w->is_odd &&
           w->degree_0.empty() &&
           w->extremity.empty() &&
           w->found_cycle;
    //*****************************
    //*****************************
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


bool HasCycle_Witness::is_equal(const Witness &rhs) const {
    if (HasCycle_Witness const *e = dynamic_cast<HasCycle_Witness const *>(&rhs)) {
        HasCycle_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    } else {
        cerr << "ERROR: in HasCycle_Witness::is_equal cast error" << endl;
        exit(20);
    }
}

bool HasCycle_Witness::is_less(const Witness &rhs) const {
    if (HasCycle_Witness const *e = dynamic_cast<HasCycle_Witness const *>(&rhs)) {
        HasCycle_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    } else {
        cerr << "ERROR: in HasCycle_Witness::is_less cast error" << endl;
        exit(20);
    }
}

Witness &HasCycle_Witness::set_equal(Witness &witness) {
    if (HasCycle_Witness *e = dynamic_cast<HasCycle_Witness *>(&witness)) {
        HasCycle_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    } else {
        cerr << "ERROR: in HasCycle_Witness::operator= cast error" << endl;
        exit(20);
    }
}
void HasCycle_DynamicCore::createInitialWitnessSet() {
    HasCycle_WitnessSetPointer witnessSet(new HasCycle_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

HasCycle_WitnessPointer HasCycle_DynamicCore::createWitness() {
    HasCycle_WitnessPointer w(new HasCycle_Witness);
    return w;
}

void HasCycle_DynamicCore::copyWitness(HasCycle_WitnessPointer w_input, HasCycle_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

WitnessSetPointer HasCycle_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (HasCycle_Witness *e = dynamic_cast<HasCycle_Witness *>(&witness)) {
        HasCycle_WitnessPointer w = e->shared_from_this();
        HasCycle_WitnessSetPointer witnessSet(new HasCycle_WitnessSet);
        intro_v_implementation(i, b, w, witnessSet);
        return clean(witnessSet);
    } else {
        cerr << "ERROR: in HasCycle_DynamicCore::intro_v cast error" << endl;
        exit(20);
    }
}

WitnessSetPointer HasCycle_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (HasCycle_Witness *e = dynamic_cast<HasCycle_Witness *>(&witness)) {
        HasCycle_WitnessPointer w = e->shared_from_this();
        HasCycle_WitnessSetPointer witnessSet(new HasCycle_WitnessSet);
        intro_e_implementation(i, j, b, w, witnessSet);
        return clean(witnessSet);
    } else {
        cerr << "ERROR: in HasCycle_DynamicCore::intro_e cast error" << endl;
        exit(20);
    }
}

WitnessSetPointer HasCycle_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (HasCycle_Witness *e = dynamic_cast<HasCycle_Witness *>(&witness)) {
        HasCycle_WitnessPointer w = e->shared_from_this();
        HasCycle_WitnessSetPointer witnessSet(new HasCycle_WitnessSet);
        forget_v_implementation(i, b, w, witnessSet);
        return clean(witnessSet);
    } else {
        cerr << "ERROR: in HasCycle_DynamicCore::forget_v cast error" << endl;
        exit(20);
    }
}

WitnessSetPointer HasCycle_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (HasCycle_Witness *e1 = dynamic_cast<HasCycle_Witness *>(&witness1)) {
        if (HasCycle_Witness *e2 = dynamic_cast<HasCycle_Witness *>(&witness2)) {
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            HasCycle_WitnessPointer w1 = e1->shared_from_this();
            HasCycle_WitnessPointer w2 = e2->shared_from_this();
            HasCycle_WitnessSetPointer witnessSet(new HasCycle_WitnessSet);
            join_implementation(b, w1, w2, witnessSet);
            return clean(witnessSet);
        } else {
            cerr << "ERROR: in HasCycle_DynamicCore::join cast error" << endl;
            exit(20);
        }
    } else {
        cerr << "ERROR: in HasCycle_DynamicCore::join cast error" << endl;
        exit(20);
    }
}

bool HasCycle_DynamicCore::is_final_witness(Witness &witness) {
    if (HasCycle_Witness *e = dynamic_cast<HasCycle_Witness *>(&witness)) {
        HasCycle_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    } else {
        cerr << "ERROR: in HasCycle_DynamicCore::is_final_witness cast error" << endl;
        exit(20);
    }
}