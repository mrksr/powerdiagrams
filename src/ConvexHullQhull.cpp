#include "ConvexHullQhull.h"
#include <libqhullcpp/Qhull.h>
#include <iostream>

IncidenceLattice<Eigen::VectorXd> ConvexHullQhull::hullOf(const std::vector<Eigen::VectorXd>& points)
{
    const size_t dimension = points[0].size();

    // FIXME: Copy might not be necessary if we use a map<>?
    // Copy the points to qhull format
    double* qhullpoints = new double[dimension * points.size()];

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = 0; j < dimension; ++j) {
            qhullpoints[i * dimension + j] = points[i][j];
        }
    }
    orgQhull::Qhull qhull;
    qhull.setErrorStream(&std::cerr);
    qhull.setOutputStream(&std::cout);
    qhull.useOutputStream = true;
    qhull.runQhull("", dimension, points.size(), qhullpoints, "n");

    delete[] qhullpoints;

    IncidenceLattice<Eigen::VectorXd> lattice(Eigen::VectorXd::Zero(1));
    return lattice;
}
