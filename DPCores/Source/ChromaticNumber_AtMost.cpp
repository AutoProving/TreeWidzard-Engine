#include "ChromaticNumber_AtMost.h"

bool ChromaticNumber_AtMost_Witness::is_equal_implementation(
	const ChromaticNumber_AtMost_WitnessPointerConst w) const {
	/*return this->partialColoring == w->partialColoring and
		this->numberUsedColors == w->numberUsedColors;*/
	return this->partialColoring == w->partialColoring;
}

bool ChromaticNumber_AtMost_Witness::is_less_implementation(
	const ChromaticNumber_AtMost_WitnessPointerConst w) const {
	/*if(this->numberUsedColors < w->numberUsedColors) return true;
	if(this->numberUsedColors == w->numberUsedColors) return
	this->partialColoring < w->partialColoring; retrun false;*/
	return this->partialColoring < w->partialColoring;
}

Witness &ChromaticNumber_AtMost_Witness::set_equal_implementation(
	ChromaticNumber_AtMost_WitnessPointerConst w) {
	// this->numberUsedColors = w->numberUsedColors;
	this->partialColoring = w->partialColoring;
	return *this;
}

void ChromaticNumber_AtMost_Witness::print() const {
	cout << "partialColoring:";
	for (auto cell : partialColoring) {
		cout << "{ ";
		for (auto item : cell) {
			cout << item;
			if (item != *cell.crbegin()) {
				cout << ",";
			}
		}
		cout << "}";
	}
	cout << endl;
}

string ChromaticNumber_AtMost_Witness::witnessInformation() const {
	string info;
	info = "partialColoring:";
	for (auto cell : partialColoring) {
		info = info + "{ ";
		for (auto item : cell) {
			info += to_string(item);
			if (item != *cell.crbegin()) {
				info += ",";
			}
		}
		info += "}";
	}
	info = info + "\n";
	return info;
}

shared_ptr<Witness> ChromaticNumber_AtMost_Witness::relabel(
	const map<unsigned, unsigned> &relabelingMap) const {
	ChromaticNumber_AtMost_WitnessPointer relabeledWitness(
		new ChromaticNumber_AtMost_Witness);
	for (auto cell : this->partialColoring) {
		set<unsigned> relabeledCell;
		for (auto item : cell) {
			auto it = relabelingMap.find(item);
			if (it != relabelingMap.end()) {
				relabeledCell.insert(it->second);
			} else {
				cout << "Error: ChromaticNumber_AtMost_Witness::relabel "
						"function error"
					 << endl;
				exit(20);
			}
		}
		relabeledWitness->partialColoring.insert(relabeledCell);
	}
	return relabeledWitness;
}

void ChromaticNumber_AtMost_Witness::hash(Hasher &h) const {
	for (auto s : partialColoring) {
		for (auto c : s) h << c;
		h << -1u;
	}
}

ChromaticNumber_AtMost_Witness::ChromaticNumber_AtMost_Witness() {}

ChromaticNumber_AtMost_Witness::~ChromaticNumber_AtMost_Witness() {}

ChromaticNumber_AtMost_DynamicCore::ChromaticNumber_AtMost_DynamicCore() {
	// Initializing attributes
	addAttribute("CoreName", "ChromaticNumber");
	addAttribute("CoreType", "Bool");
	addAttribute("ParameterType", "UnsignedInt");
	addAttribute("PrimaryOperator", "AtMost");
}

ChromaticNumber_AtMost_DynamicCore::ChromaticNumber_AtMost_DynamicCore(
	unsigned k) {
	this->k = k;
	createInitialWitnessSet();
	// Initializing attributes
	addAttribute("CoreName", "ChromaticNumber");
	addAttribute("CoreType", "Bool");
	addAttribute("ParameterType", "UnsignedInt");
	addAttribute("PrimaryOperator", "AtMost");
}

