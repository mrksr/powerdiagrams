#include "ConvexHullQhull.h"
#include "PowerDiagram.h"
#include <Eigen/Dense>
#include <iostream>
#include <vector>
#include <tuple>
#include <string>

int main(int argc, char *argv[])
{
    {
        // Qhull playground
        std::vector<Eigen::VectorXd> points {
            Eigen::Vector3d(1, 0, 0),
            Eigen::Vector3d(0, 0, 1),
            Eigen::Vector3d(1, 0, 1),
            Eigen::Vector3d(0, 0, 2),
            Eigen::Vector3d(2, 0, 0)
        };

        ConvexHullQhull conv;
        conv.hullOf(points);
    }
    /*
    {
        std::vector<PowerDiagram::Sphere_t> spheres {
            PowerDiagram::sphere(Eigen::Vector3d(1, 0, 0), 2),
            PowerDiagram::sphere(Eigen::Vector3d(0, 0, 1), 1),
            PowerDiagram::sphere(Eigen::Vector3d(1, 0, 1), 1),
            PowerDiagram::sphere(Eigen::Vector3d(0, 0, 2), 1),
            PowerDiagram::sphere(Eigen::Vector3d(2, 0, 0), 1)
        };

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
    }
    */
    return 0;
}
