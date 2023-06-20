#include "WitnessTreePACE.h"
void WitnessTreePACENode::print() {
	std::cout << node_type << std::endl;
	for (auto it : witnesses) {
		it->print();
		std::cout << std::endl;
	}
	for (auto it : children) {
		it->print();
	}
}

std::shared_ptr<WitnessTreePACENode> WitnessTreePACE::generateNode(
	std::shared_ptr<StateTreeNode> snode,
	std::shared_ptr<WitnessTreePACENode> parent, unsigned joinno,
	DynamicKernel &kernel) {
	State::ptr q = snode->get_S();
	std::vector<std::shared_ptr<Witness>> witnesses;
	witnesses.resize(parent->witnesses.size());

	for (size_t i = 0; i < parent->witnesses.size(); ++i) {
		if (parent->witnesses[i]) {
			if (strstr(parent->node_type.c_str(), "IntroVertex_")) {
				std::set<unsigned> setDiff;
				std::set<unsigned> p =
					snode->get_parent()->get_S()->get_bag().get_elements();
				std::set<unsigned> n = q->get_bag().get_elements();
				set_difference(p.begin(), p.end(), n.begin(), n.end(),
							   inserter(setDiff, setDiff.begin()));
				if (setDiff.size() != 1) {
					std::cout << "ERROR: in WitnessTreePACEN::generateNode "
								 "intro vertex is not correct";
					exit(20);
				}
				bool flag = false;
				for (auto itr = q->getWitnessSet(i)->begin();
					 itr != q->getWitnessSet(i)->end(); ++itr) {
					if (flag) {
						break;
					} else {
						Bag b = q->get_bag();

            auto singleton = q->getWitnessSet(i)->createEmptyWitnessSet();
            singleton->insert(*itr);

						std::shared_ptr<WitnessSet> w =
							kernel.pointerToCoreNumber(i)->intro_v(
								*setDiff.begin(), b, singleton);
						for (auto t = w->begin(); t != w->end(); ++t) {
							if (**t == *parent->witnesses[i]) {
								witnesses[i] = *itr;
								flag = true;
								break;
							}
						}
					}
				}
			} else if (strstr(parent->node_type.c_str(), "ForgetVertex_")) {
				std::set<unsigned> setDiff;
				std::set<unsigned> p =
					snode->get_parent()->get_S()->get_bag().get_elements();
				std::set<unsigned> n = q->get_bag().get_elements();
				set_difference(n.begin(), n.end(), p.begin(), p.end(),
							   inserter(setDiff, setDiff.begin()));
				if (setDiff.size() != 1) {
					std::cout << "ERROR: in WitnessTreePACEN::generateNode "
								 "forget vertex is not correct";
					exit(20);
				}
				bool flag = false;
				for (auto itr = q->getWitnessSet(i)->begin();
					 itr != q->getWitnessSet(i)->end(); ++itr) {
					if (flag) {
						break;
					} else {
						Bag b = q->get_bag();

            auto singleton = q->getWitnessSet(i)->createEmptyWitnessSet();
            singleton->insert(*itr);

						std::shared_ptr<WitnessSet> w =
							kernel.pointerToCoreNumber(i)->forget_v(
								*setDiff.begin(), b, singleton);
						for (auto t = w->begin(); t != w->end(); ++t) {
							if (**t == *parent->witnesses[i]) {
								witnesses[i] = *itr;
								flag = true;
								break;
							}
						}
					}
				}

			} else if (strstr(parent->node_type.c_str(), "IntroEdge_")) {
				std::pair<unsigned, unsigned> e =
					snode->get_parent()->get_S()->get_bag().get_edge();
				if (e.first == 0 or e.second == 0) {
					std::cout << "ERROR: in WitnessTreePACEN::generateNode "
								 "intro edge is not correct";
					exit(20);
				}
				bool flag = false;
				for (auto itr = q->getWitnessSet(i)->begin();
					 itr != q->getWitnessSet(i)->end(); ++itr) {
					if (flag) {
						break;
					} else {
						Bag b = q->get_bag();

            auto singleton = q->getWitnessSet(i)->createEmptyWitnessSet();
            singleton->insert(*itr);

						std::shared_ptr<WitnessSet> w =
							kernel.pointerToCoreNumber(i)->intro_e(
								e.first, e.second, b, singleton);
						for (auto t = w->begin(); t != w->end(); ++t) {
							if (**t == *parent->witnesses[i]) {
								witnesses[i] = *itr;
								flag = true;
								break;
							}
						}
					}
				}
			} else if (strstr(parent->node_type.c_str(), "Join")) {
				State::ptr nq =
					snode->get_parent()->get_children()[1 - joinno]->get_S();
				bool flag = false;
				for (auto itr = q->getWitnessSet(i)->begin();
					 itr != q->getWitnessSet(i)->end(); ++itr) {
					if (flag) {
						break;
					} else {
						Bag b = q->get_bag();
						std::shared_ptr<WitnessSet> nqw = nq->getWitnessSet(i);
						for (auto it = nqw->begin(); it != nqw->end(); ++it) {
							if (flag) {
								break;
							} else {

                auto singleton1 = q->getWitnessSet(i)->createEmptyWitnessSet();
                singleton1->insert(*itr);

                auto singleton2 = q->getWitnessSet(i)->createEmptyWitnessSet();
                singleton2->insert(*it);

								std::shared_ptr<WitnessSet> w =
									kernel.pointerToCoreNumber(i)->join(
										b, singleton1, singleton2);
								for (auto t = w->begin(); t != w->end(); ++t) {
									if (**t == *parent->witnesses[i]) {
										witnesses[i] = *itr;
										flag = true;
										break;
									}
								}
							}
						}
					}
				}
			} else {
				std::cout << "ERROR: in WitnessTreePACEN::generateNode node's "
							 "type is not correct"
						  << std::endl;
				exit(20);
			}
		}
	}
	std::shared_ptr<WitnessTreePACENode> node(new WitnessTreePACENode);
	node->node_type = snode->get_nodeType();
	node->witnesses = witnesses;
	node->parent = parent;
	for (size_t c = 0; c < snode->get_children().size(); ++c) {
		node->children.push_back(
			generateNode(snode->get_children()[c], node, c, kernel));
	}
	return node;
}

