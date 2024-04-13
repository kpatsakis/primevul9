TEST_F(ExprMatchTest, ConstantNegativeNumberExpressionMatchesCorrectly) {
    createMatcher(fromjson("{$expr: -1}"));

    ASSERT_TRUE(matches(BSON("x" << 2)));
}