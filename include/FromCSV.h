#ifndef FROMCSV_H
#define FROMCSV_H

#include "PowerDiagram.h"
#include <vector>

class FromCSV {
    public:
        virtual ~FromCSV() { }

        static std::vector<PowerDiagram::Sphere_t> spheres(const char* centers, const char* radiuss);

    private:
        FromCSV();
};

#endif
