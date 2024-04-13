TEST_F(ExprMatchTest, ConstantNumberZeroExpressionMatchesCorrectly) {
    createMatcher(fromjson("{$expr: 0}"));

    ASSERT_FALSE(matches(BSON("x" << 2)));
}