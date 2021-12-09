#include "TreeDecompositionPACE.h"

RawAbstractTreeDecomposition::RawAbstractTreeDecomposition(){
  // constructor
}
void RawAbstractTreeDecomposition::print(unsigned &k,unsigned parentno){
    k++;
    cout<<k<<" "<<type<<"(" <<parentno <<") " ;
    bag.print();
    for(auto it=color_to_vertex_map.begin(); it!=color_to_vertex_map.end();it++){
            cout<<it->first<<"->"<<it->second<<" ";
    }
    cout<<endl;
    unsigned num = k;
    for(size_t i=0; i < children.size(); i++){
        children[i]->print(k,num);
    }
}

void RawAbstractTreeDecomposition::printNode(){
    bag.print();
    for(auto it=color_to_vertex_map.begin(); it!=color_to_vertex_map.end();it++){
        cout<<it->first<<"->"<<it->second<<" ";
    }
}

TreeDecompositionPACE::TreeDecompositionPACE(){
    num_vertices=0;
    num_graph_vertices=0;
    num_edges=0;
    width=0;
}

void TreeDecompositionPACE::setNum_vertices(unsigned n){
    bags.resize(n);
    num_vertices = n;
}

void TreeDecompositionPACE::setNum_graph_vertices(unsigned n){
    num_graph_vertices=n;
}

void TreeDecompositionPACE::setWidth(unsigned w){
    width=w;
}

bool TreeDecompositionPACE::addVertexToBag(unsigned vertex, unsigned bagnumber){

 if(bagnumber>0){

    bags[bagnumber-1].insert(vertex);
 }else{
    return false;
 }
    return true;
}


bool TreeDecompositionPACE::setABag(vertex_t s, unsigned bagnumber){
    if(bagnumber>0 and bagnumber <= bags.size()){
        bags[bagnumber-1] = s;
        return true;
    }else{
        return false;
    }
}

