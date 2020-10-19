// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "HamiltonianCycle.h"

extern "C" {
DynamicCore * create() {
    return new HamiltonianCycle_DynamicCore;
}
}
bool HamiltonianCycle_Witness::is_equal(const Witness &rhs) const{
    if (HamiltonianCycle_Witness const *e = dynamic_cast<HamiltonianCycle_Witness const *>(&rhs)) {
        shared_ptr<HamiltonianCycle_Witness const> p = e->shared_from_this();
        //*****************************
        //*****************************
        if(this->degree_0 == p->degree_0 &&
           this->degree_1 == p->degree_1 &&
           this->degree_2 == p->degree_2 &&
           this->matching == p->matching &&
           this->closed == p->closed
                ){
            return true;
        }else{
            return false;
        }
        //*****************************
        //*****************************
    }else{
        cerr<<"ERROR: in HamiltonianCycle_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool HamiltonianCycle_Witness::is_less(const Witness &rhs)const {
    if (HamiltonianCycle_Witness const *e = dynamic_cast<HamiltonianCycle_Witness const *>(&rhs)) {
        shared_ptr<HamiltonianCycle_Witness const> p = e->shared_from_this();
        //*****************************
        //*****************************
        if(this->degree_0 == p->degree_0){
            if(this->degree_1 == p->degree_1){
                if(this->degree_2 == p->degree_2){
                    if(this->matching == p->matching){
                        return this->closed < p->closed;
                    }else return this->matching < p->matching;
                }else return this->degree_2 < p->degree_2;
            }else return this->degree_1 < p->degree_1;
        }else return this->degree_0 < p->degree_0;
        //*****************************
        //*****************************
    }else{
        cerr<<"ERROR: in HamiltonianCycle_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &HamiltonianCycle_Witness::set_equal(Witness &witness) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)) {
        shared_ptr<HamiltonianCycle_Witness> p = e->shared_from_this();
        //*****************************
        //*****************************
        this->degree_0 = p->degree_0;
        this->degree_1 = p->degree_1;
        this->degree_2 = p->degree_2;
        this->matching = p->matching;
        this->closed = p->closed;
        return *this;
        //*****************************
        //*****************************
    }else{
        cerr<<"ERROR: in HamiltonianCycle_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

void HamiltonianCycle_Witness::print() {
    //*****************************
    //*****************************
    cout << "Vertices of degree 0:" << endl << "\t";
    for(auto &v: this->degree_0) cout << v << " ";
    cout << endl;

    cout << "Vertices of degree 1:" << endl << "\t";
    for(auto &v: this->degree_1) cout << v << " ";
    cout << endl;

    cout << "Vertices of degree 2:" << endl << "\t";
    for(auto &v: this->degree_2) cout << v << " ";
    cout << endl;

    cout << "Matching:" << endl << "\t";
    for(auto &v: this->matching) cout << "(" << v.first << ", " << v.second << ") ";
    cout << endl;

    cout << "Closed: " << this->closed << endl;
    //*****************************
    //*****************************
}

HamiltonianCycle_Witness::~HamiltonianCycle_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************
}

HamiltonianCycle_DynamicCore::HamiltonianCycle_DynamicCore(){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName","HamiltonianCycle");
    addAttribute("ParameterType","NoParameter");
    addAttribute("PrimaryOperator","---");
    createInitialWitnessSet();
}

void HamiltonianCycle_DynamicCore::createInitialWitnessSet() {

    shared_ptr<HamiltonianCycle_Witness> witness(new HamiltonianCycle_Witness);
    shared_ptr<HamiltonianCycle_WitnessSet> witnessSet(new HamiltonianCycle_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    //*****************************
    //*****************************
    //The only initial witness is the empty map. Nothing needs to be done here.
    //*****************************
    //*****************************
    this->insertIntoInitialWitnessSet(witness);
}

shared_ptr<WitnessSet> HamiltonianCycle_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness  ) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)){
        shared_ptr<HamiltonianCycle_Witness> p = e->shared_from_this();
        shared_ptr<HamiltonianCycle_WitnessSet> witnessSet (new HamiltonianCycle_WitnessSet);
        //*****************************
        //*****************************
        shared_ptr<HamiltonianCycle_Witness> w1(new HamiltonianCycle_Witness);
        w1->set_equal(*p);
        //Only accepts a vertex if it's new and the witness is open
        if(w1->degree_0.count(i) == 0 && w1->degree_1.count(i) == 0 && w1->degree_2.count(i) == 0 && !w1->closed){
            w1->degree_0.insert(i);
            witnessSet->insert(w1);
        }
        //*****************************
        //*****************************
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> HamiltonianCycle_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)){
        shared_ptr<HamiltonianCycle_Witness> p = e->shared_from_this();
        shared_ptr<HamiltonianCycle_WitnessSet> witnessSet (new HamiltonianCycle_WitnessSet);
        //*****************************
        //*****************************
        shared_ptr<HamiltonianCycle_Witness> w0(new HamiltonianCycle_Witness);
        w0->set_equal(*p);
        //New witness without this edge
        witnessSet->insert(w0);

        //Only introduces an edge if it's open
        if(w0->closed) return clean(witnessSet);

        if(p->degree_0.count(i) && p->degree_0.count(j)){
            //Both vertices with degree 0
            shared_ptr<HamiltonianCycle_Witness> w1(new HamiltonianCycle_Witness);
            w1->set_equal(*p);
            w1->degree_0.erase(i);
            w1->degree_0.erase(j);
            w1->degree_1.insert(i);
            w1->degree_1.insert(j);
            w1->matching[i] = j;
            w1->matching[j] = i;
            witnessSet->insert(w1);
        }else if(p->degree_0.count(i) && p->degree_1.count(j)){
            //Vertex i with degree 0 and j degree 1
            shared_ptr<HamiltonianCycle_Witness> w1(new HamiltonianCycle_Witness);
            w1->set_equal(*p);
            w1->degree_0.erase(i);
            w1->degree_1.erase(j);
            w1->degree_1.insert(i);
            w1->degree_2.insert(j);
            //Changing the matching
            unsigned  m = w1->matching[j];
            w1->matching.erase(j);
            w1->matching[m] = i;
            w1->matching[i] = m;
            witnessSet->insert(w1);
        }else if(p->degree_1.count(i) && p->degree_0.count(j)){
            //Vertex j with degree 0 and i degree 1
            shared_ptr<HamiltonianCycle_Witness> w1(new HamiltonianCycle_Witness);
            w1->set_equal(*p);
            w1->degree_1.erase(i);
            w1->degree_0.erase(j);
            w1->degree_2.insert(i);
            w1->degree_1.insert(j);
            //Changing the matching
            unsigned  m = w1->matching[i];
            w1->matching.erase(i);
            w1->matching[m] = j;
            w1->matching[j] = m;
            witnessSet->insert(w1);
        }else if(p->degree_1.count(i) && p->degree_1.count(j)){
            //Vertices j an i with degree 1
            if(p->matching[i] != j){
                //Not in the match
                shared_ptr<HamiltonianCycle_Witness> w1(new HamiltonianCycle_Witness);
                w1->set_equal(*p);
                w1->degree_1.erase(i);
                w1->degree_1.erase(j);
                w1->degree_2.insert(i);
                w1->degree_2.insert(j);
                unsigned  mi = w1->matching[i];
                unsigned  mj = w1->matching[j];
                w1->matching.erase(i);
                w1->matching.erase(j);
                w1->matching[mj] = mi;
                w1->matching[mi] = mj;
                witnessSet->insert(w1);
            }else if(p->matching[i] == j && p->degree_0.size() == 0 && p->degree_1.size() == 2){
                //They are matching
                shared_ptr<HamiltonianCycle_Witness> w1(new HamiltonianCycle_Witness);
                w1->set_equal(*p);
                w1->degree_1.erase(i);
                w1->degree_1.erase(j);
                w1->degree_2.insert(i);
                w1->degree_2.insert(j);
                w1->matching.erase(i);
                w1->matching.erase(j);
                w1->closed = true;
                witnessSet->insert(w1);
            }
        }
        //*****************************
        //*****************************
        return clean(witnessSet);
    } else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> HamiltonianCycle_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)){
        shared_ptr<HamiltonianCycle_Witness> p = e->shared_from_this();
        shared_ptr<HamiltonianCycle_WitnessSet> witnessSet (new HamiltonianCycle_WitnessSet);
        //*****************************
        //*****************************
        shared_ptr<HamiltonianCycle_Witness> w1(new HamiltonianCycle_Witness);
        w1->set_equal(*p);

        //Only foget vertices with degree 2
        if(w1->degree_2.count(i) == 1){
            w1->degree_2.erase(i);
            witnessSet->insert(w1);
        }
        //*****************************
        //*****************************
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> HamiltonianCycle_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(HamiltonianCycle_Witness *e1 = dynamic_cast<HamiltonianCycle_Witness *>(&witness1)){
        if(HamiltonianCycle_Witness *e2 = dynamic_cast<HamiltonianCycle_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            shared_ptr<HamiltonianCycle_Witness> p1 = e1->shared_from_this();
            shared_ptr<HamiltonianCycle_Witness> p2 = e2->shared_from_this();
            shared_ptr<HamiltonianCycle_WitnessSet> witnessSet (new HamiltonianCycle_WitnessSet);
            //*****************************
            //*****************************
            shared_ptr<HamiltonianCycle_Witness> w1(new HamiltonianCycle_Witness);
            w1->set_equal(*p1); // Working Witness
            bool impossible = false;
            //We have copied the input witness1 into w1 and the input witness2 into w2.
            //The final witness will be w1.

            //Treating vertices of degree 0 in p1
            for (unsigned i: p1->degree_0)
            {
                // If i has degree 0 in p2 then it will have degree 0 in w1. Therefore this case does need to be treated.
                if (p2->degree_2.count(i))
                {
                    // In this case, i will have degree 2 in the new w1
                    w1->degree_0.erase(i);
                    w1->degree_2.insert(i);
                }
            }

            //Treating vertices of degree 2 in p1
            for (unsigned i: p1->degree_2)
            {
                // If i has degree 0 in p2 then it will have degree 2 in w1. Therefore this case does need to be treated.
                if (p2->degree_2.count(i))
                {
                    // in this case, i is being overused. This means that there is no way of combining the input witnesses into a valid witness
                    impossible = true;
                    break;
                }
            }

            if (not impossible){
                //Treating the edges of the matching.
                map<unsigned,unsigned> auxMatching = p2->matching;
                while(auxMatching.size())
                {
                    pair<unsigned,unsigned> edge = *(auxMatching.begin());
                    auxMatching.erase(edge.first);
                    auxMatching.erase(edge.second);
                    //We will treat case by case
                    if(w1->degree_0.count(edge.first) and w1->degree_0.count(edge.second))
                    {
                        // degrees 0 and 0
                        w1->degree_0.erase(edge.first);
                        w1->degree_1.insert(edge.first);
                        w1->degree_0.erase(edge.second);
                        w1->degree_1.insert(edge.second);
                        w1->matching[edge.first]=edge.second;
                        w1->matching[edge.second]=edge.first;
                    }
                    else if(w1->degree_0.count(edge.first) and w1->degree_1.count(edge.second))
                    {
                        // degrees 0 and 1
                        w1->degree_0.erase(edge.first);
                        w1->degree_1.insert(edge.first);
                        const unsigned m = w1->matching[edge.second];
                        w1->matching[edge.first] = m;
                        w1->matching[m] = edge.first;
                        w1->matching.erase(edge.second);
                        w1->degree_1.erase(edge.second);
                        w1->degree_2.insert(edge.second);
                    }
                    else if(w1->degree_0.count(edge.first) and w1->degree_2.count(edge.second))
                    {
                        // degrees 0 and 2
                        // vertices in overuse
                        // BAD WITNESS JOIN...
                        impossible = true;
                        break;
                    }
                    else if(w1->degree_1.count(edge.first) and w1->degree_0.count(edge.second))
                    {
                        // degrees 1 and 0
                        w1->degree_0.erase(edge.second);
                        w1->degree_1.insert(edge.second);
                        const unsigned m = w1->matching[edge.first];
                        w1->matching[edge.second] = m;
                        w1->matching[m] = edge.second;
                        w1->matching.erase(edge.first);
                        w1->degree_1.erase(edge.first);
                        w1->degree_2.insert(edge.first);
                    }
                    else if(w1->degree_1.count(edge.first) and w1->degree_1.count(edge.second))
                    {
                        // degrees 1 and 1
                        const unsigned m1 = w1->matching[edge.first]; // matching of edge.first
                        const unsigned m2 = w1->matching[edge.second]; // matching of edge.second
                        // Checking for closing cycles
                        if(m1 == edge.second)
                        {
                            if(not w1->closed)
                            {
                                // if not yet closed
                                w1->closed = true;
                                w1->matching.erase(m1);
                                w1->matching.erase(m2);
                                w1->degree_1.erase(m1);
                                w1->degree_1.erase(m2);
                                w1->degree_2.insert(m1);
                                w1->degree_2.insert(m2);
                            }else{
                                // impossible to close again
                                // BAD WITNESS JOIN...
                                impossible = true;
                                break;
                            }
                        }else
                        {
                            // Not closing cycle
                            w1->degree_1.erase(edge.first);
                            w1->degree_1.erase(edge.second);
                            w1->degree_2.insert(edge.first);
                            w1->degree_2.insert(edge.second);
                            w1->matching.erase(edge.first);
                            w1->matching.erase(edge.second);
                            w1->matching[m1] = m2;
                            w1->matching[m2] = m1;
                        }
                    }
                    else if(w1->degree_1.count(edge.first) and w1->degree_2.count(edge.second))
                    {
                        // degrees 1 and 2
                        // vertices in overuse
                        // BAD WITNESS JOIN...
                        impossible = true;
                        break;
                    }
                    else if(w1->degree_2.count(edge.first) and w1->degree_0.count(edge.second))
                    {
                        // degrees 2 and 2
                        // vertices in overuse
                        // BAD WITNESS JOIN...
                        impossible = true;
                        break;
                    }
                    else if(w1->degree_2.count(edge.first) and w1->degree_1.count(edge.second))
                    {
                        // degrees 2 and 2
                        // vertices in overuse
                        // BAD WITNESS JOIN...
                        impossible = true;
                        break;
                    }
                    else if(w1->degree_2.count(edge.first) and w1->degree_2.count(edge.second))
                    {
                        // degrees 2 and 2
                        // vertices in overuse
                        // BAD WITNESS JOIN...
                        impossible = true;
                        break;
                    }
                }

                // Verifying if it is a valid join
                if (not impossible)
                {
                    if(w1->closed)
                    {
                        if(w1->degree_0.size() == 0 and w1->degree_1.size() == 0)
                        {
                            // Only finishes closed if it's empty
                            witnessSet->insert(w1);
                        }
                    }
                    else
                    {
                        witnessSet->insert(w1);
                    }
                }
            }
            //*****************************
            //*****************************
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in HamiltonianCycle_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> HamiltonianCycle_DynamicCore::clean(shared_ptr<WitnessSet> witnessSet) {
    //*****************************
    //*****************************
    // In most cases, you will not need to change this function.
    //*****************************
    //*****************************
    return witnessSet;
}



bool HamiltonianCycle_DynamicCore::is_final_witness(Witness &witness) {
    if (HamiltonianCycle_Witness *e = dynamic_cast<HamiltonianCycle_Witness *>(&witness)) {
        shared_ptr<HamiltonianCycle_Witness> p = e->shared_from_this();
        //*****************************
        //*****************************
        // Verifies if there is no node with degree 0 or 1, and checks if it's already closed
        if(p->degree_0.size() == 0 &&
           p->degree_1.size() == 0 &&
           p->matching.size() == 0 &&
           p->closed
                ){
            return true;
        }else{
            return false;
        }
        //*****************************
        //*****************************
    }else{
        cerr<<"ERROR: in HamiltonianCycle_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}
