#include "PowerDiagramDual.hpp"

#include <gflags/gflags.h>
#include <iostream>
#include <vector>

DECLARE_bool(verbose);

using Eigen::MatrixXd;
using Eigen::VectorXd;

static VectorXd normalToAffineSpace(const std::vector<VectorXd>& vectors)
{
    MatrixXd A(vectors.size() - 1, vectors[0].size());
    for (size_t i = 1; i < vectors.size(); ++i) {
        A.row(i - 1) = vectors[i] - vectors[0];
    }

    return A.fullPivLu().kernel().col(0).normalized();
}

/**
 * @brief Calculate a normal which, in relation to the convex hull of the
 * vertices, faces outwards.
 * Since we assume the polytope to be fully dimensional, there has
 * to exist some point not in the facet. The dot product of the
 * normal and this point has to be smaller than zero if the normal
 * points outwards.
 *
 * @param normal Normal vector of some facet which might point inwards.
 * @param vertexOnFacet A vertex guaranteed to be on the facet.
 * @param vertices All vertices.
 *
 * @return Either normal or (-1) * normal, whichever points outwards.
 */
static VectorXd outwardsNormal(
        const VectorXd& normal,
        const VectorXd& vertexOnFacet,
        const std::vector<VectorXd>& vertices)
{
    // FIXME: This might cause numerical issues.
    const double epsilon = 1e-3;
    for (auto& vertex : vertices) {
        const auto dot = normal.dot(vertex - vertexOnFacet);
        if (dot > epsilon) {
            return (-1) * normal;
        } else if (dot < -epsilon) {
            return normal;
        }
    }

    // This should only happen if the hull is not fully dimensional.
    return normal;
}

static VectorXd polarOfSphere(const PowerDiagram::Sphere_t& sphere)
{
    VectorXd center;
    double radius;
    std::tie(center, radius) = sphere;

    VectorXd res(center.size() + 1);
    res << center, center.dot(center) - radius * radius;
    return res;
}

static VectorXd polarOfHyperplane(const VectorXd& normal, double offset)
{
    const auto last = normal.size() - 1;

    VectorXd res = normal;
    res *= 0.5;
    res[last] = -offset;
    res *= -1.0 / normal[last];

    return res;
}

IncidenceLattice<VectorXd> PowerDiagramDual::fromSpheres(const std::vector<Sphere_t>& spheres)
{
    const auto dimension = std::get<0>(spheres[0]).size();

    // Find polars
    std::vector<VectorXd> polars(spheres.size());
    std::transform(spheres.begin(), spheres.end(), polars.begin(), [](Sphere_t sphere) {
            return polarOfSphere(sphere);
        });

    // Calculate their convex hull
    auto dualIncidences = hull_.hullOf(polars);
    using Keys_t = typename decltype(dualIncidences)::Keys_t;
    using Key_t = typename decltype(dualIncidences)::Key_t;

    // Calculate normals of the hyperplanes (facets),
    // Restrict the incidence lattice to the facets on the bottom side
    Keys_t bottoms;
    for (auto& facet : dualIncidences.maximals()) {
        std::vector<VectorXd> facetPoints;
        for (auto& key : dualIncidences.minimalsOf(facet)) {
            facetPoints.push_back(dualIncidences.value(key));
        }

        // Find any normal
        auto normal = normalToAffineSpace(facetPoints);

        // Make sure the normal points outwards
        normal = outwardsNormal(normal, facetPoints[0], polars);

        if (FLAGS_verbose) {
          std::cerr << "Normal: " << normal.transpose();
        }

        // Save normal in incidence lattice
        dualIncidences.value(facet) = normal;

        if (normal[dimension] < 0) {
            bottoms.insert(facet);
            if (FLAGS_verbose) {
              std::cerr << " Is bottom!";
            }
        }
        if (FLAGS_verbose) {
          std::cerr << std::endl;
        }
    }
    dualIncidences.restrictToMaximals(bottoms);

    // Calculate the dual (i.e. project the hyperplanes),
    // project the dual points onto H0 (i.e. forget last coordinate)
    for (auto& facet : dualIncidences.maximals()) {
        const auto normal = dualIncidences.value(facet);
        const auto vertex = *dualIncidences.minimalsOf(facet).begin();
        const double offset = normal.dot(dualIncidences.value(vertex));

        const auto polar = polarOfHyperplane(normal, offset);
        dualIncidences.value(facet) = polar.head(dimension);

        if (FLAGS_verbose) {
            std::cerr << "0-Face at: " << polar.head(dimension).transpose() << std::endl;
        }
    }

    // Project Sphere centers back to the original space from the polar points.
    // FIXME(mrksr): We recover the radii a bit clumsily here by comparing vectors.
    for (auto& sphere : dualIncidences.minimals()) {
        auto& polar = dualIncidences.value(sphere);

        const auto it = std::find(polars.begin(), polars.end(), polar);
        assert(it != polars.end());
        const auto idx = std::distance(polars.begin(), it);

        // To make it possible to recover the radius, we add it as the (d+1)st
        // value into the sphere.
        polar[dimension] = std::get<1>(spheres.at(idx));
    }

    // Find directions of all the edges. If the edge is an extremal one, we
    // find the "correct" direction starting from the existing 0-face.
    // We call the maximals "point" here since we have dualized them before
    if (dimension > 1) {
        std::unordered_set<Key_t> visitedEdges;

        for (auto& point : dualIncidences.maximals()) {
            for (auto& edge : dualIncidences.predecessors(point)) {
                // If an "edge" is minimal, there is an edge missing.
                assert(!dualIncidences.isMinimal(edge) && "There is probably an edge missing.");

                if (visitedEdges.find(edge) == visitedEdges.end()) {
                    visitedEdges.insert(edge);

                    std::vector<VectorXd> spheres;
                    for (auto& sphere : dualIncidences.minimalsOf(edge)) {
                        spheres.push_back(dualIncidences.value(sphere).head(dimension));
                    }

                    auto direction = normalToAffineSpace(spheres);

                    if (dualIncidences.successors(edge).size() == 1) {
                        // This is an extremal edge, so we care about the sign
                        // of the direction.
                        // We find the direction by comparing the power on the
                        // edge to the power of the additional sphere which
                        // defines the point "point" the edge is also adjacent
                        // to. The correct direction is then the one facing
                        // "away" (in terms of power) from this sphere.

                        const auto& minimalsOfPoint = dualIncidences.minimalsOf(point);
                        const auto& minimalsOfEdge = dualIncidences.minimalsOf(edge);
                        Keys_t candidates;
                        std::set_difference(
                                minimalsOfPoint.begin(),
                                minimalsOfPoint.end(),
                                minimalsOfEdge.begin(),
                                minimalsOfEdge.end(),
                                std::inserter(candidates, candidates.begin())
                                );

                        const auto testPoint = dualIncidences.value(point) + direction;

                        const auto& activeSphere = dualIncidences.value(*minimalsOfEdge.begin());
                        const auto activePower = PowerDiagram::power(
                                PowerDiagram::sphere(
                                    activeSphere.head(dimension),
                                    activeSphere[dimension]
                                    ),
                                testPoint);

                        const auto& inactiveSphere = dualIncidences.value(*candidates.begin());
                        const auto inactivePower = PowerDiagram::power(
                                PowerDiagram::sphere(
                                    inactiveSphere.head(dimension),
                                    inactiveSphere[dimension]
                                    ),
                                testPoint);

                        if (activePower > inactivePower) {
                            direction *= (-1);
                        }
                    }

                    dualIncidences.value(edge) = direction;
                }
            }
        }
    }

    return dualIncidences;
}
