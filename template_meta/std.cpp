#include <iostream>
#include <type_traits>

// Wrapper classes for use in LinkedList

template <int N>
struct Integer {
    enum {value = N};
};

template <typename LinkedList>
struct ListWrapper {
    typedef LinkedList value;
};

// Linked list

struct Null {};

// Head must have value member
template <typename Head, typename Tail>
struct LinkedList {
    typedef Head head;
    typedef Tail tail;
};

// Variadic templates to make writing lists more readable

template <typename Head, typename... Tail>
struct List {
    typedef LinkedList<
        Head,
        typename List<Tail...>::list>
    list;
};

template <typename Head>
struct List<Head> {
    typedef LinkedList<
        Head,
        Null>
    list;
};

// List of Integers

template <int Head, int... Tail>
struct IntList {
    typedef LinkedList<
        Integer<Head>,
        typename IntList<Tail...>::list>
    list;
};

template <int Head>
struct IntList<Head> {
    typedef LinkedList<
        Integer<Head>,
        Null>
    list;
};

// Maximum of two elements (wrapped Integers)
template <typename A, typename B>
struct Max {
    typedef std::conditional_t<
        (static_cast<int>(A::value) > static_cast<int>(B::value)),
        A,
        B>
    value;
};

// Last element of a non-empty list
template <typename List>
struct Last {
    typedef typename Last<typename List::tail>::value value;
};

template <typename T>
struct Last<LinkedList<T, Null>> {
    typedef T value;
};

// Generate empty row of DP table
template <typename List>
struct GenerateEmptyRow {
    typedef LinkedList<
        Integer<0>,
        typename GenerateEmptyRow<typename List::tail>::value>
    value;
};

template <>
struct GenerateEmptyRow<Null> {
    typedef LinkedList<
        Integer<0>,
        Null>
    value;
};

// Filling out a row of the DP
template <typename Head, typename List1, typename PrevRow, typename PrevElem>
struct FillRow {
    typedef std::conditional_t<
        std::is_same<Head, typename List1::head>::value,
        Integer<PrevRow::head::value + 1>,
        typename Max<
            typename PrevRow::tail::head,
            PrevElem>::value> currentElem;

    typedef LinkedList<
        currentElem,
        typename FillRow<
            Head,
            typename List1::tail,
            typename PrevRow::tail,
            currentElem>::value>
    value;
};

template <typename Head, typename PrevRow, typename PrevElem>
struct FillRow<Head, Null, PrevRow, PrevElem> {
    typedef Null value;
};

// Longest common subsequence

// Expects two LinkedLists containing integers.
// Table containst the rows of the DP table in reverse order, i.e. last row is first in the list
template <typename List1, typename List2, typename Table = void>
struct LongestCommonSubsequence {
    enum {value = LongestCommonSubsequence<
        List1,
        typename List2::tail,
        LinkedList<
            ListWrapper<
                LinkedList<
                    Integer<0>,
                    typename FillRow<
                        typename List2::head,
                        List1,
                        typename Table::head::value,
                        Integer<0>>::value>>,
            Table>>::value
        };
};

template <typename List1, typename Table>
struct LongestCommonSubsequence<List1, Null, Table> {
    enum {value = Last<typename Table::head::value>::value::value};
};

template <typename List1, typename List2>
struct LongestCommonSubsequence<List1, List2, void> {
    enum {value = LongestCommonSubsequence<
        List1,
        List2,
        LinkedList<
            ListWrapper<typename GenerateEmptyRow<List1>::value>,
            Null>>::value
        };
};

int main() {
    // Testing lists
    static_assert(LinkedList<Integer<1>,LinkedList<Integer<2>,LinkedList<Integer<3>,Null>>>::head::value == 1, "");
    static_assert(List<Integer<1>,Integer<2>,Integer<3>>::list::head::value == 1, "");
    static_assert(IntList<1,2,3>::list::head::value == 1, "");
    static_assert(List<ListWrapper<IntList<4,5,6>::list>, ListWrapper<IntList<7,8,9>::list>>::list::head::value::head::value == 4, "");
    static_assert(List<ListWrapper<IntList<4,5,6>::list>, ListWrapper<IntList<7,8,9>::list>>::list::tail::head::value::head::value == 7, "");

    // Testing longest common subsequence
    static_assert(LongestCommonSubsequence<
        typename IntList<1,3,1,4,2>::list,
        typename IntList<3,2,4,1>::list>::value
        == 2, "");
    static_assert(LongestCommonSubsequence<
        typename IntList<3,2,4,1>::list,
        typename IntList<1,3,1,4,2>::list>::value
        == 2, "");

    // TODO: add traceback
}