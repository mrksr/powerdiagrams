#ifndef POWERDIAGRAMDUAL_H
#define POWERDIAGRAMDUAL_H

#include "PowerDiagram.hpp"
#include "ConvexHullAlgorithm.hpp"
#include "IncidenceLattice.hpp"
#include <Eigen/Dense>
#include <tuple>
#include <vector>

class PowerDiagramDual {
    public:
        typedef PowerDiagram::Sphere_t Sphere_t;

        PowerDiagramDual() { }
        virtual ~PowerDiagramDual() { }

        virtual IncidenceLattice<Eigen::VectorXd> fromSpheres(ConvexHullAlgorithm& hull, const std::vector<PowerDiagram::Sphere_t>& spheres);

    private:
};

#endif
