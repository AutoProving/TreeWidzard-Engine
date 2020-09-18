// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "CliqueNumber_AtLeast.h"

bool CliqueNumber_AtLeast_Witness::is_equal(const Witness &rhs) const{
    if (CliqueNumber_AtLeast_Witness const *e = dynamic_cast<CliqueNumber_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<CliqueNumber_AtLeast_Witness const> p = e->shared_from_this();
        return found == p->found and edges == p->edges;
    }else{
        cerr<<"ERROR: in Dynamic_Clique_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool CliqueNumber_AtLeast_Witness::is_less(const Witness &rhs)const {
    if (CliqueNumber_AtLeast_Witness const *e = dynamic_cast<CliqueNumber_AtLeast_Witness const *>(&rhs)) {
        shared_ptr<CliqueNumber_AtLeast_Witness const> p = e->shared_from_this();
        return found < p->found or (found == p->found and edges < p->edges);
    }else{
        cerr<<"ERROR: in Dynamic_Clique_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &CliqueNumber_AtLeast_Witness::set_equal(Witness &witness) {
    if (CliqueNumber_AtLeast_Witness *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
        shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();
        found = p->found;
        edges = p->edges;
        return *this;
    }else{
        cerr<<"ERROR: in Dynamic_Clique_Witness::operator= cast error"<<endl;
        exit(20);
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
        cerr<<"ERROR: in Dynamic_Clique_WitnessSet::is_equal cast error"<<endl;
        exit(20);
    }
}*/

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

CliqueNumber_AtLeast_Witness::~CliqueNumber_AtLeast_Witness() {

}


CliqueNumber_AtLeast_DynamicCore::CliqueNumber_AtLeast_DynamicCore(unsigned cliqueSize){
    this->cliqueSize = cliqueSize;
    createInitialWitnessSet();
}

void CliqueNumber_AtLeast_DynamicCore::createInitialWitnessSet() {
    shared_ptr<CliqueNumber_AtLeast_Witness> witness(new CliqueNumber_AtLeast_Witness);
    witness->found = false;
	insertIntoInitialWitnessSet(witness);
}

shared_ptr<WitnessSet> CliqueNumber_AtLeast_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (CliqueNumber_AtLeast_Witness *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)){
        shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();
        shared_ptr<WitnessSet> witnessSet (new WitnessSet);
        witnessSet->insert(p); 
        return witnessSet;
    }else{
        cerr<<"ERROR: in Dynamic_Clique::intro_v cast error"<<endl;
        exit(20);
    }
}


