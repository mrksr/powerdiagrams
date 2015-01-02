#ifndef ALLCHOICES_H
#define ALLCHOICES_H

#include <cstddef>
#include <vector>

class AllChoices {
    public:
        virtual ~AllChoices() { }

        template<typename InputIt, typename OutputIt>
        static OutputIt tuplesOfLength(
                InputIt first,
                InputIt last,
                OutputIt d_first,
                size_t length)
        {
            return tuplesOfLengthPrepend(first, last, d_first, length, {});
        }

    private:
        AllChoices();

        template<typename InputIt, typename OutputIt>
        static OutputIt tuplesOfLengthPrepend(
                InputIt first,
                InputIt last,
                OutputIt d_first,
                size_t length,
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

                    d_first = tuplesOfLengthPrepend(it, last, d_first, length - 1, nextPrepend);
                }

                return d_first;
            }
        }
};

#endif
