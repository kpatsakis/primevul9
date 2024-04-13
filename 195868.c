TEST_F(ExprMatchTest, AndWithDistinctMatchAndNonMatchSubTreeMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$and: [{$eq: ['$x', 1]}, {$eq: ['$y', '$z']}]}}"));

    ASSERT_TRUE(matches(BSON("x" << 1 << "y" << 2 << "z" << 2)));

    ASSERT_FALSE(matches(BSON("x" << 2 << "y" << 2 << "z" << 2)));
    ASSERT_FALSE(matches(BSON("x" << 1 << "y" << 2 << "z" << 10)));
    ASSERT_FALSE(matches(BSON("x" << 1 << "y" << 2)));
}