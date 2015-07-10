#ifndef POWERDIAGRAMNAIVE_H
#define POWERDIAGRAMNAIVE_H

#include "PowerDiagram.hpp"

class PowerDiagramNaive : public PowerDiagram {
    public:
        PowerDiagramNaive() { }
        virtual ~PowerDiagramNaive() { }

        virtual IncidenceLattice<Eigen::VectorXd> fromSpheres(const std::vector<PowerDiagram::Sphere_t>& spheres);

    private:
};

#endif