ChromaticNumber_AtMost_DynamicCore::ChromaticNumber_AtMost_DynamicCore(
	const parameterType &parameters) {
	if (parameters.size() != 1) {
		cerr << "ChromaticNumber Core received an invalid parameter list"
			 << endl;
		exit(20);
	}
	for (const auto &prop : parameters) {
		std::visit(overloaded{[&](char *) {
								  cerr << "ChromaticNumber Core received an "
										  "invalid parameter list"
									   << endl;
								  exit(20);
							  },
							  [&](auto p) { this->k = p; }},
				   prop);
	}

	this->k = k;
	createInitialWitnessSet();
	// Initializing attributes
	addAttribute("CoreName", "ChromaticNumber");
	addAttribute("CoreType", "Bool");
	addAttribute("ParameterType", "UnsignedInt");
	addAttribute("PrimaryOperator", "AtMost");
}

void ChromaticNumber_AtMost_DynamicCore::createInitialWitnessSet_implementation(
	WitnessSet &witnessSet) {
	shared_ptr<ChromaticNumber_AtMost_Witness> witness = createWitness();
	// witness->numberUsedColors = 0;
	witnessSet.insert(witness);
}

void ChromaticNumber_AtMost_DynamicCore::intro_v_implementation(
	unsigned i, const Bag &, ChromaticNumber_AtMost_WitnessPointerConst w,
	ChromaticNumber_AtMost_WitnessSetPointer witnessSet) {
	// insert to one of the cells of  w->partialColoring
	for (auto cell : w->partialColoring) {
		ChromaticNumber_AtMost_WitnessPointer witness = createWitness();
		witness->set_equal(*w);
		witness->partialColoring.erase(cell);
		cell.insert(i);
		witness->partialColoring.insert(cell);
		witnessSet->insert(witness);
	}
	if (w->partialColoring.size() < this->k) {
		// i can be added as an separated cell to the partition
		ChromaticNumber_AtMost_WitnessPointer witness = createWitness();
		witness->set_equal(*w);
		// witness->numberUsedColors = witness->numberUsedColors +1;
		set<unsigned> iCell;
		iCell.insert(i);
		witness->partialColoring.insert(iCell);
		witnessSet->insert(witness);
	}
}

void ChromaticNumber_AtMost_DynamicCore::intro_e_implementation(
	unsigned int i, unsigned int j, const Bag &,
	ChromaticNumber_AtMost_WitnessPointerConst w,
	ChromaticNumber_AtMost_WitnessSetPointer witnessSet) {
	// If i and j are in different cell then w will be returned, otherwise, it
	// is invalid
	set<unsigned> iCell;
	set<unsigned> jCell;
	for (auto cell : w->partialColoring) {
		if (cell.count(i)) {
			iCell = cell;
		}
		if (cell.count(j)) {
			jCell = cell;
		}
	}
	if (jCell != iCell) {
		ChromaticNumber_AtMost_WitnessPointer witness = createWitness();
		witness->set_equal(*w);
		witnessSet->insert(witness);
	}
}

void ChromaticNumber_AtMost_DynamicCore::forget_v_implementation(
	unsigned i, const Bag &, ChromaticNumber_AtMost_WitnessPointerConst w,
	ChromaticNumber_AtMost_WitnessSetPointer witnessSet) {
	for (auto cell : w->partialColoring) {
		if (cell.count(i)) {
			ChromaticNumber_AtMost_WitnessPointer witness = createWitness();
			witness->set_equal(*w);
			witness->partialColoring.erase(cell);
			cell.erase(i);
			if (!cell.empty()) {
				witness->partialColoring.insert(cell);
			}
			witnessSet->insert(witness);
			break;
		}
	}
}

void ChromaticNumber_AtMost_DynamicCore::join_implementation(
	const Bag &, ChromaticNumber_AtMost_WitnessPointerConst w1,
	ChromaticNumber_AtMost_WitnessPointerConst w2,
	ChromaticNumber_AtMost_WitnessSetPointer witnessSet) {
	if (w1->partialColoring == w2->partialColoring) {
		ChromaticNumber_AtMost_WitnessPointer witness = createWitness();
		witness->set_equal(*w1);
		witnessSet->insert(witness);
	}
}

