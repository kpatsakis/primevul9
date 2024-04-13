TEST_F(ExprMatchTest, OrWithDistinctMatchRewritableAndNonMatchRewritableChildrenMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$or: [{$eq: ['$x', 1]}, {$eq: ['$y', '$z']}]}}"));

    ASSERT_TRUE(matches(BSON("x" << 1)));
    ASSERT_TRUE(matches(BSON("y" << 1 << "z" << 1)));

    ASSERT_FALSE(matches(BSON("x" << 2 << "y" << 3)));
    ASSERT_FALSE(matches(BSON("y" << 1)));
    ASSERT_FALSE(matches(BSON("y" << 1 << "z" << 2)));
}