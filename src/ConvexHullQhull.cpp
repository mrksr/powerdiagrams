#include "ConvexHullQhull.h"
#include <iostream>
#include <libqhullcpp/Qhull.h>
#include <libqhullcpp/QhullError.h>
#include <libqhullcpp/QhullFacet.h>
#include <libqhullcpp/QhullFacetList.h>
#include <libqhullcpp/QhullLinkedList.h>
#include <libqhullcpp/QhullQh.h>
#include <libqhullcpp/QhullVertex.h>
#include <libqhullcpp/RboxPoints.h>

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

    /* orgQhull::Qhull qhull; */
    /* qhull.setErrorStream(&std::cerr); */
    /* qhull.setOutputStream(&std::cout); */
    /* qhull.useOutputStream = true; */

    /* std::cout << qhull.qhullStatus() << std::endl; */

    /* qhull.runQhull("", dimension, points.size(), qhullpoints, "s"); */

    // FIXME: This simple hull does not work :(
    orgQhull::RboxPoints rbox;
    rbox.appendPoints("10");
    orgQhull::Qhull qhull(rbox, "s");

    std::cout << qhull.qhullStatus() << std::endl;
    std::cout << "Vertices (" << qhull.vertexCount() << "): " << std::endl << qhull.vertexList();
    if (qhull.hasQhullMessage()) {
        std::cout << "----" << std::endl << "Message:" << std::endl;
        std::cout << qhull.qhullMessage() << std::endl;
    }

    delete[] qhullpoints;

    IncidenceLattice<Eigen::VectorXd> lattice(Eigen::VectorXd::Zero(1));
    return lattice;
}