bool ChromaticNumber_AtMost_DynamicCore::is_final_witness_implementation(
	ChromaticNumber_AtMost_WitnessPointerConst) {
	return true;
}

ChromaticNumber_AtMost_WitnessSetPointer
ChromaticNumber_AtMost_DynamicCore::clean_implementation(
	ChromaticNumber_AtMost_WitnessSetPointer witnessSet) {
	return witnessSet;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ChromaticNumber_AtMost_Witness::is_equal(const Witness &rhs) const {
	if (ChromaticNumber_AtMost_Witness const *e =
			dynamic_cast<ChromaticNumber_AtMost_Witness const *>(&rhs)) {
		ChromaticNumber_AtMost_WitnessPointerConst w = e->shared_from_this();
		return is_equal_implementation(w);
	} else {
		cerr << "ERROR: in ChromaticNumber_AtMost_Witness::is_equal cast error"
			 << endl;
		exit(20);
	}
}

bool ChromaticNumber_AtMost_Witness::is_less(const Witness &rhs) const {
	if (ChromaticNumber_AtMost_Witness const *e =
			dynamic_cast<ChromaticNumber_AtMost_Witness const *>(&rhs)) {
		ChromaticNumber_AtMost_WitnessPointerConst w = e->shared_from_this();
		return is_less_implementation(w);
	} else {
		cerr << "ERROR: in ChromaticNumber_AtMost_Witness::is_less cast error"
			 << endl;
		exit(20);
	}
}

Witness &ChromaticNumber_AtMost_Witness::set_equal(const Witness &witness) {
	if (const ChromaticNumber_AtMost_Witness *e =
			dynamic_cast<const ChromaticNumber_AtMost_Witness *>(&witness)) {
		ChromaticNumber_AtMost_WitnessPointerConst w = e->shared_from_this();
		return set_equal_implementation(w);
	} else {
		cerr << "ERROR: in ChromaticNumber_AtMost_Witness::operator= cast error"
			 << endl;
		exit(20);
	}
}

shared_ptr<WitnessSet>
ChromaticNumber_AtMost_WitnessSet::createEmptyWitnessSet() {
	ChromaticNumber_AtMost_WitnessSetPointer witnessSet(
		new ChromaticNumber_AtMost_WitnessSet);
	return witnessSet;
}

ChromaticNumber_AtMost_WitnessPointer
ChromaticNumber_AtMost_DynamicCore::createWitness() {
	ChromaticNumber_AtMost_WitnessPointer w(new ChromaticNumber_AtMost_Witness);
	return w;
}

void ChromaticNumber_AtMost_DynamicCore::copyWitness(
	ChromaticNumber_AtMost_WitnessPointer w_input,
	ChromaticNumber_AtMost_WitnessPointer w_output) {
	w_output->set_equal_implementation(w_input);
}

void ChromaticNumber_AtMost_DynamicCore::createInitialWitnessSet() {
	ChromaticNumber_AtMost_WitnessSetPointer witnessSet(
		new ChromaticNumber_AtMost_WitnessSet);
	createInitialWitnessSet_implementation(*witnessSet);
	this->setInitialWitnessSet(witnessSet);
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::intro_v(
	unsigned i, const Bag &b, const Witness &witness) {
	if (const ChromaticNumber_AtMost_Witness *e =
			dynamic_cast<const ChromaticNumber_AtMost_Witness *>(&witness)) {
		ChromaticNumber_AtMost_WitnessPointerConst w = e->shared_from_this();
		ChromaticNumber_AtMost_WitnessSetPointer witnessSet(
			new ChromaticNumber_AtMost_WitnessSet);
		intro_v_implementation(i, b, w, witnessSet);
		return clean(witnessSet);
	} else {
		cerr << "ERROR: in ChromaticNumber_AtMost_DynamicCore::intro_v cast "
				"error"
			 << endl;
		exit(20);
	}
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::intro_e(
	unsigned i, unsigned j, const Bag &b, const Witness &witness) {
	if (const ChromaticNumber_AtMost_Witness *e =
			dynamic_cast<const ChromaticNumber_AtMost_Witness *>(&witness)) {
		ChromaticNumber_AtMost_WitnessPointerConst w = e->shared_from_this();
		ChromaticNumber_AtMost_WitnessSetPointer witnessSet(
			new ChromaticNumber_AtMost_WitnessSet);
		intro_e_implementation(i, j, b, w, witnessSet);
		return clean(witnessSet);
	} else {
		cerr << "ERROR: in ChromaticNumber_AtMost_DynamicCore::intro_e cast "
				"error"
			 << endl;
		exit(20);
	}
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::forget_v(
	unsigned i, const Bag &b, const Witness &witness) {
	if (const ChromaticNumber_AtMost_Witness *e =
			dynamic_cast<const ChromaticNumber_AtMost_Witness *>(&witness)) {
		ChromaticNumber_AtMost_WitnessPointerConst w = e->shared_from_this();
		ChromaticNumber_AtMost_WitnessSetPointer witnessSet(
			new ChromaticNumber_AtMost_WitnessSet);
		forget_v_implementation(i, b, w, witnessSet);
		return clean(witnessSet);
	} else {
		cerr << "ERROR: in ChromaticNumber_AtMost_DynamicCore::forget_v cast "
				"error"
			 << endl;
		exit(20);
	}
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::join(
	const Bag &b, const Witness &witness1, const Witness &witness2) {
	if (const ChromaticNumber_AtMost_Witness *e1 =
			dynamic_cast<const ChromaticNumber_AtMost_Witness *>(&witness1)) {
		if (const ChromaticNumber_AtMost_Witness *e2 =
				dynamic_cast<const ChromaticNumber_AtMost_Witness *>(
					&witness2)) {
			// Note: Do not merge the two "if" instructions above into a single
			// "if". Potential for errors
			ChromaticNumber_AtMost_WitnessPointerConst w1 =
				e1->shared_from_this();
			ChromaticNumber_AtMost_WitnessPointerConst w2 =
				e2->shared_from_this();
			ChromaticNumber_AtMost_WitnessSetPointer witnessSet(
				new ChromaticNumber_AtMost_WitnessSet);
			join_implementation(b, w1, w2, witnessSet);
			return clean(witnessSet);
		} else {
			cerr << "ERROR: in ChromaticNumber_AtMost_DynamicCore::join cast "
					"error"
				 << endl;
			exit(20);
		}
	} else {
		cerr << "ERROR: in ChromaticNumber_AtMost_DynamicCore::join cast error"
			 << endl;
		exit(20);
	}
}

WitnessSetPointer ChromaticNumber_AtMost_DynamicCore::clean(
	WitnessSetPointer witnessSet) {
	if (ChromaticNumber_AtMost_WitnessSetPointer e =
			dynamic_pointer_cast<ChromaticNumber_AtMost_WitnessSet>(
				witnessSet)) {
		return clean_implementation(e);
	} else {
		cerr << "ERROR: in ChromaticNumber_AtMost_DynamicCore::clean cast error"
			 << endl;
		exit(20);
	}
}

bool ChromaticNumber_AtMost_DynamicCore::is_final_witness(
	const Bag &, const Witness &witness) {
	if (const ChromaticNumber_AtMost_Witness *e =
			dynamic_cast<const ChromaticNumber_AtMost_Witness *>(&witness)) {
		ChromaticNumber_AtMost_WitnessPointerConst w = e->shared_from_this();
		return is_final_witness_implementation(w);
	} else {
		cerr
			<< "ERROR: in ChromaticNumber_AtMost_DynamicCore::is_final_witness "
			   "cast error"
			<< endl;
		exit(20);
	}
}

//////////////////// The functions below are used by the plugin handler
////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
DynamicCore *create(const parameterType &parameters) {
	return new ChromaticNumber_AtMost_DynamicCore(parameters);
}
std::map<std::string, std::string> *metadata() {
	return new std::map<std::string, std::string>(
		ChromaticNumber_AtMost_DynamicCore().getAttributes());
}
}
