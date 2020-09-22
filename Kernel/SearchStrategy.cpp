// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#include "SearchStrategy.h"

pair<bool, ConcreteTreeDecomposition> SearchStrategy::search() {
	cout << "ERROR: Based class search function called!" << endl;
	exit(20);
}

SearchStrategy::SearchStrategy(DynamicKernel *dynamicKernel,
							   Conjecture *conjecture, Flags *flags) {
	this->kernel = dynamicKernel;
	this->conjecture = conjecture;
	this->flags = flags;
}
