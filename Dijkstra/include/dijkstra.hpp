#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <climits>
#include <algorithm> // 包含 std::find_if
#include <list>
#include <functional>

struct Node
{
    std::string name;
    std::unordered_map<Node *, double> next_neibors; //<邻居，相连的边距离>
    struct{
        Node *node = nullptr;
        double distance_to_start;
    }optimal;
    bool for_node = false;
    bool operator<(Node *second)  { // 从小到大
        return this->optimal.distance_to_start < second->optimal.distance_to_start;
    }
    Node(const std::string &name, double distance_to_start = std::numeric_limits<double>::max()) : name(name) {
        optimal.distance_to_start = distance_to_start;
    }
};

class PathPlanner{
public:
static void Dijkstra(Node *start, Node *end)
{
    std::priority_queue<Node *, std::vector<Node *>> for_nodes, temp_for_nodes;
    std::unordered_map<Node *, double> optimal_nodes;
    // std::vector<Node &> temp_for_nodes;

    // step1:
    start->optimal.distance_to_start = 0;
    for_nodes.emplace(start);

    while (!for_nodes.empty())
    {
        // step2
        while (!for_nodes.empty()){
            for (auto &neibor : for_nodes.top()->next_neibors){
                auto this_distance_to_start = for_nodes.top()->optimal.distance_to_start + neibor.second;
                if(this_distance_to_start < neibor.first->optimal.distance_to_start){
                    neibor.first->optimal.distance_to_start = this_distance_to_start;
                    neibor.first->optimal.node = for_nodes.top();  //更新邻居们的历史最短距离和上一个节点
                }
                if(neibor.first->for_node==false){  //没有被遍历过，就加入下一轮遍历
                    temp_for_nodes.emplace(neibor.first);
                }  
            }
            for_nodes.top()->for_node = true;
            for_nodes.pop();
        }
        //step3
        while(!temp_for_nodes.empty()){
            for_nodes.emplace(temp_for_nodes.top());
            temp_for_nodes.pop();
        }
    }
}
}; //class PathPlanner

#endif
