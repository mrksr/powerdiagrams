#ifndef BIDIRECTIONALGRAPH_H
#define BIDIRECTIONALGRAPH_H

#include <queue>
#include <map>
#include <set>
#include <tuple>

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

        Value_t& value(const Key_t& key)
        {
            return std::get<0>(rep_[key]);
        }
        void value(const Key_t& key, const Value_t& value)
        {
            std::get<0>(rep_[key]) = value;
        }

        const Keys_t& predecessors(const Key_t& key)
        {
            return immPreds(key);
        }
        const Keys_t& successors(const Key_t& key)
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
                    [this](const Key_t& k) { return this->immPreds(k); }
                    );
        }

        bool isMinimal(const Key_t& key)
        {
            return predecessors(key).size() == 0;
        }
        bool isMaximal(const Key_t& key)
        {
            return successors(key).size() == 0;
        }
        Keys_t minimalElements()
        {
            Keys_t res;

            for (auto& item : rep_) {
                if (isMinimal(item.first)) {
                    res.insert(item.first);
                }
            }

            return res;
        }
        Keys_t maximalElements()
        {
            Keys_t res;

            for (auto& item : rep_) {
                if (isMaximal(item.first)) {
                    res.insert(item.first);
                }
            }

            return res;
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

    private:
        Graph_t rep_;

        Keys_t& immPreds(const Key_t& key)
        {
            return std::get<1>(rep_[key]);
        }
        Keys_t& immSuccs(const Key_t& key)
        {
            return std::get<2>(rep_[key]);
        }

        template <typename Next>
        Keys_t reachable(const Key_t& key, Next&& next)
        {
            return reachableFilter(key, [](const Key_t& x) { return true; } , next);
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
