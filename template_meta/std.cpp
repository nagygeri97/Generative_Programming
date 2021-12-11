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

template <typename... Tail>
struct List {};

template <typename Head, typename... Tail>
struct List<Head, Tail...> {
    typedef LinkedList<
        Head,
        typename List<Tail...>::list>
    list;
};

template <>
struct List<> {
    typedef Null list;
};

// List of Integers

template <int... Tail>
struct IntList {};

template <int Head, int... Tail>
struct IntList<Head, Tail...> {
    typedef LinkedList<
        Integer<Head>,
        typename IntList<Tail...>::list>
    list;
};

template <>
struct IntList<> {
    typedef Null list;
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

// Reverse a list
template <typename List, typename Result = Null>
struct Reverse {
    typedef typename Reverse<
        typename List::tail,
        LinkedList<
            typename List::head,
            Result>>::value
    value;
};

template <typename Result>
struct Reverse<Null, Result> {
    typedef Result value;
};

// Reverse a wrapped list
template <typename WrappedList>
struct ReverseWrapped {
    typedef ListWrapper<typename Reverse<typename WrappedList::value>::value> value;
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

// Map all elements of a list
template <template<typename> typename F, typename List>
struct Map {
    typedef LinkedList<
        typename F<typename List::head>::value,
        typename Map<F, typename List::tail>::value>
    value;
};

template <template<typename> typename F>
struct Map<F, Null> {
    typedef Null value;
};

// Remove the head of a non-empty wrapped list, and wrap it again
template <typename WrappedList>
struct RemoveWrappedHead {
    typedef ListWrapper<typename WrappedList::value::tail> value;
};


// Traceback to generate the longes common subsequence from the DP table
template <typename List1, typename List2, typename Table, typename Result = Null>
struct Traceback {
    typedef typename std::conditional_t<
        std::is_same_v<typename List1::head, typename List2::head>,
        Traceback<
            typename List1::tail,
            typename List2::tail, 
            typename Map<RemoveWrappedHead, typename Table::tail>::value,
            LinkedList<typename List1::head, Result>>,
        std::conditional_t<
            (static_cast<int>(Table::head::value::tail::head::value) > static_cast<int>(Table::tail::head::value::head::value)),
            Traceback<
                typename List1::tail,
                List2,
                typename Map<RemoveWrappedHead, Table>::value,
                Result>,
            Traceback<
                List1,
                typename List2::tail,
                typename Table::tail,
                Result>>>::value value;
};

template <typename List2, typename Table, typename Result>
struct Traceback<Null, List2, Table, Result> {
    typedef Result value;
};

template <typename List1, typename Table, typename Result>
struct Traceback<List1, Null, Table, Result> {
    typedef Result value;
};

template <typename Table, typename Result>
struct Traceback<Null, Null, Table, Result> {
    typedef Result value;
};


// Longest common subsequence

// Expects two LinkedLists containing integers.
// Table containst the rows of the DP table in reverse order, i.e. last row is first in the list
template <typename List1, typename List2, typename OriginalList2 = List2, typename Table = LinkedList<ListWrapper<typename GenerateEmptyRow<List1>::value>, Null>>
struct LongestCommonSubsequence {
    typedef LongestCommonSubsequence<
        List1,
        typename List2::tail,
        OriginalList2,
        LinkedList<
            ListWrapper<
                LinkedList<
                    Integer<0>,
                    typename FillRow<
                        typename List2::head,
                        List1,
                        typename Table::head::value,
                        Integer<0>>::value>>,
            Table>>
    lcs;

    enum {length = lcs::length };
    typedef typename lcs::sequence sequence;
};

template <typename List1, typename OriginalList2, typename Table>
struct LongestCommonSubsequence<List1, Null, OriginalList2, Table> {
    enum {length = Last<typename Table::head::value>::value::value};

