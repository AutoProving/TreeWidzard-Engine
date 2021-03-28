// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.
#include "EdgeColoringCompact_AtMost.h"

bool EdgeColoringCompact_AtMost_Witness::is_equal_implementation(const EdgeColoringCompact_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->colorIncidence == w->colorIncidence;
    //*****************************
    //*****************************
}

bool EdgeColoringCompact_AtMost_Witness::is_less_implementation(const EdgeColoringCompact_AtMost_WitnessPointerConst w) const {
    //*****************************
    //*****************************
    return this->colorIncidence < w->colorIncidence;
    //*****************************
    //*****************************
}

Witness & EdgeColoringCompact_AtMost_Witness::set_equal_implementation(EdgeColoringCompact_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    this->colorIncidence = w->colorIncidence;
    return *this;
    //*****************************
    //*****************************
}

shared_ptr<Witness> EdgeColoringCompact_AtMost_Witness::relabel(map<unsigned int, unsigned int> relabelingMap) {
    EdgeColoringCompact_AtMost_WitnessPointer relabeledWitness(new EdgeColoringCompact_AtMost_Witness);
    for(auto m:colorIncidence){
	set<unsigned> relabeledSet;
        for(auto x:m){	
		if(relabelingMap.count(x)){
        	    relabeledSet.insert(relabelingMap[x]);  
	        }else{
        	    cout<<"Error: EdgeColoringCompact_AtMost_Witness::relabel "<< x << " is not in the map"<<endl;
               	    exit(20);
        	}
	}
	relabeledWitness->colorIncidence.insert(relabeledSet);
    }
    return relabeledWitness;
}

void EdgeColoringCompact_AtMost_Witness::print() {
    //*****************************
    //*****************************
    // EXPECTED OUTPUT EXAMPLE
    // Witness :
    // (vertex1, { (#edge1, #color1) (#edge2, #color2) } )
    // (vertex2, { (#edge1, #color1) (#edge3, #color4) } )
    cout << "EdgeColoringCompact Witness:";
    for (auto s: this->colorIncidence){
	cout << "{";
	unsigned counter =0;
        for (auto x:s) {
            cout << x;
	    counter++; 
            if (counter < s.size()) cout << ",";
        }
        cout << "} ";
    }
    //*****************************
    //*****************************
}

string EdgeColoringCompact_AtMost_Witness::witnessInformation() {
    //*****************************
    //*****************************
    // EXPECTED OUTPUT EXAMPLE
    // Witness :
    // (vertex1, { (#edge1, #color1) (#edge2, #color2) } )
    // (vertex2, { (#edge1, #color1) (#edge3, #color4) } )
    string info;
    info = "EdgeColoringCompact Witness:";
    for (auto s: this->colorIncidence) {
        info = info +  "{";
	unsigned counter = 0;
        for (auto x:s) {
            info = info + to_string(x);
            counter++; 
	    if (counter < s.size()) info = info + ",";
        }
        info = info +"} ";
    }
    return info;
    //*****************************
    //*****************************
}
EdgeColoringCompact_AtMost_Witness::~EdgeColoringCompact_AtMost_Witness() {
    //*****************************
    //*****************************
    // Only edit this part
    // In most cases this function can be empty, if you need a specific destructor you can implement it here.
    //*****************************
    //*****************************

}
EdgeColoringCompact_AtMost_DynamicCore::EdgeColoringCompact_AtMost_DynamicCore() {
    // Initializing attributes
    addAttribute("CoreName","EdgeColoringCompact");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
}

EdgeColoringCompact_AtMost_DynamicCore::EdgeColoringCompact_AtMost_DynamicCore(unsigned parameter){
    //*****************************
    //*****************************
    // In most cases, you will not to need to change this function.
    //*****************************
    //*****************************
    // Initializing attributes
    addAttribute("CoreName","EdgeColoringCompact");
    addAttribute("ParameterType","UnsignedInt");
    addAttribute("PrimaryOperator","AtMost");
    this->parameter = parameter;
    createInitialWitnessSet();
}

