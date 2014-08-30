#include "ConvexHullQhull.h"
#include "FromCSV.h"
#include "PowerDiagram.h"
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

        ConvexHullQhull conv;
        PowerDiagram::fromSpheres(conv, spheres);

        return 0;
    }
}