bool TreeDecompositionPACE::addEdge(unsigned e1, unsigned e2){
    if(e1<=bags.size() and e2 <= bags.size()){
        if(e1>e2){
            edges.insert(make_pair(e2,e1));
            return true;
        }else if(e1<e2){
            edges.insert(make_pair(e1,e2));
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

void TreeDecompositionPACE::printBags()
{
    for(size_t i =0 ; i< bags.size(); i++){
        cout<< "b " << i+1 <<  " ";
        for(auto it = bags[i].begin(); it!=bags[i].end(); it++){
            cout<<*it;
            if(next(it)!= bags[i].end()){
                cout<<" ";
            }
        }
        cout<<""<<endl;
    }
}

void TreeDecompositionPACE::printEdges(){
    for(auto it=edges.begin(); it!=edges.end();it++){
        cout<<it->first<<"  " << it->second<<endl;
    }
}

void TreeDecompositionPACE::print(){
    cout<<"s td " << num_vertices<< "  "<<width+1<<"  "<<num_graph_vertices<<endl;
    printBags();
    printEdges();
}

void TreeDecompositionPACE::printTree(){
    unsigned k=0;
    root->print(k,0);
}

shared_ptr<RawAbstractTreeDecomposition> TreeDecompositionPACE::constructInnerNodes(set<unsigned> &visited_bags, unsigned neighbor){
    shared_ptr<RawAbstractTreeDecomposition> node (new RawAbstractTreeDecomposition);
    node->bag.set_elements(bags[neighbor-1]);
    visited_bags.insert(neighbor);
    for(auto it=edges.begin(); it!=edges.end(); it++){
        if(it->first==neighbor and visited_bags.count(it->second)==0 ){
            shared_ptr<RawAbstractTreeDecomposition> new_node;
            new_node = constructInnerNodes(visited_bags,it->second);
            new_node->parent = node;
            node->children.push_back(new_node);
        }else if(it->second==neighbor and visited_bags.count(it->first)==0){
            shared_ptr<RawAbstractTreeDecomposition> new_node;
            new_node = constructInnerNodes(visited_bags,it->first);
            new_node->parent = node;
            node->children.push_back(new_node);
        }
    }
    return node;
}

bool TreeDecompositionPACE::constructRaw(){
    shared_ptr<RawAbstractTreeDecomposition> root_node(new RawAbstractTreeDecomposition);
    root = root_node;
    if(bags.size()>0){
        root->bag.set_elements(bags[0]);
        set<unsigned> visited_bags;
        visited_bags.insert(1);
        //create nodes for neighbors
        for(auto it=edges.begin(); it!=edges.end(); it++){
            if(it->first==1){
                shared_ptr<RawAbstractTreeDecomposition> node;
                node = constructInnerNodes(visited_bags,it->second);
                node->parent = root;
                root->children.push_back(node);
            }else if(it->second==1){
                shared_ptr<RawAbstractTreeDecomposition> node;
                node = constructInnerNodes(visited_bags,it->first);
                node->parent = root;
                root->children.push_back(node);
            }
        }
        return true;
    }else{
        cout<<"ERROR in TreeDecompositionPACE::constructRaw bags size is zero"<<endl;
        return false;
    }

}

bool TreeDecompositionPACE::convertToBinary(shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()>2){
        // If "node" has n>2 children, then the algorithm creates "new_node"
        // and set  "new_node" as a second child of "node", and set n-1 children of "node" as children of "new_node".
        shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
        new_node->bag = node->bag;
        // TODO: make this function more efficient
        for(size_t i=1; i<node->children.size();i++){
            new_node->children.push_back(node->children[i]);
            node->children[i]->parent = new_node;
        }
        node->children.resize(1);
        node->children.push_back(new_node);
        new_node->parent = node;
        convertToBinary(node->children[0]);
        convertToBinary(node->children[1]);
    }else{
        for(int i = 0; i < node->children.size(); i++){
            if(!convertToBinary(node->children[i])){
                return false;
            }
        }
    }
    return true;
}

bool TreeDecompositionPACE::joinFormat(shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()>2){
        cout<<"ERROR in TreeDecompositionPACE::joinFormat, the Raw abstract tree decomposition is not in binary form"<<endl;
        exit(20);
    }else if(node->children.size()==2){
            node->type="Join";
            if(!(node->bag ==node->children[0]->bag)){
                shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                new_node->bag = node->bag;
                new_node->children.push_back(node->children[0]);
                node->children[0]->parent = new_node;
                node->children[0] = new_node;
                new_node->parent = node;
                joinFormat(new_node->children[0]);
            }else{
                joinFormat(node->children[0]);
            }
            if(!(node->bag == node->children[1]->bag)){
                shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                new_node->bag = node->bag;
                new_node->children.push_back(node->children[1]);
                node->children[1]->parent = new_node;
                node->children[1] = new_node;
                new_node->parent = node;
                joinFormat(new_node->children[0]);
            }else{
                joinFormat(node->children[1]);
            }
    }else if(node->children.size()==1){
        joinFormat(node->children[0]);
    }else{

    }
    return true;
}

bool TreeDecompositionPACE::addEmptyNodes(shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()==0){
        if(node->bag.get_elements().size()>0){
            shared_ptr<RawAbstractTreeDecomposition> empty_node(new RawAbstractTreeDecomposition);
            node->children.push_back(empty_node);
            empty_node->parent = node;
            empty_node->type = "Leaf";
        }else{
            node->type = "Leaf";
        }
    }else{
        for( size_t i=0; i<node->children.size(); i++){
            addEmptyNodes(node->children[i]);
        }
    }
    return true;
}

