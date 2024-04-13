TEST_F(ExprMatchTest, GtWithLHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$gt: ['$x', 3]}}"));

    ASSERT_TRUE(matches(BSON("x" << 10)));

    ASSERT_FALSE(matches(BSON("x" << 1)));
    ASSERT_FALSE(matches(BSON("x" << 3)));
}