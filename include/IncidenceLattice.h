#ifndef INCIDENCELATTICE_H
#define INCIDENCELATTICE_H

#include "BidirectionalGraph.h"
#include <map>

template <typename Vertex_t>
class IncidenceLattice {
    typedef int Key_t;

    public:
        IncidenceLattice(const Vertex_t& defaultVertex):
            rep_(),
            keys_(),
            nextKey_(0),
            defaultVertex_(defaultVertex)
        { }
        virtual ~IncidenceLattice() { }

        Key_t keyOf(const Vertex_t& vertex)
        {
            return keys_[vertex];
        }

        Key_t addVertex(const Vertex_t& vertex)
        {
            auto key = nextKey();
            rep_.insertNode(key, vertex);
            keys_[key] = vertex;

            return key;
        }

    private:
        BidirectionalGraph<Key_t, Vertex_t> rep_;
        std::map<Vertex_t, Key_t> keys_;
        Key_t nextKey_;
        Vertex_t defaultVertex_;

        Key_t nextKey() { return nextKey_++; }
};

#endif