bool WitnessTreePACE::stateTreeToWitnessTreePACE(StateTree &stateTree,
												 DynamicKernel &kernel) {
	std::shared_ptr<WitnessTreePACENode> node(new WitnessTreePACENode);
	State::ptr q = stateTree.root->get_S();
	// std::vector<std::shared_ptr<WitnessSet> > witnessSet =
	// q->get_witnessSetstd::vector();
	std::vector<std::shared_ptr<Witness>> witnesses;
	witnesses.resize(q->numberOfComponents());
	for (size_t i = 0; i < q->numberOfComponents(); ++i) {
		for (auto it = q->getWitnessSet(i)->begin();
			 it != q->getWitnessSet(i)->end(); ++it) {

      auto singleton = q->getWitnessSet(i)->createEmptyWitnessSet();
      singleton->insert(*it);

			if (kernel.pointerToCoreNumber(i)->is_final_witness_set(q->get_bag(), singleton)) {
				witnesses[i] = *it;
				break;
			}
		}
		if (witnesses[i] == NULL) {
			std::cout << "The root node doesn't have a final witness"
					  << std::endl;
			return false;
		}
	}
	node->node_type = stateTree.root->get_nodeType();
	node->witnesses = witnesses;
	for (size_t c = 0; c < stateTree.root->get_children().size(); ++c) {
		node->children.push_back(
			generateNode(stateTree.root->get_children()[c], node, c, kernel));
	}
	this->root = node;
	return true;
}
void WitnessTreePACE::print() { root->print(); }
