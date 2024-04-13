TEST_F(ExprMatchTest, InWithLhsFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$in: ['$x', [1, 2, 3]]}}"));

    ASSERT_TRUE(matches(BSON("x" << 1)));
    ASSERT_TRUE(matches(BSON("x" << 3)));

    ASSERT_FALSE(matches(BSON("x" << 5)));
    ASSERT_FALSE(matches(BSON("y" << 2)));
    ASSERT_FALSE(matches(BSON("x" << BSON("y" << 2))));
}