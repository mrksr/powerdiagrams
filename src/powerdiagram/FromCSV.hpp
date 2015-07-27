#ifndef FROMCSV_H
#define FROMCSV_H

#include "PowerDiagram.hpp"

#include <vector>

class FromCSV {
    public:
        virtual ~FromCSV() { }

        /**
         * @brief Parse spheres from two files containing centers and radii.
         *
         * @param centers Name of the file containing the centers.
         * @param radiuss Name of the file containing the radii.
         *
         * @return A vector of spheres.
         */
        static std::vector<PowerDiagram::Sphere_t> spheres(const char* centers, const char* radiuss);

    private:
        FromCSV();
};

#endif
