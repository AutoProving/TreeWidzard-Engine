#include "CliqueNumber_AtLeast.h"


/////////////////////////////Implementation////////////////////////////////////////////////

bool CliqueNumber_AtLeast_Witness::is_equal_implementation(const CliqueNumber_AtLeast_WitnessPointerConst w) const{
    return this->found == w->found and this->edges==w->edges;
}

bool CliqueNumber_AtLeast_Witness::is_less_implementation(const CliqueNumber_AtLeast_WitnessPointerConst w) const{
    if(this->found >  w->found) return true;
    if(this->found == w->found) return this->edges < w->edges;
    return false;
}

Witness& CliqueNumber_AtLeast_Witness::set_equal_implementation(CliqueNumber_AtLeast_WitnessPointer w){
    this->found = w->found;
    this->edges = w->edges;
    return *this;
}



shared_ptr<Witness> CliqueNumber_AtLeast_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    if(this->found){
        return this->shared_from_this();
    }else{
        CliqueNumber_AtLeast_WitnessPointer relabeledWitness (new CliqueNumber_AtLeast_Witness);
        relabeledWitness->found = false;
        for(auto p:edges){
            if(relabelingMap.count(p.first) and relabelingMap.count(p.second)){
                relabeledWitness->edges.insert(make_pair(relabelingMap[p.first],relabelingMap[p.second]));
            }else{
                cout<<"Error: CliqueNumber_AtLeast_Witness::relabel "<< p.first<< " or "<< p.second<<" is not "
                                                                                                     "in the map"<<endl;
                print();
                for(auto l:relabelingMap){
                    cout<<l.first<<"->"<<l.second<<endl;
                }
                exit(20);
            }
        }
        return relabeledWitness;
    }
}

void CliqueNumber_AtLeast_Witness::print() {
    cout << "found = " << found << " ";
	cout<<"E={";
    for(set<pair<unsigned,unsigned>>::iterator it = edges.begin() ; it != edges.end() ; ++it){
        cout<<"("<<it->first<<","<<it->second<<")";
    }
    cout<<"}"; 
}

string CliqueNumber_AtLeast_Witness::witnessInformation() {
    string info;
    info =  "found = " +  to_string(found) + " ";
    info = info + "E={";
    for(set<pair<unsigned,unsigned>>::iterator it = edges.begin() ; it != edges.end() ; ++it){
        info = info + "(" + to_string(it->first) + "," + to_string(it->second) + ")";
    }
    info = info + "}";
    return info;
}

// Auxiliary Function. Vertices seen in the partial clique.
set<unsigned> verticesOnPartialClique(set<pair<unsigned,unsigned>> edges){
	set<unsigned> aux;
	for (set<pair<unsigned,unsigned>>::iterator it = edges.begin(); it!= edges.end(); it++){
	    aux.insert(it->first);
	    aux.insert(it->second); 
	}
	return aux;
}

// Auxiliary Function. Neighbors of i seen in the clique. 
set<unsigned> neighborsOnPartialClique(set<pair<unsigned,unsigned>> edges, int i){
	set<unsigned> aux;
	for (set<pair<unsigned,unsigned>>::iterator it = edges.begin(); it!= edges.end(); it++){
	    if (it->first == i) {
                aux.insert(it->second);
            }else if (it->second == i){
                aux.insert(it->first);
	    }	
	}
	return aux;
}


CliqueNumber_AtLeast_DynamicCore::CliqueNumber_AtLeast_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","CliqueNumber");
    addAttribute("CoreType","Bool");
    addAttribute("Restriction","SimpleGraphs");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtLeast");

}

CliqueNumber_AtLeast_DynamicCore::CliqueNumber_AtLeast_DynamicCore(unsigned cliqueSize){
    this->cliqueSize = cliqueSize;
    createInitialWitnessSet();
    // Initializing attributes
    addAttribute("CoreName","CliqueNumber");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtLeast");
}

void CliqueNumber_AtLeast_DynamicCore::createInitialWitnessSet_implementation() {
    CliqueNumber_AtLeast_WitnessPointer w = createWitness();
    w->found = false;
    this->insertIntoInitialWitnessSet(w);
}

void CliqueNumber_AtLeast_DynamicCore::intro_v_implementation(unsigned i, Bag &b, CliqueNumber_AtLeast_WitnessPointer w,
            CliqueNumber_AtLeast_WitnessSetPointer witnessSet){
     witnessSet->insert(w);
}

