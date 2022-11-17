#ifndef PROPERTYASSIGNMENT_H
#define PROPERTYASSIGNMENT_H

#include <iostream>
#include <string>
#include <vector>

class PropertyAssignment {
  private:
	std::string name;
	std::string op;
	int parameter;
	std::vector<int> parameters; // Designated for multi parameters cores.
	std::string parameterType;	 // NoParameter, UnSignedInt, InputFile, and MultiParameter
	std::string type; // Bool, Min, Max
    std::vector<char*> parametersVec;
  public:
    const std::string &getName() const;
    void setName(const std::string &name);
    const std::string &getOp() const;
    void setOp(const std::string &op);
    const std::vector<int> &getParameters() const;
    void setParameters(const std::vector<int> &parameters);
    const std::string &getParameterType() const;
    void setParameterType(const std::string &parameterType);
    const std::string &getType() const;
    void setType(const std::string &type);
    int getParameter() const;
    void setParameter(int parameter);
    const std::vector<char *> &getParametersVec() const;
    void setParametersVec(const std::vector<char *> &parametersVec);
};



#endif
