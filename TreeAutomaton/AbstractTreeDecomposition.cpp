




int AbstractTreeDecompositionNodeContent::symbolType(string s) {
    if(s.find("Leaf") != string::npos){
        return 0;
    }else if(s.find("IntroVertex") != string::npos){
        return 1;
    } else if(s.find("ForgetVertex") != string::npos) {
        return 2;
    }else     if(s.find("IntroEdge") != string::npos) {
        return 3;
    }else if(s.find("Join") != string::npos) {
        return 4;
    }else{
        cout<<"Error: AbstractTreeDecompositionNodeContent::symbolType, string is not in formal format."<<endl;
        exit(20);
    }
}

vector<int> AbstractTreeDecompositionNodeContent::symbolNumbers(string s) {
    // IntroVertex_i
}

void AbstractTreeDecompositionNodeContent::setSymbol(string s) {
    symbol = s;
}


