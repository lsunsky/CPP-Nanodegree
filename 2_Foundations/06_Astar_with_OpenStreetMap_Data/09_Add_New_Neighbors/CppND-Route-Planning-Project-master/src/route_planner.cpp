#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {

    //input from USER is 0~100, we treat it as the percentage of the map
    //convert input to percentage
    start_x*= 0.01;
    start_y*= 0.01;
    end_x*= 0.01;
    end_y*= 0.01;

    this->start_node = &m_Model.FindClosestNode(start_x,start_y);
    this->end_node =   &m_Model.FindClosestNode(end_x,end_y);


    total_distance = 0.0;
}




/*

a.Iterate through the node parents until a node with parent
    equal to nullptr is reached - this will be the start node, which has no parent.
    Each node in the iteration should be pushed to the path_found vector.
b.To keep track of the total path distance, in each step of the iteration,
    add the distance between a node and its parent to the class distance variable.
c.Before the method returns, scale the distance by multiplying by the
    model's scale: m_Model.MetricScale().
    This is done since node coordinates are scaled down when they are stored in the model.
    They must be rescaled to get an accurate distance.
d.Return the path_found.

*/
std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node){

    std::vector<RouteModel::Node> path_found;

    RouteModel::Node tmpParentNode;

    while (current_node->parent != nullptr){

        path_found.push_back(*current_node);
        tmpParentNode = *(current_node->parent);
        this->total_distance += current_node->distance(tmpParentNode);
        //move cur_node pointer to its parent
        current_node = current_node->parent;

    }

    //push the current_node to vector, which is the start node
    path_found.push_back(*current_node);
    this->total_distance *= m_Model.MetricScale();

    return path_found;
}





/*
This time we make a STUB func for testing the env.

2.In route_planner.cpp define the AStarSearch method. The method should do the following:
    Set the parent of end_node to the start_node.
    Set m_Model.path to the result of calling ConstructFinalPath on end_node.

*/
//entry point
void RoutePlanner::AStarSearch(){

    //for testing only in this round.
    this->end_node->parent = this->start_node;
    m_Model.path = this->ConstructFinalPath(end_node);

    return ;
}


//The method should return the distance from the passed argument to the end_node.
float RoutePlanner::CalculateHValue(const RouteModel::Node* inputNode ){

    return inputNode->distance(this->end_node);
}


/*
目前open_list 還是空的, 先想像之後有會一直把node 加入 open_list

a.Sort the open_list according to the f-value, which is the sum of a node's h-value and g-value.
b.Create a copy of the pointer to the node with the lowest f-value.
c.Erase that node pointer from open_list.
d.Return the pointer copy.
*/
RouteModel::Node* RoutePlanner::NextNode(){

    //*************************************
    //sorted open list by some key value
    //sorted 手法
    //*************************************
    std::sort(open_node_list.begin(), open_node_list.end(), [](const auto &_1st, const auto &_2nd){
        // 小< 大 , 因此第一個node, f最小, 越左邊越小
        return _1st->h_value + _1st->g_value < _2nd->h_value + _2nd->g_value;
    }

    //assign 第一個node 給 minfNode, 用 front()
    RouteModel::Node node* minfNode=open_node_list.front();

    //因為 earse 需要一個 iterator, 因此放 .begin
    open_node_list.earse(open_node_list.begin())
    return minfNode;
}


/*
Pseudocode:
AddNeighbors(RouteModel::Node *current_node):

1.Call FindNeighbors() on current_node to populate the current_node's neighbors vector.
2.For each neighbor in the current_node's neighbors
    a.Set the neighbors parent to the current_node.
    b.Set the neighbor's g_value to the sum of the current_node's
        g_value plus the distance from the curent_node to the neighbor.
    c.Set the neighbor's h_value using CalculateHValue
    d.Push the neighbor to the back of the open_list.
    e.Mark the neighbor as visited.

*/
void RoutePlanner::AddNeighbors(RouteModel::Node* current_node){


    //1.Call FindNeighbors() on current_node to populate the current_node's neighbors vector.
    // help us to set std::vector<Node*> neighbors
    current_node->FindNeighbors();
    //call 完之後, neighbors 裡面有 n個離cur node最近的幾個node, 這個n 就是 與curnode 相連的 n條 road
    for(RouteModel::Node* neighbor_node : current_node->neighbors){


        //當時在  RouteModel constructor 中, 把 Model::Node 轉進 RouteModel::Node
        //只 initial 了 idx, parent_model*, Model::Node,
        //都沒有對 g,h visited ...等變數給值, 也就是說 這邊是第一次給 g,h,visitd,parent..
        neighbor_node->parent = current_node;
        neighbor_node->g_value = current_node->g_value + current_node->distance(*neighbor_node);
        neighbor_node->h_value = this->CalculateHValue(neighbor_node);
        neighbor_node->visited = true;

        //add to open node list
        this->open_node_list.push_back(neighbor_node);



    }






}
