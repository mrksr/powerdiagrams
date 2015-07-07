#ifndef BIDIRECTIONALGRAPH_H
#define BIDIRECTIONALGRAPH_H

#include <queue>
#include <map>
#include <set>
#include <tuple>

/**
 * @brief A datastructure containing a directed graph with efficient lookup in both directions.
 *
 * A BidirectionalGraph contains a directed graph. It is called BiDirectional
 * because edges can be traversed in both directions efficiently.
 * A BidirectionalGraph uses Keys as an index of the nodes and stores a Value
 * in every node.
 * A node is called minimal if it has no predecessors and maximal if it has no
 * successors.
 *
 * @tparam Key_t Type of Objects used to index nodes.
 * @tparam Value_t Type of Values stored in each node.
 */
template <typename Key_t, typename Value_t>
class BidirectionalGraph {
    public:
        typedef std::set<Key_t> Keys_t;
        typedef std::tuple<Value_t, Keys_t, Keys_t> Entry_t;
        typedef std::map<Key_t, Entry_t> Graph_t;

        BidirectionalGraph(): rep_() { }
        virtual ~BidirectionalGraph() { }

        bool nodeExists(const Key_t& key) const
        {
            return rep_.count(key) != 0;
        }
        bool edgeExists(const Key_t& from, const Key_t& to) const
        {
            return nodeExists(from) && nodeExists(to) &&
                immSuccs(from).find(to) != immSuccs(from).end();
        }

        Value_t& value(const Key_t& key)
        {
            return std::get<0>(rep_[key]);
        }
        void value(const Key_t& key, const Value_t& value)
        {
            std::get<0>(rep_[key]) = value;
        }

        const Keys_t& predecessors(const Key_t& key) const
        {
            return immPreds(key);
        }
        const Keys_t& successors(const Key_t& key) const
        {
            return immSuccs(key);
        }
        Keys_t allPredecessors(const Key_t& key)
        {
            return reachable(
                    key,
                    [this](const Key_t& k) { return this->immPreds(k); }
                    );
        }
        Keys_t allSuccessors(const Key_t& key)
        {
            return reachable(
                    key,
                    [this](const Key_t& k) { return this->immSuccs(k); }
                    );
        }

        Keys_t minimalPredecessors(const Key_t& key)
        {
            return reachableFilter(
                    key,
                    [this](const Key_t& k) { return this->isMinimal(k); },
                    [this](const Key_t& k) { return this->immPreds(k); }
                    );
        }
        Keys_t maximalSuccessors(const Key_t& key)
        {
            return reachableFilter(
                    key,
                    [this](const Key_t& k) { return this->isMaximal(k); },
                    [this](const Key_t& k) { return this->immSuccs(k); }
                    );
        }

        bool isMinimal(const Key_t& key) const
        {
            return predecessors(key).size() == 0;
        }
        bool isMaximal(const Key_t& key) const
        {
            return successors(key).size() == 0;
        }
        Keys_t minimalElements() const
        {
            Keys_t res;

            for (auto& item : rep_) {
                if (isMinimal(item.first)) {
                    res.insert(item.first);
                }
            }

            return res;
        }
        Keys_t maximalElements() const
        {
            Keys_t res;

            for (auto& item : rep_) {
                if (isMaximal(item.first)) {
                    res.insert(item.first);
                }
            }

            return res;
        }

        template <typename Predicate>
        void restrictTo(Predicate&& pred)
        {
            auto it = rep_.begin();

            // Fairly hacky way to iterate over the map while deleting contents.
            while(it != rep_.end()) {
                if (!pred(it->first)) {
                    auto toErase = it;
                    ++it;

                    deleteNode(toErase->first);
                } else {
                    ++it;
                }
            }
        }

        void insertNode(const Key_t& key, const Value_t& value)
        {
            rep_[key] = std::make_tuple(value, Keys_t(), Keys_t());
        }

        void insertEdge(const Key_t& from, const Key_t& to)
        {
            if (nodeExists(from) && nodeExists(to)) {
                immSuccs(from).insert(to);
                immPreds(to).insert(from);
            }
        }

        void deleteNode(const Key_t& key)
        {
            for (auto& succ : immSuccs(key)) {
                immPreds(succ).erase(key);
            }
            for (auto& pred : immPreds(key)) {
                immSuccs(pred).erase(key);
            }

            rep_.erase(key);
        }
        void deleteEdge(const Key_t& from, const Key_t& to)
        {
            if (edgeExists(from, to)) {
                immSuccs(from).erase(to);
                immPreds(to).erase(from);
            }
        }

    private:
        Graph_t rep_;

        const Keys_t& immPreds(const Key_t& key) const
        {
            return std::get<1>(rep_.at(key));
        }
        Keys_t& immPreds(const Key_t& key)
        {
            return std::get<1>(rep_[key]);
        }
        const Keys_t& immSuccs(const Key_t& key) const
        {
            return std::get<2>(rep_.at(key));
        }
        Keys_t& immSuccs(const Key_t& key)
        {
            return std::get<2>(rep_[key]);
        }

        template <typename Next>
        Keys_t reachable(const Key_t& key, Next&& next)
        {
            return reachableFilter(key,
                    [](const Key_t& x) { return true; },
                    std::forward<Next>(next)
                    );
        }

        template <typename Filter, typename Next>
        Keys_t reachableFilter(const Key_t& key, Filter&& filter, Next&& next)
        {
            Keys_t visited { key };
            std::queue<Key_t> tovisit;
            tovisit.push(key);

            Keys_t result;
            if (filter(key)) {
                result.insert(key);
            }

            while (! tovisit.empty()) {
                auto& element = tovisit.front();
                tovisit.pop();

                visited.insert(element);
                for (auto& item : next(element)) {
                    if (visited.count(item) == 0) {
                        tovisit.push(item);
                        visited.insert(item);

                        if (filter(item)) {
                            result.insert(item);
                        }
                    }
                }
            }

            return result;
        }
};

#endif