void EdgeColoringCompact_AtMost_DynamicCore::createInitialWitnessSet_implementation() {
    EdgeColoringCompact_AtMost_WitnessPointer witness = createWitness();
    this->insertIntoInitialWitnessSet(witness);
}

void EdgeColoringCompact_AtMost_DynamicCore::intro_v_implementation(unsigned int i, Bag &b,
                                                             EdgeColoringCompact_AtMost_WitnessPointer w,
                                                             EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    //Just return a singleton with the input witness
    EdgeColoringCompact_AtMost_WitnessPointer witness = createWitness();
    witness->set_equal(*w);
    witnessSet->insert(witness);
    //*****************************
    //*****************************
}

void EdgeColoringCompact_AtMost_DynamicCore::intro_e_implementation(unsigned int i, unsigned int j, Bag &b,
                                                             EdgeColoringCompact_AtMost_WitnessPointer w,
                                                             EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    for (auto it = w->colorIncidence.begin(); it!=w->colorIncidence.end(); it++){
        bool equalPrevious = false;
        if (it!=w->colorIncidence.begin()){
            auto itPrime = it;
            itPrime--;
            if (*itPrime == *it){
                equalPrevious = true;
            }
	    }
        if (!equalPrevious){
            if ( (it->find(i)== it->end()) and (it->find(j)==it->end())){
                multiset<set<unsigned> > newColorIncidence;
                    // copies all sets except the one represented by the iterator it
                for (auto itPrime = w->colorIncidence.begin(); itPrime !=w->colorIncidence.end(); itPrime++){
                    if (itPrime != it){
                        newColorIncidence.insert(*itPrime);
                    }
                }
                // adds the mofied version of the set represneted by it
                set<unsigned> modifiedSet = *it;
                modifiedSet.insert(i);
                modifiedSet.insert(j);
                newColorIncidence.insert(modifiedSet);
                // creates a new witness
                EdgeColoringCompact_AtMost_WitnessPointer witness = createWitness();
                witness->set_equal(*w);
                witness->colorIncidence = newColorIncidence;
                witnessSet->insert(witness);
            }
        }
    }
    if (w->colorIncidence.size()<this->parameter){
        // In this case, there is a possibility of creating a new color for the edge {i,j}
        EdgeColoringCompact_AtMost_WitnessPointer witness = createWitness();
        witness->set_equal(*w);
        set<unsigned> newColor;
        newColor.insert(i);
        newColor.insert(j);
        witness->colorIncidence.insert(newColor);
        witnessSet->insert(witness);
    }
    //*****************************
    //*****************************
}

void EdgeColoringCompact_AtMost_DynamicCore::forget_v_implementation(unsigned int i, Bag &b,
                                                              EdgeColoringCompact_AtMost_WitnessPointer w,
                                                              EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet) {
    //*****************************
    //*****************************
    //Erases the vertex from each set and, if empty remove the set. 
    EdgeColoringCompact_AtMost_WitnessPointer witness = createWitness();
    witness->set_equal(*w);
    multiset<set<unsigned> > newColorIncidence; 
    for (auto s:w->colorIncidence){
    	if (s.find(i)== s.end()){
		    newColorIncidence.insert(s);
        }else{
            s.erase(i);
            if (s.size()>0){
                newColorIncidence.insert(s);
            }
        }
    }
    witness->colorIncidence = newColorIncidence;
    witnessSet->insert(witness);
    //*****************************
    //*****************************
}



