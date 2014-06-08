#ifndef BIDIRECTIONALGRAPH_H
#define BIDIRECTIONALGRAPH_H

#include <queue>
#include <map>
#include <set>
#include <tuple>

template <typename Key_t, typename Value_t>
class BidirectionalGraph {
    typedef std::set<Key_t> Keys_t;
    typedef std::tuple<Value_t, Keys_t, Keys_t> Entry_t;
    typedef std::map<Key_t, Entry_t> Graph_t;

    public:
        BidirectionalGraph(): rep_() {}
        virtual ~BidirectionalGraph() { }

        bool nodeExists(const Key_t& key) const
        {
            return rep_.count(key) != 0;
        }

        Value_t& value(const Key_t& key)
        {
            return rep_[key];
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
            return reachable(key, [this](const Key_t& k) { return this->immPreds(k); });
        }
        Keys_t allSuccessors(const Key_t& key)
        {
            return reachable(key, [this](const Key_t& k) { return this->immSuccs(k); });
        }

        Keys_t minimalElements()
        {
            Keys_t res;

            for (auto& item : rep_) {
                if (predecessors(item.first).size() == 0) {
                    res.insert(item.first);
                }
            }

            return res;
        }
        Keys_t maximalElements()
        {
            Keys_t res;

            for (auto& item : rep_) {
                if (successors(item.first).size() == 0) {
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

        template <typename Lambda>
        Keys_t reachable(const Key_t& key, Lambda&& next)
        {
            Keys_t visited { key };
            std::queue<Key_t> tovisit;
            tovisit.push(key);

            while (! tovisit.empty()) {
                auto& element = tovisit.front();
                tovisit.pop();

                visited.insert(element);
                for (auto& item : next(element)) {
                    if (visited.count(item) == 0) {
                        tovisit.push(item);
                        visited.insert(item);
                    }
                }
            }

            return visited;
        }
};

#endif
