TEST_F(ExprMatchTest, LtWithRHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$lt: [3, '$x']}}"));

    ASSERT_TRUE(matches(BSON("x" << 10)));

    ASSERT_FALSE(matches(BSON("x" << 3)));
    ASSERT_FALSE(matches(BSON("x" << 1)));
}