#include <iostream>
#include <boost/type_traits.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/always.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/pair_view.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/pop_front.hpp>

using namespace boost;
using namespace boost::mpl;

template <typename Vector>
struct EmptyRow {
    typedef typename push_front<typename transform<Vector, always<integral_c<int, 0>>>::type, integral_c<int, 0>>::type type;
};

template <typename Head, typename Vector1, typename PrevRow>
struct FillRow {
    typedef typename second<
        typename fold<
            pair_view<Vector1, typename pop_front<typename PrevRow::type>::type>, 
            pair<integral_c<int, 0>, vector1_c<int, 0>>,
            lambda<
                pair<
                    second<_2>,
                    if_<
                        is_same<Head, first<_2>>,
                        push_back<second<_1>, next<first<_1>>>,
                        push_back<second<_1>, max<second<_2>, back<second<_1>>>>
                        >
                    >
                >
            >::type
        >::type type;
};


template <typename Vector1, typename Vector2>
struct LongestCommonSubsequence {

    typedef typename fold<
        Vector2,
        vector1<EmptyRow<Vector1>>,
        lambda<
            push_back<_1, FillRow<_2, Vector1, back<_1>>>
        >
    >::type table;

    enum {length = back<typename back<table>::type>::type::value};
};

int main() {
    // LongestCommonSubsequence::length
    static_assert(LongestCommonSubsequence<
        vector_c<int,1,3,1,4,2>,
        vector_c<int,3,2,4,1>>::length
        == 2, "");
    static_assert(LongestCommonSubsequence<
        vector_c<int,3,2,4,1>,
        vector_c<int,1,3,1,4,2>>::length
        == 2, "");

    static_assert(LongestCommonSubsequence<
        vector_c<int,15,12,4,20,1,17,24>,
        vector_c<int,12,24,4,1,18,15,17>>::length
        == 4, "");
    static_assert(LongestCommonSubsequence<
        vector_c<int,12,24,4,1,18,15,17>,
        vector_c<int,15,12,4,20,1,17,24>>::length
        == 4, "");

    static_assert(LongestCommonSubsequence<
        vector_c<int,42,42,42>,
        vector_c<int,42,42,42>>::length
        == 3, "");
}