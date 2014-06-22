#ifndef CONVEXHULLQHULL_H
#define CONVEXHULLQHULL_H

#include "ConvexHullAlgorithm.h"
#include <Eigen/Dense>

class ConvexHullQhull : public ConvexHullAlgorithm {
    public:
        ConvexHullQhull() { };
        virtual ~ConvexHullQhull() { }


        virtual IncidenceLattice<Eigen::VectorXd> hullOf(const std::vector<Eigen::VectorXd>& points) {
            return IncidenceLattice<Eigen::VectorXd>(Eigen::VectorXd::Zero(1));
        }
    private:
};

#endif
