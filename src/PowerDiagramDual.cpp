#include "PowerDiagramDual.hpp"
#include <iostream>
#include <map>
#include <vector>

typedef Eigen::VectorXd VectorXd;
typedef Eigen::MatrixXd MatrixXd;

// FIXME: This vector is a fairly useless temporary object
static VectorXd normalToAffineSpace(const std::vector<VectorXd>& vectors)
{
    MatrixXd A(vectors.size() - 1, vectors[0].size());
    for (size_t i = 1; i < vectors.size(); ++i) {
        A.row(i - 1) = vectors[i] - vectors[0];
    }

    return A.fullPivLu().kernel().col(0).normalized();
}

// Since we assume the polytope to be fully dimensional, there has
// to exist some point not in the facet. The dot product of the
// normal and this point has to be smaller than zero if the normal
// points outwards.
static VectorXd outwardsNormal(const VectorXd& normal, const VectorXd& vertexOnFacet, const std::vector<VectorXd>& vertices)
{
    // FIXME: This might cause numerical issues.
    const double epsilon = 1e-3;
    for (auto& vertex : vertices) {
        const auto dot = normal.dot(vertex - vertexOnFacet);
        if (dot > epsilon) {
            return (-1) * normal;
        } else if (dot < epsilon) {
            return normal;
        }
    }

    // This should not happen
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

#ifdef _VERBOSE_
    for (auto& polar : polars) {
        std::cout << "Polar point: " << polar.transpose() << std::endl;
    }
#endif

    // Calculate their convex hull
    auto dualIncidences = hull_.hullOf(polars);

    // Calculate normals of the hyperplanes (facets),
    // Restrict the incidence lattice to the facets on the bottom side
    std::set<decltype(dualIncidences)::Key_t> bottoms;
    for (auto& facet : dualIncidences.maximals()) {
        std::vector<VectorXd> facetPoints;
        for (auto& key : dualIncidences.minimalsOf(facet)) {
            facetPoints.push_back(dualIncidences.value(key));
        }

        // Find any normal
        auto normal = normalToAffineSpace(facetPoints);

        // Make sure the normal points outwards
        normal = outwardsNormal(normal, facetPoints[0], polars);

        // Save normal in incidence lattice
        dualIncidences.value(facet) = normal;

        if (normal[dimension - 1] < 0) {
            bottoms.insert(facet);
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
        dualIncidences.value(facet, polar.head(dimension));
    }

    return dualIncidences;
}
