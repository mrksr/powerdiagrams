#ifndef CONVEXHULLQHULL_H
#define CONVEXHULLQHULL_H

#include "ConvexHullAlgorithm.h"
#include "IncidenceLattice.h"
#include <Eigen/Dense>
#include <vector>

class ConvexHullQhull : public ConvexHullAlgorithm {
    public:
        ConvexHullQhull() { };
        virtual ~ConvexHullQhull() { }


        virtual IncidenceLattice<Eigen::VectorXd> hullOf(const std::vector<Eigen::VectorXd>& points);
    private:
};

#endif