bool TreeDecompositionPACE::addIntroVertex(shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()==1){
        set<unsigned> set_diff;
        set<unsigned> elements_node = node->bag.get_elements();
        set<unsigned> elements_node_child = node->children[0]->bag.get_elements();
        set_difference(elements_node.begin(),elements_node.end(), elements_node_child.begin(), elements_node_child.end(), inserter(set_diff, set_diff.begin()));
        if(set_diff.size()>1){
            shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
            new_node->children.push_back(node->children[0]);
            new_node->children[0]->parent = new_node;
            node->children[0] = new_node;
            new_node->parent = node;
            set<unsigned> elements_node_new = node->bag.get_elements();
            elements_node_new.erase(*set_diff.begin());
            new_node->bag.set_elements(elements_node_new);
            node->type = "IntroVertex_"+to_string(*set_diff.begin());
            addIntroVertex(new_node);
        }else if(set_diff.size()==1){
           node->type="IntroVertex_"+to_string(*set_diff.begin());
           addIntroVertex(node->children[0]);
        }else{
            addIntroVertex(node->children[0]);
        }
    }else if(node->children.size()==2){
        addIntroVertex(node->children[0]);
        addIntroVertex(node->children[1]);
    }
    for(auto it:node->children){
        if(node->type == it->type and node->type!="Join"){
            cout<<"ERROR in TreeDecompositionPACE::addIntroVertex, has same IntroVertex type"<<endl;
            cout<<node->type<<" "<<it->type<<endl;
            exit(20);
        }
    }
    return true;
}

bool TreeDecompositionPACE::addForgetVertex(shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()==1){
        set<unsigned> set_diff;
        set<unsigned> elements_node = node->bag.get_elements();
        set<unsigned> elements_node_child = node->children[0]->bag.get_elements();
        set_difference(elements_node_child.begin(), elements_node_child.end(), elements_node.begin(), elements_node.end(), inserter(set_diff, set_diff.begin()));
        if(set_diff.size()>1){
            if(node->type==""){
                shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                new_node->children.push_back(node->children[0]);
                new_node->children[0]->parent = new_node;
                node->children[0] = new_node;
                new_node->parent = node;
                set<unsigned> elements_node_new = node->bag.get_elements();
                elements_node_new.insert(*set_diff.rbegin());
                //set<unsigned> elements_node_new = new_node->children[0]->bag.get_elements();
                //elements_node_new.erase(*set_diff.rbegin());
                new_node->bag.set_elements(elements_node_new);
                node->type = "ForgetVertex_"+to_string(*set_diff.rbegin());
                addForgetVertex(new_node);
            }else{
                set_diff.clear();
                set_difference(elements_node.begin(),elements_node.end(), elements_node_child.begin(), elements_node_child.end(), inserter(set_diff, set_diff.begin()));
                if(set_diff.size()!=1){
                  cout<<"Error in TreeDecompositionPACE::addForgetVertex, set_diff size is not 1.";
                  exit(20);
                }else{
                  shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                  new_node->children.push_back(node->children[0]);
                  new_node->children[0]->parent = new_node;
                  node->children[0] = new_node;
                  new_node->parent = node;
                  set<unsigned> elements_node_new = node->bag.get_elements();
                  elements_node_new.erase(*set_diff.begin());
                  new_node->bag.set_elements(elements_node_new);
                  addForgetVertex(new_node);
                }
            }
        }else if(set_diff.size()==1){
           if(node->type==""){
                node->type="ForgetVertex_"+to_string(*set_diff.begin());
                addForgetVertex(node->children[0]);
           }else{
               set_diff.clear();
               set_difference(elements_node.begin(),elements_node.end(), elements_node_child.begin(), elements_node_child.end(), inserter(set_diff, set_diff.begin()));
               if(set_diff.size()!=1){
                   cout<<"Error in TreeDecompositionPACE::addForgetVertex, set_diff size is not 1.";
                   exit(20);
               }
                shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                new_node->children.push_back(node->children[0]);
                new_node->children[0]->parent = new_node;
                node->children[0] = new_node;
                new_node->parent = node;
                set<unsigned> elements_node_new = node->bag.get_elements();
                //set<unsigned> elements_node_new = new_node->children[0]->bag.get_elements();
                //elements_node_new.erase(*set_diff.begin());
                elements_node_new.erase(*set_diff.begin());
                new_node->bag.set_elements(elements_node_new);
                addForgetVertex(new_node);
           }
        }else{
            addForgetVertex(node->children[0]);
        }
    }else if(node->children.size()==2){
        addForgetVertex(node->children[0]);
        addForgetVertex(node->children[1]);
    }
    for(auto it:node->children){
        if(node->type == it->type and node->type!="Join"){
            cout<<"ERROR in TreeDecompositionPACE::addIntroVertex, has same ForgetVertex type"<<endl;
            exit(20);
        }
    }
    return true;
}

