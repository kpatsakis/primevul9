TEST_F(ExprMatchTest, GtWithRHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$gt: [3, '$x']}}"));

    ASSERT_TRUE(matches(BSON("x" << 1)));

    ASSERT_FALSE(matches(BSON("x" << 3)));
    ASSERT_FALSE(matches(BSON("x" << 10)));
}