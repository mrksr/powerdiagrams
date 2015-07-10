#ifndef INCIDENCELATTICE_H
#define INCIDENCELATTICE_H

#include "BidirectionalGraph.hpp"
#include <algorithm>
#include <queue>

/**
 * @brief A datastructure containing incidences of faces in a d-dimensional polyhedron.
 *
 * @tparam Value_t (Vector-)Type describing 0- and d-faces.
 */
template <typename Value_t>
class IncidenceLattice {
    public:
        using Key_t = size_t;
        using Keys_t = typename BidirectionalGraph<Key_t, Value_t>::Keys_t;

        IncidenceLattice(const Value_t& defaultValue):
            rep_(),
            nextKey_(0),
            defaultValue_(defaultValue)
        { }
        virtual ~IncidenceLattice() { }

        Value_t& value(const Key_t& key)
        {
            return std::get<0>(rep_.value(key));
        }
        const Value_t& value(const Key_t& key) const
        {
            return std::get<0>(rep_.value(key));
        }

        const Keys_t& predecessors(const Key_t& key) const
        {
            return rep_.predecessors(key);
        }
        const Keys_t& successors(const Key_t& key) const
        {
            return rep_.predecessors(key);
        }
        Keys_t minimals() const
        {
            return rep_.minimalElements();
        }
        Keys_t maximals() const
        {
            return rep_.maximalElements();
        }

        Keys_t minimalsOf(const Key_t& key) const
        {
            // See definition of rep_
            return std::get<1>(rep_.value(key));
        }
        Keys_t maximalsOf(const Key_t& key) const
        {
            return rep_.maximalSuccessors(key);
        }

        // FIXME: Potentially (too) expensive set intersection.
        void restrictToMaximals(const Keys_t& maximals)
        {
            rep_.restrictTo([&maximals, this](const Key_t& k) {
                    const auto& succs = rep_.maximalSuccessors(k);
                    Keys_t intersection;
                    std::set_intersection(
                        maximals.begin(),
                        maximals.end(),
                        succs.begin(),
                        succs.end(),
                        std::inserter(intersection, intersection.begin()));
                    return !intersection.empty();
                    });
        }
        void restrictToMinimals(const Keys_t& minimals)
        {
            rep_.restrictTo([&minimals, this](const Key_t& k) {
                    const auto& preds = rep_.minimalPredecessors(k);
                    Keys_t intersection;
                    std::set_intersection(
                        minimals.begin(),
                        minimals.end(),
                        preds.begin(),
                        preds.end(),
                        std::inserter(intersection, intersection.begin()));
                    return !intersection.empty();
                    });
        }

        Key_t addMinimal(const Value_t& value)
        {
            auto key = nextKey();
            rep_.insertNode(key, std::make_tuple(value, Keys_t{key}));

            return key;
        }
        Key_t addFace(const Keys_t& faces)
        {
            auto groups = bestGroups(faces);

            if (groups.size() == 1) {
                // This face already exists, return it
                return *groups.begin();
            } else {
                Key_t lub;

                auto key = nextKey();
                rep_.insertNode(key, defaultValue_);
                if (leastUpperBound(faces, lub)) {
                    for (auto& group : groups) {
                        rep_.deleteEdge(group, lub);
                    }

                    rep_.insertEdge(key, lub);
                }

                for (auto& group : groups) {
                    rep_.insertEdge(group, key);
                }

                return key;
            }
        }

    private:
        // Besides the Value, we save the minimal nodes to speed up face inserts.
        BidirectionalGraph<Key_t, std::tuple<Value_t, Keys_t>> rep_;
        Key_t nextKey_;
        Value_t defaultValue_;

        Key_t nextKey() {
            return nextKey_++;
        }

        bool leastUpperBound(const Keys_t& faces, Key_t& out)
        {
            if (faces.empty()) {
                return false;
            }

            // A Node is an Upperbound if it is a successor of all nodes in
            // faces.
            Keys_t minimals;
            for (auto& face : faces) {
                const auto mins = minimalsOf(face);
                minimals.insert(mins.begin(), mins.end());
            }
            const auto isUb = [&minimals, this](const Key_t& k) {
                const auto kmins = minimalsOf(k);
                return std::all_of(
                    minimals.begin(),
                    minimals.end(),
                    [&kmins](const Key_t& face) {
                        return kmins.count(face) > 0;
                    });
            };

            const auto lubs = rep_.findNodes(
                *faces.begin(),
                isUb,
                [&isUb](const Key_t& k) { return !isUb(k); },
                [this](const Key_t& k) { return rep_.successors(k); }
                );

            if (lubs.empty()) {
                return false;
            } else {
                out = *lubs.begin();
                return true;
            }
        }

        Keys_t bestGroups(const Keys_t& faces)
        {
            // A Node is a group if it only contains minimals who are also
            // minimals of some of the faces.
            Keys_t minimals;
            for (auto& face : faces) {
                const auto mins = minimalsOf(face);
                minimals.insert(mins.begin(), mins.end());
            }

            const auto isGroup = [&minimals, this](const Key_t& k) {
                const auto kmins = minimalsOf(k);
                return std::all_of(
                    kmins.begin(),
                    kmins.end(),
                    [&minimals](const Key_t& min) {
                        return minimals.count(min) > 0;
                    });
            };

            Keys_t groups;
            for (auto& face : faces) {
                const auto candidates = rep_.findNodes(
                    face,
                    isGroup,
                    isGroup,
                    [this](const Key_t& k) { return rep_.successors(k); }
                    );

                // Since face itself is a group, this is well-defined.
                groups.insert(*std::max_element(
                        candidates.begin(),
                        candidates.end(),
                        [this](const Key_t& a, const Key_t& b) {
                            return minimalsOf(a).size() < minimalsOf(b).size();
                        }));
            }

            return groups;
        }
};

#endif
