// Copyright 2020 Mateus de Oliveira Oliveira, Farhad Vadiee and CONTRIBUTORS.

#ifndef PROPERTYASSIGNMENT_H
#define PROPERTYASSIGNMENT_H

#include <iostream>
#include <vector>

class PropertyAssignment {
  private:
	std::string name;
	std::string operatorSign;
	int value;
	std::vector<int> parameters; // Designated for multi parameters cores.
	std::string type;	 // NoParameter, UnsignedInt, InputFile, and MultiParameter
	std::string filePath; // if the type is InputFile, then filePath is the location of the file
	std::string label;	// label of the core
	//Todo: change label to coreVariable
    // coreVariable is a variable name associated with the core.
  public:
	void print();

	const std::string &get_name() const;

	void set_name(const std::string &name);

	const std::string &get_operatorSign() const;

	void set_operatorSign(const std::string &op);

	int get_value() const;

	void set_value(int v);

	const std::string &get_type() const;

	void set_type(const std::string &type);

	const std::string &get_filePath() const;

	void set_filePath(const std::string &filePath);

	const std::string &get_label() const;

	void set_label(const std::string &label);

	std::vector<int> &get_parameters();

	void set_parameters(const std::vector<int> &parameters);
};

#endif
