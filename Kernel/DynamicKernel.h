// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef DYNAMIC_KERNEL_H
#define DYNAMIC_KERNEL_H
#include "DynamicCore.h"
#include "State.h"
#include <unordered_map>
#include <list>
#include <stack>
#include "PropertyAssignment.h"
#include "Width.h"

using namespace std;

class DynamicKernel: public std::enable_shared_from_this<DynamicKernel> {
    private:
        Width width;
        vector<PropertyAssignment> properties;
        vector<DynamicCore*> cores;

    public:
        void addCore(DynamicCore &core);
        Width &get_width();
        void set_width(Width &width);
        //void set_properties(vector<PropertyAssignment> &vectorProperties);
		void addProperty(PropertyAssignment&);
        const vector<PropertyAssignment> &get_properties() const;
        shared_ptr<State> initialState();
        shared_ptr<State> intro_v(shared_ptr<State> q, unsigned i);
        shared_ptr<State> intro_e(shared_ptr<State> q, const unsigned i, const unsigned j);
        shared_ptr<State> forget_v(shared_ptr<State> q, unsigned i);
        shared_ptr<State> join(shared_ptr<State> q1, shared_ptr<State> q2);
        DynamicCore* pointerToCoreNumber(unsigned i);
        
};

#endif
