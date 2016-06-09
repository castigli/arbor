#include <vector>

#include "gtest.h"
#include "util.hpp"

#include "../src/algorithms.hpp"


TEST(algorithms, sum)
{
    // sum of 10 times 2 is 20
    std::vector<int> v1(10, 2);
    EXPECT_EQ(10*2, nest::mc::algorithms::sum(v1));

    // make an array 1:20 and sum it up using formula for arithmetic sequence
    std::vector<int> v2(20);
    std::iota(v2.begin(), v2.end(), 1);
    auto n = 20;
    EXPECT_EQ((n+1)*n/2, nest::mc::algorithms::sum(v2));
}

TEST(algorithms, make_index)
{
    {
        std::vector<int> v(10, 1);
        auto index = nest::mc::algorithms::make_index(v);

        EXPECT_EQ(index.size(), 11u);
        EXPECT_EQ(index.front(), 0);
        EXPECT_EQ(index.back(), nest::mc::algorithms::sum(v));
    }

    {
        std::vector<int> v(10);
        std::iota(v.begin(), v.end(), 1);
        auto index = nest::mc::algorithms::make_index(v);

        EXPECT_EQ(index.size(), 11u);
        EXPECT_EQ(index.front(), 0);
        EXPECT_EQ(index.back(), nest::mc::algorithms::sum(v));
    }
}

TEST(algorithms, minimal_degree)
{
    {
        std::vector<int> v = {0};
        EXPECT_TRUE(nest::mc::algorithms::is_minimal_degree(v));
    }

    {
        std::vector<int> v = {0, 0, 1, 2, 3, 4};
        EXPECT_TRUE(nest::mc::algorithms::is_minimal_degree(v));
    }

    {
        std::vector<int> v = {0, 0, 1, 2, 0, 4};
        EXPECT_TRUE(nest::mc::algorithms::is_minimal_degree(v));
    }

    {
        std::vector<int> v = {0, 0, 1, 2, 0, 4, 5, 4};
        EXPECT_TRUE(nest::mc::algorithms::is_minimal_degree(v));
    }

    {
        std::vector<int> v = {1};
        EXPECT_FALSE(nest::mc::algorithms::is_minimal_degree(v));
    }

    {
        std::vector<int> v = {0, 2};
        EXPECT_FALSE(nest::mc::algorithms::is_minimal_degree(v));
    }

    {
        std::vector<int> v = {0, 1, 2};
        EXPECT_FALSE(nest::mc::algorithms::is_minimal_degree(v));
    }
}

