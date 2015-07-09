#include "ConvexHullQhull.hpp"

#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/QhullFacet.h>
#include <libqhullcpp/QhullFacetList.h>
#include <libqhullcpp/QhullVertex.h>
#include <libqhullcpp/QhullRidge.h>
#include <unordered_map>
#include <vector>

#ifdef _VERBOSE_
  #include <iostream>
#endif

typedef int qhullID_t;
typedef Eigen::VectorXd VectorXd;

IncidenceLattice<VectorXd> ConvexHullQhull::hullOf(const std::vector<VectorXd>& points)
{
    const size_t dimension = points[0].size();

    // FIXME: Copy might not be necessary if we use an Eigen::map<>?
    // Copy the points to qhull format
    std::vector<coordT> qhullpoints;
    qhullpoints.reserve(dimension * points.size());

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
    qhull.runQhull("", dimension, points.size(), &qhullpoints[0], "f i s");

#ifdef _VERBOSE_
    std::cout << "Final Status: " << qhull.qhullStatus() << std::endl;
    std::cout << "Vertices (" << qhull.vertexCount() << ") " << std::endl;
    std::cout << "Facets (" << qhull.facetCount() << "): " << std::endl << qhull.facetList();
#endif

    // Create incidence lattice
    IncidenceLattice<VectorXd> lattice(VectorXd::Zero(dimension));
    std::unordered_map<qhullID_t, decltype(lattice)::Key_t> vertexMap;

#ifdef _VERBOSE_
    int i = 0;
#endif
    decltype(lattice)::Keys_t vertices;
    // Bookkeeping to ensure we visit every ridge only once.
    // Note that we use the C-API here with the "qh" macro, since this
    // functionality is not exposed through the C++ API.
    qh visit_id++;
    for (auto& facet : qhull.facetList().toStdVector()) {
        vertices.clear();

        // Add the facet
        for (auto& vertex : facet.vertices()) {
            const auto id = vertex.point().id(qhull.runId());
            if (vertexMap.count(id) <= 0) {
                vertexMap[id] = lattice.addMinimal(points[id]);
            }

            vertices.insert(vertexMap[id]);
        }

        lattice.addFace(vertices);

        // Add the ridges
        // See FAQ of qhull about makeridges.
        // http://www.qhull.org/html/qh-faq.htm#ridges
        qh_makeridges(facet.getFacetT());
        facet.getFacetT()->visitid = qh visit_id;

        for (auto& ridge : facet.ridges().toStdVector()) {
            const auto neighbourVisited = otherfacet_(
                ridge.getRidgeT(),
                facet.getFacetT()
                )->visitid;
            if (neighbourVisited != qh visit_id) { 
                vertices.clear();

                for (auto& vertex : ridge.vertices()) {
                    const auto id = vertex.point().id(qhull.runId());
                    if (vertexMap.count(id) <= 0) {
                        vertexMap[id] = lattice.addMinimal(points[id]);
                    }

                    vertices.insert(vertexMap[id]);
                }

                lattice.addFace(vertices);
#ifdef _VERBOSE_
                std::cout << "Ridge " << ++i << std::endl;
#endif
            }
        }
    }

    return lattice;
}
