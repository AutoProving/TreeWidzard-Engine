// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef PROPERTYASSIGNMENT_H
#define PROPERTYASSIGNMENT_H

#include <iostream>
#include <vector>
using namespace std;

class PropertyAssignment {
  private:
	string name;
	string operatorSign;
	int value;
	std::vector<int> parameters; // Designated for multi parameters cores.
	string type;	 // NoParameter, UnsignedInt, InputFile, and MultiParameter
	string filePath; // if the type is InputFile, then filePath is the location of the file
	string label;	// label of the core
	//Todo: change label to coreVariable
    // coreVariable is a variable name associated with the core.
  public:
	void print();

	const string &get_name() const;

	void set_name(const string &name);

	const string &get_operatorSign() const;

	void set_operatorSign(const string &op);

	int get_value() const;

	void set_value(int v);

	const string &get_type() const;

	void set_type(const string &type);

	const string &get_filePath() const;

	void set_filePath(const string &filePath);

	const string &get_label() const;

	void set_label(const string &label);

	vector<int> &get_parameters();

    void set_parameters(const vector<int> &parameters);
};

#endif

