TEST_F(ExprMatchTest, GteWithRHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$gte: [3, '$x']}}"));

    ASSERT_TRUE(matches(BSON("x" << 3)));
    ASSERT_TRUE(matches(BSON("x" << 1)));

    ASSERT_FALSE(matches(BSON("x" << 10)));
}