TEST(algorithms, is_strictly_monotonic_increasing)
{
    EXPECT_TRUE(
        nest::mc::algorithms::is_strictly_monotonic_increasing(
            std::vector<int>{0}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_strictly_monotonic_increasing(
            std::vector<int>{0, 1, 2, 3}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_strictly_monotonic_increasing(
            std::vector<int>{8, 20, 42, 89}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_strictly_monotonic_increasing(
            std::vector<int>{0, 0}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_strictly_monotonic_increasing(
            std::vector<int>{8, 20, 20, 89}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_strictly_monotonic_increasing(
            std::vector<int>{3, 2, 1, 0}
        )
    );
}

TEST(algorithms, is_strictly_monotonic_decreasing)
{
    EXPECT_TRUE(
        nest::mc::algorithms::is_strictly_monotonic_decreasing(
            std::vector<int>{0}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_strictly_monotonic_decreasing(
            std::vector<int>{3, 2, 1, 0}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_strictly_monotonic_decreasing(
            std::vector<int>{0, 1, 2, 3}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_strictly_monotonic_decreasing(
            std::vector<int>{8, 20, 42, 89}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_strictly_monotonic_decreasing(
            std::vector<int>{0, 0}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_strictly_monotonic_decreasing(
            std::vector<int>{8, 20, 20, 89}
        )
    );
}

TEST(algorithms, is_positive)
{
    EXPECT_TRUE(
        nest::mc::algorithms::is_positive(
            std::vector<int>{}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_positive(
            std::vector<int>{3, 2, 1}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_positive(
            std::vector<int>{3, 2, 1, 0}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_positive(
            std::vector<int>{-1}
        )
    );
}

TEST(algorithms, has_contiguous_segments)
{
    //
    //       0
    //       |
    //       1
    //       |
    //       2
    //      /|\.
    //     3 7 4
    //    /     \.
    //   5       6
    //
    EXPECT_FALSE(
        nest::mc::algorithms::has_contiguous_segments(
            std::vector<int>{0, 0, 1, 2, 2, 3, 4, 2}
        )
    );

    //
    //       0
    //       |
    //       1
    //       |
    //       2
    //      /|\.
    //     3 6 5
    //    /     \.
    //   4       7
    //
    EXPECT_FALSE(
        nest::mc::algorithms::has_contiguous_segments(
            std::vector<int>{0, 0, 1, 2, 3, 2, 2, 5}
        )
    );

    //
    //       0
    //       |
    //       1
    //       |
    //       2
    //      /|\.
    //     3 7 5
    //    /     \.
    //   4       6
    //
    EXPECT_TRUE(
        nest::mc::algorithms::has_contiguous_segments(
            std::vector<int>{0, 0, 1, 2, 3, 2, 5, 2}
        )
    );

    //
    //         0
    //         |
    //         1
    //        / \.
    //       2   7
    //      / \.
    //     3   5
    //    /     \.
    //   4       6
    //
    EXPECT_TRUE(
        nest::mc::algorithms::has_contiguous_segments(
            std::vector<int>{0, 0, 1, 2, 3, 2, 5, 1}
        )
    );

    // Soma-only list
    EXPECT_TRUE(
        nest::mc::algorithms::has_contiguous_segments(
            std::vector<int>{0}
        )
    );

    // Empty list
    EXPECT_TRUE(
        nest::mc::algorithms::has_contiguous_segments(
            std::vector<int>{}
        )
    );
}

TEST(algorithms, is_unique)
{
    EXPECT_TRUE(
        nest::mc::algorithms::is_unique(
            std::vector<int>{}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_unique(
            std::vector<int>{0}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_unique(
            std::vector<int>{0,1,100}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_unique(
            std::vector<int>{0,0}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_unique(
            std::vector<int>{0,1,2,2,3,4}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_unique(
            std::vector<int>{0,1,2,3,4,4}
        )
    );
}

TEST(algorithms, is_sorted)
{
    EXPECT_TRUE(
        nest::mc::algorithms::is_sorted(
            std::vector<int>{}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_sorted(
            std::vector<int>{100}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_sorted(
            std::vector<int>{0,1,2}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_sorted(
            std::vector<int>{0,2,100}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_sorted(
            std::vector<int>{0,0}
        )
    );
    EXPECT_TRUE(
        nest::mc::algorithms::is_sorted(
            std::vector<int>{0,1,2,2,2,2,3,4,5,5,5}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_sorted(
            std::vector<int>{0,1,2,1}
        )
    );
    EXPECT_FALSE(
        nest::mc::algorithms::is_sorted(
            std::vector<int>{1,0}
        )
    );
}

TEST(algorithms, child_count)
{
    {
        //
        //        0
        //       /|\.
        //      1 4 6
        //     /  |  \.
        //    2   5   7
        //   /         \.
        //  3           8
        //             / \.
        //            9   11
        //           /     \.
        //          10      12
        //                   \.
        //                    13
        //
        std::vector<int> parent_index =
            { 0, 0, 1, 2, 0, 4, 0, 6, 7, 8, 9, 8, 11, 12 };
        std::vector<int> expected_child_count =
            { 3, 1, 1, 0, 1, 0, 1, 1, 2, 1, 0, 1, 1, 0 };

        // auto count = nest::mc::algorithms::child_count(parent_index);
        EXPECT_EQ(expected_child_count,
                  nest::mc::algorithms::child_count(parent_index));
    }

}

TEST(algorithms, branches)
{
    using namespace nest::mc;

    {
        //
        //        0
        //       /|\.
        //      1 4 6
        //     /  |  \.
        //    2   5   7
        //   /         \.
        //  3           8
        //             / \.
        //            9   11
        //           /     \.
        //          10      12
        //                   \.
        //                    13
        //
        std::vector<int> parent_index =
            { 0, 0, 1, 2, 0, 4, 0, 6, 7, 8, 9, 8, 11, 12 };
        std::vector<int> expected_branches =
            { 0, 1, 1, 1, 2, 2, 3, 3, 3, 4, 4, 5, 5, 5 };

        auto actual_branches = algorithms::branches(parent_index);
        EXPECT_EQ(expected_branches, actual_branches);
    }

    {
        //
        //    0
        //    |
        //    1
        //    |
        //    2
        //    |
        //    3
        //
        std::vector<int> parent_index =
            { 0, 0, 1, 2 };
        std::vector<int> expected_branches =
            { 0, 1, 1, 1 };

        auto actual_branches = algorithms::branches(parent_index);
        EXPECT_EQ(expected_branches, actual_branches);
    }

    {
        //
        //    0
        //    |
        //    1
        //    |
        //    2
        //   / \.
        //  3   4
        //       \.
        //        5
        //
        std::vector<int> parent_index =
            { 0, 0, 1, 2, 2, 4 };
        std::vector<int> expected_branches =
            { 0, 1, 1, 2, 3, 3 };

        auto actual_branches = algorithms::branches(parent_index);
        EXPECT_EQ(expected_branches, actual_branches);
    }

    {
        std::vector<int> parent_index = { 0 };
        std::vector<int> expected_branches = { 0 };

        auto actual_branches = algorithms::branches(parent_index);
        EXPECT_EQ(expected_branches, actual_branches);
    }
}

TEST(algorithms, index_into)
{
    using C = std::vector<int>;

    // by default index_into assumes that the inputs satisfy
    // quite a strong set of prerequisites
    //
    // TODO: test that the EXPECTS() catch bad inputs when DEBUG mode is enabled
    //       put this in a seperate unit test
    auto tests = {
        std::make_pair(C{}, C{}),
        std::make_pair(C{100}, C{}),
        std::make_pair(C{0,1,3,4,6,7,10,11}, C{0,4,6,7,11}),
        std::make_pair(C{0,1,3,4,6,7,10,11}, C{0}),
        std::make_pair(C{0,1,3,4,6,7,10,11}, C{11}),
        std::make_pair(C{0,1,3,4,6,7,10,11}, C{4}),
        std::make_pair(C{0,1,3,4,6,7,10,11}, C{0,11}),
        std::make_pair(C{0,1,3,4,6,7,10,11}, C{4,11}),
        std::make_pair(C{0,1,3,4,6,7,10,11}, C{0,1,3,4,6,7,10,11})
    };

    auto test_result = [] (const C& super, const C& sub, const C& index) {
        if(sub.size()!=index.size()) return false;
        for(auto i=0u; i<sub.size(); ++i) {
            if(index[i]>=C::value_type(super.size())) return false;
            if(super[index[i]]!=sub[i]) return false;
        }
        return true;
    };

    for(auto& t : tests) {
        EXPECT_TRUE(
            test_result(
                t.first, t.second,
                nest::mc::algorithms::index_into(t.first, t.second)
            )
        );
    }
}
