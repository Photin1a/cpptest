#include <dijkstra.hpp>

int main()
{
    std::vector<Node *> nodes;
    nodes.emplace_back(new Node("V1"));
    nodes.emplace_back(new Node("V2"));
    nodes.emplace_back(new Node("V3"));
    nodes.emplace_back(new Node("V4"));
    nodes.emplace_back(new Node("V5"));
    nodes.emplace_back(new Node("V6"));

    nodes[0]->next_neibors.emplace(nodes[2], 10);
    nodes[0]->next_neibors.emplace(nodes[4], 30);
    nodes[0]->next_neibors.emplace(nodes[5], 100);

    nodes[1]->next_neibors.emplace(nodes[2], 5);

    nodes[2]->next_neibors.emplace(nodes[3], 50);

    nodes[3]->next_neibors.emplace(nodes[5], 10);

    nodes[4]->next_neibors.emplace(nodes[3], 20);
    nodes[4]->next_neibors.emplace(nodes[5], 60);

    PathPlanner::Dijkstra(nodes[0], nodes[6]);

    for(auto node:nodes){
        if(node->optimal.node==nullptr)std::cout<<"====>"<<node->name<<std::endl;
        else std::cout<<node->optimal.node->name<<"====>"<<node->name<<"==="<<node->optimal.distance_to_start<<std::endl;
    }
    return 0;
}
