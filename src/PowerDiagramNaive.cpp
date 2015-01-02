#include "PowerDiagramNaive.hpp"

#include "AllChoices.hpp"
#include <iostream>

typedef PowerDiagram::Sphere_t Sphere_t;
typedef Eigen::VectorXd VectorXd;
typedef Eigen::MatrixXd MatrixXd;
typedef std::tuple<VectorXd, double> Hyperplane_t;

static std::pair<bool, VectorXd> possible0Face(const std::vector<Sphere_t>& spheres, const std::vector<size_t>& group)
{
    std::vector<std::vector<size_t>> pairs;
    AllChoices::indexTuplesOfLength<size_t>(2, group.begin(), group.end(), std::back_inserter(pairs));

    std::vector<Hyperplane_t> planes(pairs.size());
    std::transform(pairs.begin(), pairs.end(), planes.begin(),
            [&spheres, &group](const std::vector<size_t>& pair) {
                VectorXd c1, c2;
                double r1, r2;
                std::tie(c1, r1) = spheres[group[pair[0]]];
                std::tie(c2, r2) = spheres[group[pair[1]]];

                const VectorXd normal = 2 * (c1 - c2);
                const double bias = r2 * r2 - r1 * r1 - c2.dot(c2) + c1.dot(c1);
                return std::make_tuple(normal, bias);
            });

    MatrixXd A(planes.size(), std::get<0>(planes[0]).size());
    VectorXd b(planes.size());

    for (size_t i = 0; i < planes.size(); ++i) {
        A.row(i) = std::get<0>(planes[i]);
        b[i] = std::get<1>(planes[i]);
    }

    const VectorXd result = A.fullPivLu().solve(b);
    //FIXME: This might cause numerical issues.
    const bool hasSolution = (A * result).isApprox(b);

    return std::make_pair(hasSolution, result);
}

static bool is0Face(const std::vector<Sphere_t>& spheres, const std::vector<size_t>& group, const VectorXd& point)
{
    std::vector<double> powers(spheres.size());
    std::transform(spheres.begin(), spheres.end(), powers.begin(),
            [&point](const Sphere_t& sphere) {
                return PowerDiagram::power(sphere, point);
            });

    const auto minimal = *std::min_element(powers.begin(), powers.end());
    const auto groupPower = PowerDiagram::power(spheres[group[0]], point);

    //FIXME: This might cause numerical issues.
    return std::abs(groupPower - minimal) <= 1e-3;
}

IncidenceLattice<VectorXd> PowerDiagramNaive::fromSpheres(const std::vector<Sphere_t>& spheres)
{
    const size_t dim = std::get<0>(spheres[0]).size();
    std::vector<std::vector<size_t>> groups;
    AllChoices::indexTuplesOfLength<size_t>(dim, spheres.begin(), spheres.end(), std::back_inserter(groups));

    /* for (auto& group : groups) { */
    /*     std::cout << "For sphere indices: "; */
    /*     for (auto& i : group) { */
    /*         std::cout << " " << i; */
    /*     } */
    /*     std::cout << std::endl; */

    /*     const auto point = possible0Face(spheres, group).second; */
    /*     std::cout << "Possible 0Face: " << point.transpose() << std::endl; */

    /*     const auto valid = is0Face(spheres, group, point); */
    /*     std::cout << "Is a 0Face: " << valid<< std::endl; */
    /* } */

    for (auto& group : groups) {
        bool hasSolution;
        VectorXd point;
        std::tie(hasSolution, point) = possible0Face(spheres, group);

        if (hasSolution) {
            const auto validFace = is0Face(spheres, group, point);

            if (validFace) {
                std::cout << "0-Face at: " << point.transpose() << std::endl;

            }
        }
    }

    return IncidenceLattice<VectorXd>(VectorXd::Zero(0));
}
