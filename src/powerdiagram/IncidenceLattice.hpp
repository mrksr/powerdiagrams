#ifndef INCIDENCELATTICE_H
#define INCIDENCELATTICE_H

#include "BidirectionalGraph.hpp"
#include <algorithm>
#include <cassert>
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

        Keys_t minimalsOf(const Key_t& key)
        {
            // See definition of rep_
            return std::get<1>(rep_.value(key));
        }
        const Keys_t& minimalsOf(const Key_t& key) const
        {
            // See definition of rep_
            return std::get<1>(rep_.value(key));
        }
        Keys_t maximalsOf(const Key_t& key) const
        {
            return rep_.maximalSuccessors(key);
        }

        void restrictToMaximals(const Keys_t& maximals)
        {
            rep_.restrictTo([&maximals, this](const Key_t& k) {
                    const auto succs = maximalsOf(k);
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
                    const auto& preds = minimalsOf(k);
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

        /**
         * @brief Add any new face.
         * The face to add can also be maximal, using this function is just
         * slower than using addMaximalFace in that case.
         *
         * @param faces The new face will be the union of all faces
         *
         * @return Key of the added face
         */
        Key_t addFace(const Keys_t& faces)
        {
            return addFace(faces, false);
        }
        /**
         * @brief Add a face which is guaranteed to be maximal.
         *
         * @param faces The new face will be the union of all faces
         *
         * @return Key of the added face
         */
        Key_t addMaximalFace(const Keys_t& faces)
        {
            return addFace(faces, true);
        }

    private:
        // Besides the Value, we save the minimal nodes to speed up face inserts.
        BidirectionalGraph<Key_t, std::tuple<Value_t, Keys_t>> rep_;
        Key_t nextKey_;
        Value_t defaultValue_;

        Key_t nextKey() {
            return nextKey_++;
        }

        /**
         * @brief Search for the least Upper bounds of the given minimals.
         * A least upper bound is a node which is connected to all minimals
         * and where there is no other upper bound below it.
         * @param startFace Hint where to start the BFS for the lubs
         *
         * @return A possibly empty set of all lubs
         */
        Keys_t leastUpperBounds(const Keys_t& minimals, const Key_t& startFace)
        {
            // A Node is an Upperbound if it is a successor of all nodes in
            // faces.
            const auto isUb = [&minimals, this](const Key_t& k) {
                const auto& kmins = minimalsOf(k);
                return std::all_of(
                    minimals.begin(),
                    minimals.end(),
                    [&kmins](const Key_t& face) {
                        return kmins.find(face) != kmins.end();
                    });
            };

            const auto lubs = rep_.findNodes(
                startFace,
                isUb,
                [&isUb](const Key_t& k) { return !isUb(k); },
                [this](const Key_t& k) { return rep_.successors(k); }
                );

            return lubs;
        }

        /**
         * @brief Search for the largest "pure" successors for all the faces in faces.
         * An example of a group of vertices 1 and 2 would be {1, 2},
         * but not {1, 2, 3}.
         *
         * @param faces Faces to be grouped
         * @param minimals Pre-computed union of all minimals in the faces
         *
         * @return A non-empty set of largest groups containing all minimals at least once
         */
        Keys_t bestGroups(const Keys_t& faces, const Keys_t& minimals)
        {
            // A Node is a group if it only contains minimals who are also
            // minimals of some of the faces.
            const auto isGroup = [&minimals, this](const Key_t& k) {
                const auto& kmins = minimalsOf(k);
                return std::all_of(
                    kmins.begin(),
                    kmins.end(),
                    [&minimals](const Key_t& min) {
                        return minimals.find(min) != minimals.end();
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

        /**
         * @brief Add the face constructed by merging faces to the lattice.
         * To add this face, we need to remove the proper edges and replace
         * them with new edges through this new face. The edges from below are
         * given by the bestGroups, while the edges above are given by the
         * least upper bounds.
         *
         * @param isEnsuredMaximal If the face is maximal, we can skip the lubs
         */
        Key_t addFace(const Keys_t& faces, bool isEnsuredMaximal)
        {
            assert(!faces.empty() && "Cannot add the empty face.");

            Keys_t minimals;
            for (auto& face : faces) {
                const auto& mins = minimalsOf(face);
                minimals.insert(mins.begin(), mins.end());
            }

            auto groups = bestGroups(faces, minimals);

            if (groups.size() == 1) {
                // This face already exists, return it
                return *groups.begin();
            } else {
                auto key = nextKey();
                rep_.insertNode(key, std::make_tuple(defaultValue_, minimals));

                if (!isEnsuredMaximal) {
                    Keys_t lubs = leastUpperBounds(minimals, *faces.begin());
                    for (auto& lub : lubs) {
                        for (auto& group : groups) {
                            rep_.deleteEdge(group, lub);
                        }

                        rep_.insertEdge(key, lub);
                    }
                }

                for (auto& group : groups) {
                    rep_.insertEdge(group, key);
                }

                return key;
            }
        }
};

#endif