void  EdgeColoringCompact_AtMost_DynamicCore::constructCombinations(multiset<set<unsigned> > m_1, multiset<set<unsigned> > m_2, multiset<set<unsigned> > result, set<multiset<set<unsigned> > > &L){
	if (m_1.size()==0){
		if (result.size()+ m_2.size() <= this->parameter){
			multiset<set<unsigned> > newResult = result; 
			for (auto s:m_2){
				newResult.insert(s); 
			}
//			cout << endl; 	
//			cout << "----------------------------------" << endl; 
//			cout << "----------BEGIN CASE 1--------------" << endl;
//			cout << "----------------------------------" << endl; 
//
//			cout << "Printing m_1" << endl; 
//			for (auto s:m_1){
//				cout << "{";
//	       			for (auto a:s){
//					cout << a << ","; 
//				}	
//				cout << "}"; 
//			}
//			cout << endl;
//			cout << "Printing m_2" << endl; 
//		      	for (auto s:m_2){
//				cout << "{";
//			       	for (auto a:s){
//					cout << a << ","; 
//				}	
//				cout << "}"; 
//			}
//		      	cout << endl; 
//			cout << "Printing Result" << endl; 
//		      	for (auto s:result){
//				cout << "{";
//			       	for (auto a:s){
//					cout << a << ","; 
//				}	
//				cout << "}"; 
//			}
//		      	cout << endl; 
//			cout << "NewResult:" << endl; 
//			for (auto s:newResult){
//				cout << "{";
//			       	for (auto a:s){
//					cout << a << ","; 
//				}	
//				cout << "}"; 
//			}
//	
//			cout << endl; 	
//			cout << "----------------------------------" << endl; 
//			cout << "----------END CASE 1--------------" << endl;
//			cout << "----------------------------------" << endl; 
//



			L.insert(newResult);
		}
	} else {
		if (result.size()<this->parameter){
			// selects the first element of m_1
			multiset<set<unsigned> > m_1_aux = m_1;
			set<unsigned > s_1 = *(m_1_aux.begin());
			m_1_aux.erase(m_1_aux.begin());
			// Adds the first color in m_1 as an independent color in newResult
			multiset<set<unsigned> > newResult = result; 
		        newResult.insert(s_1); 	

//			cout << endl;
//			cout << "----------------------------------" << endl; 
//			cout << "----------- BEGIN CASE 2-----------"<< endl;
//			cout << "----------------------------------" << endl;
//			cout << "Printing m_1" << endl; 
//			for (auto s:m_1){
//				cout << "{";
//	       			for (auto a:s){
//					cout << a << ","; 
//				}	
//				cout << "}"; 
//			}
//			cout << endl;
//			cout << "Printing m_2" << endl; 
//		      	for (auto s:m_2){
//				cout << "{";
//			       	for (auto a:s){
//					cout << a << ","; 
//				}	
//				cout << "}"; 
//			}
//		      	cout << endl; 
//			cout << "Printing Result" << endl; 
//		      	for (auto s:result){
//				cout << "{";
//			       	for (auto a:s){
//					cout << a << ","; 
//				}	
//				cout << "}"; 
//			}
//		      	cout << endl;
//			cout << "Printing m_1_aux" << endl; 
//			for (auto s:m_1_aux){
//				cout << "{";
//	       			for (auto a:s){
//					cout << a << ","; 
//				}	
//				cout << "}"; 
//			}
//			cout << endl;
//
//			cout << "NewResult:" << endl; 
//			for (auto s:newResult){
//				cout << "{";
//			       	for (auto a:s){
//					cout << a << ","; 
//				}	
//				cout << "}"; 
//			}
//			cout << endl; 	
//			cout << "----------------------------------" << endl; 
//			cout << "----------END CASE 2--------------" << endl;
//			cout << "----------------------------------" << endl; 
//

			
			
			
			
			constructCombinations(m_1_aux, m_2, newResult, L);
			// Combines the first color in m_1 with some color in newResult
			for (auto it = m_2.begin(); it!=m_2.end(); it++){
				//test whether s_1 is disjoint from *it
				set<unsigned> unionSets = s_1;
				for (auto a:(*it)){
					unionSets.insert(a); 
				}
				//  if s_1 is disjoint from *it, the union has size s_1.size()+*it.size(). 
				if (unionSets.size() == s_1.size() + (*it).size()){
					//creates a copy of m_2 with *it removed. Note that we cannot use m_2_aux.erase(*it) because this would erase all elements equal to *it, and we only want to delete one. 
					multiset<set<unsigned> > m_2_aux;
					for (auto itPrime = m_2.begin(); itPrime != m_2.end(); itPrime++){
						if (itPrime != it){
							m_2_aux.insert(*itPrime);
						}
					}
					// add unionSets as a new color in newResult
					newResult = result; 
				        newResult.insert(unionSets); 
//
//					cout << "----------------------------------" << endl; 
//					cout << "----------- BEGIN CASE 3-----------"<< endl;
//					cout << "----------------------------------" << endl;
//					cout << "Printing m_1" << endl; 
//					for (auto s:m_1){
//						cout << "{";
//			       			for (auto a:s){
//							cout << a << ","; 
//						}	
//						cout << "}"; 
//					}
//					cout << endl;
//					cout << "Printing m_2" << endl; 
//				      	for (auto s:m_2){
//						cout << "{";
//			       			for (auto a:s){
//							cout << a << ","; 
//						}	
//						cout << "}"; 
//					}
//				      	cout << endl; 
//					cout << "Printing Result" << endl; 
//				      	for (auto s:result){
//						cout << "{";
//					       	for (auto a:s){
//							cout << a << ","; 
//						}	
//						cout << "}"; 
//					}
//				      	cout << endl;
//					cout << "Printing m_1_aux" << endl; 
//					for (auto s:m_1_aux){
//						cout << "{";
//	       					for (auto a:s){
//							cout << a << ","; 
//						}	
//						cout << "}"; 
//					}
//					cout << endl;
//					cout << "Printing m_2_aux" << endl; 
//					for (auto s:m_2_aux){
//						cout << "{";
//	       					for (auto a:s){
//							cout << a << ","; 
//						}	
//						cout << "}"; 
//					}
//					cout << endl;
//					cout << "NewResult:" << endl; 
//					for (auto s:newResult){
//						cout << "{";
//					       	for (auto a:s){
//							cout << a << ","; 
//						}	
//						cout << "}"; 
//					}
//					cout << endl; 	
//					cout << "----------------------------------" << endl; 
//					cout << "----------END CASE 3--------------" << endl;
//					cout << "----------------------------------" << endl; 
//




					constructCombinations(m_1_aux, m_2_aux, newResult, L);
				}
			}
		
		}
	}
}



