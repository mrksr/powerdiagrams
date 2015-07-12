#include "ConvexHullQhull.hpp"

#include <gflags/gflags.h>
#include <iostream>
#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/QhullFacet.h>
#include <libqhullcpp/QhullFacetList.h>
#include <libqhullcpp/QhullRidge.h>
#include <libqhullcpp/QhullVertex.h>
#include <unordered_map>
#include <vector>

DEFINE_string(qhullout, "", "Output string for Qhull (e.g. \"f i s\")");
DECLARE_bool(verbose);

using qhullID_t = int;
using Eigen::VectorXd;

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
    qhull.setErrorStream(&std::cerr);
    qhull.setOutputStream(&std::cout);

    if (FLAGS_verbose) {
        std::cerr << "Starting Qhull" << std::endl;
    }
    // Find convex hull
    qhull.runQhull("", dimension, points.size(), &qhullpoints[0], FLAGS_qhullout.c_str());

    if (!FLAGS_qhullout.empty()) {
        std::cerr << "Qhull Message for parameters: " << FLAGS_qhullout << std::endl;
        qhull.outputQhull();
    }

    if (FLAGS_verbose) {
        std::cout << "Qhull is done." << std::endl;
    }

    // Create incidence lattice
    IncidenceLattice<VectorXd> lattice;
    std::unordered_map<qhullID_t, decltype(lattice)::Key_t> vertexMap;

    // Add facets and ridges
    const auto facets = qhull.facetList().toStdVector();
    // Bookkeeping to ensure we visit every ridge only once.
    qhull.qhullQh()->visit_id++;
    // Temporary Set
    decltype(lattice)::Keys_t vertices;
    // Verbosity Things
    size_t currentFacet = 0;
    const size_t allFacets = facets.size();
    const size_t outputStep = std::max<size_t>(allFacets / 100, 10);

    for (auto& facet : facets) {
        // Add the facet
        if (FLAGS_verbose && currentFacet % outputStep == 0) {
            std::cerr
                << "Adding Facet No. "
                << currentFacet
                << " of "
                << allFacets
                << std::endl;
        }
        currentFacet++;

        vertices.clear();

        // Add the facet
        for (auto& vertex : facet.vertices()) {
            const auto id = vertex.point().id(qhull.runId());
            if (vertexMap.count(id) <= 0) {
                vertexMap[id] = lattice.addMinimal(points[id]);
            }

            vertices.insert(vertexMap[id]);
        }

        lattice.addMaximalFace(vertices);

        // Add the ridges
        // See FAQ of qhull about makeridges.
        // http://www.qhull.org/html/qh-faq.htm#ridges
        qh_makeridges(facet.getFacetT());
        facet.getFacetT()->visitid = qhull.qhullQh()->visit_id;

        for (auto& ridge : facet.ridges().toStdVector()) {
            const auto neighbourVisited = otherfacet_(
                ridge.getRidgeT(),
                facet.getFacetT()
                )->visitid;
            if (neighbourVisited != qhull.qhullQh()->visit_id) { 
                vertices.clear();

                for (auto& vertex : ridge.vertices()) {
                    const auto id = vertex.point().id(qhull.runId());
                    if (vertexMap.count(id) <= 0) {
                        vertexMap[id] = lattice.addMinimal(points[id]);
                    }

                    vertices.insert(vertexMap[id]);
                }

                lattice.addFace(vertices);
            }
        }
    }

    return lattice;
}
