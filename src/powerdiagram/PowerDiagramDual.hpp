#ifndef POWERDIAGRAMDUAL_H
#define POWERDIAGRAMDUAL_H

#include "ConvexHullAlgorithm.hpp"
#include "IncidenceLattice.hpp"
#include "PowerDiagram.hpp"

#include <Eigen/Dense>
#include <tuple>
#include <vector>

class PowerDiagramDual : public PowerDiagram {
    public:
        PowerDiagramDual(ConvexHullAlgorithm& hull) : hull_(hull) { }
        virtual ~PowerDiagramDual() { }

        virtual IncidenceLattice<Eigen::VectorXd> fromSpheres(const std::vector<PowerDiagram::Sphere_t>& spheres);

    private:
        ConvexHullAlgorithm& hull_;
};

#endif
