#ifndef POWERDIAGRAM_H
#define POWERDIAGRAM_H

#include "ConvexHullAlgorithm.h"
#include "IncidenceLattice.h"
#include <Eigen/Dense>
#include <tuple>
#include <vector>

class PowerDiagram {
    public:
        typedef std::tuple<Eigen::VectorXd, double> Sphere_t;

        virtual ~PowerDiagram() { }

        static Sphere_t sphere(const Eigen::VectorXd& center, double radius)
        {
            return std::make_tuple(center, radius);
        }
        static IncidenceLattice<Eigen::VectorXd> fromSpheres(ConvexHullAlgorithm& hull, const std::vector<Sphere_t>& spheres);

    private:
        PowerDiagram();
};

#endif
