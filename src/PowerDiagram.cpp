#include "PowerDiagram.h"

#include <vector>
#include <iostream>

typedef Eigen::VectorXd VectorXd;
typedef Eigen::MatrixXd MatrixXd;

// FIXME: This vector is a fairly useless temporary object
static VectorXd normal(const std::vector<VectorXd>& vectors)
{
    MatrixXd m(vectors.size(), vectors[0].rows());
    for (size_t i = 0; i < vectors.size(); ++i) {
        m.row(i) = vectors[i];
    }

    return m.fullPivLu().kernel().col(0).normalized();
}

static VectorXd polarOfSphere(const PowerDiagram::Sphere_t& sphere)
{
    VectorXd center;
    double radius;
    std::tie(center, radius) = sphere;

    VectorXd res(center.rows() + 1);
    res << center, center.dot(center) - radius * radius;
    return res;
}

static VectorXd polarOfHyperplane(const VectorXd& normal, double offset)
{
    const auto last = normal.rows() - 1;

    VectorXd res = normal;
    res *= 0.5;
    res[last] = - offset;
    res *= -1.0 / normal[last];

    return res;
}

IncidenceLattice<VectorXd> PowerDiagram::fromSpheres(const std::vector<Sphere_t>& spheres)
{
    const auto dimension = std::get<0>(spheres[0]).rows();

    std::cout << "Spheres:" << std::endl;
    for (auto& item : spheres) {
        std::cout << "Center: " << std::get<0>(item).transpose() << " - Radius: " << std::get<1>(item) << std::endl;
    }

    std::vector<VectorXd> vecs(spheres.size());
    std::transform(spheres.begin(), spheres.end(), vecs.begin(), [](Sphere_t tup) { return std::get<0>(tup); });
    auto n = normal(vecs);
    std::cout << "Normal to all sphere centers:" << std::endl << n << std::endl;

    std::cout << "Polar of first Sphere:" << std::endl << polarOfSphere(spheres[0]) << std::endl;
    std::cout << "Polar of Plain with Normal (0,1,2) through 1:" << std::endl << polarOfHyperplane(Eigen::Vector3d(0, 1, 2), 1) << std::endl;

    return IncidenceLattice<VectorXd>(VectorXd::Zero(dimension));
}
