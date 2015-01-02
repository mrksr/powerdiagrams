#ifndef POWERDIAGRAM_H
#define POWERDIAGRAM_H

#include "ConvexHullAlgorithm.hpp"
#include "IncidenceLattice.hpp"
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
        static double power(const Sphere_t& sphere, const Eigen::VectorXd& point)
        {
            Eigen::VectorXd center;
            double radius;
            std::tie(center, radius) = sphere;

            const auto diff = center - point;
            return diff.dot(diff) - radius * radius;
        }

        virtual IncidenceLattice<Eigen::VectorXd> fromSpheres(const std::vector<Sphere_t>& spheres) = 0;

    protected:
        PowerDiagram() { }
};

#endif
