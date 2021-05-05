enum TokenType { NONE, OP, SEP, NAME, NUMBER };
struct Token {
    double x;
    asl::String val;
    TokenType type;
    Token() {}
    Token(char c) : type(OP), val(c) {}
    Token(TokenType t, const asl::String& v) : type(t), val(v) {}
    Token(double v) : type(NUMBER), x(v) {}
    Token(asl::String& v) : type(NAME), val(v) {}
};

class ConjectureNode{
    private:
        TokenType type;
        string val;
        double x;
        vector<shared_ptr<ConjectureNode> > children;
        shared_ptr<ConjectureNode> parent;
    public:
        
}


class Conjecture {

    private:
        typedef double(*Function)(double);
        map<string,shared_ptr<DynamicCore>> variables;
        map<string,Function> functions;
        shared_ptr<ConjectureNode> root;
    public:


}