void EdgeColoringCompact_AtMost_DynamicCore::join_implementation(Bag &b, EdgeColoringCompact_AtMost_WitnessPointer w1,
                                                          EdgeColoringCompact_AtMost_WitnessPointer w2,
                                                          EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet){
	set<multiset<set<unsigned> > > L;
	multiset<set<unsigned> > emptyColorSet;
//	cout << endl; 
//	cout << "----------------------------------" << endl; 
//	cout << "----------ENTERING JOIN ----------" << endl;
//	cout << "----------------------------------" << endl; 
////	cout << "Printing w1" << endl; 
////	w1->print();
////	cout << endl;
////	cout << "Printing w2" << endl; 
////	w2->print();
////

	constructCombinations(w1->colorIncidence,w2->colorIncidence,emptyColorSet,L);
        
//	cout << "----------------------------------" << endl;
//	cout << "Printing w1" << endl; 
//	w1->print();
//	cout << endl;
//	cout << "Printing w2" << endl; 
//	w2->print();
//        cout << endl; 	
//	cout << "Combinations:" << endl;
//        int counter = 0;	
//	for (auto m:L){
//		cout << counter << ": "; 
//		counter++;
//		for (auto s:m){
//			cout << "{";
//		       	for (auto a:s){
//				cout << a << ","; 
//			}	
//			cout << "}"; 
//		}
//		cout << endl; 
//	}
//	cout << endl; 
//	cout << "----------------------------------" << endl; 
//	cout << "----------EXITING JOIN -----------" << endl;
//	cout << "----------------------------------" << endl; 

	for (auto it = L.begin(); it!= L.end(); it++){
		EdgeColoringCompact_AtMost_WitnessPointer witness = createWitness();
		witness->colorIncidence = *it; 
		witnessSet->insert(witness);
	}
}

