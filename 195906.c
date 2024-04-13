TEST_F(ExprMatchTest, LteWithRHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$lte: [3, '$x']}}"));

    ASSERT_TRUE(matches(BSON("x" << 3)));
    ASSERT_TRUE(matches(BSON("x" << 10)));

    ASSERT_FALSE(matches(BSON("x" << 1)));
}