#include "TreeDecompositionPACE.h"

RawAbstractTreeDecomposition::RawAbstractTreeDecomposition(){
  // constructor
}
void RawAbstractTreeDecomposition::print(unsigned &k,unsigned parentno){
    k++;
    std::cout<<k<<" "<<type<<"(" <<parentno <<") " ;
    bag.print();
    for(auto it=color_to_vertex_map.begin(); it!=color_to_vertex_map.end();it++){
            std::cout<<it->first<<"->"<<it->second<<" ";
    }
    std::cout<<std::endl;
    unsigned num = k;
    for(size_t i=0; i < children.size(); i++){
        children[i]->print(k,num);
    }
}

void RawAbstractTreeDecomposition::printNode(){
    bag.print();
    for(auto it=color_to_vertex_map.begin(); it!=color_to_vertex_map.end();it++){
        std::cout<<it->first<<"->"<<it->second<<" ";
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
            edges.insert(std::make_pair(e2,e1));
            return true;
        }else if(e1<e2){
            edges.insert(std::make_pair(e1,e2));
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
        std::cout<< "b " << i+1 <<  " ";
        for(auto it = bags[i].begin(); it!=bags[i].end(); it++){
            std::cout<<*it;
            if(next(it)!= bags[i].end()){
                std::cout<<" ";
            }
        }
        std::cout<<""<<std::endl;
    }
}

void TreeDecompositionPACE::printEdges(){
    for(auto it=edges.begin(); it!=edges.end();it++){
        std::cout<<it->first<<"  " << it->second<<std::endl;
    }
}

void TreeDecompositionPACE::print(){
    std::cout<<"s td " << num_vertices<< "  "<<width+1<<"  "<<num_graph_vertices<<std::endl;
    printBags();
    printEdges();
}

void TreeDecompositionPACE::printTree(){
    unsigned k=0;
    if(root == nullptr){
        std::cout << "\033[1;31mError in TreeDecompositionPACE::printTree : the root is not set!\033[0m"<<std::endl;
    }else{
        root->print(k,0);
    }

}

std::shared_ptr<RawAbstractTreeDecomposition> TreeDecompositionPACE::constructInnerNodes(std::set<unsigned> &visited_bags, unsigned neighbor){
    std::shared_ptr<RawAbstractTreeDecomposition> node (new RawAbstractTreeDecomposition);
    node->bag.set_elements(bags[neighbor-1]);
    visited_bags.insert(neighbor);
    for(auto it=edges.begin(); it!=edges.end(); it++){
        if(it->first==neighbor and visited_bags.count(it->second)==0 ){
            std::shared_ptr<RawAbstractTreeDecomposition> new_node;
            new_node = constructInnerNodes(visited_bags,it->second);
            if(bags[neighbor-1] != bags[it->second-1]){
                new_node->parent = node;
                node->children.push_back(new_node);
            }else{
                // parent and child has the same bags.
                if(new_node->children.size() != 0){
                    for (int i = 0; i < new_node->children.size(); ++i) {
                        node->children.push_back(new_node->children[i]);
                        new_node->children[i]->parent = node;
                    }
                }else{
                    // nothing happens
                }
            }
        }else if(it->second==neighbor and visited_bags.count(it->first)==0){
            std::shared_ptr<RawAbstractTreeDecomposition> new_node;
            new_node = constructInnerNodes(visited_bags,it->first);

            if(bags[neighbor-1] != bags[it->first-1]){
                new_node->parent = node;
                node->children.push_back(new_node);
            }else{
                // parent and child has the same bags.
                if(new_node->children.size() != 0){
                    for (int i = 0; i < new_node->children.size(); ++i) {
                        node->children.push_back(new_node->children[i]);
                        new_node->children[i]->parent = node;
                    }
                }else{
                    // nothing happens
                }
            }
        }
    }
    return node;
}