void CliqueNumber_AtLeast_DynamicCore::intro_e_implementation(unsigned i, unsigned j, Bag &b,
        CliqueNumber_AtLeast_WitnessPointer w, CliqueNumber_AtLeast_WitnessSetPointer witnessSet){
    
    if (w->found){
            witnessSet->insert(w);
    } else {
        set<pair<unsigned,unsigned> > aux = w->edges;
        pair<unsigned,unsigned> newEdge;
        if (i<j){
            newEdge = make_pair(i,j);
        } else if (j<i) {
            newEdge = make_pair(j,i);
        }else {
            cerr<<"Error: Loop is inserted in intro_e"<<endl;
            exit(20);
        }
        auto itExist = aux.find(newEdge);
        if(itExist == aux.end()){
            //{i,j} isn't added to the clqiue.
            set<unsigned> vertices = verticesOnPartialClique(aux);// All vertices in the clique.
            if(vertices.find(i) != vertices.end() and vertices.find(j)!=vertices.end()){
                // Both i and j are in the clique, so the edge {i,j} will be added to the clqiue
                aux.insert(newEdge);
                CliqueNumber_AtLeast_WitnessPointer witness = createWitness();
                witness->edges = aux;
                if(vertices.size() == cliqueSize and witness->edges.size() == ((cliqueSize*(cliqueSize-1))/2) ){
                    // It is a complete clique of size cliqueSize
                    witness->found = true;
                    witness->edges.clear();
                }else{
                    witness->found = false;
                }
                witnessSet->insert(witness);

            }else if(vertices.find(i)!=vertices.end() and vertices.size()!=cliqueSize){
                // i is in the clique, but j is not in the clique, and size of the clique is smaller than cliqueSize
                // Therefore, j can be added to the clique
                // j is added to the clique and {i,j} is added to the clique
                aux.insert(newEdge);
                CliqueNumber_AtLeast_WitnessPointer witness = createWitness();
                witness->edges = aux;
                if(vertices.size() == cliqueSize-1 and witness->edges.size() == ((cliqueSize*(cliqueSize-1))/2) ){
                    // It is a complete clique of size cliqueSize
                    //(vertices.size() == cliqueSize-1 => because before adding j to the clique it should have
                    // a size cliqueSize-1, therefore, after adding j it has a size of cliqueSize
                    witness->found = true;
                    witness->edges.clear();
                }else{
                    witness->found = false;
                }
                witnessSet->insert(witness);

            }else if(vertices.find(j)!=vertices.end() and vertices.size()!=cliqueSize){
                // j is in the clique, but i is not in the clique, and size of the clique is smaller than cliqueSize
                // Therefore, i can be added to the clique
                // i is added to the clique and {i,j} is added to the clique
                aux.insert(newEdge);
                CliqueNumber_AtLeast_WitnessPointer witness = createWitness();
                witness->edges = aux;
                if(vertices.size() == cliqueSize-1 and witness->edges.size() == ((cliqueSize*(cliqueSize-1))/2) ){
                    // It is a complete clique of size cliqueSize
                    //(vertices.size() == cliqueSize-1 => because before adding i to the clique it should have
                    // a size cliqueSize-1, therefore, after adding i it has a size of cliqueSize
                    witness->found = true;
                    witness->edges.clear();
                }else{
                    witness->found = false;
                }
                witnessSet->insert(witness);

            }else if(vertices.size() < cliqueSize -1){
                // i and j both are not in the clqiue, and size of the clique is less than cliqueSize-1
                // Therefore, i and j can be added to the clique
                aux.insert(newEdge);
                CliqueNumber_AtLeast_WitnessPointer witness = createWitness();
                witness->edges = aux;
                if(vertices.size() == cliqueSize-2 and witness->edges.size() == ((cliqueSize*(cliqueSize-1))/2) ){
                    // It is a complete clique of size cliqueSize
                    //(vertices.size() == cliqueSize-1 => because before adding i and j to the clique it should have
                    // a size cliqueSize-2, therefore, after adding i and j it has a size of cliqueSize
                    witness->found = true;
                    witness->edges.clear();
                }else{
                    witness->found = false;
                }
                witnessSet->insert(witness);

            }else{
                //All possible cases to add {i,j} did not happen, so there is not a valid witness, and witnessSet
                // will be empty.
            }

        }else{
            // {i,j} has been added, therefore, w will be return.
            witnessSet->insert(w);
        }
    }
}

void CliqueNumber_AtLeast_DynamicCore::forget_v_implementation(unsigned i, Bag &b, CliqueNumber_AtLeast_WitnessPointer w,
        CliqueNumber_AtLeast_WitnessSetPointer witnessSet){
    set<unsigned> vertices = verticesOnPartialClique(w->edges);
    if(w->found){
        witnessSet->insert(w);
    }else{
	     // Otherwise the witness will be affected in some way.
        set<unsigned> neighbors = neighborsOnPartialClique(w->edges, i);
        if (neighbors.size() == cliqueSize-1){
                witnessSet->insert(w); // We don't change the witness. Edges containing is still remain there in.
        } 
         // Note that if the if condition is not satisfied, then the witness is deffective,
         // because we tried to delete a vertex before adding all its neighbors on the clique.
         // As a consequence, witnessSet is emtpy (has no witness) 
    }
    /* My approach
    // Observation: If a graph has a clique of size cliqueSize, then there exist a bag in
    // all treedecompositions of the graph such that that bag contains all vertices of the clique.
    // Base on the observation, if i is part of a clique of size cliqueSize, then it will
    // appear in one bag with all vertices of the clique. Therefore, when forgetting i from 
    // a witness with an incomplete clique, that witness will be invalid.
    if(w->found){
        witnessSet->insert(w);
    }
    */

}

