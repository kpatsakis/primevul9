TEST_F(ExprMatchTest, EqWithTwoFieldPathsMatchesCorrectly) {
    createMatcher(fromjson("{$expr: {$eq: ['$x', '$y']}}"));

    ASSERT_TRUE(matches(BSON("x" << 2 << "y" << 2)));

    ASSERT_FALSE(matches(BSON("x" << 2 << "y" << 3)));
    ASSERT_FALSE(matches(BSON("x" << 2)));
}