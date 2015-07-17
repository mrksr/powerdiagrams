#include "powerdiagram/FromCSV.hpp"
#include "powerdiagram/IncidenceLattice.hpp"
#include "powerdiagram/PowerDiagramDual.hpp"
#include "powerdiagram/PowerDiagramNaive.hpp"
#include <Eigen/Dense>
#include <gflags/gflags.h>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef HAVE_QHULL
#include "powerdiagram/ConvexHullQhull.hpp"
#endif

#ifdef HAVE_QHULL
DEFINE_bool(dual, true, "Run the Dual Algorithm");
DEFINE_bool(draw, false, "Output Information needed to draw the Diagram (implies --dual and --nonaive)");
#else
DECLARE_bool(draw);
#endif
DEFINE_bool(naive, false, "Run the Naive Algorithm");
DEFINE_bool(verbose, false, "Verbose output");

DECLARE_bool(help);
DECLARE_string(helpmatch);

#ifdef HAVE_QHULL
template<typename Spheres>
static void dual(const Spheres& spheres)
{
    std::cout << "Dual algorithm:" << std::endl;

    ConvexHullQhull conv;
    PowerDiagramDual dual(conv);
    auto diagram = dual.fromSpheres(spheres);

    std::cout << "Number of minimal nodes: " << diagram.minimals().size() << std::endl;
    for (auto& minimal : diagram.minimals()) {
        std::cout << "Minimal: " << diagram.value(minimal).transpose() << std::endl;
    }

    std::cout << "Number of maximal nodes: " << diagram.maximals().size() << std::endl;
    for (auto& maximal : diagram.maximals()) {
        std::cout << "Maximal: " << diagram.value(maximal).transpose() << std::endl;
        std::cout << "Direct Predecessors: " << std::endl;
        for (auto& pred : diagram.predecessors(maximal)) {
            std::cout << "Id: " << pred << std::endl;
            for (auto& min : diagram.minimalsOf(pred)) {
                std::cout << "  - " << diagram.value(min).transpose() << std::endl;
            }
        }
    }
}

template<typename Spheres>
static void draw(const Spheres& spheres)
{
    ConvexHullQhull conv;
    PowerDiagramDual dual(conv);
    auto diagram = dual.fromSpheres(spheres);
    using Key_t = typename decltype(diagram)::Key_t;

    // Give every sphere a number and output it.
    std::unordered_map<Key_t, size_t> sphereMap;
    {
        size_t i = 1;
        for (auto& sphere : diagram.minimals()) {
            sphereMap[sphere] = i;
            // The last entry here is the radius
            std::cout << "s" << i << " " << diagram.value(sphere).transpose() << std::endl;

            i++;
        }
    }

    std::cout << std::endl;

    // Give every point (0-face) a number and output it
    std::unordered_map<Key_t, size_t> pointMap;
    {
        size_t i = 1;
        for (auto& point : diagram.maximals()) {
            pointMap[point] = i;
            std::cout << "p" << i << " " << diagram.value(point).transpose() << std::endl;

            i++;
        }
    }

    std::cout << std::endl;

    // Output inner edges (1-face) as combination of points
    // And extremal edges (1-face) as a point and a direction
    const auto dimension = std::get<0>(spheres[0]).size();
    if (dimension > 1) {
        std::unordered_set<Key_t> visitedEdges;

        for (auto& point : diagram.maximals()) {
            for (auto& edge : diagram.predecessors(point)) {
                if (visitedEdges.find(edge) == visitedEdges.end()) {
                    visitedEdges.insert(edge);

                    const auto points = diagram.successors(edge);
                    if (points.size() == 1 ) {
                        std::cout << "ee p" << pointMap.at(*points.begin());

                        for (auto& sphere : diagram.minimalsOf(edge)) {
                            std::cout << " s" << sphereMap.at(sphere);
                        }

                        std::cout << " d" << diagram.value(edge).transpose();
                    } else {
                        // This means > 1 since 0 is not possible (point is a successor)
                        std::cout << "ei";

                        for (auto& pt : points) {
                            std::cout << " p" << pointMap.at(pt);
                        }

                        for (auto& sphere : diagram.minimalsOf(edge)) {
                            std::cout << " s" << sphereMap.at(sphere);
                        }
                    }

                    std::cout << std::endl;
                }
            }
        }
    }
}
#endif

template<typename Spheres>
static void naive(const Spheres& spheres)
{
    std::cout << "Naive algorithm:" << std::endl;

    PowerDiagramNaive naive;
    auto diagram = naive.fromSpheres(spheres);

    std::cout << "Number of minimal nodes: " << diagram.minimals().size() << std::endl;
    for (auto& minimal : diagram.minimals()) {
        std::cout << "Minimal: " << diagram.value(minimal).transpose() << std::endl;
    }

    std::cout << "Number of maximal nodes: " << diagram.maximals().size() << std::endl;
    for (auto& maximal : diagram.maximals()) {
        std::cout << "Maximal: " << diagram.value(maximal).transpose() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    std::string usage;
    usage += "This program calculates powerdiagrams from a set of spheres in n dimensions.\n";
    usage += "Sample usage:\n\t";
    usage += argv[0];
    usage += " [Options] <centers> <radii>\n";
    usage += "For a complete help, use options --help or --helpfull.\n";
    gflags::SetUsageMessage(usage);
    gflags::ParseCommandLineNonHelpFlags(&argc, &argv, true);
    if (FLAGS_help) {
        FLAGS_help = false;
        FLAGS_helpmatch = "powerdiagram";
    }
    gflags::HandleCommandLineHelpFlags();

    if (argc < 3) {
        std::cout << gflags::ProgramUsage();
        return 2;
    } else {
        const auto& spheres = FromCSV::spheres(argv[1], argv[2]);

        if (spheres.size() < 1) {
            std::cout << "Error: Empty input. Maybe the Filenames are wrong?"<< std::endl;
            return 1;
        }

        if (FLAGS_verbose) {
          std::cerr << "Spheres:" << std::endl;
          for (auto& item : spheres) {
            std::cerr
              << "Center: "
              << std::get<0>(item).transpose()
              << " - Radius: "
              << std::get<1>(item)
              << std::endl;
          }
          std::cerr << std::endl << std::endl;
        }

#ifdef HAVE_QHULL
        if (FLAGS_draw) {
            draw(spheres);
        } else if (FLAGS_dual) {
            dual(spheres);
        }
#endif

        if (FLAGS_naive && !FLAGS_draw) {
            naive(spheres);
        }

        return 0;
    }
}