void CliqueNumber_AtLeast_DynamicCore::join_implementation(Bag &b, CliqueNumber_AtLeast_WitnessPointer w1,
        CliqueNumber_AtLeast_WitnessPointer w2, CliqueNumber_AtLeast_WitnessSetPointer witnessSet){

   if (w1->found){
       witnessSet->insert(w1);
   } else if (w2->found){
       witnessSet->insert(w2);
   } else{
       set<pair<unsigned,unsigned> > aux;
       set_union(w1->edges.begin(),w1->edges.end(),w2->edges.begin(),w2->edges.end(),
           std::inserter(aux,aux.begin()));
       set<unsigned> vertices = verticesOnPartialClique(aux);
       if (vertices.size() == cliqueSize  and aux.size() == cliqueSize*(cliqueSize-1)/2){
          // In this case a clique was found.
          CliqueNumber_AtLeast_WitnessPointer witness = createWitness();
          witness->found = true;
          witnessSet->insert(witness); //
      } else if (vertices.size() <=  cliqueSize and aux.size() != cliqueSize*(cliqueSize-1)/2){
          CliqueNumber_AtLeast_WitnessPointer witness = createWitness();
          witness->found = false;
          witness->edges = aux;
          witnessSet->insert(witness); //
      } else{
          // In this case, the partial clique has more vertices than required and the witness is deffective
          // this implies that witnessSet is empty. (This else has no commands.)
      }
   }   
}

CliqueNumber_AtLeast_WitnessSetPointer CliqueNumber_AtLeast_DynamicCore::clean_implementation(CliqueNumber_AtLeast_WitnessSetPointer witnessSet){
     for(auto witness:(*witnessSet)){
        if (CliqueNumber_AtLeast_WitnessPointer w = dynamic_pointer_cast<CliqueNumber_AtLeast_Witness>(witness)) {
            if(w->found){
                CliqueNumber_AtLeast_WitnessSetPointer newWitnessSet (new CliqueNumber_AtLeast_WitnessSet);
                newWitnessSet->insert(w);
                return newWitnessSet;
            }
        }else{
            cerr<<"ERROR: in CliqueNumber_AtLeast_DynamicCore::clean_implementation cast error"<<endl;
            exit(20);
        }
    }
    return witnessSet;
}

bool CliqueNumber_AtLeast_DynamicCore::is_final_witness_implementation(CliqueNumber_AtLeast_WitnessPointer w){
    return w->found;
}


///////////////////////////////////////////////////////////////////////////////////////////