bool TreeDecompositionPACE::addIntroEdge(shared_ptr<RawAbstractTreeDecomposition> node, set<unsigned> &visited_edges) {
    if (strstr(node->type.c_str(), "IntroVertex")) {
        set<unsigned> set_diff;
        set<unsigned> elements_node = node->bag.get_elements();
        set<unsigned> elements_node_child = node->children[0]->bag.get_elements();
        set_difference(elements_node.begin(), elements_node.end(), elements_node_child.begin(),
                       elements_node_child.end(), inserter(set_diff, set_diff.begin()));
        if (set_diff.size() != 1) {
            cout << "ERROR in TreeDecompositionPACE::addIntroEdgeRecursion, set_diff is not verified" << endl;
            node->bag.print();
            cout << "\n" << node->type << endl;
            node->children[0]->bag.print();
            cout << "\n" << node->children[0]->type << endl;
            node->children[0]->children[0]->bag.print();
            cout << "\n" << node->children[0]->children[0]->type << endl;
            node->children[0]->children[0]->children[0]->bag.print();
            cout << "\n" << node->children[0]->children[0]->children[0]->type << endl;
            exit(20);
        } else {
            unsigned introducedVertex = (unsigned) *set_diff.begin();
            multimap<unsigned, unsigned> incidence = multigraph->getIncidenceMap();
            vector<shared_ptr<RawAbstractTreeDecomposition> > generated_nodes;
            for (auto it = incidence.begin(); it != incidence.end(); it++) {
                if (it->second == introducedVertex) {
                    if (visited_edges.find(it->first) == visited_edges.end()) {
                        for (auto itr = incidence.begin(); itr != incidence.end(); itr++) {
                            if (itr->second != it->second and itr->first==it->first and (node->bag.get_elements().count(itr->second) > 0)) {
                                visited_edges.insert(it->first);
                                shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                                new_node->bag = node->bag;
                                if (it->second < itr->second) {
                                    new_node->type =
                                            "IntroEdge_" + to_string(it->second) + "_" + to_string(itr->second);
                                    new_node->bag.set_edge(it->second, itr->second);
                                } else {
                                    new_node->type =
                                            "IntroEdge_" + to_string(itr->second) + "_" + to_string(it->second);
                                    new_node->bag.set_edge(itr->second, it->second);
                                }
                                generated_nodes.push_back(new_node);
                                break;
                            }
                        }
                    }
                }
            }
            for (size_t i = 0; i < generated_nodes.size(); i++) {
                if (i == 0) {
                    if (!node->parent) {
                        root = generated_nodes[0];
                    } else {
                        generated_nodes[0]->parent = node->parent;
                        for (size_t t = 0; t < node->parent->children.size(); t++) {
                            if (node->parent->children[t] == node) {
                                node->parent->children[t] = generated_nodes[0];
                                break;
                            }
                        }
                    }
                } else {
                    generated_nodes[i]->parent = generated_nodes[i - 1];
                    generated_nodes[i - 1]->children.push_back(generated_nodes[i]);
                }
            }
            if (generated_nodes.size() > 0) {
                generated_nodes[generated_nodes.size() - 1]->children.push_back(node);
                node->parent = generated_nodes[generated_nodes.size() - 1];
            }
            return addIntroEdge(node->children[0], visited_edges);

        }
    } else if (strstr(node->type.c_str(), "Join")) {
        return addIntroEdge(node->children[0], visited_edges) and addIntroEdge(node->children[1], visited_edges);
    } else if (strstr(node->type.c_str(), "ForgetVertex")) {
        return addIntroEdge(node->children[0], visited_edges);
    } else if (strstr(node->type.c_str(), "Leaf")) {
        return true;
    } else {
        cout << "ERROR in TreeDecompositionPACE::addIntroEdgeRecursion, node type is not satisfied" << endl;
        exit(20);
    }

}
bool TreeDecompositionPACE::colorNode(shared_ptr<RawAbstractTreeDecomposition> node, vector<unsigned> &color_vertex, vector<unsigned> &vertex_color){
    if(!node->parent){
        // here is for root coloring
        unsigned color=1;
        set<unsigned> elements = node->bag.get_elements();
        for(auto it=elements.begin(); it!=elements.end(); it++){
               vertex_color[*it-1]=color;
               color_vertex[color-1]=*it;
               node->color_to_vertex_map.insert(make_pair(color,*it));
               color++;
        }
    }else{
        set<unsigned> set_diff;
        set<unsigned> elements_node = node->bag.get_elements();
        set<unsigned> elements_node_parent = node->parent->bag.get_elements();
        set_difference(elements_node_parent.begin(), elements_node_parent.end(), elements_node.begin(), elements_node.end(),inserter(set_diff, set_diff.begin()));
        node->color_to_vertex_map = node->parent->color_to_vertex_map;
        for(auto it=set_diff.begin(); it!=set_diff.end(); it++){
            color_vertex[vertex_color[*it-1]-1]=0;
            node->color_to_vertex_map.erase(vertex_color[*it-1]);
        }
        set_diff.clear();
        set_difference(elements_node.begin(), elements_node.end(), elements_node_parent.begin(), elements_node_parent.end(), inserter(set_diff, set_diff.begin()));
        for(auto it=set_diff.begin(); it!=set_diff.end(); it++){
            auto itr=find(color_vertex.begin(), color_vertex.end(), 0);
            if(itr!=color_vertex.end()){
                color_vertex[itr-color_vertex.begin()]=*it;
                vertex_color[*it-1]=itr-color_vertex.begin()+1;
                node->color_to_vertex_map.insert(make_pair(itr-color_vertex.begin()+1,*it));
            }else{
                cout<<"ERROR in TreeDecompositionPACE::colorNode there is no an available color!"<<endl;
                exit(20);
            }
        }
    }
    if(node->children.size()==2){
        // Because tree decomposition is divided to two subtrees, we have to pass color vectors
        vector<unsigned > color_vertex1 = color_vertex;
        vector<unsigned > color_vertex2 = color_vertex;
        vector<unsigned > vertex_color1 = vertex_color;
        vector<unsigned > vertex_color2 = vertex_color;
        colorNode(node->children[0], color_vertex1, vertex_color1);
        colorNode(node->children[1], color_vertex2, vertex_color2);
    }else if( node->children.size()==1){
        colorNode(node->children[0], color_vertex, vertex_color);
    }

    return true;
}
bool TreeDecompositionPACE::colorTree(){
    vector<unsigned> color_vertex;
    vector<unsigned> vertex_color;
    color_vertex.resize(width+1,0);
    vertex_color.resize(multigraph->getVertices().size(),0);
    return  colorNode(root,color_vertex,vertex_color);
}
bool TreeDecompositionPACE::updateInnerNodeTD(shared_ptr<RawAbstractTreeDecomposition> node, unsigned &number, unsigned parentno){
    number++;
    parentno=number;
    bags.push_back(node->bag.get_elements());
    num_vertices++;
    num_edges = num_edges + node->children.size();
    for(size_t i=0; i < node->children.size();i++){
        edges.insert(make_pair(parentno,number+1));
        updateInnerNodeTD(node->children[i],number,parentno);
    }
    return true;
}
bool TreeDecompositionPACE::updateTD(){
    bags.clear();
    edges.clear();
    num_edges=0;
    num_vertices=0;
    unsigned number=0;
    unsigned parentno=0;
    updateInnerNodeTD(root,number, parentno);
    return true;
}

