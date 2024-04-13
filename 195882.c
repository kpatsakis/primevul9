TEST_F(ExprMatchTest, InWithoutLhsFieldPathMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$in: [2, [1, 2, 3]]}}"));
    ASSERT_TRUE(matches(BSON("x" << 2)));

    createMatcher(fromjson("{$expr: {$in: [2, [5, 6, 7]]}}"));
    ASSERT_FALSE(matches(BSON("x" << 2)));
}