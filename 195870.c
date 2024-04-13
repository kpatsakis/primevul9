TEST_F(ExprMatchTest, GteWithLHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$gte: ['$x', 3]}}"));

    ASSERT_TRUE(matches(BSON("x" << 3)));
    ASSERT_TRUE(matches(BSON("x" << 10)));

    ASSERT_FALSE(matches(BSON("x" << 1)));
}