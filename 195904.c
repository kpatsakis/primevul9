TEST_F(ExprMatchTest, ConstantPositiveNumberExpressionMatchesCorrectly) {
    createMatcher(fromjson("{$expr: 1}"));

    ASSERT_TRUE(matches(BSON("x" << 2)));
}