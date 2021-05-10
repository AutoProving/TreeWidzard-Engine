#ifndef CONJECTURE_H
#define CONJECTURE_H
#include "../Kernel/DynamicKernel.h"
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
enum TokenType { OPERATOR , EXP_VARIABLE , NUMBER, FUNC, CORE_VARIABLE };
typedef double(*Function)(double);

class ConjectureNode{
    private:
        TokenType type; // Type of the Node
        string val; // Name of the Node
        double x=0; // Value of the Node if it is NUMBER
        vector<ConjectureNode*> children;
        ConjectureNode* parent;
    public:

    ConjectureNode(TokenType type, const string &val);

    ConjectureNode(TokenType type, double x);

    ConjectureNode(TokenType type, const string &val, double x);

    TokenType getType() const;

    void setType(TokenType type);

    const string &getVal() const;

    void setVal(const string &val);

    double getX() const;

    void setX(double x);

    const vector<ConjectureNode *> &getChildren() const;

    void setChildren(const vector<ConjectureNode *> &children);

    ConjectureNode *getParent() const;

    void setParent(ConjectureNode *parent);

    void printWithNumbers(unsigned &label);

    void print();

};

class Conjecture {
    private:
        map<string,string> variablesToCoreName; // Map from variables To CoreName
        map<string,Function> functions;
        ConjectureNode* root;
        DynamicKernel* kernel;
    public:

    ConjectureNode *getRoot() const;

    void setRoot(ConjectureNode *root);

    // evaluate the conjectureNode for a given state
        int evaluateConjectureNodeOnState(State &q, ConjectureNode* node);
        // evaluate the conjecture for a given state
        int evaluateConjectureOnState(State &q);


        int evaluatePremiseOnState(State &q);

        bool validateConjectureGrammar();

        DynamicKernel *getKernel() const;

        void setKernel(DynamicKernel *kernel);

        bool checkConjectureStructure(ConjectureNode* node); // check conjecture tree is valid

        void print();

    const map<string, string> &getVariablesToCoreName() const;

    void setVariablesToCoreName(const map<string, string> &variablesToCoreName);
};

#endif