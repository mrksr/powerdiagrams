#include "powerdiagram/FromCSV.hpp"
#include "powerdiagram/IncidenceLattice.hpp"
#include "powerdiagram/PowerDiagramDual.hpp"
#include "powerdiagram/PowerDiagramNaive.hpp"
#include <Eigen/Dense>
#include <gflags/gflags.h>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#ifdef HAVE_QHULL
#include "powerdiagram/ConvexHullQhull.hpp"
#endif

#ifdef HAVE_QHULL
DEFINE_bool(dual, true, "Run the Dual Algorithm");
#endif
DEFINE_bool(naive, false, "Run the Naive Algorithm");
DEFINE_bool(verbose, false, "Verbose output");

DECLARE_bool(help);
DECLARE_bool(helpshort);

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
        FLAGS_helpshort = true;
    }
    gflags::HandleCommandLineHelpFlags();

    if (argc < 3) {
        std::cout << gflags::ProgramUsage();
        return 1;
    } else {
        const auto& spheres = FromCSV::spheres(argv[1], argv[2]);

        if (spheres.size() < 1) {
            std::cout << "Error: Empty input. Maybe the Filenames are wrong?"<< std::endl;
            return 1;
        }

        if (FLAGS_verbose) {
          std::cout << "Spheres:" << std::endl;
          for (auto& item : spheres) {
            std::cout
              << "Center: "
              << std::get<0>(item).transpose()
              << " - Radius: "
              << std::get<1>(item)
              << std::endl;
          }
          std::cout << std::endl << std::endl;
        }

#ifdef HAVE_QHULL
        if (FLAGS_dual) {
            std::cout << "Dual algorithm:" << std::endl;
            {
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
            std::cout << std::endl << std::endl;
        }
#endif

        if (FLAGS_naive) {
            std::cout << "Naive algorithm:" << std::endl;
            {
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
        }

        return 0;
    }
}
