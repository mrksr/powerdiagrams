#ifndef CONVEXHULLALGORITHM_H
#define CONVEXHULLALGORITHM_H

#include "IncidenceLattice.hpp"

#include <Eigen/Dense>
#include <vector>

class ConvexHullAlgorithm {
    public:
        ConvexHullAlgorithm() { }
        virtual ~ConvexHullAlgorithm() { }

        /**
         * @brief D-Dimensional Convex Hull calculation.
         * Calculates the d-dimensional convex hull of the points provided. The
         * incidence lattice returned should at least contain (d-2)-faces
         * besides 0-faces (vertices) and (d-1)-faces (facets).
         *
         * @return An incidence lattice of the convex hull.
         */
        virtual IncidenceLattice<Eigen::VectorXd> hullOf(const std::vector<Eigen::VectorXd>& points) = 0;

    private:
};

#endif