bool CliqueNumber_AtLeast_Witness::is_equal(const Witness &rhs) const{
    if(CliqueNumber_AtLeast_Witness const *e = dynamic_cast<CliqueNumber_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<CliqueNumber_AtLeast_Witness const> w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in CliqueNumber_AtLeast_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool CliqueNumber_AtLeast_Witness::is_less(const Witness &rhs)const {
    if (CliqueNumber_AtLeast_Witness const *e = dynamic_cast<CliqueNumber_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<CliqueNumber_AtLeast_Witness const> w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in CliqueNumber_AtLeast_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &CliqueNumber_AtLeast_Witness::set_equal(Witness &witness) {
   if (CliqueNumber_AtLeast_Witness  *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
        shared_ptr<CliqueNumber_AtLeast_Witness> w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in CliqueNumber_AtLeast_Witness::is_less cast error"<<endl;
        exit(20);
    }
}



/*void CliqueNumber_AtLeast_WitnessSet::print() {
    for (WitnessSetIterator it = this->begin() ; it != this->end() ; ++it) {
        (*it)->print();
    }

}*/

/*WitnessSet *CliqueNumber_AtLeast_WitnessSet::createWitnessSet() {
    CliqueNumber_AtLeast_WitnessSet *w = new CliqueNumber_AtLeast_WitnessSet;
    return w;
}*/

/*bool CliqueNumber_AtLeast_WitnessSet::is_equal(WitnessSet &rhs) {
    if (CliqueNumber_AtLeast_WitnessSet *p = dynamic_cast<CliqueNumber_AtLeast_WitnessSet *>(&rhs)) {
        if(this->container.size() == p->container.size()){
            WitnessSetIterator itr = p->begin();
            for(WitnessSetIterator it = this->begin(); it != this->end() ; ++it){
                if((**it == **itr)){
                    itr++;
                }else{
                    return false;
                }
            }
            return true;
        }else{
            return false;
        }
    }else{
        cerr<<"ERROR: in Dynamic_CliqueNumber_WitnessSet::is_equal cast error"<<endl;
        exit(20);
    }
}*/



void CliqueNumber_AtLeast_DynamicCore::createInitialWitnessSet() {
    CliqueNumber_AtLeast_WitnessSetPointer witnessSet(new CliqueNumber_AtLeast_WitnessSet);
    setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

void CliqueNumber_AtLeast_DynamicCore::copyWitness(CliqueNumber_AtLeast_WitnessPointer w_input,
        CliqueNumber_AtLeast_WitnessPointer w_output){

    w_output->set_equal_implementation(w_input);
}

CliqueNumber_AtLeast_WitnessPointer CliqueNumber_AtLeast_DynamicCore::createWitness(){
    CliqueNumber_AtLeast_WitnessPointer w (new CliqueNumber_AtLeast_Witness);
    return w;
}

WitnessSetPointer CliqueNumber_AtLeast_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
     if (CliqueNumber_AtLeast_Witness *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
       // shared_ptr<CliqueNumber_AtLeast_Witness> p = make_shared<CliqueNumber_AtLeast_Witness>(*e);
        CliqueNumber_AtLeast_WitnessPointer w = e->shared_from_this();
        CliqueNumber_AtLeast_WitnessSetPointer witnessSet(new CliqueNumber_AtLeast_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in CliqueNumber_AtLeast_Witness::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer CliqueNumber_AtLeast_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
     if (CliqueNumber_AtLeast_Witness *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
       // shared_ptr<CliqueNumber_AtLeast_Witness> p = make_shared<CliqueNumber_AtLeast_Witness>(*e);
        CliqueNumber_AtLeast_WitnessPointer w = e->shared_from_this();
        CliqueNumber_AtLeast_WitnessSetPointer witnessSet(new CliqueNumber_AtLeast_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in CliqueNumber_AtLeast_Witness::intro_e cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer CliqueNumber_AtLeast_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
   if (CliqueNumber_AtLeast_Witness *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
       // shared_ptr<CliqueNumber_AtLeast_Witness> p = make_shared<CliqueNumber_AtLeast_Witness>(*e);
        CliqueNumber_AtLeast_WitnessPointer w = e->shared_from_this();
        CliqueNumber_AtLeast_WitnessSetPointer witnessSet(new CliqueNumber_AtLeast_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in CliqueNumber_AtLeast_Witness::forget_v cast error"<<endl;
        exit(20);
    }  
}

WitnessSetPointer CliqueNumber_AtLeast_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if (CliqueNumber_AtLeast_Witness  *e1 = dynamic_cast<CliqueNumber_AtLeast_Witness  *>(&witness1)){
          if(CliqueNumber_AtLeast_Witness  *e2 = dynamic_cast<CliqueNumber_AtLeast_Witness  *>(&witness2)) {
              shared_ptr<CliqueNumber_AtLeast_Witness> w1 = e1->shared_from_this();
              shared_ptr<CliqueNumber_AtLeast_Witness> w2 = e2->shared_from_this();
              shared_ptr<CliqueNumber_AtLeast_WitnessSet> witnessSet(new CliqueNumber_AtLeast_WitnessSet);
              join_implementation(b,w1,w2,witnessSet);
              return clean(witnessSet);
          }else{
              cerr<<"ERROR: in CliqueNumber_AtLeast_Witness::join cast error"<<endl;
              exit(20);
          }
      }else{
          cerr<<"ERROR: in CliqueNumber_AtLeast_Witness::join cast error"<<endl;
          exit(20);
      }
}

WitnessSetPointer CliqueNumber_AtLeast_DynamicCore::clean(WitnessSetPointer witnessSet) {
    if (CliqueNumber_AtLeast_WitnessSetPointer e = dynamic_pointer_cast<CliqueNumber_AtLeast_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in CliqueNumber_AtLeast_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool CliqueNumber_AtLeast_DynamicCore::is_final_witness(Witness &witness) {

    if (CliqueNumber_AtLeast_Witness *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
        CliqueNumber_AtLeast_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in CliqueNumber_AtLeast_Witness::is_final_witness cast error"<<endl;
        exit(20);
    }
}
shared_ptr<WitnessSet> CliqueNumber_AtLeast_WitnessSet::createEmptyWitnessSet() {
    CliqueNumber_AtLeast_WitnessSetPointer witnessSet(new CliqueNumber_AtLeast_WitnessSet);
    return witnessSet;
}
///////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new CliqueNumber_AtLeast_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new CliqueNumber_AtLeast_DynamicCore(param);
}
}
