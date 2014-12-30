#include "ConvexHullQhull.hpp"
#include "FromCSV.hpp"
#include "PowerDiagramDual.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <vector>
#include <tuple>
#include <string>

#define _VERBOSE_

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

        ConvexHullQhull conv;
        PowerDiagramDual dual;
        auto diagram = dual.fromSpheres(conv, spheres);
        std::cout << "Number of maximal nodes: " << diagram.maximals().size() << std::endl;

        return 0;
    }
}
