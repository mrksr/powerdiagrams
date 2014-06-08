#include <BidirectionalGraph.h>
#include <Eigen/Dense>
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
    BidirectionalGraph<std::string, int> graph;

    graph.insertNode("wurst", 5);
    graph.insertNode("salat", 10);
    graph.insertNode("soße", 15);
    graph.insertEdge("wurst", "salat");
    graph.insertEdge("salat", "soße");
    auto succs = graph.allSuccessors("wurst");
    for (auto& item : succs) {
        std::cout << "Successor: " << item << std::endl;
    }
    for (auto& item : graph.allPredecessors("soße")) {
        std::cout << "Predecessor: " << item << std::endl;
    }
    std::cout << "Heyo!" << std::endl;
    return 0;
}
