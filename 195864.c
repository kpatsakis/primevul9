TEST_F(ExprMatchTest, LtWithLHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$lt: ['$x', 3]}}"));

    ASSERT_TRUE(matches(BSON("x" << 1)));

    ASSERT_FALSE(matches(BSON("x" << 3)));
    ASSERT_FALSE(matches(BSON("x" << 10)));
}