void TreeDecompositionPACE::construct() {
    constructRaw();
    convertToBinary(root);
    joinFormat(root);
    addEmptyNodes(root);
    addIntroVertex(root);
    addForgetVertex(root);

    set<unsigned> visited_edges;
    addIntroEdge(root,visited_edges);

    colorTree();
    updateTD();
    validateTree(root);
    cout<<"Construction finished"<<endl;
}

void TreeDecompositionPACE::createCTDNode(shared_ptr<TermNode<ConcreteNode>> cnode, shared_ptr<RawAbstractTreeDecomposition> rnode) {
    ConcreteNode concrete;
    concrete.setSymbol(rnode->type);
    Bag bag;
    set<unsigned> bag_elements;
    pair<unsigned,unsigned> e_new = make_pair(0,0);
    pair<unsigned,unsigned> e = rnode->bag.get_edge();
    for(auto it= rnode->color_to_vertex_map.begin(); it!= rnode->color_to_vertex_map.end();it++){
        bag_elements.insert(it->first);
        if(e.first == it->second){
            e_new.first = it->first;
        }else if (e.second == it->second){
            e_new.second = it->first;
        }
    }
    if(e_new.first > e_new.second){
        swap(e_new.first,e_new.second);
    }
    bag.set_elements(bag_elements);
    bag.set_edge(e_new.first,e_new.second);
    concrete.setBag(bag);
    vector<shared_ptr<TermNode<ConcreteNode>>> children;
    for(size_t i=0 ; i < rnode->children.size(); i++){
        shared_ptr<TermNode<ConcreteNode> > ctdnode(new TermNode<ConcreteNode>);
        createCTDNode(ctdnode, rnode->children[i]);
        children.push_back(ctdnode);
        ctdnode->setParent(cnode);
    }

    cnode->setChildren(children);
    if(strstr(rnode->type.c_str(),"IntroEdge_")){
        string type = "IntroEdge_"+to_string(e_new.first)+"_"+to_string(e_new.second);
        concrete.setSymbol(type);
    }else if(strstr(rnode->type.c_str(),"IntroVertex_")){
        set<unsigned> set_diff;
        set<unsigned> elements_node = concrete.getBag().get_elements();
        set<unsigned> elements_node_child = cnode->getChildren()[0]->getNodeContent().getBag().get_elements();
        set_difference( elements_node.begin(), elements_node.end(),elements_node_child.begin(), elements_node_child.end(),inserter(set_diff, set_diff.begin()));
        if(set_diff.size()!=1){
            cout<<"ERROR in TreeDecompositionPACE::createCTDNode(IntroVertex), set_diff is not valid"<<endl;
            exit(20);
        }
        concrete.setSymbol("IntroVertex_"+to_string(*set_diff.begin()));
    }else if(strstr(rnode->type.c_str(),"ForgetVertex_")){
        set<unsigned> set_diff;
        set<unsigned> elements_node = concrete.getBag().get_elements();
        set<unsigned> elements_node_child = cnode->getChildren()[0]->getNodeContent().getBag().get_elements();
        set_difference(elements_node_child.begin(),elements_node_child.end(),elements_node.begin(), elements_node.end(),inserter(set_diff, set_diff.begin()));
        if(set_diff.size()!=1){
            cout<<"ERORR in TreeDecompositionPACE::createCTDNode(ForgetVertex), set_diff is not valid"<<endl;
            exit(20);
        }
        concrete.setSymbol("ForgetVertex_"+to_string(*set_diff.begin()));
    }
    cnode->setNodeContent(concrete);
}

