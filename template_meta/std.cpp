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
template <typename Head, typename List1, typename PrevRow, bool IsEnabled = std::is_same<Head, typename List1::head>::value>
struct FillRow {
    typedef LinkedList<
        Integer<PrevRow::head::value + 1>,
        typename FillRow<
            Head,
            typename List1::tail,
            typename PrevRow::tail>::value>
    value;
};

template <typename Head, typename List1, typename PrevRow>
struct FillRow<Head, List1, PrevRow, false> {
    typedef LinkedList<
        typename PrevRow::tail::head,
        typename FillRow<
            Head,
            typename List1::tail,
            typename PrevRow::tail>::value>
    value;
};

template <typename Head, typename PrevRow, bool IsEnabled>
struct FillRow<Head, Null, PrevRow, IsEnabled> {
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
            typename FillRow<
                typename List2::head,
                List1,
                typename Table::head>::value,
            Table>>::value
        };
};

template <typename List1, typename Table>
struct LongestCommonSubsequence<List1, Null, Table> {
    enum {value = Last<typename Table::head::value>::value};
};

template <typename List1, typename List2>
struct LongestCommonSubsequence<List1, List2, void> {
    enum {value = LongestCommonSubsequence<
        List1,
        List2,
        LinkedList<
            typename GenerateEmptyRow<List1>::value,
            Null>>::value
        };
};

int main() {
    static_assert(LinkedList<Integer<1>,LinkedList<Integer<2>,LinkedList<Integer<3>,Null>>>::head::value == 1, "");
    static_assert(List<Integer<1>,Integer<2>,Integer<3>>::list::head::value == 1, "");
    static_assert(IntList<1,2,3>::list::head::value == 1, "");
    static_assert(List<ListWrapper<IntList<4,5,6>::list>, ListWrapper<IntList<7,8,9>::list>>::list::head::value::head::value == 4, "");
    static_assert(List<ListWrapper<IntList<4,5,6>::list>, ListWrapper<IntList<7,8,9>::list>>::list::tail::head::value::head::value == 7, "");
}