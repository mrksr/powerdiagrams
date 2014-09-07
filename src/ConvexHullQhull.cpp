#include "ConvexHullQhull.h"
#include <iostream>
#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/QhullError.h>
#include <libqhullcpp/QhullFacet.h>
#include <libqhullcpp/QhullFacetList.h>
#include <libqhullcpp/QhullLinkedList.h>
#include <libqhullcpp/QhullQh.h>
#include <libqhullcpp/QhullVertex.h>
#include <libqhullcpp/QhullVertexSet.h>
#include <libqhullcpp/RboxPoints.h>
#include <map>
#include <set>

typedef int qhullID_t;

IncidenceLattice<Eigen::VectorXd> ConvexHullQhull::hullOf(const std::vector<Eigen::VectorXd>& points)
{
    const size_t dimension = points[0].size();

    // FIXME: Copy might not be necessary if we use a map<>?
    // Copy the points to qhull format
    coordT* qhullpoints = new coordT[dimension * points.size()];

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = 0; j < dimension; ++j) {
            qhullpoints[i * dimension + j] = points[i][j];
        }
    }

    orgQhull::Qhull qhull;
#ifdef _VERBOSE_
    qhull.setErrorStream(&std::cerr);
    qhull.setOutputStream(&std::cout);
    qhull.useOutputStream = true;
#endif

    // Find convex hull
    qhull.runQhull("", dimension, points.size(), qhullpoints, "s");

#ifdef _VERBOSE_
    std::cout << qhull.qhullStatus() << std::endl;
    std::cout << "Vertices (" << qhull.vertexCount() << "): " << std::endl << qhull.vertexList();
    std::cout << "Facets (" << qhull.facetCount() << "): " << std::endl << qhull.facetList();
#endif

    // Create incidence lattice
    IncidenceLattice<Eigen::VectorXd> lattice(Eigen::VectorXd::Zero(dimension));
    std::map<qhullID_t, decltype(lattice)::Key_t> vertexMap;

    for (auto& facet : qhull.facetList().toStdVector()) {
        std::set<decltype(lattice)::Key_t> vertices;

        for (auto& vertex : facet.vertices()) {
            const auto id = vertex.point().id(qhull.runId());
            if (vertexMap.count(id) <= 0) {
                vertexMap[id] = lattice.addMinimal(points[id]);
            }

            vertices.insert(vertexMap[id]);
        }

        lattice.addFace(vertices);
    }

    delete[] qhullpoints;
    return lattice;
}
