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
    if(bagnumber>0){
        bags[bagnumber-1] = s;
        return true;
    }else{

        return false;
    }
}

bool TreeDecompositionPACE::addEdge(unsigned e1, unsigned e2){

    if(e1>e2){
        edges.insert(make_pair(e2,e1));
        return true;
    }else if(e1<e2){
        edges.insert(make_pair(e1,e2));
        return true;
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
    if(it->first==neighbor and visited_bags.find(it->second)==visited_bags.end() ){
            shared_ptr<RawAbstractTreeDecomposition> new_node;
            new_node = constructInnerNodes(visited_bags,it->second);
            new_node->parent = node;
            node->children.push_back(new_node); 
        }else if(it->second==neighbor and visited_bags.find(it->first)==visited_bags.end()){
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
        shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
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
    }
    return true;
}

bool TreeDecompositionPACE::joinFormat(shared_ptr<RawAbstractTreeDecomposition> node){
    if(node->children.size()>2){
        cout<<"ERPOR in TreeDecompositionPACE::joinFormat, the Raw abstract tree decomposition is not in binary form"<<endl;
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
            empty_node->type = "Empty";
 
        }else{

            node->type = "Empty";
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
            elements_node_new.erase(*set_diff.rbegin());
            new_node->bag.set_elements(elements_node_new);
            node->type = "IntroVertex_"+to_string(*set_diff.rbegin());
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
                set<unsigned> elements_node_new = new_node->children[0]->bag.get_elements();
                elements_node_new.erase(*set_diff.rbegin());
                new_node->bag.set_elements(elements_node_new);
                node->type = "ForgetVertex_"+to_string(*set_diff.rbegin());
                addForgetVertex(new_node);
            }else{
                shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                new_node->children.push_back(node->children[0]);
                new_node->children[0]->parent = new_node;
                node->children[0] = new_node;
                new_node->parent = node;
                new_node->bag = node->bag;
                addForgetVertex(new_node);
            }

        }else if(set_diff.size()==1){
           if(node->type==""){ 
                node->type="ForgetVertex_"+to_string(*set_diff.begin());
                addForgetVertex(node->children[0]);
           }else{
                shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                new_node->children.push_back(node->children[0]);
                new_node->children[0]->parent = new_node;
                node->children[0] = new_node;
                new_node->parent = node;
                set<unsigned> elements_node_new = new_node->children[0]->bag.get_elements();
                elements_node_new.erase(*set_diff.rbegin());
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
    return true;
}

bool TreeDecompositionPACE::addIntroEdge(shared_ptr<RawAbstractTreeDecomposition> node,set<unsigned> &visited_edges){
    if(strstr(node->type.c_str(),"IntroVertex")){
        set<unsigned> set_diff;
        set<unsigned> elements_node = node->bag.get_elements();
        set<unsigned> elements_node_child = node->children[0]->bag.get_elements();
        set_difference(elements_node.begin(),elements_node.end(), elements_node_child.begin(), elements_node_child.end(), inserter(set_diff, set_diff.begin()));
        if(set_diff.size()!=1){
            cout<<"ERORR in TreeDecompositionPACE::addIntroEdgeRecursion, set_diff is not verified"<<endl;
            exit(20); 
        }else{
            multimap<unsigned,unsigned> incidence = multigraph->getIncidenceMap();
            vector<shared_ptr<RawAbstractTreeDecomposition> > generated_nodes;
            for(auto it=incidence.begin(); it!=incidence.end(); it++){
                if(it->second == (unsigned)*set_diff.begin()){
                    if(visited_edges.find(it->first)==visited_edges.end()){
                            visited_edges.insert(it->first);
                            auto range_it = incidence.equal_range(it->first);
                            for(auto itr=range_it.first;itr!=range_it.second; itr++){
                                if(itr->second != it->second){
                                    shared_ptr<RawAbstractTreeDecomposition> new_node(new RawAbstractTreeDecomposition);
                                    new_node->bag = node->bag;
                                    if(it->second<itr->second){
                                        new_node->type= "IntroEdge_"+to_string(it->second)+"_"+to_string(itr->second);
                                        new_node->bag.set_edge(it->second,itr->second);
                                    }else{
                                        new_node->type= "IntroEdge_"+to_string(itr->second)+"_"+to_string(it->second);
                                        new_node->bag.set_edge(itr->second,it->second);
                                    }
                                    generated_nodes.push_back(new_node);
                                }
                            }
                        } 
                }
            }
            for(size_t i=0 ; i<generated_nodes.size();i++){
                if(i==0){
                    if(!node->parent){
                        root = generated_nodes[0];
                    }else{
                        generated_nodes[0]->parent = node->parent;
                        for(size_t t=0 ;t<node->parent->children.size(); t++){
                                    if(node->parent->children[t]==node){
                                        node->parent->children[t]=generated_nodes[0];
                                        break;
                                    }
                                }
                            }
                        }else{
                            generated_nodes[i]->parent = generated_nodes[i-1];
                            generated_nodes[i-1]->children.push_back(generated_nodes[i]);
                        }
                    }
                    if(generated_nodes.size()>0){
                        generated_nodes[generated_nodes.size()-1]->children.push_back(node);
                        node->parent = generated_nodes[generated_nodes.size()-1];
                    }
                    return addIntroEdge(node->children[0],visited_edges);
                    
                }
    }else if(strstr(node->type.c_str(),"Join")){
        return addIntroEdge(node->children[0],visited_edges) and addIntroEdge(node->children[1],visited_edges);

        
    }else if(strstr(node->type.c_str(),"ForgetVertex")){
       return addIntroEdge(node->children[0],visited_edges);

    }else if(strstr(node->type.c_str(),"Empty")){

        return true;
    }else{

        cout<<"ERROR in TreeDecompositionPACE::addIntroEdgeRecursion, node type is not satisfied"<<endl;
        exit(20);   
    }

}
bool TreeDecompositionPACE::colorNode(shared_ptr<RawAbstractTreeDecomposition> node, vector<unsigned> &color_vertex, vector<unsigned> &vertex_color){
    if(!node->parent){
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
    for(size_t i=0; i<node->children.size();i++){
        colorNode(node->children[i], color_vertex, vertex_color);
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
    for(size_t i=0; i<node->children.size();i++){
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
}

void TreeDecompositionPACE::createCTDNode(shared_ptr<CTDNodeNew> cnode, shared_ptr<RawAbstractTreeDecomposition> rnode) {
    cnode->set_nodeType(rnode->type);
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
    bag.set_elements(bag_elements);
    bag.set_edge(e_new.first,e_new.second);
    cnode->set_B(bag);
    if(strstr(rnode->type.c_str(),"IntroEdge_")){
        string type = "IntroEdge_"+to_string(e_new.first)+"_"+to_string(e_new.second);
        cnode->set_nodeType(type);
    }else if(strstr(rnode->type.c_str(),"IntroVertex_")){
        set<unsigned> set_diff;
        set<unsigned> elements_node = rnode->bag.get_elements();
        set<unsigned> elements_node_child = rnode->children[0]->bag.get_elements();
        set_difference( elements_node.begin(), elements_node.end(),elements_node_child.begin(), elements_node_child.end(),inserter(set_diff, set_diff.begin()));
        if(set_diff.size()!=1){
            cout<<"ERORR in TreeDecompositionPACE::createCTDNode, set_diff is not valid"<<endl;
            exit(20);
        }
        cnode->set_nodeType("IntroVertex_"+to_string(*set_diff.begin()));
    }else if(strstr(rnode->type.c_str(),"ForgetVertex_")){
        set<unsigned> set_diff;
        set<unsigned> elements_node = rnode->bag.get_elements();
        set<unsigned> elements_node_child = rnode->children[0]->bag.get_elements();
        set_difference(elements_node_child.begin(),elements_node_child.end(),elements_node.begin(), elements_node.end(),inserter(set_diff, set_diff.begin()));
        if(set_diff.size()!=1){
            cout<<"ERORR in TreeDecompositionPACE::createCTDNode, set_diff is not valid"<<endl;
            exit(20);
        }
        cnode->set_nodeType("ForgetVertex_"+to_string(*set_diff.begin()));
    }


    vector<shared_ptr<CTDNodeNew>> children;
    for(size_t i=0 ; i<rnode->children.size(); i++){
        shared_ptr<CTDNodeNew> ctdnode(new CTDNodeNew);
        createCTDNode(ctdnode, rnode->children[i]);
        children.push_back(ctdnode);
        ctdnode->set_parent(cnode);
    }
    cnode->set_children(children);
}

shared_ptr<ConcreteTreeDecomposition> TreeDecompositionPACE::convertToConcreteTreeDecomposition() {
    shared_ptr<ConcreteTreeDecomposition> concreteTreeDecomposition(new ConcreteTreeDecomposition);
    shared_ptr<CTDNodeNew> ctd_root(new CTDNodeNew);
    concreteTreeDecomposition->root = ctd_root;
    createCTDNode(ctd_root, root);
    return concreteTreeDecomposition;
}
