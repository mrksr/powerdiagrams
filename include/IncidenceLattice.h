#ifndef INCIDENCELATTICE_H
#define INCIDENCELATTICE_H

#include "BidirectionalGraph.h"
#include <map>

/**
 * @brief A datastructure containing incidences of faces in a d-dimensional polyhedron.
 *
 * @tparam Value_t (Vector-)Type describing 0- and d-faces.
 */
template <typename Value_t>
class IncidenceLattice {
    typedef int Key_t;
    typedef typename BidirectionalGraph<Key_t, Value_t>::Keys_t Keys_t;

    public:
        IncidenceLattice(const Value_t& defaultValue):
            rep_(),
            keys_(),
            nextKey_(0),
            defaultValue_(defaultValue)
        { }
        virtual ~IncidenceLattice() { }

        Key_t key(const Value_t& value)
        {
            return keys_[value];
        }
        Value_t value(const Key_t& key)
        {
            return rep_.value(key);
        }
        void value(const Key_t& key, const Value_t& value)
        {
            keys_.erase(value(key));
            rep_.value(key, value);
            keys_[value] = key;
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

        Key_t addMinimal(const Value_t& value)
        {
            auto key = nextKey();
            rep_.insertNode(key, value);
            keys_[value] = key;

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
        std::map<Value_t, Key_t> keys_;
        Key_t nextKey_;
        Value_t defaultValue_;

        Key_t nextKey() {
            return nextKey_++;
        }
};

#endif
