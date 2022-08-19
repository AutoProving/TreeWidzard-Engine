// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef PROPERTYASSIGNMENT_H
#define PROPERTYASSIGNMENT_H

#include <iostream>
#include <vector>
using namespace std;

class PropertyAssignment {
  private:
	string name;
	string op;
	int parameter;
	std::vector<int> parameters; // Designated for multi parameters cores.
	string parameterType;	 // NoParameter, UnSignedInt, InputFile, and MultiParameter
	string type; // Bool, Min, Max
  public:
    const string &getName() const;

    void setName(const string &name);

    const string &getOp() const;

    void setOp(const string &op);

    const vector<int> &getParameters() const;

    void setParameters(const vector<int> &parameters);

    const string &getParameterType() const;

    void setParameterType(const string &parameterType);

    const string &getType() const;

    void setType(const string &type);

    int getParameter() const;

    void setParameter(int parameter);

};

#endif

