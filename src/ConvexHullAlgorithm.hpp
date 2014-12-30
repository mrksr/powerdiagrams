#ifndef CONVEXHULLALGORITHM_H
#define CONVEXHULLALGORITHM_H

#include "IncidenceLattice.h"
#include <Eigen/Dense>
#include <vector>

class ConvexHullAlgorithm {
    public:
        ConvexHullAlgorithm() { }
        virtual ~ConvexHullAlgorithm() { }

        /**
         * @brief D-Dimensional Convex Hull calculation.
         * Calculates the d-dimensional convex hull of the points provided. The
         * incidence lattice returned must contain exactly the facetts and the
         * points of the convex hull.
         *
         * @return A shallow incidence lattice of the convex hull.
         */
        virtual IncidenceLattice<Eigen::VectorXd> hullOf(const std::vector<Eigen::VectorXd>& points) = 0;

    private:
};

#endif
