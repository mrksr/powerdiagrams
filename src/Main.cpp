#include "ConvexHullQhull.hpp"
#include "FromCSV.hpp"
#include "PowerDiagramDual.hpp"
#include "PowerDiagramNaive.hpp"
#include "IncidenceLattice.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <vector>
#include <tuple>
#include <string>

void printUsage()
{
    std::cout << "Usage: ./powerdiagrams <centers> <radii>" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printUsage();

        return 1;
    } else {
        const auto& spheres = FromCSV::spheres(argv[1], argv[2]);

#ifdef _VERBOSE_
        std::cout << "Spheres:" << std::endl;
        for (auto& item : spheres) {
            std::cout
                << "Center: "
                << std::get<0>(item).transpose()
                << " - Radius: "
                << std::get<1>(item)
                << std::endl;
        }
        std::cout << std::endl << std::endl;
#endif

        std::cout << "Dual algorithm:" << std::endl;
        {
            ConvexHullQhull conv;
            PowerDiagramDual dual(conv);
            auto diagram = dual.fromSpheres(spheres);
            std::cout << "Number of minimal nodes: " << diagram.minimals().size() << std::endl;
            for (auto& minimal : diagram.minimals()) {
                std::cout << "Minimal: " << diagram.value(minimal).transpose() << std::endl;
            }
            std::cout << "Number of maximal nodes: " << diagram.maximals().size() << std::endl;
            for (auto& maximal : diagram.maximals()) {
                std::cout << "Maximal: " << diagram.value(maximal).transpose() << std::endl;
                std::cout << "Direct Predecessors: " << std::endl;
                for (auto& pred : diagram.predecessors(maximal)) {
                  std::cout << "Id: " << pred << std::endl;
                  for (auto& min : diagram.minimalsOf(pred)) {
                      std::cout << "  - " << diagram.value(min).transpose() << std::endl;
                  }
                }
            }
        }

        std::cout << std::endl << std::endl;
        std::cout << "Naive algorithm:" << std::endl;
        {
            PowerDiagramNaive naive;
            auto diagram = naive.fromSpheres(spheres);
            std::cout << "Number of minimal nodes: " << diagram.minimals().size() << std::endl;
            for (auto& minimal : diagram.minimals()) {
                std::cout << "Minimal: " << diagram.value(minimal).transpose() << std::endl;
            }
            std::cout << "Number of maximal nodes: " << diagram.maximals().size() << std::endl;
            for (auto& maximal : diagram.maximals()) {
                std::cout << "Maximal: " << diagram.value(maximal).transpose() << std::endl;
            }
        }

        return 0;
    }
}