shared_ptr<WitnessSet> CliqueNumber_AtLeast_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (CliqueNumber_AtLeast_Witness *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)){
        //shared_ptr<CliqueNumber_AtLeast_Witness> p = make_shared<CliqueNumber_AtLeast_Witness>(*e);
        shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();

        shared_ptr<WitnessSet> witnessSet (new WitnessSet);
        if (p->found){
            witnessSet->insert(p);
        } else {
            set<pair<unsigned,unsigned> > aux = p->edges;
            set<unsigned> vertices = verticesOnPartialClique(aux);
            pair<unsigned,unsigned> newedge;
            if (i<j){
                newedge = make_pair(i,j);
            } else if (j<i) {
                newedge = make_pair(j,i);
            }
            if (vertices.find(i) != vertices.end() and vertices.find(j) != vertices.end()){
                // Suppose that both endpoints of the edge are in the partial clique.
                // Then we have no choice. We need to add the edge to the partial clique.
                aux.insert(newedge);
                if (vertices.size() == cliqueSize and aux.size() == cliqueSize*(cliqueSize-1)/2){
                    // if the partial clique gets complete, we are done, meaning that we set found to true and the edgeset to empty.
                    shared_ptr<CliqueNumber_AtLeast_Witness> w(new CliqueNumber_AtLeast_Witness);
                    w->found = true;
                    witnessSet->insert(w);
                } else {
                    // otherwise, found is still false and the edge set is equal to the augmented edge set.
                    shared_ptr<CliqueNumber_AtLeast_Witness> w(new CliqueNumber_AtLeast_Witness);
                    w->found = false;
                    w->edges = aux;
                    witnessSet->insert(w);
                }   
            } else {
                // If one of the endpoints is not in the partial clique, we can choose either to add the edge or not to add.
                witnessSet->insert(p); // here we choose not to add the edge
                aux.insert(newedge);
                if (verticesOnPartialClique(aux).size() <= cliqueSize) {
                    shared_ptr<CliqueNumber_AtLeast_Witness> w(new CliqueNumber_AtLeast_Witness);
                    w->found = false;
                    w->edges = aux;
                    witnessSet->insert(w);
                }
             }
        }
	return witnessSet;
    } else{
        cerr<<"ERROR: in Dynamic_Clique::intro_e cast error"<< endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> CliqueNumber_AtLeast_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (CliqueNumber_AtLeast_Witness *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)){
        shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();
        shared_ptr<WitnessSet> witnessSet(new WitnessSet);

	    set<unsigned> vertices = verticesOnPartialClique(p->edges);
            
	    if (vertices.find(i) == vertices.end() or p->found){
	 	// this means that i does not belong to the clique and therefore it does not affect the witness.
		 witnessSet->insert(p); // MAKE SURE THAT p IS A SHARED POINTER
	    } else {
                // Otherwise the witness will be affected in some way.
                set<unsigned> neighbors = neighborsOnPartialClique(p->edges, i);
                if (neighbors.size() == cliqueSize-1){
                    witnessSet->insert(p); // We don't change the witness. Edges containing is still remain there in.
                } 
                // Note that if the if condition is not satisfied, then the witness is deffective,
                // because we tried to delete a vertex before adding all its neighbors on the clique.
                // As a consequence, witnessSet is emtpy (has no witness)
	    }
        return witnessSet;
    }else{
        cerr<<"ERROR: in Dynamic_Clique::forget_v cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> CliqueNumber_AtLeast_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(CliqueNumber_AtLeast_Witness *e1 = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness1)){
        if(CliqueNumber_AtLeast_Witness *e2 = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness2)){
            shared_ptr<CliqueNumber_AtLeast_Witness> p1 = e1->shared_from_this();
            shared_ptr<CliqueNumber_AtLeast_Witness> p2 = e2->shared_from_this();
            // Note: Do not try to join these two ifs into a single if using AND. Potential for errors.
            shared_ptr<WitnessSet> witnessSet(new WitnessSet);
            set<pair<unsigned,unsigned> > aux;
            set_union(p1->edges.begin(),p1->edges.end(),p2->edges.begin(),p2->edges.end(),std::inserter(aux,aux.begin()));
            set<unsigned> vertices = verticesOnPartialClique(aux);
            if (p1->found){
                witnessSet->insert(p1);
            } else if (p2->found){
                witnessSet->insert(p2);
            } else if (vertices.size() == cliqueSize  and aux.size() == cliqueSize*(cliqueSize-1)/2){
                // In this case a clique was found.
                shared_ptr<CliqueNumber_AtLeast_Witness> w(new CliqueNumber_AtLeast_Witness);
                w->found = true;
                witnessSet->insert(w); //
            } else if (vertices.size() <=  cliqueSize and aux.size() != cliqueSize*(cliqueSize-1)/2){
                shared_ptr<CliqueNumber_AtLeast_Witness> w(new CliqueNumber_AtLeast_Witness);
                w->found = false;
                w->edges = aux;
                witnessSet->insert(w); //
            } else{
                // In this case, the partial clique has more vertices than required and the witness is deffective
                // this implies that witnessSet is empty. (This else has no commands.)
            }
	    return witnessSet; 
	}else{
        	cerr<<"ERROR: in Dynamic_Clique::join cast error"<<endl;
        	exit(20);
    	}
    }else{
        cerr<<"ERROR: in Dynamic_Clique::join cast error"<<endl;
        exit(20);
    }
}

bool CliqueNumber_AtLeast_DynamicCore::is_final_witness(Witness &witness) {
    if (CliqueNumber_AtLeast_Witness *e = dynamic_cast<CliqueNumber_AtLeast_Witness *>(&witness)) {
        shared_ptr<CliqueNumber_AtLeast_Witness> p = e->shared_from_this();
        return p->found;
    }else{
        cerr<<"ERROR: in Dynamic_Clique::is_final_witness cast error"<<endl;
        exit(20);
    }
}
