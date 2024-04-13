TEST_F(ExprMatchTest, EqWithRHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$eq: [3, '$x']}}"));

    ASSERT_TRUE(matches(BSON("x" << 3)));

    ASSERT_FALSE(matches(BSON("x" << 1)));
    ASSERT_FALSE(matches(BSON("x" << 10)));
}