    typedef typename Traceback<
        typename Reverse<List1>::value,
        typename Reverse<OriginalList2>::value, 
        typename Map<ReverseWrapped, Table>::value>::value
    sequence;
};

int main() {
    // Lists
    static_assert(LinkedList<Integer<1>,LinkedList<Integer<2>,LinkedList<Integer<3>,Null>>>::head::value == 1);
    static_assert(List<Integer<1>,Integer<2>,Integer<3>>::list::head::value == 1);
    static_assert(IntList<1,2,3>::list::head::value == 1);
    static_assert(List<ListWrapper<IntList<4,5,6>::list>, ListWrapper<IntList<7,8,9>::list>>::list::head::value::head::value == 4);
    static_assert(List<ListWrapper<IntList<4,5,6>::list>, ListWrapper<IntList<7,8,9>::list>>::list::tail::head::value::head::value == 7);

    // Reverse
    static_assert(std::is_same_v<
        typename IntList<3,2,1>::list, 
        typename Reverse<typename IntList<1,2,3>::list>::value>);

    // RemoveWrappedHead
    static_assert(std::is_same_v<
        ListWrapper<typename IntList<2,3>::list>,
        typename RemoveWrappedHead<ListWrapper<typename IntList<1,2,3>::list>>::value>);

    // Map
    static_assert(std::is_same_v<
        typename List<ListWrapper<IntList<5,6>::list>, ListWrapper<IntList<8,9>::list>>::list,
        typename Map<RemoveWrappedHead, List<ListWrapper<IntList<4,5,6>::list>, ListWrapper<IntList<7,8,9>::list>>::list>::value>);

    // LongestCommonSubsequence::length
    static_assert(LongestCommonSubsequence<
        typename IntList<1,3,1,4,2>::list,
        typename IntList<3,2,4,1>::list>::length
        == 2);
    static_assert(LongestCommonSubsequence<
        typename IntList<3,2,4,1>::list,
        typename IntList<1,3,1,4,2>::list>::length
        == 2);

    static_assert(LongestCommonSubsequence<
        typename IntList<15,12,4,20,1,17,24>::list,
        typename IntList<12,24,4,1,18,15,17>::list>::length
        == 4);
    static_assert(LongestCommonSubsequence<
        typename IntList<12,24,4,1,18,15,17>::list,
        typename IntList<15,12,4,20,1,17,24>::list>::length
        == 4);

    static_assert(LongestCommonSubsequence<
        typename IntList<42,42,42>::list,
        typename IntList<42,42,42>::list>::length
        == 3);

    static_assert(LongestCommonSubsequence<
        typename IntList<>::list,
        typename IntList<1,2,3>::list>::length
        == 0);
    static_assert(LongestCommonSubsequence<
        typename IntList<1,2,3>::list,
        typename IntList<>::list>::length
        == 0);

    static_assert(LongestCommonSubsequence<
        typename IntList<1,2,3,4,5>::list,
        typename IntList<6,7,8>::list>::length
        == 0);
    static_assert(LongestCommonSubsequence<
        typename IntList<6,7,8>::list,
        typename IntList<1,2,3,4,5>::list>::length
        == 0);

    static_assert(LongestCommonSubsequence<
        typename IntList<2,1,7,3,5,4,8,9,3,6>::list,
        typename IntList<1,3,5,2,4,3,6,7,8,9>::list>::length
        == 6);
    static_assert(LongestCommonSubsequence<
        typename IntList<2,1,7,3,5,4,8,9,3,6>::list,
        typename IntList<1,3,5,2,4,3,6,7,8,9>::list>::length
        == 6);

    // LongestCommonSubsequence::sequence
    static_assert(std::is_same_v<
        typename IntList<3,2>::list,
        typename LongestCommonSubsequence<
            typename IntList<1,3,1,4,2>::list,
            typename IntList<3,2,4,1>::list>::sequence>);
    static_assert(std::is_same_v<
        typename IntList<3,1>::list,
        typename LongestCommonSubsequence<
            typename IntList<3,2,4,1>::list,
            typename IntList<1,3,1,4,2>::list>::sequence>);

    static_assert(std::is_same_v<
        typename IntList<12,4,1,17>::list,
        typename LongestCommonSubsequence<
            typename IntList<15,12,4,20,1,17,24>::list,
            typename IntList<12,24,4,1,18,15,17>::list>::sequence>);
    static_assert(std::is_same_v<
        typename IntList<12,4,1,17>::list,
        typename LongestCommonSubsequence<
            typename IntList<12,24,4,1,18,15,17>::list,
            typename IntList<15,12,4,20,1,17,24>::list>::sequence>);

    static_assert(std::is_same_v<
        typename IntList<42,42,42>::list,
        typename LongestCommonSubsequence<
            typename IntList<42,42,42>::list,
            typename IntList<42,42,42>::list>::sequence>);

    static_assert(std::is_same_v<
        typename IntList<>::list,
        typename LongestCommonSubsequence<
            typename IntList<>::list,
            typename IntList<1,2,3>::list>::sequence>);
    static_assert(std::is_same_v<
        typename IntList<>::list,
        typename LongestCommonSubsequence<
            typename IntList<1,2,3>::list,
            typename IntList<>::list>::sequence>);

    static_assert(std::is_same_v<
        typename IntList<>::list,
        typename LongestCommonSubsequence<
            typename IntList<1,2,3,4,5>::list,
            typename IntList<6,7,8>::list>::sequence>);
    static_assert(std::is_same_v<
        typename IntList<>::list,
        typename LongestCommonSubsequence<
            typename IntList<6,7,8>::list,
            typename IntList<1,2,3,4,5>::list>::sequence>);

    static_assert(std::is_same_v<
        typename IntList<1,3,5,4,3,6>::list,
        typename LongestCommonSubsequence<
            typename IntList<2,1,7,3,5,4,8,9,3,6>::list,
            typename IntList<1,3,5,2,4,3,6,7,8,9>::list>::sequence>);
    static_assert(std::is_same_v<
        typename IntList<1,3,5,4,8,9>::list,
        typename LongestCommonSubsequence<
            typename IntList<1,3,5,2,4,3,6,7,8,9>::list,
            typename IntList<2,1,7,3,5,4,8,9,3,6>::list>::sequence>);
}