shared_ptr<ConcreteTreeDecomposition> TreeDecompositionPACE::convertToConcreteTreeDecomposition() {
    shared_ptr<ConcreteTreeDecomposition> concreteTreeDecomposition(new ConcreteTreeDecomposition);
    shared_ptr<TermNode<ConcreteNode> > ctd_root(new TermNode<ConcreteNode>);
    concreteTreeDecomposition->setRoot(ctd_root);
    createCTDNode(ctd_root, root);
    return concreteTreeDecomposition;
}

void TreeDecompositionPACE::setWidthType(const string &widthType) {
    width_type = widthType;
}

unsigned int TreeDecompositionPACE::getWidth() const {
    return width;
}

const string &TreeDecompositionPACE::getWidthType() const {
    return width_type;
}

bool TreeDecompositionPACE::validateTree(shared_ptr<RawAbstractTreeDecomposition> node) {
    if(node->children.size()==2){
        if(node->type!="Join"){
            cout<<"Error in TreeDecompositionPACE::validateTree, join type"<<endl;
            exit(20);
        }
        if(node->children[0]->bag.get_elements() != node->children[1]->bag.get_elements()){
            cout<< "  Error in TreeDecompositionPACE::validateTree, children of a join node do not have a same bagSet"<<endl;
            exit(20);
        }
        return validateTree(node->children[0]) and validateTree(node->children[1]);
    }else if (node->children.size()==1){
        if(strstr(node->type.c_str(),"IntroVertex")){
            set<unsigned > set_diff;
            set<unsigned > elements = node->bag.get_elements();
            set<unsigned > childElements = node->children[0]->bag.get_elements();
            set_difference(elements.begin(),elements.end(),childElements.begin(),
                           childElements.end(), inserter(set_diff,set_diff.begin()));
            if(set_diff.size()!=1){
                cout<<"Error in TreeDecompositionPACE::validateTree, IntroVertex, set_diff is invalid"<<endl;
                exit(20);
            }
        }else if (strstr(node->type.c_str(),"ForgetVertex")){
            set<unsigned > set_diff;
            set<unsigned > elements = node->bag.get_elements();
            set<unsigned > childElements = node->children[0]->bag.get_elements();
            set_difference(childElements.begin(),
                           childElements.end(), elements.begin(),elements.end(),inserter(set_diff,set_diff.begin()));
            if(set_diff.size()!=1){
                cout<<"Error in TreeDecompositionPACE::validateTree, ForgetVertex, set_diff is invalid"<<endl;
                exit(20);
            }
        }else if(strstr(node->type.c_str(),"IntroEdge")){
            pair<unsigned ,unsigned > e = make_pair(0,0);
            pair nodeEdge = node->bag.get_edge();
            if(nodeEdge == e){
                cout<<"Error in TreeDecompositionPACE::validateTree, IntroEdge, nodeEdge is invalid"<<endl;
                exit(20);
            }
        }else{
            cout<<"Error in TreeDecompositionPACE::validateTree, node type is invalid ("<<node->type<<")"<<endl;
            exit(20);
        }
        return validateTree(node->children[0]);
    }else if(node->children.size()==0){
        if(strstr(node->type.c_str(),"Leaf")){
            if(!node->bag.get_elements().empty()){
                cout<<"Error in TreeDecompositionPACE::validateTree, Leaf is invalid"<<endl;
                exit(20);
            }
        }else{
            cout<<"Error in TreeDecompositionPACE::validateTree, node type is invalid ("<<node->type<<")"<<endl;
            exit(20);
        }
        return true;
    }else{
        cout<<"Error in TreeDecompositionPACE::validateTree, node children is invalid"<<endl;
        exit(20);
    }
}
