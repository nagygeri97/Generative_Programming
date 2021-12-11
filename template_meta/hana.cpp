#include <boost/hana.hpp>

using namespace boost::hana;
using namespace boost::hana::literals;

template <typename T>
constexpr auto empty_row(T&& t) {
    // return prepend(transform(t, [](auto){return 0_c;}), 0_c);
    // return prepend(t | [](auto&&){return tuple_c<int, 0>;}, 0_c);
    return replicate<tuple_tag>(0_c, length(t) + size_c<1>);
}

template <typename T1, typename T2, typename Table>
constexpr auto traceback(T1&& t1, T2&& t2, Table&& table) {
    return while_(
        [](auto&& state) {
            return (state[1_c] > size_c<0>) && (state[2_c] > size_c<0>);
        },
        make_tuple(make_tuple(), length(t1), length(t2)),
        [&](auto&& state) {
            auto result = state[0_c];
            auto n = state[1_c];
            auto m = state[2_c];
            return if_(
                t1[n - size_c<1>] == t2[m - size_c<1>],
                make_tuple(prepend(result, t1[n - size_c<1>]), n - size_c<1>, m - size_c<1>),
                if_(
                    table[m - size_c<1>][n] >= table[m][n - size_c<1>],
                    make_tuple(result, n, m - size_c<1>),
                    make_tuple(result, n - size_c<1>, m)
                )
            );
        }
    )[0_c];
}

template <typename T1, typename T2>
constexpr auto longest_common_subsequence(T1&& t1, T2&& t2) {
    auto next_row = [&t1](auto&& prevRow, auto&& head) {
        return fold(
            zip(t1, drop_front(prevRow, 1_c)),
            make_tuple(0_c, tuple_c<int,0>),
            [&head](auto&& acc, auto&& x) {
                return make_tuple(x[1_c], 
                    append(
                        acc[1_c],
                        if_(
                            head == x[0_c],
                            acc[0_c] + 1_c,
                            max(x[1_c], back(acc[1_c]))
                        )
                    )
                );
            }
        )[1_c];
    };

    auto table = scan_left(t2, empty_row(t1), next_row);
    auto length = back(back(table));
    auto sequence = traceback(t1, t2, table);
    return make_pair(length, sequence);
}

int main() {
    // longest_common_subsequence
    static_assert(longest_common_subsequence(
        tuple_c<int,1,3,1,4,2>,
        tuple_c<int,3,2,4,1>)
        == make_pair(2_c, tuple_c<int,3,2>));
    static_assert(longest_common_subsequence(
        tuple_c<int,3,2,4,1>,
        tuple_c<int,1,3,1,4,2>)
        == make_pair(2_c, tuple_c<int,3,1>));

    static_assert(longest_common_subsequence(
        tuple_c<int,15,12,4,20,1,17,24>,
        tuple_c<int,12,24,4,1,18,15,17>)
        == make_pair(4_c, tuple_c<int,12,4,1,17>));
    static_assert(longest_common_subsequence(
        tuple_c<int,12,24,4,1,18,15,17>,
        tuple_c<int,15,12,4,20,1,17,24>)
        == make_pair(4_c, tuple_c<int,12,4,1,17>));

    static_assert(longest_common_subsequence(
        tuple_c<int,42,42,42>,
        tuple_c<int,42,42,42>)
        == make_pair(3_c, tuple_c<int,42,42,42>));

    static_assert(longest_common_subsequence(
        tuple_c<int>,
        tuple_c<int,1,2,3>)
        == make_pair(0_c, tuple_c<int>));
    static_assert(longest_common_subsequence(
        tuple_c<int,1,2,3>,
        tuple_c<int>)
        == make_pair(0_c, tuple_c<int>));

    static_assert(longest_common_subsequence(
        tuple_c<int,1,2,3,4,5>,
        tuple_c<int,6,7,8>)
        == make_pair(0_c, tuple_c<int>));
    static_assert(longest_common_subsequence(
        tuple_c<int,1,2,3,4,5>,
        tuple_c<int,6,7,8>)
        == make_pair(0_c, tuple_c<int>));

    static_assert(longest_common_subsequence(
        tuple_c<int,2,1,7,3,5,4,8,9,3,6>,
        tuple_c<int,1,3,5,2,4,3,6,7,8,9>)
        == make_pair(6_c, tuple_c<int,1,3,5,4,3,6>));
    static_assert(longest_common_subsequence(
        tuple_c<int,1,3,5,2,4,3,6,7,8,9>,
        tuple_c<int,2,1,7,3,5,4,8,9,3,6>)
        == make_pair(6_c, tuple_c<int,1,3,5,4,8,9>));
}