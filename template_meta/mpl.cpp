#include <boost/core/enable_if.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/always.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/pair_view.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/type_traits.hpp>

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

template <typename Vector1, typename Vector2, typename Table, typename N, typename M, typename Enable = void>
struct Traceback {
    typedef typename if_<
        is_same<typename at<Vector1, typename prior<N>::type>::type, typename at<Vector2, typename prior<M>::type>::type>,
        typename push_back<
            typename Traceback<
                Vector1,
                Vector2,
                Table,
                typename prior<N>::type,
                typename prior<M>::type
                >::type, 
            typename at<Vector1, typename prior<N>::type>::type
            >::type,
        typename if_<
            greater_equal<
                typename at<typename at<Table, typename prior<M>::type>::type::type, N>::type,
                typename at<typename at<Table, M>::type::type, typename prior<N>::type>::type
                >,
            typename Traceback<
                Vector1,
                Vector2,
                Table,
                N,
                typename prior<M>::type
                >::type,
            typename Traceback<
                Vector1,
                Vector2,
                Table,
                typename prior<N>::type,
                M
                >::type
            >::type
        >::type type;
};

template <typename Vector1, typename Vector2, typename Table, typename N, typename M>
struct Traceback<Vector1, Vector2, Table, N, M,
    typename enable_if<typename or_<bool_<N::value == 0>, bool_<M::value == 0>>::type>::type> {
    typedef vector0_c<int> type;
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

    enum {length = back<typename back<table>::type::type>::type::value};

    typedef typename Traceback<
        Vector1,
        Vector2,
        table,
        typename size<Vector1>::type,
        typename size<Vector2>::type
    >::type sequence;
};

int main() {
    // LongestCommonSubsequence::length
    static_assert(LongestCommonSubsequence<
        vector_c<int,1,3,1,4,2>,
        vector_c<int,3,2,4,1>>::length
        == 2);
    static_assert(LongestCommonSubsequence<
        vector_c<int,3,2,4,1>,
        vector_c<int,1,3,1,4,2>>::length
        == 2);

    static_assert(LongestCommonSubsequence<
        vector_c<int,15,12,4,20,1,17,24>,
        vector_c<int,12,24,4,1,18,15,17>>::length
        == 4);
    static_assert(LongestCommonSubsequence<
        vector_c<int,12,24,4,1,18,15,17>,
        vector_c<int,15,12,4,20,1,17,24>>::length
        == 4);

    static_assert(LongestCommonSubsequence<
        vector_c<int,42,42,42>,
        vector_c<int,42,42,42>>::length
        == 3);

    static_assert(LongestCommonSubsequence<
        vector_c<int>,
        vector_c<int,1,2,3>>::length
        == 0);
    static_assert(LongestCommonSubsequence<
        vector_c<int,1,2,3>,
        vector_c<int>>::length
        == 0);

    static_assert(LongestCommonSubsequence<
        vector_c<int,1,2,3,4,5>,
        vector_c<int,6,7,8>>::length
        == 0);
    static_assert(LongestCommonSubsequence<
        vector_c<int,1,2,3,4,5>,
        vector_c<int,6,7,8>>::length
        == 0);

    static_assert(LongestCommonSubsequence<
        vector_c<int,2,1,7,3,5,4,8,9,3,6>,
        vector_c<int,1,3,5,2,4,3,6,7,8,9>>::length
        == 6);
    static_assert(LongestCommonSubsequence<
        vector_c<int,1,3,5,2,4,3,6,7,8,9>,
        vector_c<int,2,1,7,3,5,4,8,9,3,6>>::length
        == 6);

    // LongestCommonSubsequence::sequence
    BOOST_MPL_ASSERT((equal<
        vector_c<int,3,2>,
        typename LongestCommonSubsequence<
            vector_c<int,1,3,1,4,2>,
            vector_c<int,3,2,4,1>>::sequence>));
    BOOST_MPL_ASSERT((equal<
        vector_c<int,3,1>,
        typename LongestCommonSubsequence<
            vector_c<int,3,2,4,1>,
            vector_c<int,1,3,1,4,2>>::sequence>));

    BOOST_MPL_ASSERT((equal<
        vector_c<int,12,4,1,17>,
        typename LongestCommonSubsequence<
            vector_c<int,15,12,4,20,1,17,24>,
            vector_c<int,12,24,4,1,18,15,17>>::sequence>));
    BOOST_MPL_ASSERT((equal<
        vector_c<int,12,4,1,17>,
        typename LongestCommonSubsequence<
            vector_c<int,12,24,4,1,18,15,17>,
            vector_c<int,15,12,4,20,1,17,24>>::sequence>));

    BOOST_MPL_ASSERT((equal<
        vector_c<int,42,42,42>,
        typename LongestCommonSubsequence<
            vector_c<int,42,42,42>,
            vector_c<int,42,42,42>>::sequence>));
    
    BOOST_MPL_ASSERT((equal<
        vector_c<int>,
        typename LongestCommonSubsequence<
            vector_c<int>,
            vector_c<int,1,2,3>>::sequence>));
    BOOST_MPL_ASSERT((equal<
        vector_c<int>,
        typename LongestCommonSubsequence<
            vector_c<int,1,2,3>,
            vector_c<int>>::sequence>));

    BOOST_MPL_ASSERT((equal<
        vector_c<int>,
        typename LongestCommonSubsequence<
            vector_c<int,1,2,3,4,5>,
            vector_c<int,6,7,8>>::sequence>));
    BOOST_MPL_ASSERT((equal<
        vector_c<int>,
        typename LongestCommonSubsequence<
            vector_c<int,1,2,3,4,5>,
            vector_c<int,6,7,8>>::sequence>));

    BOOST_MPL_ASSERT((equal<
        vector_c<int,1,3,5,4,3,6>,
        typename LongestCommonSubsequence<
            vector_c<int,2,1,7,3,5,4,8,9,3,6>,
            vector_c<int,1,3,5,2,4,3,6,7,8,9>>::sequence>));
    BOOST_MPL_ASSERT((equal<
        vector_c<int,1,3,5,4,8,9>,
        typename LongestCommonSubsequence<
            vector_c<int,1,3,5,2,4,3,6,7,8,9>,
            vector_c<int,2,1,7,3,5,4,8,9,3,6>>::sequence>));
}