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
            return groupsOfLengthPrepend(length, indices.begin(), indices.end(), d_first, {});
        }

        template<typename InputIt, typename OutputIt>
        static OutputIt groupsOfLength(
                size_t length,
                InputIt first,
                InputIt last,
                OutputIt d_first)
        {
            return groupsOfLengthPrepend(length, first, last, d_first, {});
        }

    private:
        AllChoices();

        template<typename InputIt, typename OutputIt>
        static OutputIt groupsOfLengthPrepend(
                size_t length,
                InputIt first,
                InputIt last,
                OutputIt d_first,
                const std::vector<typename std::iterator_traits<InputIt>::value_type>& prepend)
        {
            typedef typename std::iterator_traits<InputIt>::value_type value_t;

            if (length == 0) {
                return d_first;
            } else if (length == 1) {
                std::vector<value_t> choice {prepend};
                choice.resize(choice.size() + 1);
                const auto lastIndex = choice.size() - 1;

                for (auto it = first; it != last; ++it) {
                    choice[lastIndex] = *it;
                    // Note the copy-constructor
                    *d_first++ = choice;
                }

                return d_first;
            } else {
                std::vector<value_t> nextPrepend {prepend};
                nextPrepend.resize(nextPrepend.size() + 1);
                const auto lastIndex = nextPrepend.size() - 1;

                for (auto it = first; it != last; ) {
                    nextPrepend[lastIndex] = *it;
                    ++it;

                    d_first = groupsOfLengthPrepend(length - 1, it, last, d_first, nextPrepend);
                }

                return d_first;
            }
        }
};

#endif
