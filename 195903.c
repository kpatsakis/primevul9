TEST_F(ExprMatchTest, EqWithLHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$eq: ['$x', 3]}}"));

    ASSERT_TRUE(matches(BSON("x" << 3)));

    ASSERT_FALSE(matches(BSON("x" << 1)));
    ASSERT_FALSE(matches(BSON("x" << 10)));
}