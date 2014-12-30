#ifndef INCIDENCELATTICE_H
#define INCIDENCELATTICE_H

#include "BidirectionalGraph.h"
#include <algorithm>
#include <map>

/**
 * @brief A datastructure containing incidences of faces in a d-dimensional polyhedron.
 *
 * @tparam Value_t (Vector-)Type describing 0- and d-faces.
 */
template <typename Value_t>
class IncidenceLattice {
    public:
        typedef int Key_t;
        typedef typename BidirectionalGraph<Key_t, Value_t>::Keys_t Keys_t;

        IncidenceLattice(const Value_t& defaultValue):
            rep_(),
            nextKey_(0),
            defaultValue_(defaultValue)
        { }
        virtual ~IncidenceLattice() { }

        Value_t value(const Key_t& key)
        {
            return rep_.value(key);
        }
        void value(const Key_t& key, const Value_t& value)
        {
            rep_.value(key, value);
        }

        Keys_t minimals()
        {
            return rep_.minimalElements();
        }
        Keys_t maximals()
        {
            return rep_.maximalElements();
        }

        Keys_t minimalsOf(const Key_t& key)
        {
            return rep_.minimalPredecessors(key);
        }
        Keys_t maximalsOf(const Key_t& key)
        {
            return rep_.maximalSuccessors(key);
        }

        // FIXME: Potentially (too) expensive set intersection.
        void restrictToMaximals(const Keys_t& maximals)
        {
            rep_.restrictTo([maximals, this](const Key_t& k) {
                    const auto& succs = this->rep_.maximalSuccessors(k);
                    std::vector<Key_t> intersection;
                    std::set_intersection(
                        maximals.begin(),
                        maximals.end(),
                        succs.begin(),
                        succs.end(),
                        intersection.begin());
                    return intersection.empty();
                    });
        }
        void restrictToMinimals(const Keys_t& minimals)
        {
            rep_.restrictTo([minimals, this](const Key_t& k) {
                    const auto& succs = this->rep_.minimalPredecessors(k);
                    std::vector<Key_t> intersection;
                    std::set_intersection(
                        minimals.begin(),
                        minimals.end(),
                        succs.begin(),
                        succs.end(),
                        intersection.begin());
                    return intersection.empty();
                    });
        }

        Key_t addMinimal(const Value_t& value)
        {
            auto key = nextKey();
            rep_.insertNode(key, value);

            return key;
        }
        Key_t addFace(const Keys_t& face)
        {
            auto key = nextKey();
            rep_.insertNode(key, defaultValue_);

            for (auto& item : face) {
                rep_.insertEdge(item, key);
            }

            return key;
        }

    private:
        BidirectionalGraph<Key_t, Value_t> rep_;
        Key_t nextKey_;
        Value_t defaultValue_;

        Key_t nextKey() {
            return nextKey_++;
        }
};

#endif
