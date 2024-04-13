TEST_F(ExprMatchTest, LteWithLHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$lte: ['$x', 3]}}"));

    ASSERT_TRUE(matches(BSON("x" << 3)));
    ASSERT_FALSE(matches(BSON("x" << 10)));
}