#include "PowerDiagramNaive.hpp"

#include "AllChoices.hpp"

#include <gflags/gflags.h>
#include <iostream>
#include <unordered_map>

DECLARE_bool(verbose);

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Hyperplane_t = std::tuple<VectorXd, double>;
using Sphere_t = PowerDiagram::Sphere_t;

/**
 * @brief For a group of spheres check whether they form a 0-face.
 *
 * @param spheres Vector of all spheres.
 * @param group The indices of the current group.
 *
 * @return A pair of a boolean signifying whether there is a 0-face and the
 * position if it exists.
 */
static std::pair<bool, VectorXd> possible0Face(
        const std::vector<Sphere_t>& spheres,
        const std::vector<size_t>& group)
{
    // Find all chordales needed to define the 0-face
    std::vector<std::vector<size_t>> pairs;
    for (size_t i = 1; i < group.size(); ++i) {
        pairs.push_back({0, i});
    }

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

    // Find the (possibly not existing) intersection of the chordales
    MatrixXd A(planes.size(), std::get<0>(planes[0]).size());
    VectorXd b(planes.size());

    for (size_t i = 0; i < planes.size(); ++i) {
        A.row(i) = std::get<0>(planes[i]);
        b[i] = std::get<1>(planes[i]);
    }

    const VectorXd result = A.fullPivLu().solve(b);
    //Check if there actually is an intersection
    //FIXME: This might cause numerical issues.
    const bool hasSolution = (A * result).isApprox(b);

    return std::make_pair(hasSolution, result);
}

/**
 * @brief For a group of spheres and a possible 0-face location, check if it is
 * actually part of the power diagram.
 *
 * @param spheres Vector of all spheres.
 * @param group The indices of the current group.
 * @param point Location of the candidate 0-face.
 *
 * @return True if there is no sphere with lower power than the ones in group.
 */
static bool is0Face(
        const std::vector<Sphere_t>& spheres,
        const std::vector<size_t>& group,
        const VectorXd& point)
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

    // Find all possible groups which might form a 0-face
    std::vector<std::vector<size_t>> groups;
    AllChoices::indexGroupsOfLength<size_t>(
            dim + 1,
            spheres.begin(),
            spheres.end(),
            std::back_inserter(groups));

    IncidenceLattice<VectorXd> lattice;
    std::unordered_map<size_t, decltype(lattice)::Key_t> vertexMap;

    // For all the groups, check if they actually form a 0-face
    for (auto& group : groups) {
        bool hasSolution;
        VectorXd point;
        std::tie(hasSolution, point) = possible0Face(spheres, group);

        if (hasSolution) {
            const auto validFace = is0Face(spheres, group, point);

            if (validFace) {
                // Add the 0-face to the lattice
                if (FLAGS_verbose) {
                    std::cerr << "0-Face at: " << point.transpose() << std::endl;
                }

                decltype(lattice)::Keys_t vertices;
                for (auto& index : group) {
                    if (vertexMap.count(index) <= 0) {
                        vertexMap[index] = lattice.addMinimal(std::get<0>(spheres[index]));
                    }

                    vertices.insert(vertexMap[index]);
                }

                lattice.value(lattice.addMaximalFace(vertices)) = point;
            }
        }
    }

    return lattice;
}
