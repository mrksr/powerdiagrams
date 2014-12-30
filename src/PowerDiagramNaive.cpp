#include "PowerDiagramNaive.hpp"

typedef Eigen::VectorXd VectorXd;

IncidenceLattice<VectorXd> PowerDiagramNaive::fromSpheres(const std::vector<Sphere_t>& spheres)
{
    return IncidenceLattice<VectorXd>(VectorXd::Zero(0));
}
