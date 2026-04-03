#ifndef CONJECTURE_H
#define CONJECTURE_H
#include "../Kernel/DynamicKernel.h"
#include <math.h>       /* pow */
//struct Token {
//    double x;
//    asl::String val;
//    TokenType type;
//    Token() {}
//    Token(char c) : type(OP), val(c) {}
//    Token(TokenType t, const asl::String& v) : type(t), val(v) {}
//    Token(double v) : type(NUMBER), x(v) {}
//    Token(asl::String& v) : type(NAME), val(v) {}
//};
enum TokenType {OPERATOR , EXP_VARIABLE , NUMBER, FUNCTION_BINARY, FUNCTION_UNARY, CORE_VARIABLE, INV};
typedef double(*Function_Unary)(double);
typedef double(*Function_Binary)(double,double);
class ConjectureNode {
private:
    TokenType type; // Type of the Node
    std::string val; // Name of the Node
    double x = 0; // Value of the Node if it is NUMBER
    std::vector<ConjectureNode *> children;
    ConjectureNode *parent = nullptr;
public:
    ConjectureNode(TokenType type, const std::string &val);
    ConjectureNode(TokenType type, double x);
    ConjectureNode(TokenType type, const std::string &val, double x);
    ~ConjectureNode();
    ConjectureNode(const ConjectureNode &) = delete;
    ConjectureNode &operator=(const ConjectureNode &) = delete;
    ConjectureNode(ConjectureNode &&) = delete;
    ConjectureNode &operator=(ConjectureNode &&) = delete;
    TokenType getType() const;
    void setType(TokenType type);
    const std::string &getVal() const;
    void setVal(const std::string &val);
    double getX() const;
    void setX(double x);
    const std::vector<ConjectureNode *> &getChildren() const;
    void setChildren(const std::vector<ConjectureNode *> &children);
    ConjectureNode *getParent() const;
    void setParent(ConjectureNode *parent);
    void printWithNumbers(unsigned &label);
    void printInfix();
    void print();
    ConjectureNode *cloneSubtree() const;
    static void deleteSubtree(ConjectureNode *node);
};
class Conjecture {
private:
    std::map<std::string, std::string> variablesToCoreName; // Map from variables To CoreName
    std::map<std::string, Function_Binary> functions_binary;
    std::map<std::string, Function_Unary> functions_unary;
    ConjectureNode *root = nullptr;
    bool ownsRoot = false;
    DynamicKernel *kernel = nullptr;
    std::vector<std::unique_ptr<PropertyAssignment>> ownedPropertyAssignments;
    std::map<std::string, PropertyAssignment *> varToProperty; // map from var to Property
public:
    Conjecture();
    ~Conjecture();
    Conjecture(const Conjecture &other);
    Conjecture &operator=(const Conjecture &other);
    Conjecture(Conjecture &&other) noexcept;
    Conjecture &operator=(Conjecture &&other) noexcept;
    ConjectureNode *getRoot() const;
    void setRoot(ConjectureNode *root);
    void setOwnedRoot(ConjectureNode *root);
    // evaluate the conjectureNode for a given state
    double evaluateConjectureNodeOnState(const State &q, const ConjectureNode *node);
    // evaluate the conjecture for a given state
    double evaluateConjectureOnState(const State &q);
    int evaluatePremiseOnState(const State &q);
    bool validateConjectureGrammar();
    DynamicKernel *getKernel() const;
    void setKernel(DynamicKernel *kernel);
    bool checkConjectureStructure(ConjectureNode *node); // check conjecture tree is valid
    void print();
    void printValues(const State &q, const ConjectureNode *node);

    const std::map<std::string, std::string> &getVariablesToCoreName() const;
    void setVariablesToCoreName(const std::map<std::string, std::string> &variablesToCoreName);

    void setvarToProperty(const std::map<std::string, PropertyAssignment *> &varToProperty);
    PropertyAssignment* getPropertyAssignment(const std::string& var) const {
        auto it = varToProperty.find(var);
        return it != varToProperty.end() ? it->second : nullptr;
    }
};
#endif
