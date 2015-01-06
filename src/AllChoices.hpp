#ifndef ALLCHOICES_H
#define ALLCHOICES_H

#include <algorithm>
#include <cstddef>
#include <vector>

class AllChoices {
    public:
        virtual ~AllChoices() { }

        template<typename index_t, typename InputIt, typename OutputIt>
        static OutputIt indexGroupsOfLength(
                size_t length,
                InputIt first,
                InputIt last,
                OutputIt d_first)
        {
            std::vector<index_t> indices(std::distance(first, last));
            std::iota(indices.begin(), indices.end(), 0);
            std::vector<index_t> prepend;

            return groupsOfLengthPrepend(
                    length,
                    indices.begin(),
                    indices.end(),
                    d_first,
                    prepend);
        }

        template<typename InputIt, typename OutputIt>
        static OutputIt groupsOfLength(
                size_t length,
                InputIt first,
                InputIt last,
                OutputIt d_first)
        {
            typedef typename std::iterator_traits<InputIt>::value_type value_t;
            std::vector<value_t> prepend;

            return groupsOfLengthPrepend(
                    length,
                    first,
                    last,
                    d_first,
                    prepend);
        }

    private:
        AllChoices();

        template<typename InputIt, typename OutputIt>
        static OutputIt groupsOfLengthPrepend(
                size_t length,
                InputIt first,
                InputIt last,
                OutputIt d_first,
                std::vector<typename std::iterator_traits<InputIt>::value_type>& prepend)
        {
            if (length == 0) {
                return d_first;
            } else if (length == 1) {
                prepend.resize(prepend.size() + 1);
                const auto lastIndex = prepend.size() - 1;

                for (auto it = first; it != last; ++it) {
                    prepend[lastIndex] = *it;
                    // Note the copy-constructor
                    *d_first++ = prepend;
                }

                prepend.erase(prepend.end() - 1);
                return d_first;
            } else {
                prepend.resize(prepend.size() + 1);
                const auto lastIndex = prepend.size() - 1;

                for (auto it = first; it != last; ) {
                    prepend[lastIndex] = *it;
                    ++it;

                    d_first = groupsOfLengthPrepend(
                            length - 1,
                            it,
                            last,
                            d_first,
                            prepend);
                }

                prepend.erase(prepend.end() - 1);
                return d_first;
            }
        }
};

#endif
