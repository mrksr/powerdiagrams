#include "PowerDiagram.h"
#include <Eigen/Dense>
#include <iostream>
#include <vector>
#include <tuple>
#include <string>

int main(int argc, char *argv[])
{
    {
        std::cout << "Some Graph Tests:" << std::endl;
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

        for (auto& item : graph.minimalElements()) {
            std::cout << "Minimal: " << item << std::endl;
        }
        for (auto& item : graph.maximalElements()) {
            std::cout << "Maximal: " << item << std::endl;
        }
    }

    std::cout << std::endl << std::endl;

    {
        std::vector<PowerDiagram::Sphere_t> spheres {
            PowerDiagram::sphere(Eigen::Vector3d(1, 0, 0), 2),
            PowerDiagram::sphere(Eigen::Vector3d(0, 0, 1), 1),
            PowerDiagram::sphere(Eigen::Vector3d(1, 0, 1), 1),
            PowerDiagram::sphere(Eigen::Vector3d(0, 0, 2), 1),
            PowerDiagram::sphere(Eigen::Vector3d(2, 0, 0), 1)
        };
        PowerDiagram::fromSpheres(spheres);
    }
    return 0;
}
