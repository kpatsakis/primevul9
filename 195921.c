TEST_F(ExprMatchTest, NeWithLHSFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$ne: ['$x', 3]}}"));

    ASSERT_TRUE(matches(BSON("x" << 1)));
    ASSERT_TRUE(matches(BSON("x" << 10)));

    ASSERT_FALSE(matches(BSON("x" << 3)));
}