bool TreeDecompositionPACE::constructRaw(){
    std::shared_ptr<RawAbstractTreeDecomposition> root_node(new RawAbstractTreeDecomposition);
    root = root_node;
    if(bags.size()>0){
        root->bag.set_elements(bags[0]);
        std::set<unsigned> visited_bags;
        visited_bags.insert(1);
        //create nodes for neighbors
        for(auto it=edges.begin(); it!=edges.end(); it++){
            // The reason that always the first pair should have edge with number 1 is that the edge set is an ordered set.
            if(it->first==1){
                std::shared_ptr<RawAbstractTreeDecomposition> node;
                node = constructInnerNodes(visited_bags,it->second);
                node->parent = root;
                root->children.push_back(node);
            }else if(it->second==1){
                std::shared_ptr<RawAbstractTreeDecomposition> node;
                node = constructInnerNodes(visited_bags,it->first);
                node->parent = root;
                root->children.push_back(node);
            }
        }
        return true;
    }else{
        std::cout<<"ERROR in TreeDecompositionPACE::constructRaw bags size is zero"<<std::endl;
        return false;
    }
}

bool TreeDecompositionPACE::convertToBinary(std::shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()>2){
        // If "node" has n>2 children, then the algorithm creates "new_node"
        // and set  "new_node" as a second child of "node", and set n-1 children of "node" as children of "new_node".
        std::shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
        new_node->bag = node->bag;
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

bool TreeDecompositionPACE::joinFormat(std::shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()>2){
        std::cout<<"ERROR in TreeDecompositionPACE::joinFormat, the Raw abstract tree decomposition is not in binary form"<<std::endl;
        exit(20);
    }else if(node->children.size()==2){
            node->type="Join";
            if(!(node->bag ==node->children[0]->bag)){
                std::shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
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
                std::shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
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

bool TreeDecompositionPACE::addEmptyNodes(std::shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()==0){
        if(node->bag.get_elements().size()>0){
            std::shared_ptr<RawAbstractTreeDecomposition> empty_node(new RawAbstractTreeDecomposition);
            empty_node->type = "Leaf";
            node->children.push_back(empty_node);
            empty_node->parent = node;
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

bool TreeDecompositionPACE::addIntroVertex(std::shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()==1){
        std::set<unsigned> set_diff;
        std::set<unsigned> elements_node = node->bag.get_elements();
        std::set<unsigned> elements_node_child = node->children[0]->bag.get_elements();
        set_difference(elements_node.begin(),elements_node.end(), elements_node_child.begin(), elements_node_child.end(), inserter(set_diff, set_diff.begin()));
        if(set_diff.size()>1){
            std::shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
            new_node->children.push_back(node->children[0]);
            new_node->children[0]->parent = new_node;
            node->children[0] = new_node;
            new_node->parent = node;
            std::set<unsigned> elements_node_new = node->bag.get_elements();
            elements_node_new.erase(*set_diff.begin());
            new_node->bag.set_elements(elements_node_new);
            node->type = "IntroVertex_"+std::to_string(*set_diff.begin());
            addIntroVertex(new_node);
        }else if(set_diff.size()==1){
           node->type="IntroVertex_"+std::to_string(*set_diff.begin());
           addIntroVertex(node->children[0]);
        }else if (elements_node == elements_node_child){
            std::cerr<< "duplicated bags" <<std::endl;
            exit(20);
        }
        else{
            addIntroVertex(node->children[0]);
        }
    }else if(node->children.size()==2){
        addIntroVertex(node->children[0]);
        addIntroVertex(node->children[1]);
    }else if(node->children.size() >2){
        std::cout << "ERROR in TreeDecompositionPACE::addIntroVertex, node has more that two children. Number of Children = " << node->children.size()<<std::endl;
        exit(20);
    }
//    for(auto it:node->children){
//        if(node->type == it->type and node->type!="Join"){
//            std::cout<<"ERROR in TreeDecompositionPACE::addIntroVertex, has same IntroVertex type"<<std::endl;
//            std::cout<<node->type<<" "<<it->type<<std::endl;
////            node->bag.print();
////            std::cout<<std::endl;
////            it->bag.print();
////            std::cout<<"\n"<<it->children.size()<<std::endl;
////            printTree();
//            exit(20);
//        }
//    }
    return true;
}

bool TreeDecompositionPACE::addForgetVertex(std::shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()==1){
        std::set<unsigned> set_diff;
        std::set<unsigned> elements_node = node->bag.get_elements();
        std::set<unsigned> elements_node_child = node->children[0]->bag.get_elements();
        set_difference(elements_node_child.begin(), elements_node_child.end(), elements_node.begin(), elements_node.end(), inserter(set_diff, set_diff.begin()));
        if(set_diff.size()>1){
            if(node->type==""){
                std::shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                new_node->children.push_back(node->children[0]);
                new_node->children[0]->parent = new_node;
                node->children[0] = new_node;
                new_node->parent = node;
                std::set<unsigned> elements_node_new = node->bag.get_elements();
                elements_node_new.insert(*set_diff.rbegin());
                //set<unsigned> elements_node_new = new_node->children[0]->bag.get_elements();
                //elements_node_new.erase(*set_diff.rbegin());
                new_node->bag.set_elements(elements_node_new);
                node->type = "ForgetVertex_"+std::to_string(*set_diff.rbegin());
                addForgetVertex(new_node);
            }else{
                set_diff.clear();
                set_difference(elements_node.begin(),elements_node.end(), elements_node_child.begin(), elements_node_child.end(), inserter(set_diff, set_diff.begin()));
                if(set_diff.size()!=1){
                  std::cout<<"Error in TreeDecompositionPACE::addForgetVertex, set_diff size is not 1.";
                  exit(20);
                }else{
                  std::shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                  new_node->children.push_back(node->children[0]);
                  new_node->children[0]->parent = new_node;
                  node->children[0] = new_node;
                  new_node->parent = node;
                  std::set<unsigned> elements_node_new = node->bag.get_elements();
                  elements_node_new.erase(*set_diff.begin());
                  new_node->bag.set_elements(elements_node_new);
                  addForgetVertex(new_node);
                }
            }
        }else if(set_diff.size()==1){
           if(node->type==""){
                node->type="ForgetVertex_"+std::to_string(*set_diff.begin());
                addForgetVertex(node->children[0]);
           }else{
               set_diff.clear();
               set_difference(elements_node.begin(),elements_node.end(), elements_node_child.begin(), elements_node_child.end(), inserter(set_diff, set_diff.begin()));
               if(set_diff.size()!=1){
                   std::cout<<"Error in TreeDecompositionPACE::addForgetVertex, set_diff size is not 1.";
                   exit(20);
               }
                std::shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                new_node->children.push_back(node->children[0]);
                new_node->children[0]->parent = new_node;
                node->children[0] = new_node;
                new_node->parent = node;
                std::set<unsigned> elements_node_new = node->bag.get_elements();
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
            std::cout<<"ERROR in TreeDecompositionPACE::addIntroVertex, has same ForgetVertex type"<<std::endl;
            exit(20);
        }
    }
    return true;
}

bool TreeDecompositionPACE::addIntroEdge(std::shared_ptr<RawAbstractTreeDecomposition> node, std::set<unsigned> &visited_edges) {
    if (strstr(node->type.c_str(), "IntroVertex")) {
        std::set<unsigned> set_diff;
        std::set<unsigned> elements_node = node->bag.get_elements();
        std::set<unsigned> elements_node_child = node->children[0]->bag.get_elements();
        set_difference(elements_node.begin(), elements_node.end(), elements_node_child.begin(),
                       elements_node_child.end(), inserter(set_diff, set_diff.begin()));
        if (set_diff.size() != 1) {
            std::cout << "ERROR in TreeDecompositionPACE::addIntroEdgeRecursion, set_diff is not verified" << std::endl;
            node->bag.print();
            std::cout << "\n" << node->type << std::endl;
            node->children[0]->bag.print();
            std::cout << "\n" << node->children[0]->type << std::endl;
            node->children[0]->children[0]->bag.print();
            std::cout << "\n" << node->children[0]->children[0]->type << std::endl;
            node->children[0]->children[0]->children[0]->bag.print();
            std::cout << "\n" << node->children[0]->children[0]->children[0]->type << std::endl;
            exit(20);
        } else {
            unsigned introducedVertex = (unsigned) *set_diff.begin();
            std::multimap<unsigned, unsigned> incidence = multigraph->getIncidenceMap();
            std::vector<std::shared_ptr<RawAbstractTreeDecomposition> > generated_nodes;
            for (auto it = incidence.begin(); it != incidence.end(); it++) {
                if (it->second == introducedVertex) {
                    if (visited_edges.find(it->first) == visited_edges.end()) {
                        for (auto itr = incidence.begin(); itr != incidence.end(); itr++) {
                            if (itr->second != it->second and itr->first==it->first and (node->bag.get_elements().count(itr->second) > 0)) {
                                visited_edges.insert(it->first);
                                std::shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                                new_node->bag = node->bag;
                                if (it->second < itr->second) {
                                    new_node->type =
                                            "IntroEdge_" + std::to_string(it->second) + "_" + std::to_string(itr->second);
                                    new_node->bag.set_edge(it->second, itr->second);
                                } else {
                                    new_node->type =
                                            "IntroEdge_" + std::to_string(itr->second) + "_" + std::to_string(it->second);
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
        std::cout << "ERROR in TreeDecompositionPACE::addIntroEdgeRecursion, node type is not satisfied" << std::endl;
        exit(20);
    }

}
bool TreeDecompositionPACE::colorNode(std::shared_ptr<RawAbstractTreeDecomposition> node, std::vector<unsigned> &color_vertex, std::vector<unsigned> &vertex_color){
    if(!node->parent){
        // here is for root coloring
        unsigned color=1;
        std::set<unsigned> elements = node->bag.get_elements();
        for(auto it=elements.begin(); it!=elements.end(); it++){
               vertex_color[*it-1]=color;
               color_vertex[color-1]=*it;
               node->color_to_vertex_map.insert(std::make_pair(color,*it));
               color++;
        }
    }else{
        std::set<unsigned> set_diff;
        std::set<unsigned> elements_node = node->bag.get_elements();
        std::set<unsigned> elements_node_parent = node->parent->bag.get_elements();
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
                node->color_to_vertex_map.insert(std::make_pair(itr-color_vertex.begin()+1,*it));
            }else{
                std::cout<<"ERROR in TreeDecompositionPACE::colorNode there is no an available color!"<<std::endl;
                exit(20);
            }
        }
    }
    if(node->children.size()==2){
        // Because tree decomposition is divided to two subtrees, we have to pass color vectors
        std::vector<unsigned> color_vertex1 = color_vertex;
        std::vector<unsigned> color_vertex2 = color_vertex;
        std::vector<unsigned> vertex_color1 = vertex_color;
        std::vector<unsigned> vertex_color2 = vertex_color;
        colorNode(node->children[0], color_vertex1, vertex_color1);
        colorNode(node->children[1], color_vertex2, vertex_color2);
    }else if( node->children.size()==1){
        colorNode(node->children[0], color_vertex, vertex_color);
    }

    return true;
}
bool TreeDecompositionPACE::colorTree(){
    std::vector<unsigned> color_vertex;
    std::vector<unsigned> vertex_color;
    color_vertex.resize(width+1,0);
    vertex_color.resize(multigraph->getVertices().size(),0);
    return  colorNode(root,color_vertex,vertex_color);
}
bool TreeDecompositionPACE::updateInnerNodeTD(std::shared_ptr<RawAbstractTreeDecomposition> node, unsigned &number, unsigned parentno){
    number++;
    parentno=number;
    bags.push_back(node->bag.get_elements());
    num_vertices++;
    num_edges = num_edges + node->children.size();
    for(size_t i=0; i < node->children.size();i++){
        edges.insert(std::make_pair(parentno,number+1));
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
    std::set<unsigned> visited_edges;
    addIntroEdge(root,visited_edges);
   // for(auto item:visited_edges){std::cout<<item<<",";}std::cout<<std::endl;
    colorTree();
    updateTD();
    validateTree(root);
    std::cout<<"Construction finished"<<std::endl;
}

void TreeDecompositionPACE::createCTDNode(std::shared_ptr<TermNode<ConcreteNode>> cnode, std::shared_ptr<RawAbstractTreeDecomposition> rnode) {
    ConcreteNode concrete;
    concrete.setSymbol(rnode->type);
    Bag bag;
    std::set<unsigned> bag_elements;
    std::pair<unsigned,unsigned> e_new = std::make_pair(0,0);
    std::pair<unsigned,unsigned> e = rnode->bag.get_edge();
    for(auto it= rnode->color_to_vertex_map.begin(); it!= rnode->color_to_vertex_map.end();it++){
        bag_elements.insert(it->first);
        if(e.first == it->second){
            e_new.first = it->first;
        }else if (e.second == it->second){
            e_new.second = it->first;
        }
    }
    if(e_new.first > e_new.second){
        std::swap(e_new.first,e_new.second);
    }
    bag.set_elements(bag_elements);
    bag.set_edge(e_new.first,e_new.second);
    concrete.setBag(bag);
    std::vector<std::shared_ptr<TermNode<ConcreteNode>>> children;
    for(size_t i=0 ; i < rnode->children.size(); i++){
        std::shared_ptr<TermNode<ConcreteNode> > ctdnode(new TermNode<ConcreteNode>);
        createCTDNode(ctdnode, rnode->children[i]);
        children.push_back(ctdnode);
        ctdnode->setParent(cnode);
    }

    cnode->setChildren(children);
    if(strstr(rnode->type.c_str(),"IntroEdge_")){
        std::string type = "IntroEdge_"+std::to_string(e_new.first)+"_"+std::to_string(e_new.second);
        concrete.setSymbol(type);
    }else if(strstr(rnode->type.c_str(),"IntroVertex_")){
        std::set<unsigned> set_diff;
        std::set<unsigned> elements_node = concrete.getBag().get_elements();
        std::set<unsigned> elements_node_child = cnode->getChildren()[0]->getNodeContent().getBag().get_elements();
        set_difference( elements_node.begin(), elements_node.end(),elements_node_child.begin(), elements_node_child.end(),inserter(set_diff, set_diff.begin()));
        if(set_diff.size()!=1){
            std::cout<<"ERROR in TreeDecompositionPACE::createCTDNode(IntroVertex), set_diff is not valid"<<std::endl;
            exit(20);
        }
        concrete.setSymbol("IntroVertex_"+std::to_string(*set_diff.begin()));
    }else if(strstr(rnode->type.c_str(),"ForgetVertex_")){
        std::set<unsigned> set_diff;
        std::set<unsigned> elements_node = concrete.getBag().get_elements();
        std::set<unsigned> elements_node_child = cnode->getChildren()[0]->getNodeContent().getBag().get_elements();
        set_difference(elements_node_child.begin(),elements_node_child.end(),elements_node.begin(), elements_node.end(),inserter(set_diff, set_diff.begin()));
        if(set_diff.size()!=1){
            std::cout<<"ERORR in TreeDecompositionPACE::createCTDNode(ForgetVertex), set_diff is not valid"<<std::endl;
            exit(20);
        }
        concrete.setSymbol("ForgetVertex_"+std::to_string(*set_diff.begin()));
    }
    cnode->setNodeContent(concrete);
}

std::shared_ptr<ConcreteTreeDecomposition> TreeDecompositionPACE::convertToConcreteTreeDecomposition() {
    std::shared_ptr<ConcreteTreeDecomposition> concreteTreeDecomposition(new ConcreteTreeDecomposition);
    std::shared_ptr<TermNode<ConcreteNode> > ctd_root(new TermNode<ConcreteNode>);
    concreteTreeDecomposition->setRoot(ctd_root);
    createCTDNode(ctd_root, root);
    return concreteTreeDecomposition;
}

void TreeDecompositionPACE::setWidthType(const std::string &widthType) {
    width_type = widthType;
}

unsigned int TreeDecompositionPACE::getWidth() const {
    return width;
}

const std::string &TreeDecompositionPACE::getWidthType() const {
    return width_type;
}

bool TreeDecompositionPACE::validateTree(std::shared_ptr<RawAbstractTreeDecomposition> node) {
    if(node->children.size()==2){
        if(node->type!="Join"){
            std::cout<<"Error in TreeDecompositionPACE::validateTree, join type"<<std::endl;
            exit(20);
        }
        if(node->children[0]->bag.get_elements() != node->children[1]->bag.get_elements()){
            std::cout<< "  Error in TreeDecompositionPACE::validateTree, children of a join node do not have a same bagSet"<<std::endl;
            exit(20);
        }
        return validateTree(node->children[0]) and validateTree(node->children[1]);
    }else if (node->children.size()==1){
        if(strstr(node->type.c_str(),"IntroVertex")){
            std::set<unsigned > set_diff;
            std::set<unsigned > elements = node->bag.get_elements();
            std::set<unsigned > childElements = node->children[0]->bag.get_elements();
            set_difference(elements.begin(),elements.end(),childElements.begin(),
                           childElements.end(), inserter(set_diff,set_diff.begin()));
            if(set_diff.size()!=1){
                std::cout<<"Error in TreeDecompositionPACE::validateTree, IntroVertex, set_diff is invalid"<<std::endl;
                exit(20);
            }
        }else if (strstr(node->type.c_str(),"ForgetVertex")){
            std::set<unsigned > set_diff;
            std::set<unsigned > elements = node->bag.get_elements();
            std::set<unsigned > childElements = node->children[0]->bag.get_elements();
            set_difference(childElements.begin(),
                           childElements.end(), elements.begin(),elements.end(),inserter(set_diff,set_diff.begin()));
            if(set_diff.size()!=1){
                std::cout<<"Error in TreeDecompositionPACE::validateTree, ForgetVertex, set_diff is invalid"<<std::endl;
                exit(20);
            }
        }else if(strstr(node->type.c_str(),"IntroEdge")){
            std::pair<unsigned ,unsigned > e = std::make_pair(0,0);
            std::pair nodeEdge = node->bag.get_edge();
            if(nodeEdge == e){
                std::cout<<"Error in TreeDecompositionPACE::validateTree, IntroEdge, nodeEdge is invalid"<<std::endl;
                exit(20);
            }
        }else{
            std::cout<<"Error in TreeDecompositionPACE::validateTree, node type is invalid ("<<node->type<<")"<<std::endl;
            exit(20);
        }
        return validateTree(node->children[0]);
    }else if(node->children.size()==0){
        if(strstr(node->type.c_str(),"Leaf")){
            if(!node->bag.get_elements().empty()){
                std::cout<<"Error in TreeDecompositionPACE::validateTree, Leaf is invalid"<<std::endl;
                exit(20);
            }
        }else{
            std::cout<<"Error in TreeDecompositionPACE::validateTree, node type is invalid ("<<node->type<<")"<<std::endl;
            exit(20);
        }
        return true;
    }else{
        std::cout<<"Error in TreeDecompositionPACE::validateTree, node children is invalid"<<std::endl;
        exit(20);
    }
}