EdgeColoringCompact_AtMost_WitnessSetPointer EdgeColoringCompact_AtMost_DynamicCore::clean_implementation(
        EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet) {
    return witnessSet;
}

bool EdgeColoringCompact_AtMost_DynamicCore::is_final_witness_implementation(EdgeColoringCompact_AtMost_WitnessPointer w) {
    //*****************************
    //*****************************
    return true; // All witnesses are consistent, since we guarantee that no more than this->parameter colors occur at each step.
    //*****************************
    //*****************************
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool EdgeColoringCompact_AtMost_Witness::is_equal(const Witness &rhs) const{
    if (EdgeColoringCompact_AtMost_Witness const *e = dynamic_cast<EdgeColoringCompact_AtMost_Witness const *>(&rhs)) {
        EdgeColoringCompact_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_equal_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoringCompact_AtMost_Witness::is_equal cast error"<<endl;
        exit(20);
    }
}

bool EdgeColoringCompact_AtMost_Witness::is_less(const Witness &rhs)const {
    if (EdgeColoringCompact_AtMost_Witness const *e = dynamic_cast<EdgeColoringCompact_AtMost_Witness const *>(&rhs)) {
        EdgeColoringCompact_AtMost_WitnessPointerConst w = e->shared_from_this();
        return is_less_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoringCompact_AtMost_Witness::is_less cast error"<<endl;
        exit(20);
    }
}

Witness &EdgeColoringCompact_AtMost_Witness::set_equal(Witness &witness) {
    if (EdgeColoringCompact_AtMost_Witness *e = dynamic_cast<EdgeColoringCompact_AtMost_Witness *>(&witness)) {
        EdgeColoringCompact_AtMost_WitnessPointer w = e->shared_from_this();
        return set_equal_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoringCompact_AtMost_Witness::operator= cast error"<<endl;
        exit(20);
    }
}

shared_ptr<WitnessSet> EdgeColoringCompact_AtMost_WitnessSet::createEmptyWitnessSet() {
    EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet(new EdgeColoringCompact_AtMost_WitnessSet);
    return witnessSet;
}

EdgeColoringCompact_AtMost_WitnessPointer EdgeColoringCompact_AtMost_DynamicCore::createWitness() {
    EdgeColoringCompact_AtMost_WitnessPointer w(new EdgeColoringCompact_AtMost_Witness);
    return w;
}

void EdgeColoringCompact_AtMost_DynamicCore::copyWitness(EdgeColoringCompact_AtMost_WitnessPointer w_input,
                                                  EdgeColoringCompact_AtMost_WitnessPointer w_output) {
    w_output->set_equal_implementation(w_input);
}

void EdgeColoringCompact_AtMost_DynamicCore::createInitialWitnessSet() {
    EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet(new EdgeColoringCompact_AtMost_WitnessSet);
    this->setInitialWitnessSet(witnessSet);
    createInitialWitnessSet_implementation();
}

WitnessSetPointer EdgeColoringCompact_AtMost_DynamicCore::intro_v(unsigned i, Bag &b, Witness &witness) {
    if (EdgeColoringCompact_AtMost_Witness *e = dynamic_cast<EdgeColoringCompact_AtMost_Witness *>(&witness)){
        EdgeColoringCompact_AtMost_WitnessPointer w = e->shared_from_this();
        EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet (new EdgeColoringCompact_AtMost_WitnessSet);
        intro_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in EdgeColoringCompact_AtMost_DynamicCore::intro_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer EdgeColoringCompact_AtMost_DynamicCore::intro_e(unsigned i, unsigned j, Bag &b, Witness &witness) {
    if (EdgeColoringCompact_AtMost_Witness *e = dynamic_cast<EdgeColoringCompact_AtMost_Witness *>(&witness)){
        EdgeColoringCompact_AtMost_WitnessPointer w = e->shared_from_this();
        EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet (new EdgeColoringCompact_AtMost_WitnessSet);
        intro_e_implementation(i,j,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in EdgeColoringCompact_AtMost_DynamicCore::intro_e cast error"<< endl;
        exit(20);
    }
}

WitnessSetPointer EdgeColoringCompact_AtMost_DynamicCore::forget_v(unsigned i, Bag &b, Witness &witness) {
    if (EdgeColoringCompact_AtMost_Witness *e = dynamic_cast<EdgeColoringCompact_AtMost_Witness *>(&witness)){
        EdgeColoringCompact_AtMost_WitnessPointer w = e->shared_from_this();
        EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet (new EdgeColoringCompact_AtMost_WitnessSet);
        forget_v_implementation(i,b,w,witnessSet);
        return clean(witnessSet);
    }else{
        cerr<<"ERROR: in EdgeColoringCompact_AtMost_DynamicCore::forget_v cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer EdgeColoringCompact_AtMost_DynamicCore::join(Bag &b, Witness &witness1, Witness &witness2) {
    if(EdgeColoringCompact_AtMost_Witness *e1 = dynamic_cast<EdgeColoringCompact_AtMost_Witness *>(&witness1)){
        if(EdgeColoringCompact_AtMost_Witness *e2 = dynamic_cast<EdgeColoringCompact_AtMost_Witness *>(&witness2)){
            // Note: Do not merge the two "if" instructions above into a single "if". Potential for errors
            EdgeColoringCompact_AtMost_WitnessPointer w1 = e1->shared_from_this();
            EdgeColoringCompact_AtMost_WitnessPointer w2 = e2->shared_from_this();
            EdgeColoringCompact_AtMost_WitnessSetPointer witnessSet (new EdgeColoringCompact_AtMost_WitnessSet);
            join_implementation(b,w1,w2,witnessSet);
            return clean(witnessSet);
        }else{
            cerr<<"ERROR: in EdgeColoringCompact_AtMost_DynamicCore::join cast error"<<endl;
            exit(20);
        }
    }else{
        cerr<<"ERROR: in EdgeColoringCompact_AtMost_DynamicCore::join cast error"<<endl;
        exit(20);
    }
}

WitnessSetPointer EdgeColoringCompact_AtMost_DynamicCore::clean(WitnessSetPointer witnessSet) {
    if (EdgeColoringCompact_AtMost_WitnessSetPointer e = dynamic_pointer_cast<EdgeColoringCompact_AtMost_WitnessSet >(witnessSet)) {
        return clean_implementation(e);
    }else{
        cerr<<"ERROR: in EdgeColoringCompact_DynamicCore::clean cast error"<<endl;
        exit(20);
    }
}

bool EdgeColoringCompact_AtMost_DynamicCore::is_final_witness(Witness &witness) {
    if (EdgeColoringCompact_AtMost_Witness *e = dynamic_cast<EdgeColoringCompact_AtMost_Witness *>(&witness)) {
        EdgeColoringCompact_AtMost_WitnessPointer w = e->shared_from_this();
        return is_final_witness_implementation(w);
    }else{
        cerr<<"ERROR: in EdgeColoringCompact_AtMost_DynamicCore::is_final_witness cast error"<<endl;
        exit(20);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// The functions below are used by the plugin handler /////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" {
DynamicCore * create() {
    return new EdgeColoringCompact_AtMost_DynamicCore;
}
DynamicCore * create_int(unsigned param) {
    return new EdgeColoringCompact_AtMost_DynamicCore(param);
}
}
