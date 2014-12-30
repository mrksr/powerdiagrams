#include "FromCSV.hpp"
#include <Eigen/Dense>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

typedef Eigen::VectorXd VectorXd;
typedef Eigen::MatrixXd MatrixXd;

VectorXd nextCenter(std::istream& centerStream)
{
    // Yes, this is actually ugly.
    std::string line;
    std::getline(centerStream, line);

    std::istringstream lineStream(line);
    std::string cell;

    std::vector<double> entries;
    double value;
    while (std::getline(lineStream,cell,',')) {
        std::istringstream cellStream(cell);
        cellStream >> value;
        entries.push_back(value);
    }

    VectorXd result(entries.size());
    for (size_t i = 0; i < entries.size(); ++i) {
        result[i] = entries[i];
    }
    return result;
}

double nextRadius(std::istream& radiusStream)
{
    std::string line;
    std::getline(radiusStream, line);

    std::istringstream lineStream(line);
    double result;
    lineStream >> result;
    return result;
}

std::vector<PowerDiagram::Sphere_t> FromCSV::spheres(const char* centers, const char* radiuss)
{
    std::vector<PowerDiagram::Sphere_t> spheres;

    std::ifstream centerStream(centers);
    std::ifstream radiusStream(radiuss);

    while (centerStream.good() && radiusStream.good()) {
        auto center = nextCenter(centerStream);
        auto radius = nextRadius(radiusStream);

        if (center.size() > 0) {
            spheres.push_back(PowerDiagram::sphere( center, radius));
        }
    }

    return spheres;
}
